#pragma pack_matrix(row_major)

struct PSInput
{
    float4 Position : SV_Position;
    float4 Color : COLOR0;
    float4 SecondaryColor : COLOR1;
    float4 TextureCoordinate : TEXCOORD0;
};

cbuffer CB : register(b0)
{
    float4x4 ViewProjection;
};

PSInput main_vs(float4 InTextureCoordinate : TEXCOORD0, float2 InPosition : POSITIONT0,
float4 InColor : COLOR0, float4 InSecondaryColor : COLOR1)
{
    PSInput VOut = (PSInput)0;
    
    float4 WorldPosition = float4(InPosition.xy, 0, 1);
    
    VOut.Color = InColor;
    VOut.SecondaryColor = InSecondaryColor;
    VOut.TextureCoordinate = InTextureCoordinate;
    
    VOut.Position = mul(WorldPosition, ViewProjection);
    
    return VOut;
}

Texture2D t_Texture : register(t0);
SamplerState s_Sampler : register(s0);

float2 GetUV(PSInput VIn, uint UVIndex)
{
    float4 UVVector = VIn.TextureCoordinate;
    return UVIndex ? UVVector.xy : UVVector.zw;
}

float4 getColor(PSInput VIn, float2 UV)
{
    return t_Texture.Sample(s_Sampler, UV);
}

float4 getDefaultElementColor(PSInput VIn)
{
    return getColor(VIn, GetUV(VIn, 0) * GetUV(VIn, 1));
}

float4 main_ps(PSInput VIn) : SV_Target0
{
    float4 OutColor;
    OutColor = getDefaultElementColor(VIn) * VIn.Color;
    
    return OutColor;
}