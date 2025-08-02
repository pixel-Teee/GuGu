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
    float4 shaderParams;
    float4 shaderParams2;
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
    return UVIndex ? UVVector.zw : UVVector.xy;
}

float4 getColor(PSInput VIn, float2 UV)
{
    return t_Texture.Sample(s_Sampler, UV) * VIn.Color;
}

float4 getBorderElementColor(PSInput VIn)
{
    float4 outColor = VIn.Color;
    float4 inTexCoords = VIn.TextureCoordinate;
    float2 newUV;
    if(inTexCoords.z == 0.0f && inTexCoords.w == 0.0f)
    {
        newUV = inTexCoords.xy;
    }
    else
    {
        float2 minUV;
        float2 maxUV;
        
        if(inTexCoords.z > 0.0f)
        {
            minUV = float2(shaderParams.x, 0.0f);
            maxUV = float2(shaderParams.y, 1.0f);
            inTexCoords.w = 1.0f;
        }
        else
        {
            minUV = float2(0.0f, shaderParams.z);
            maxUV = float2(1.0f, shaderParams.w);
            inTexCoords.z = 1.0f;
        }
        
        newUV = inTexCoords.xy * inTexCoords.xw;
        newUV = frac(newUV);
        newUV = lerp(minUV, maxUV, newUV);
    }
    
    float4 textureColor = t_Texture.Sample(s_Sampler, newUV);
    outColor *= textureColor;
    return outColor;
}

float4 getDefaultElementColor(PSInput VIn)
{
    return getColor(VIn, GetUV(VIn, 0) * GetUV(VIn, 1));
}

float4 getFontColor(PSInput VIn)
{
    float4 color = VIn.Color;
    color.a *= t_Texture.Sample(s_Sampler, GetUV(VIn, 0) * GetUV(VIn, 1));
    return color;
}

float4 getLineSegmentElementColor(PSInput VIn)
{
    float lineWidth = shaderParams.x;
    float filterWidthScale = shaderParams.y;
    float gradient = GetUV(VIn, 0).x;
    float2 gradientDerivative = float2(abs(ddx(gradient)), abs(ddy(gradient)));
    float pixelSizeInUV = sqrt(dot(gradientDerivative, gradientDerivative));
    float halfLineWidthUV = 0.5f * pixelSizeInUV * lineWidth;
    float halfFilterWidthUV = filterWidthScale * pixelSizeInUV;
    float distanceToLineCenter = abs(0.5f - gradient);
    float lineCoverage = smoothstep(halfLineWidthUV + halfFilterWidthUV, halfLineWidthUV - halfFilterWidthUV, distanceToLineCenter);
    
    if(lineCoverage <= 0.0f)
    {
        discard;
    }
    
    float4 color = VIn.Color;
    color.a *= lineCoverage;
    return color;
}

float getRoundedBoxDistance(float2 pos, float2 center, float radius, float inset)
{
    //distance from center
    pos = abs(pos - center);
    
    //distance from the inner coner
    pos = pos - (center - float2(radius + inset, radius + inset));

    return lerp(length(pos) - radius, max(pos.x - radius, pos.y - radius), float(pos.x <= 0 || pos.y <= 0));
}

float4 getRoundedBoxElementColorInternal(float2 size, float2 UV, float thickNess, float4 cornerRadii, float4 fillColor, float4 borderColor)
{
    float2 pos = size * UV;
    float2 center = size / 2.0f;
    
    //if uv <= 0.5, 0.5 return 1, else return 0 
    float2 quadrant = step(UV, float2(0.5f, 0.5f));

    //cornerRadii x = top left
    //cornerRadii y = top right
    //cornerRadii z = bottom right
    //cornerRadii w = bottom left
   
    float left = lerp(cornerRadii.y, cornerRadii.x, quadrant.x);
    float right = lerp(cornerRadii.z, cornerRadii.w, quadrant.x);
    float radius = lerp(right, left, quadrant.y);
    
    //compute the distances internal and external to the border outline
    float dext = getRoundedBoxDistance(pos, center, radius, 0.0f);
    float din = getRoundedBoxDistance(pos, center, max(radius - thickNess, 0), thickNess);
    
    //compute the border intensity and fill intensity with a smooth transition
    float bi_spread = 1.0;
    float bi = smoothstep(bi_spread, -bi_spread, dext);

    float fi_spread = .5;
    float fi = smoothstep(fi_spread, -fi_spread, din);

    float4 fill = fillColor;
    float4 border = borderColor;
    
    //alpha blend the external color 
    float4 OutColor = lerp(border, fill, float(thickNess > radius));
    OutColor.a = 0.0;

	//blend in the border and fill colors
    OutColor = lerp(OutColor, border, bi);
    OutColor = lerp(OutColor, fill, fi);
    return OutColor;
}

float4 getRoundedBoxElementColor(PSInput VIn)
{
    //first: box local size.xy
    //second: uv
    //third: 
    //fourth: corner radius xyzw
    
    return getRoundedBoxElementColorInternal(shaderParams.zw, VIn.TextureCoordinate.xy,
        shaderParams.y, //outline width height
        shaderParams2,
        getColor(VIn, GetUV(VIn, 0) * GetUV(VIn, 1)),
        VIn.SecondaryColor
    );
    
    //return float4(VIn.TextureCoordinate.x, VIn.TextureCoordinate.y, 0.0f, 1.0f);
}

float4 main_ps(PSInput VIn) : SV_Target0
{
    float4 OutColor;
#ifdef UI_Default
    OutColor = getDefaultElementColor(VIn);
#endif

#ifdef UI_Font
    OutColor = getFontColor(VIn);
#endif
    
#ifdef UI_Line
    OutColor = getLineSegmentElementColor(VIn);
#endif
    
#ifdef UI_RoundedBox
    OutColor = getRoundedBoxElementColor(VIn);
#endif
    
#ifdef UI_Border
    OutColor = getBorderElementColor(VIn);
#endif
    
    return OutColor;
}