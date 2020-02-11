Texture2D RenderedFrameTexture;
SamplerState RenderedFrameSampler;

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

float4 render_screen_ps(VS_OUTPUT In) : SV_TARGET {
    return float4(RenderedFrameTexture.Sample(RenderedFrameSampler, In.UVCoords).rgb, 1.0f);
}

technique11 DefaultScreen {
    pass pass0 {
        SetVertexShader(CompileShader(vs_5_0, render_screen_vs()));
		SetPixelShader(CompileShader(ps_5_0, render_screen_ps()));
		SetGeometryShader(NULL);
    }
}