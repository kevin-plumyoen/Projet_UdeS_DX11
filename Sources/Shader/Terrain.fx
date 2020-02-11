///// Gestion des lumières /////

#define MAX_POINT_LIGHTS 100
#define MAX_DIR_LIGHTS 1
#define MAX_SPOT_LIGHTS 5

cbuffer LIGHTS_COUNT
{
  int PointLightCount;
  int DirLightCount;
  int SpotLightCount;
};

struct POINT_LIGHT
{
  float4 diffuseColor; // couleur diffuse
  float4 specularColor; // couleur diffuse

  float4 pos; // position
  float range; // portée

  float intensity; // facteur d'intensité
};
struct DIR_LIGHT
{
  float4 diffuseColor; // couleur diffuse
  float4 specularColor; // couleur diffuse

  float4 dir; // direction

  float intensity; // facteur d'intensité
};
struct SPOT_LIGHT
{
  float4 diffuseColor; // couleur diffuse
  float4 specularColor; // couleur diffuse

  float4 pos; // position
  float4 dir; // direction
  float range; // portée
  float angle; // angle

  float intensity; // facteur d'intensité
};

cbuffer LIGHTS_PARAMS
{
  POINT_LIGHT point_lights[MAX_POINT_LIGHTS];
  DIR_LIGHT dir_lights[MAX_DIR_LIGHTS];
  SPOT_LIGHT spot_lights[MAX_SPOT_LIGHTS];
};

struct Light_Sortie
{
  float3 diffuse;
  float3 specular;
};

Light_Sortie processLights(float3 Normal, float3 DirCam, float3 PosWorld, float specForce, float specSpread)
{
  Light_Sortie sortie;

  int i;
  //calcul des point lights
  for (i = 0; i < PointLightCount; ++i) {
    float3 DirLum = point_lights[i].pos.xyz - PosWorld;
    float falloff = max((1 - (length(DirLum) / point_lights[i].range)), 0);
    DirLum = normalize(DirLum);

    sortie.diffuse += saturate(dot(Normal, DirLum))* point_lights[i].diffuseColor.rgb * point_lights[i].intensity * falloff;

    float3 R = normalize(2 * sortie.diffuse * Normal - DirLum);
    sortie.specular += specForce * pow(saturate(dot(R, DirCam)), specSpread) * point_lights[i].specularColor.rgb * falloff;
  }

  //calcul des dir lights
  for (i = 0; i < DirLightCount; ++i) {
    float3 DirLum = normalize(-dir_lights[i].dir.xyz);
    sortie.diffuse += saturate(dot(Normal, DirLum))* dir_lights[i].diffuseColor.rgb * dir_lights[i].intensity;

    float3 R = normalize(2 * sortie.diffuse * Normal - DirLum);
    sortie.specular += specForce * pow(saturate(dot(R, DirCam)), specSpread) * dir_lights[i].specularColor.rgb;
  }

  //calcul des spot lights
  for (i = 0; i < SpotLightCount; ++i) {
    float3 DirLum = spot_lights[i].pos.xyz - PosWorld;
    float distanceFalloff = max((1 - (length(DirLum) / spot_lights[i].range)), 0);

    DirLum = normalize(DirLum);

    float angleLum = dot(spot_lights[i].dir.xyz, -DirLum);
    angleLum = acos(angleLum / (length(spot_lights[i].dir.xyz)*length(-DirLum)));
    float directionFalloff = max((1 - (angleLum / spot_lights[i].angle)), 0);

    sortie.diffuse += saturate(dot(Normal, DirLum))* spot_lights[i].diffuseColor.rgb * spot_lights[i].intensity * distanceFalloff * directionFalloff;

    float3 R = normalize(2 * sortie.diffuse * Normal - DirLum);
    sortie.specular += specForce * pow(saturate(dot(R, DirCam)), specSpread) * spot_lights[i].specularColor.rgb * distanceFalloff * directionFalloff;
  }

  return sortie;
}

///// Gestion des matériels et des textures /////

Texture2D diffuse1;
Texture2D diffuse2;
Texture2D diffuse3;
Texture2D normal1;
Texture2D normal2;
Texture2D normal3;
Texture2D emission1;
Texture2D emission2;
Texture2D emission3;
Texture2D layout;

cbuffer TEXTURE_PRESENCE
{
  bool diffuseSet;
  bool normalSet;
  bool emissionSet;
};

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

///// Shaders ////

struct VS_Sortie
{
  float4 Pos : SV_Position;
  float3 Norm :    TEXCOORD0;
  float3 PosWorld : TEXCOORD1;
  float2 coordTex : TEXCOORD2;
  float2 coordLayout : TEXCOORD3;
};

VS_Sortie MiniPhongVS(float4 Pos : POSITION, float3 Normale : NORMAL, float2 coordTex : TEXCOORD, float2 coordLayout : LAYOUTCOORD)
{
  VS_Sortie sortie = (VS_Sortie)0;

  sortie.Pos = mul(Pos, matWorldViewProj);
  sortie.Norm = normalize(mul(float4(Normale, 0.0f), matWorld).xyz);

  // Coordonnées d'application de texture
  sortie.coordTex = coordTex;
  sortie.coordLayout = coordLayout;

  sortie.PosWorld = mul(Pos, matWorld).xyz;

  return sortie;
}

SamplerState SampleState;  // l'état de sampling

float4 MiniPhongPS(VS_Sortie vs) : SV_Target
{
  float4 couleur;

  float texFactor = layout.Sample(SampleState, vs.coordLayout).r +
                    layout.Sample(SampleState, vs.coordLayout).g +
                    layout.Sample(SampleState, vs.coordLayout).b;

  // Normaliser les paramètres
  float3 N = vs.Norm;

  if (normalSet) {
    N = (layout.Sample(SampleState, vs.coordLayout).r * normal1.Sample(SampleState, vs.coordTex).rgb +
         layout.Sample(SampleState, vs.coordLayout).g * normal2.Sample(SampleState, vs.coordTex).rgb +
         layout.Sample(SampleState, vs.coordLayout).b * normal3.Sample(SampleState, vs.coordTex).rgb) / texFactor;
  }

  //float3 L = normalize(vs.vDirLum);
  float3 V = normalize(vCamera.xyz - vs.PosWorld);

  // Calculs dépendants de la lumière
  Light_Sortie light_colors = processLights(N,V,vs.PosWorld,specForce,specSpread);

  // Échantillonner la couleur du pixel à partir de la texture
  float4 couleurTexture = float4(1.0f,1.0f,1.0f,1.0f);
  if (diffuseSet) {
    couleurTexture.rgb = (layout.Sample(SampleState, vs.coordLayout).r * diffuse1.Sample(SampleState, vs.coordTex).rgb +
                          layout.Sample(SampleState, vs.coordLayout).g * diffuse2.Sample(SampleState, vs.coordTex).rgb +
                          layout.Sample(SampleState, vs.coordLayout).b * diffuse3.Sample(SampleState, vs.coordTex).rgb) / texFactor;
  }

  float4 couleurEmission = float4(1.0f, 1.0f, 1.0f, 1.0f);
  if (emissionSet) {
    couleurEmission.rgb = (layout.Sample(SampleState, vs.coordLayout).r * emission1.Sample(SampleState, vs.coordTex).rgb +
                           layout.Sample(SampleState, vs.coordLayout).g * emission2.Sample(SampleState, vs.coordTex).rgb +
                           layout.Sample(SampleState, vs.coordLayout).b * emission3.Sample(SampleState, vs.coordTex).rgb) / texFactor;
  }

  // I = A + D * N.L + (R.V)n
  couleur = couleurTexture * vAWorld * vAMat
  + couleurTexture * float4(light_colors.diffuse, 1.0f) * vDMat
  + couleurEmission * vEMat
  + vSMat * float4(light_colors.specular, 1.0f);

  return couleur;
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