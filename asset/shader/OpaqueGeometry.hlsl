#pragma pack_matrix(row_major)

cbuffer CB : register(b0)
{
    float4x4 g_Transform;
    float4x4 g_World;
};
#ifdef SKINNED
cbuffer cbSkinned : register(b1)
{
    float4x4 g_BoneTransform[96];
};
#endif
cbuffer cbPbr : register(b2)
{
    float3 albedo;
    float metallic;
    float roughness;
    float ao;
};

cbuffer cbLight : register(b3)
{
    float4 lightPositions[4];
    float4 lightColors[4];
};

cbuffer cbPass : register(b4)
{
    float3 camPos;
};

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
    out float3 o_posWS : POSITIONT0,
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
    o_posWS = mul(float4(i_pos, 1), g_World).xyz;
    o_uv = i_uv;
    o_normal = mul(float4(i_normal, 0), g_World).xyz;
    o_tangent = mul(float4(i_tangent, 0), g_World).xyz;
}


#define PI 3.14159265359

Texture2D t_Texture : register(t0);
SamplerState s_Sampler : register(s0);

float DistributionGGX(float3 N, float3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(float3 N, float3 V, float3 L, float roughness);
float3 fresnelSchlick(float cosTheta, float3 F0);

void main_ps(
	in float4 i_pos : SV_Position,
    in float3 i_posWS : POSITIONT0,
	in float2 i_uv : UV,
    in float3 i_normal : NORMAL,
    in float3 i_tangent : TANGENT,
	out float4 o_color : SV_Target0
)
{
    
    float3 Albedo = t_Texture.Sample(s_Sampler, i_uv).xyz * albedo;
    //o_color = t_Texture.Sample(s_Sampler, i_uv);
    
    float3 N = normalize(i_normal);
    float3 V = normalize(camPos - i_posWS);

    float3 F0 = float3(0.04f, 0.04f, 0.04f);
    F0 = lerp(F0, Albedo, metallic);
    
    float3 Lo = float3(0.0f, 0.0f, 0.0f);
    
    for (int i = 0; i < 4; ++i)
    {
        float3 L = normalize(lightPositions[i].xyz - i_posWS);
        float3 H = normalize(V + L);
        float distance = length(lightPositions[i].xyz - i_posWS);
        float attenuation = 1.0 / (distance * distance);
        float3 radiance = lightColors[i].xyz * attenuation;
        
        //cook-torrance brdf
        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        float3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

        float3 kS = F;
        float3 kD = float3(1.0, 1.0, 1.0) - kS;
        kD *= 1.0 - metallic;
        
        float3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        float3 specular = numerator / denominator;
        
        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * Albedo / PI + specular) * radiance * NdotL;
    }
    
    float3 ambient = float3(0.03, 0.03, 0.03) * Albedo * ao;
    float3 color = ambient + Lo;
	
    color = color / (color + float3(1.0, 1.0, 1.0));
    color = pow(color, float3(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2));
   
    o_color = float4(color, 1.0);
}

float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
float3 fresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (float3(1.0, 1.0, 1.0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
// ----------------------------------------------------------------------------