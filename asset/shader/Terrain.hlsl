#pragma pack_matrix(row_major)

cbuffer CB : register(b0)
{
    float4x4 g_Transform;
    float4x4 g_World;
    float3 g_camWorldPos;
};

cbuffer TerrainProperties : register(b1)
{
    float2 g_beginXZ;
    float2 g_offsetXZ;
    float g_heightScale;
}

Texture2D t_HeightTexture : register(t0);
Texture2D t_BlendTexture : register(t1);
Texture2D t_TerrainTexture1 : register(t2);
Texture2D t_TerrainTexture2 : register(t3);
Texture2D t_TerrainTexture3 : register(t4);
Texture2D t_TerrainTexture4 : register(t5);
SamplerState s_Sampler : register(s0);

void main_vs(
	float3 i_pos : POSITION,
	out float4 o_pos : SV_Position,
	out float2 o_uv : UV
)
{
    float x = i_pos.x + g_offsetXZ.x;
    float z = i_pos.z + g_offsetXZ.y;
    float h = -g_beginXZ.x * 2.0;
    float v = -g_beginXZ.y * 2.0;
    // o_pos = mul(mul(float4(i_pos, 1), g_World), g_Transform);
    o_uv = float2((x - g_beginXZ.x) / h, 1.0 - (z - g_beginXZ.y) / v);
    
    //height
    float4 texCoord1 = (t_HeightTexture.SampleLevel(s_Sampler, o_uv, 0) * 2.0 - 1.0);
    
    float yOffset = texCoord1.r * g_heightScale;
    o_pos = mul(mul(float4(x, yOffset, z, 1), g_World), g_Transform);
}

void main_ps(
	in float4 i_pos : SV_Position,
    in float2 i_uv : UV,
    out float4 o_color : SV_Target0
)
{
    float4 weight = t_BlendTexture.Sample(s_Sampler, i_uv).xyzw;
    float4 color1 = t_TerrainTexture1.Sample(s_Sampler, i_uv).xyzw;
    float4 color2 = t_TerrainTexture2.Sample(s_Sampler, i_uv).xyzw;
    float4 color3 = t_TerrainTexture3.Sample(s_Sampler, i_uv).xyzw;
    float4 color4 = t_TerrainTexture4.Sample(s_Sampler, i_uv).xyzw;
    float4 totalColor = color1 * weight.x + color2 * weight.y + color3 * weight.z + color4 * (1.0 - weight.x - weight.y - weight.z);
    o_color = float4(totalColor.xyz, 1.0f);
}
