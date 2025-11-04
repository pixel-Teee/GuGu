#pragma pack_matrix(row_major)

cbuffer CB : register(b0)
{
    float4x4 g_Transform;
    float4x4 g_World;
    float3 g_camWorldPos;
    float g_Time;
};

TextureCube t_EnvironmentMap : register(t0);
SamplerState s_Sampler : register(s0);

void main_vs(
	float3 i_pos : POSITION,
	float3 i_normal : NORMAL,
    float3 i_tangent : TANGENT,
    float2 i_texCoord : UV,
    out float3 o_texCoord : UV1,
    out float4 o_pos : SV_Position
)
{
    o_texCoord = i_pos;
    o_pos = mul(mul(float4(i_pos, 1), g_World), g_Transform);
    o_pos.z = o_pos.w;
}

void main_ps(
	in float4 i_pos : SV_Position,
    in float3 i_uv : UV1,
    out float4 o_color : SV_Target0
)
{
    float4 color = t_EnvironmentMap.Sample(s_Sampler, i_uv);
    o_color = color;
}
