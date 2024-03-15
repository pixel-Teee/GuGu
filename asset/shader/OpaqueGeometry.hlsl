#pragma pack_matrix(row_major)

cbuffer CB : register(b0)
{
    float4x4 g_Transform;
};
#ifdef SKINNED
cbuffer cbSkinned : register(b1)
{
    float4x4 g_BoneTransform[96];
};
#endif

void main_vs(
	float3 i_pos : POSITION,
    float2 i_uv : UV,
    float3 i_normal : NORMAL,
    float3 i_tangent : TANGENT,
#ifdef SKINNED
    float4 i_BoneWeights : WEIGHTS,
    uint4 i_BoneIndices : BONEINDICES,
#endif
	out float4 o_pos : SV_Position,
	out float2 o_uv : UV,
    out float3 o_normal : NORMAL,
    out float3 o_tangent : TANGENT
)
{
#ifdef SKINNED
    float weights[4] = {i_BoneWeights.x, i_BoneWeights.y, i_BoneWeights.z, i_BoneWeights.w};
    //float weights[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    
    float3 posL = float3(0.0f, 0.0f, 0.0f);
    for(int i = 0; i < 4; ++i)
    {
        if(weights[i] > 0)
            posL += weights[i] * mul(float4(i_pos, 1), g_BoneTransform[i_BoneIndices[i]]);
    }
    i_pos = posL;
    //o_pos = i_pos;
#endif
    o_pos = mul(float4(i_pos, 1), g_Transform);
    o_uv = i_uv;
    o_normal = mul(float4(i_normal, 0), g_Transform).xyz;
    o_tangent = mul(float4(i_tangent, 0), g_Transform).xyz;
}


Texture2D t_Texture : register(t0);
SamplerState s_Sampler : register(s0);

void main_ps(
	in float4 i_pos : SV_Position,
	in float2 i_uv : UV,
    in float3 i_normal : NORMAL,
    in float3 i_tangent : TANGENT,
	out float4 o_color : SV_Target0
)
{
    o_color = t_Texture.Sample(s_Sampler, i_uv);
}