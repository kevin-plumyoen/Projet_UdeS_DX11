cbuffer param
{
	float4x4 matWorldViewProj;   // la matrice totale 
	float4x4 matWorld;		// matrice de transformation dans le monde 
	float4 vLumiere; 		// la position de la source d'�clairage (Point)
	float4 vCamera; 			// la position de la cam�ra
	float4 vAEcl; 			// la valeur ambiante de l'�clairage
	float4 vAMat; 			// la valeur ambiante du mat�riau
	float4 vDEcl; 			// la valeur diffuse de l'�clairage 
	float4 vDMat; 			// la valeur diffuse du mat�riau 
}

struct VS_Sortie
{
	float4 Pos : SV_Position;
	float3 Norm :    TEXCOORD0;
	float3 vDirLum : TEXCOORD1;
	float3 vDirCam : TEXCOORD2;
	float2 coordTex : TEXCOORD3;
	float2 coordLayout : TEXCOORD4;
};

VS_Sortie MiniPhongVS(float4 Pos : POSITION, float3 Normale : NORMAL, float2 coordTex : TEXCOORD, float2 coordLayout : LAYOUTCOORD)
{
	VS_Sortie sortie = (VS_Sortie)0;

	sortie.Pos = mul(Pos, matWorldViewProj);
	sortie.Norm = mul(float4(Normale, 0.0f), matWorld).xyz;

	float3 PosWorld = mul(Pos, matWorld).xyz;

	sortie.vDirLum = vLumiere.xyz - PosWorld;
	sortie.vDirCam = vCamera.xyz - PosWorld;

	// Coordonn�es d'application de texture
	sortie.coordTex = coordTex;
	sortie.coordLayout = coordLayout;

	return sortie;
}

SamplerState SampleState;  // l'�tat de sampling

float specularIntensity = 0.3f;
float specularDiffusion = 8.0f;

float4 MiniPhongPS(VS_Sortie vs) : SV_Target
{
	float3 couleur;

	// Normaliser les param�tres
	//float3 N = normalize(vs.Norm);
	// Puissance de 4 - pour l'exemple

	// �chantillonner la couleur du pixel � partir de la texture

	// I = A + D * N.L + (R.V)n
	couleur = float3(1.0f,0.f,0.f);
	
	return float4(couleur, 1.0f);
}

technique11 MiniPhong
{
	pass pass0
	{
		SetVertexShader(CompileShader(vs_5_0, MiniPhongVS()));
		SetPixelShader(CompileShader(ps_5_0, MiniPhongPS()));
		SetGeometryShader(NULL);
	}
}
