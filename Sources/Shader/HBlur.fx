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

float4 render_screen_ps(VS_OUTPUT In) : SV_TARGET{
  return RenderedFrameTexture.Sample(RenderedFrameSampler, In.UVCoords);
}

float4 blur_horizontal_ps(VS_OUTPUT In) : SV_TARGET{

  const float offset[] = { 0.0, 0.01, 0.02, 0.03, 0.04 };
const float weight[] = {
  0.2270270270, 0.1945945946, 0.1216216216,
  0.0540540541, 0.0162162162
};

float4 couleur = RenderedFrameTexture.Sample(RenderedFrameSampler, In.UVCoords);

float3 FragmentColor = float3(0.0f, 0.0f, 0.0f);

for (int i = 1; i < 5; i++) {
  FragmentColor +=
    RenderedFrameTexture.Sample(RenderedFrameSampler, In.UVCoords + float2(offset[i], 0.0f))*weight[i] +
    RenderedFrameTexture.Sample(RenderedFrameSampler, In.UVCoords - float2(offset[i], 0.0f))*weight[i];
}

return float4(FragmentColor.r,FragmentColor.g,FragmentColor.b,couleur.a);
}

technique11 DefaultScreen {
  pass pass0 {
    SetVertexShader(CompileShader(vs_5_0, render_screen_vs()));
    SetPixelShader(CompileShader(ps_5_0, blur_horizontal_ps()));
    SetGeometryShader(NULL);
  }
}