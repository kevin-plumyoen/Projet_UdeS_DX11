Texture2D RenderedFrameTexture;
SamplerState RenderedFrameSampler;

cbuffer RADIAL_BLUR
{
   float vitesseVehicule;
};

struct VS_OUTPUT {
    float2 UVCoords : TEXCOORD0;
    float4 Position : SV_POSITION;
};

VS_OUTPUT render_screen_vs(float4 vPos : POSITION, float2 uv : TEXCOORD) {
    VS_OUTPUT output;
    
    output.UVCoords = uv;
    output.Position = vPos;

    return output;
}

float4 radial_blur_ps(VS_OUTPUT In) : SV_TARGET{
    float4 couleur;
    float4 ct;
    float2 tc = In.UVCoords;
    float d, dx, dy;

    couleur = 0;
    float x = tc.x * 2 - 1.0;
    float y = tc.y * 2 - 1.0;
    dx = sqrt(x*x); // Distance du centre
    dy = sqrt(y*y); // Distance du centre

    int nb_iteration = 100;
    // le dégradé (blur) est en fonction de la distance du centre et de la variable distance
    float distance = 0.0f;
    if (sqrt(dx * dx + dy * dy) > 0.5f) {
       distance = min(((exp(sqrt(dx * dx + dy * dy) - 0.5f) - 1.0f) / (exp(sqrt(2) - 0.5f) - 1.0f))*0.001f*vitesseVehicule, 0.1f); // Plus on est éloigné du centre, plus le blur est fort
    } 
    dx = x * (distance*dx);
    dy = y * (distance*dy);

    x = x - (dx * nb_iteration);
    y = y - (dy * nb_iteration);

    tc.x = (x + 1.0) / 2.0;
    tc.y = (y + 1.0) / 2.0;

    // On peut jouer avec les itérations
    for (int i = 0; i < nb_iteration; ++i) {
        ct = float4(RenderedFrameTexture.Sample(RenderedFrameSampler, tc).rgb, 1.0f);
        couleur = couleur * 0.6 + ct * 0.4; // On peut jouer avec les pourcentages

        x = x + dx;
        y = y + dy;

        tc.x = (x + 1.0) / 2.0;
        tc.y = (y + 1.0) / 2.0;
    }
    return couleur;
}

technique11 DefaultScreen {
    pass pass0 {
        SetVertexShader(CompileShader(vs_5_0, render_screen_vs()));
        SetPixelShader(CompileShader(ps_5_0, radial_blur_ps()));
		SetGeometryShader(NULL);
    }
}