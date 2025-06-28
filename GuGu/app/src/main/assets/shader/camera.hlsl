#pragma pack_matrix(row_major)

cbuffer CB : register(b0)
{
    float4x4 g_Transform;
    float4x4 g_World;
    float3 g_camWorldPos;
};

cbuffer CameraProperties : register(b1)
{
    float3 g_color;
}

void main_vs(
	float3 i_pos : POSITION,
	out float4 o_pos : SV_Position
)
{
    o_pos = mul(mul(float4(i_pos, 1), g_World), g_Transform);
}

void main_ps(
	in float4 i_pos : SV_Position,
    out float4 o_color : SV_Target0
)
{
    o_color = float4(g_color, 1.0f);
}
