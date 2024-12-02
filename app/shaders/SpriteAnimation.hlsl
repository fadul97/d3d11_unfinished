cbuffer cbPerObject : register(b0)
{
    float4x4 gWorldViewProj;
    float2 gCellSize; // Each cell size (e.g., {1.0f / 10, 1.0f / 12})
    int gCurrentFrame; // Current frame index
    float gNumColumns; // Number of atlas columns
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
    float2 TexC : TEXCOORD;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float4 Color : COLOR;
    float2 TexC : TEXCOORD;
};

VertexOut VS(VertexIn vin)
{
    VertexOut vout;
	
    // Transform to homogeneous clip space.
    vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
	
	// Just pass vertex color into the pixel shader.
    vout.Color = vin.Color;

    vout.TexC = vin.TexC;

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
    float2 adjustedTexCoord = texOffset + pin.TexC * gCellSize;

    // Amostra a textura.
    return gDiffuseMap.Sample(samAnisotropic, adjustedTexCoord);
}