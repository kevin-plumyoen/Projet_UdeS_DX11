
#pragma once

#include "dispositif.h"

#include <d3d11.h>

namespace PM3D
{

//
//  Classe CDispositifD3D11
//
//  BUT : 	Classe permettant d'implanter un dispositif de rendu 
// 			Direct3D
//
class CDispositifD3D11 final : public CDispositif
{
public:
	CDispositifD3D11(const CDS_MODE cdsMode, const HWND hWnd);

    // Copy is disabled
    CDispositifD3D11(const CDispositifD3D11&) = delete;
    CDispositifD3D11& operator=(const CDispositifD3D11&) = delete;
	virtual ~CDispositifD3D11();

	virtual void PresentSpecific() override;

	// Fonction d'accès aux membres protégés
	ID3D11Device*           GetD3DDevice() { return pD3DDevice; }
	ID3D11DeviceContext*    GetImmediateContext() { return pImmediateContext; }
	IDXGISwapChain*         GetSwapChain() { return pSwapChain; }
	ID3D11RenderTargetView* GetRenderTargetView() { return pRenderTargetView; }
	ID3D11DepthStencilView* GetDepthStencilView() { return pDepthStencilView; }
    ID3D11DepthStencilState* GetDepthStencilState() { return pDepthStencilState; }
    ID3D11DepthStencilState* GetDepthStencilStateNoTest() { return pDepthStencilStateNoTest; }

    void SetRenderTargetView(ID3D11RenderTargetView* target, ID3D11DepthStencilView* depth);
    void ResetTargetView();

    void enable_alpha_blending();
    void disable_alpha_blending();
    void disable_depth_testing();
    void enable_depth_testing();

private:
	void InitDepthBuffer();
    void InitBlendState();

	ID3D11Device*           pD3DDevice;
	ID3D11DeviceContext*    pImmediateContext;
	IDXGISwapChain*         pSwapChain;
	ID3D11RenderTargetView* pRenderTargetView;

    ID3D11BlendState* alpha_blend_enable;
    ID3D11BlendState* alpha_blend_disable;

	// Pour le tampon de profondeur
	ID3D11Texture2D* pDepthTexture;
	ID3D11DepthStencilView* pDepthStencilView;
  ID3D11DepthStencilState* pDepthStencilState;
  ID3D11DepthStencilState* pDepthStencilStateNoTest;

	// Variables d'état
	ID3D11RasterizerState* mSolidCullBackRS;
};

} // namespace PM3D
