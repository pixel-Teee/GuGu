#pragma pack_matrix(row_major)

cbuffer CB : register(b0)
{
    float4x4 g_Transform;
    float4x4 g_World;
    float3 g_camWorldPos;
    float g_Time;
};

cbuffer WaterProperties : register(b1)
{
    float2 g_beginXZ;
    float2 g_offsetXZ;
    float g_heightScale;
}

Texture2D t_colorTexture1 : register(t0);
Texture2D t_dirTexture : register(t1);
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
    float2 uv_dir = float2((x - g_beginXZ.x) / h, 1.0 - (z - g_beginXZ.y) / v);
    
    float4 dirColor = t_dirTexture.SampleLevel(s_Sampler, uv_dir, 0);
    
    float u_dir = (dirColor.r - dirColor.b) * dirColor.a;
    float v_dir = (dirColor.g - (1.0 - dirColor.r - dirColor.b - dirColor.g)) * dirColor.a;
    
    float2 uv_water = float2(uv_dir.x + g_Time * u_dir, uv_dir.y + g_Time * v_dir);
    
    o_pos = mul(mul(float4(x, 0.0, z, 1), g_World), g_Transform);
    o_uv = uv_water;
}

void main_ps(
	in float4 i_pos : SV_Position,
    in float2 i_uv : UV,
    out float4 o_color : SV_Target0
)
{
    float4 color = t_colorTexture1.Sample(s_Sampler, i_uv).xyzw;
    o_color = color;
}
