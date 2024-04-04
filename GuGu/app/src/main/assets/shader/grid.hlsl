#pragma pack_matrix(row_major)

cbuffer CB : register(b0)
{
    float4x4 g_Transform;
    float4x4 g_World;
};

void main_vs(
	float3 i_pos : POSITION,
    float3 i_color : COLOR0,
	out float4 o_pos : SV_Position,
    out float3 o_color : COLOR0
)
{
    o_pos = mul(float4(i_pos, 1), g_Transform);
    o_color = i_color;
}

void main_ps(
	in float4 i_pos : SV_Position,
	in float3 i_color : COLOR0,
    out float4 o_color : SV_Target0
)
{
    o_color = float4(i_color, 1.0f);
    //o_color = float4(0.8f, 0.3f, 0.2f, 1.0f);
}
