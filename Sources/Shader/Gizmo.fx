cbuffer WorldParams {
    float4x4 matWorldViewProj;
};

struct VS_Sortie {
    float4 PosWorld : SV_Position;
    float4 Color    : COLOR0;
};

VS_Sortie gizmo_vs(float4 Pos : POSITION, float4 color : COLOR) {
	VS_Sortie sortie = (VS_Sortie)0;

	sortie.PosWorld = mul(Pos, matWorldViewProj);
    sortie.Color = color;

	return sortie;
}

float4 gizmo_ps(VS_Sortie vs) : SV_Target {
    return vs.Color;
}

technique11 Gizmo
{
	pass pass0
	{
		SetVertexShader(CompileShader(vs_5_0, gizmo_vs()));
		SetPixelShader(CompileShader(ps_5_0, gizmo_ps()));
		SetGeometryShader(NULL);
	}
}
