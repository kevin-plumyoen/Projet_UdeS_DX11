#include "resource.h"
#include "DispositifD3D11.h"
#include "Util.h"
#include "InfoDispositif.h"

namespace PM3D
{

    CDispositifD3D11::~CDispositifD3D11()
    {
        DXRelacher(alpha_blend_enable);
        DXRelacher(alpha_blend_disable);

        pSwapChain->SetFullscreenState(FALSE, nullptr); // passer en mode fenêtré

        DXRelacher(mSolidCullBackRS);
        DXRelacher(pDepthTexture);
        DXRelacher(pDepthStencilView);

        if (pImmediateContext)
        {
            pImmediateContext->ClearState();
        }
        DXRelacher(pRenderTargetView);
        DXRelacher(pImmediateContext);
        DXRelacher(pSwapChain);
        DXRelacher(pD3DDevice);
    }

    //  FONCTION : CDispositifD3D11, constructeur paramètré 
    //  BUT :	Constructeur de notre classe spécifique de dispositif 
    //  PARAMÈTRES:		
    //		cdsMode:	CDS_FENETRE application fenêtrée
    //					CDS_PLEIN_ECRAN application plein écran
    //
    //		hWnd:	Handle sur la fenêtre Windows de l'application,
    //    			nécessaire pour la fonction de création du 
    //				dispositif
    CDispositifD3D11::CDispositifD3D11(const CDS_MODE cdsMode,
        const HWND hWnd)
    {
        UINT largeur;
        UINT hauteur;
        UINT createDeviceFlags = 0;

#ifdef _DEBUG
        createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        D3D_FEATURE_LEVEL featureLevels[] =
        {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
        };
        const UINT numFeatureLevels = sizeof(featureLevels) / sizeof(D3D_FEATURE_LEVEL);

        pImmediateContext = nullptr;
        pSwapChain = nullptr;
        pRenderTargetView = nullptr;

        DXGI_SWAP_CHAIN_DESC sd;
        ZeroMemory(&sd, sizeof(sd));

        // Obtenir les informations de l'adaptateur de défaut
        CInfoDispositif Dispo0(ADAPTATEUR_COURANT);

        largeur = 1920;
        hauteur = 1080;

        switch (cdsMode)
        {
        case CDS_FENETRE:
            sd.Windowed = TRUE;

            break;

        case CDS_PLEIN_ECRAN:
            sd.Windowed = FALSE;
            break;

        }

        DXGI_MODE_DESC  desc;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.Height = hauteur;
        desc.Width = largeur;
        desc.RefreshRate.Numerator = 60;
        desc.RefreshRate.Denominator = 1;
        desc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        desc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

        CInfoDispositif DispoVoulu(desc);
        DispoVoulu.GetDesc(desc);

        largeur = desc.Width;
        hauteur = desc.Height;
        largeurEcran = largeur;
        hauteurEcran = hauteur;

        sd.BufferCount = 1;
        sd.BufferDesc.Width = desc.Width;
        sd.BufferDesc.Height = desc.Height;
        sd.BufferDesc.Format = desc.Format;
        sd.BufferDesc.RefreshRate.Numerator = desc.RefreshRate.Numerator;
        sd.BufferDesc.RefreshRate.Denominator = desc.RefreshRate.Denominator;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = hWnd;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;

        sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // Permettre l'échange plein écran

        // régler le problème no 1 du passage en mode fenêtré
        RECT rcClient, rcWindow;
        POINT ptDiff;
        GetClientRect(hWnd, &rcClient);
        GetWindowRect(hWnd, &rcWindow);
        ptDiff.x = (rcWindow.right - rcWindow.left) - rcClient.right;
        ptDiff.y = (rcWindow.bottom - rcWindow.top) - rcClient.bottom;
        MoveWindow(hWnd, rcWindow.left, rcWindow.top, largeur + ptDiff.x, hauteur + ptDiff.y, TRUE);

        DXEssayer(D3D11CreateDeviceAndSwapChain(
            0,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            createDeviceFlags,
            featureLevels, numFeatureLevels,
            D3D11_SDK_VERSION,
            &sd,
            &pSwapChain,
            &pD3DDevice,
            nullptr,
            &pImmediateContext),
            DXE_ERREURCREATIONDEVICE);

        // Création d'un «render target view»
        ID3D11Texture2D *pBackBuffer;
        DXEssayer(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer), DXE_ERREUROBTENTIONBUFFER);

        DXEssayer(pD3DDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pRenderTargetView), DXE_ERREURCREATIONRENDERTARGET);
        pBackBuffer->Release();

        InitDepthBuffer();

        pImmediateContext->OMSetRenderTargets(1, &pRenderTargetView, pDepthStencilView);
        pImmediateContext->OMSetDepthStencilState(pDepthStencilState, 1);

        D3D11_VIEWPORT vp;
        vp.Width = (FLOAT)largeur;
        vp.Height = (FLOAT)hauteur;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        vp.TopLeftX = 0;
        vp.TopLeftY = 0;
        pImmediateContext->RSSetViewports(1, &vp);

        // Création et initialisation des états
        D3D11_RASTERIZER_DESC rsDesc;
        ZeroMemory(&rsDesc, sizeof(D3D11_RASTERIZER_DESC));
        rsDesc.FillMode = D3D11_FILL_SOLID;
        rsDesc.CullMode = D3D11_CULL_BACK;
        rsDesc.FrontCounterClockwise = false;
        pD3DDevice->CreateRasterizerState(&rsDesc, &mSolidCullBackRS);

        pImmediateContext->RSSetState(mSolidCullBackRS);

        InitBlendState();
    }

    void CDispositifD3D11::PresentSpecific()
    {
        pSwapChain->Present(0, 0);
    }

    void CDispositifD3D11::SetRenderTargetView(ID3D11RenderTargetView* target, ID3D11DepthStencilView* depth) {
        pImmediateContext->OMSetRenderTargets(1, &target, depth);
    }

    void CDispositifD3D11::ResetTargetView() {
        ID3D11RenderTargetView* pNullRTV = nullptr;
        SetRenderTargetView(pNullRTV, NULL);

        SetRenderTargetView(GetRenderTargetView(), GetDepthStencilView());
    }

    void CDispositifD3D11::InitDepthBuffer()
    {
        D3D11_TEXTURE2D_DESC depthTextureDesc;
        ZeroMemory(&depthTextureDesc, sizeof(depthTextureDesc));
        depthTextureDesc.Width = largeurEcran;
        depthTextureDesc.Height = hauteurEcran;
        depthTextureDesc.MipLevels = 1;
        depthTextureDesc.ArraySize = 1;
        depthTextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthTextureDesc.SampleDesc.Count = 1;
        depthTextureDesc.SampleDesc.Quality = 0;
        depthTextureDesc.Usage = D3D11_USAGE_DEFAULT;
        depthTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        depthTextureDesc.CPUAccessFlags = 0;
        depthTextureDesc.MiscFlags = 0;

        DXEssayer(pD3DDevice->CreateTexture2D(&depthTextureDesc, nullptr, &pDepthTexture), DXE_ERREURCREATIONTEXTURE);

        // Création de la vue du tampon de profondeur (ou de stencil)
        D3D11_DEPTH_STENCIL_VIEW_DESC descDSView;
        ZeroMemory(&descDSView, sizeof(descDSView));
        descDSView.Format = depthTextureDesc.Format;
        descDSView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        descDSView.Texture2D.MipSlice = 0;
        DXEssayer(pD3DDevice->CreateDepthStencilView(pDepthTexture, &descDSView, &pDepthStencilView), DXE_ERREURCREATIONDEPTHSTENCILTARGET);

        D3D11_DEPTH_STENCIL_DESC dsDesc;
        // Depth test parameters
        dsDesc.DepthEnable = true;
        dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
        // Stencil test parameters
        dsDesc.StencilEnable = true;
        dsDesc.StencilReadMask = 0xFF;
        dsDesc.StencilWriteMask = 0xFF;
        // Stencil operations if pixel is front-facing
        dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
        dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        // Stencil operations if pixel is back-facing
        dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
        dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

        D3D11_DEPTH_STENCIL_DESC dsDescNoTest;
        // Depth test parameters
        dsDescNoTest.DepthEnable = false;
        dsDescNoTest.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        dsDescNoTest.DepthFunc = D3D11_COMPARISON_LESS;
        // Stencil test parameters
        dsDescNoTest.StencilEnable = true;
        dsDescNoTest.StencilReadMask = 0xFF;
        dsDescNoTest.StencilWriteMask = 0xFF;
        // Stencil operations if pixel is front-facing
        dsDescNoTest.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        dsDescNoTest.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
        dsDescNoTest.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        dsDescNoTest.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        // Stencil operations if pixel is back-facing
        dsDescNoTest.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        dsDescNoTest.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
        dsDescNoTest.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        dsDescNoTest.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

        //// Create depth stencil state
        DXEssayer(pD3DDevice->CreateDepthStencilState(&dsDescNoTest, &pDepthStencilStateNoTest));
        DXEssayer(pD3DDevice->CreateDepthStencilState(&dsDesc, &pDepthStencilState));
    }

    void CDispositifD3D11::enable_alpha_blending() {
        GetImmediateContext()->OMSetBlendState(alpha_blend_enable, nullptr, 0xffffffff);
    }

    void CDispositifD3D11::disable_alpha_blending() {
        GetImmediateContext()->OMSetBlendState(alpha_blend_disable, nullptr, 0xffffffff);
    }

    void CDispositifD3D11::disable_depth_testing() {
        GetImmediateContext()->OMSetDepthStencilState(pDepthStencilStateNoTest, 1);
    }

    void CDispositifD3D11::enable_depth_testing() {
        GetImmediateContext()->OMSetDepthStencilState(pDepthStencilState, 1);
    }

    void CDispositifD3D11::InitBlendState() {
        D3D11_BLEND_DESC blend_desc;
        ZeroMemory(&blend_desc, sizeof(D3D11_BLEND_DESC));

        blend_desc.RenderTarget[0].BlendEnable = TRUE;
        blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
        blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        DXEssayer(pD3DDevice->CreateBlendState(&blend_desc, &alpha_blend_enable));

        blend_desc.RenderTarget[0].BlendEnable = FALSE;
        DXEssayer(pD3DDevice->CreateBlendState(&blend_desc, &alpha_blend_disable));
    }

} // namespace PM3D
