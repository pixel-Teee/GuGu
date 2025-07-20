#pragma pack_matrix(row_major)

cbuffer CB : register(b0)
{
    float4x4 g_Transform;
    float4x4 g_World;
    float3 g_camWorldPos;
};

//cbuffer UIProperties : register(b1)
//{
//    float3 g_color;
//}

Texture2D t_uiTexture : register(t0);
SamplerState s_Sampler : register(s0);

void main_vs(
    float2 i_uv : TEXCOORD0,
	float3 i_pos : POSITION,
    float4 i_color : COLOR0,
	out float4 o_pos : SV_Position,
    out float2 o_uv : UV
)
{
    o_uv = i_uv;
    o_pos = mul(mul(float4(i_pos, 1), g_World), g_Transform);
}

void main_ps(
	in float4 i_pos : SV_Position,
    out float4 o_color : SV_Target0,
    in float2 i_uv : UV
)
{
    float4 color = t_uiTexture.Sample(s_Sampler, i_uv).xyzw;
    o_color = color;
}
