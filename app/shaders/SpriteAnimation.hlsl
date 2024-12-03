cbuffer cbPerObject : register(b0)
{
    float4x4 gWorldViewProj;
    float2 gTexCoord;
    float2 gCellSize;
    bool gUseTexure;
    bool gUseTexCoord;
}; 

// Nonnumeric values cannot be added to a cbuffer.
Texture2D gDiffuseMap : register(t0);

SamplerState samplerS : register(s0);

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

    // Just pass texture coordinates into the pixel shader.
    vout.TexC = vin.TexC;

    return vout;
}
 
float4 PS(VertexOut pin) : SV_Target
{
    if (gUseTexCoord == 0)
    {
        return gDiffuseMap.Sample(samplerS, pin.TexC);
    }
    
    // Adjusting coordenates for frame
    float2 adjustedTexCoord = gTexCoord.xy + pin.TexC * gCellSize;

    // Sampling texture using adjusted coorenates
    return gDiffuseMap.Sample(samplerS, adjustedTexCoord);
}