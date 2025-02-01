#pragma pack_matrix(row_major)

cbuffer CB : register(b0)
{
    float4x4 g_Transform;
    float4x4 g_World;
    float3 g_camWorldPos;
};

cbuffer GizmosProperties : register(b1)
{
    float3 g_color;
}

void main_vs(
	float3 i_pos : POSITION,
    float2 i_uv : UV,
    float3 i_normal : NORMAL,
    float3 i_tangent : TANGENT,
	out float4 o_pos : SV_Position,
    out float3 o_posWS : POSITIONT0,
	out float2 o_uv : UV,
    out float3 o_normal : NORMAL,
    out float3 o_tangent : TANGENT
)
{
    o_pos = mul(mul(float4(i_pos, 1), g_World), g_Transform);
    o_posWS = mul(float4(i_pos, 1), g_World).xyz;
    o_uv = i_uv;
    o_normal = mul(float4(i_normal, 0), g_World).xyz;
    o_tangent = mul(float4(i_tangent, 0), g_World).xyz;
    o_uv = i_uv;
}

void main_ps(
	in float4 i_pos : SV_Position,
    in float3 i_posWS : POSITIONT0,
	in float2 i_uv : UV,
    in float3 i_normal : NORMAL,
    in float3 i_tangent : TANGENT,
    out float4 o_color : SV_Target0
)
{
    o_color = float4(g_color, 1.0f);
}
