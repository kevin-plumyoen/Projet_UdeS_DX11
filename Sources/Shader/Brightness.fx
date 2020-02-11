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

float4 brightness_ps(VS_OUTPUT In) : SV_TARGET{
    float brightness_scale = 1.0f;

    float3 couleur = RenderedFrameTexture.Sample(RenderedFrameSampler, In.UVCoords).rgb;
    float brightness = 0.2126f * couleur.r + 0.7152f * couleur.g + 0.0722f * couleur.b;

    return float4(couleur * brightness * brightness_scale, 1.0f);
}

float4 blur_horizontal_ps(VS_OUTPUT In) : SV_TARGET{
    
    float factor = 17.5f;
    const float diff = 0.0015;

    const float weight[] = {
      0.009595, 0.00874, 0.007482, 0.006019, 0.00455, 0.003232, 0.002158, 0.001354, 0.000798, 0.000442, 0.00023, 0.000113, 0.000052, 0.000022, 0.000009
    };

    float4 couleur = RenderedFrameTexture.Sample(RenderedFrameSampler, In.UVCoords);

    float3 FragmentColor = couleur.rgb * 0.009898 * factor;

    for (int i = 1; i < 15; i++) {
        FragmentColor +=
        RenderedFrameTexture.Sample(RenderedFrameSampler, In.UVCoords + float2(i * diff, 0.0f)).rgb * factor * weight[i] +
        RenderedFrameTexture.Sample(RenderedFrameSampler, In.UVCoords - float2(i * diff, 0.0f)).rgb * factor *weight[i];
    }

    return float4(FragmentColor.r, FragmentColor.g, FragmentColor.b, couleur.a);
}

float4 blur_vertical_ps(VS_OUTPUT In) : SV_TARGET{

  float factor = 17.5f;
  const float diff = 0.001;

  const float weight[] = {
    0.009595, 0.00874, 0.007482, 0.006019, 0.00455, 0.003232, 0.002158, 0.001354, 0.000798, 0.000442, 0.00023, 0.000113, 0.000052, 0.000022, 0.000009
  };

    float4 couleur = RenderedFrameTexture.Sample(RenderedFrameSampler, In.UVCoords);

    float3 FragmentColor = couleur.rgb * 0.009898 * factor;

    for (int i = 1; i < 15; i++) {
        FragmentColor +=
        RenderedFrameTexture.Sample(RenderedFrameSampler, In.UVCoords + float2(0.0f, i * diff)).rgb * factor * weight[i] +
        RenderedFrameTexture.Sample(RenderedFrameSampler, In.UVCoords - float2(0.0f, i * diff)).rgb * factor * weight[i];
    }

    return float4(FragmentColor.r, FragmentColor.g, FragmentColor.b, couleur.a);
}


technique11 DefaultScreen {
    pass pass0 {
        SetVertexShader(CompileShader(vs_5_0, render_screen_vs()));
        SetPixelShader(CompileShader(ps_5_0, brightness_ps()));
        SetGeometryShader(NULL);
    }

    pass vblur {
        SetVertexShader(CompileShader(vs_5_0, render_screen_vs()));
        SetPixelShader(CompileShader(ps_5_0, blur_vertical_ps()));
        SetGeometryShader(NULL);
    }

    pass hblur {
        SetVertexShader(CompileShader(vs_5_0, render_screen_vs()));
        SetPixelShader(CompileShader(ps_5_0, blur_horizontal_ps()));
        SetGeometryShader(NULL);
    }
}