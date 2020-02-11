///// Gestion des matériels et des textures /////

Texture2D diffuse;

cbuffer MATERIAL_PARAMS
{
  float4 vAMat; 			// couleur ambiante  
  float4 vDMat; 			// couleur diffuse  
  float4 vSMat; 			// couleur speculaire
  float4 vEMat; 			// couleur emissive 

  float specForce; 			// force de la specularite 
  float specSpread; 			// etendu de la specularite 
};

///// Constantes du monde /////

cbuffer WORLD_PARAMS
{
  float4x4 matWorldViewProj;   // la matrice totale 
  float4x4 matWorld;		// matrice de transformation dans le monde 

  float4 vCamera; 			// la position de la caméra

  float4 vAWorld; 			// couleur lumière ambiante
}

struct VS_Sortie
{
  float4 Pos : SV_Position;
  float3 Norm :    TEXCOORD0;
  float2 coordTex : TEXCOORD3;
  float3 PosWorld : TEXCOORD5;
};

VS_Sortie SkyboxVS(float4 Pos : POSITION, float3 Normale : NORMAL, float2 coordTex : TEXCOORD)
{
  VS_Sortie sortie = (VS_Sortie)0;

  sortie.Pos = mul(Pos, matWorldViewProj);
  sortie.Norm = normalize(mul(float4(Normale, 0.0f), matWorld).xyz);

  // Coordonnées d'application de texture
  sortie.coordTex = coordTex;

  sortie.PosWorld = mul(Pos, matWorld).xyz;

  return sortie;
}

SamplerState SampleState;  // l'état de sampling

float4 SkyboxPS(VS_Sortie vs) : SV_Target
{
  float3 couleur;

// Normaliser les paramètres
float3 N = vs.Norm;

//float3 L = normalize(vs.vDirLum);
float3 V = normalize(vCamera.xyz - vs.PosWorld);

// Échantillonner la couleur du pixel à partir de la texture
float3 couleurTexture = diffuse.Sample(SampleState, vs.coordTex).rgb;


// I = A + D * N.L + (R.V)n
couleur = couleurTexture * vEMat;

return float4(couleur, 1.0f);
}

technique11 MiniPhong
{
  pass pass0
  {
    SetVertexShader(CompileShader(vs_5_0, SkyboxVS()));
    SetPixelShader(CompileShader(ps_5_0, SkyboxPS()));
    SetGeometryShader(NULL);
  }
}