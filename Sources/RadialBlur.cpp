#include "RadialBlur.h"

RadialBlur::RadialBlur(const wchar_t* path, PM3D::CDispositifD3D11* dispo) 
: PostFX(path, dispo) {
   // Obtenir le Device
   ID3D11Device* pD3DDevice = dispo->GetD3DDevice();

   // World Params
   D3D11_BUFFER_DESC bd;
   ZeroMemory(&bd, sizeof(bd));
   bd.Usage = D3D11_USAGE_DEFAULT;
   bd.ByteWidth = sizeof(VitesseVehicule);
   bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
   bd.CPUAccessFlags = 0;
   PM3D::DXEssayer(pD3DDevice->CreateBuffer(&bd, nullptr, &radialBlurBuffer));

   v.vitesseVehicule = 0.f; // Tmp : on doit récupérer la vrai vitesse

};

void RadialBlur::setVitesse(float vitesse) {
   v.vitesseVehicule = vitesse;

   dispositf().GetImmediateContext()->UpdateSubresource(radialBlurBuffer, 0, nullptr, &v, 0, 0);
}

void RadialBlur::configure(std::size_t , ID3DX11Effect& effect) {
    ID3DX11EffectConstantBuffer* pCB = effect.GetConstantBufferByName("RADIAL_BLUR");
    pCB->SetConstantBuffer(radialBlurBuffer);
}