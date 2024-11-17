//=============================================================================
// Basic.fx by Frank Luna (C) 2011 All Rights Reserved.
//
// Basic effect that currently supports transformations, lighting, and texturing.
//=============================================================================

#include "LightHelper.hlsl"

cbuffer cbPerObject : register(b0)
{
    float4x4 gWorld;
    float4x4 gWorldInvTranspose;
    float4x4 gWorldViewProj;
    float4x4 gTexTransform;
    Material gMaterial;
    float2 gCellSize;  // Each cell size (e.g., {1.0f / 10, 1.0f / 12})
    int gCurrentFrame; // Current frame index
    float gNumColumns; // Number of atlas columns
}; 

cbuffer cbPerFrame : register(b1)
{
    DirectionalLight gDirLights[3];
    float3 gEyePosW;

    float gFogStart;
    float gFogRange;
    float4 gFogColor;
    
    int gLightCount;
    bool gUseTexure;
};

// Nonnumeric values cannot be added to a cbuffer.
Texture2D gDiffuseMap : register(t0);

SamplerState samAnisotropic : register(s0);

struct VertexIn
{
    float3 PosL : POSITION;
    float3 NormalL : NORMAL;
    float4 Color : COLOR;
    float2 Tex : TEXCOORD;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION;
    float3 NormalW : NORMAL;
    float4 Color : COLOR;
    float2 Tex : TEXCOORD;
};

VertexOut VS(VertexIn vin)
{
    VertexOut vout;
	
    // Transform to world space space.
    vout.PosW    = mul(float4(vin.PosL, 1.0f), gWorld).xyz;
    vout.NormalW = mul(vin.NormalL, (float3x3)gWorldInvTranspose);
		
    // Transform to homogeneous clip space.
    vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);

    // Output vertex attributes for interpolation across triangle.
    vout.Tex = mul(float4(vin.Tex, 0.0f, 1.0f), gTexTransform).xy;

    // Set color
    vout.Color = vin.Color;

    return vout;
}
 
float4 PS(VertexOut pin) : SV_Target
{
    // Calcula linha e coluna do frame atual.
    int column = gCurrentFrame % int(gNumColumns);
    int row = gCurrentFrame / int(gNumColumns);

    // Offset da textura para o frame atual.
    float2 texOffset = float2(column * gCellSize.x, row * gCellSize.y);
    // return float4(column, column, column, 1.0f);

    // Coordenadas ajustadas para o frame atual.
    float2 adjustedTexCoord = texOffset + pin.Tex * gCellSize;

    // Amostra a textura.
    return gDiffuseMap.Sample(samAnisotropic, adjustedTexCoord);
}