#ifndef DEF_RADIAL_BLUR_H
#define DEF_RADIAL_BLUR_H

#include "PostFX.h"
#include "d3dx11effect.h"
//#include <d3d11.h>
//#include <d3dcompiler.h>
#include "dispositifD3D11.h"
#include "util.h"

struct VitesseVehicule
{
   float vitesseVehicule;	// vitesse du véhicule
   char padding[16 - sizeof(float)]; // PADDING TO GET TO 16 BITS. DONT TOUCH !!!
};

class RadialBlur : public PostFX {
    ID3D11Buffer* radialBlurBuffer;
    VitesseVehicule v;
public :
    RadialBlur(const wchar_t* path, PM3D::CDispositifD3D11* dispositif);

    void configure(std::size_t pass_index, ID3DX11Effect& effect) override;

    void setVitesse(float vitesse);
    //void apply(ID3D11ShaderResourceView* screen_texture);

};
#endif