#pragma pack_matrix(row_major)

cbuffer CB : register(b0)
{
    float4x4 g_Transform;
    float4x4 g_World;
    float3 g_camWorldPos;
};

cbuffer gridProperties : register(b1)
{
    float _planeAxis;
    float _majorGridDivisions;
    float _axisLineWidth;
    float _majorLineWidth;
    float _minorLineWidth;
    float3 pad0;
    float4 _baseColor;
    float4 _majorLineColor;
    float4 _minorLineColor;
    float4 _xAxisLineColor;
    float4 _xAxisDashColor;
    float4 _yAxisLineColor;
    float4 _yAxisDashColor;
    float4 _zAxisLineColor;
    float4 _zAxisDashColor;
    float _axisDashScale;
    float3 pad1;
    float4 _axisCenterColor;
}

void main_vs(
	float3 i_pos : POSITION,
    float2 i_uv : TEXCOORD0,
	out float4 o_pos : SV_Position,
    out float4 o_uv : TEXCOORD0
)
{
    o_pos = mul(float4(i_pos, 1), g_Transform);
    
    float div = max(2.0f, round(_majorGridDivisions));

    float3 worldPos = mul(float4(i_pos, 1), g_World).xyz;
    float3 cameraCenteringOffset = floor(g_camWorldPos / div) * div;
    o_uv.yx = (worldPos - cameraCenteringOffset).xz;
    o_uv.wz = worldPos.xz;
}

void main_ps(
	in float4 i_pos : SV_Position,
	in float4 i_uv : TEXCOORD0,
    out float4 o_color : SV_Target0
)
{
    float4 uvDDXY = float4(ddx(i_uv.xy), ddy(i_uv.xy));
    float2 uvDeriv = float2(length(uvDDXY.xz), length(uvDDXY.yw));

    float axisLineWidth = max(_majorLineWidth, _axisLineWidth);
    float2 axisDrawWidth = max(axisLineWidth, uvDeriv);
    float2 axisLineAA = uvDeriv * 1.5f;
    float2 axisLines2 = smoothstep(axisDrawWidth + axisLineAA, axisDrawWidth - axisLineAA, abs(i_uv.zw * 2.0f));
    axisLines2 *= saturate(axisLineWidth / axisDrawWidth);

    float div = max(2.0, round(_majorGridDivisions));
    float2 majorUVDeriv = uvDeriv / div;
    float majorLineWidth = _majorLineWidth / div;
    float2 majorDrawWidth = clamp(majorLineWidth, majorUVDeriv, 0.5f);
    float2 majorLineAA = majorUVDeriv * 1.5f;
    float2 majorGridUV = 1.0f - abs(frac(i_uv.xy / div) * 2.0f - 1.0f);
    float2 majorAxisOffset = (1.0f - saturate(abs(i_uv.zw / div * 2.0f))) * 2.0f;
    majorGridUV += majorAxisOffset;
    float2 majorGrid2 = smoothstep(majorDrawWidth + majorLineAA, majorDrawWidth - majorLineAA, majorGridUV);
    majorGrid2 *= saturate(majorLineWidth / majorDrawWidth);
    majorGrid2 *= saturate(majorGrid2 - axisLines2);
    majorGrid2 = lerp(majorGrid2, majorLineWidth, saturate(majorUVDeriv * 2.0f - 1.0f));

    float minorLineWidth = min(_minorLineWidth, _majorLineWidth);
    bool minorInvertLine = minorLineWidth > 0.5f;
    float minorTargetWidth = minorInvertLine ? 1.0 - minorLineWidth : minorLineWidth;
    float2 minorDrawWidth = clamp(minorTargetWidth, uvDeriv, 0.5f);
    float2 minorLineAA = uvDeriv * 1.5f;
    float2 minorGridUV = abs(frac(i_uv.xy) * 2.0f - 1.0f);
    minorGridUV = minorInvertLine ? minorGridUV : 1.0f - minorGridUV;
    float2 minorMajorOffset = (1.0 - saturate((1.0 - abs(frac(i_uv.zw / div) * 2.0 - 1.0)) * div)) * 2.0;\

    minorGridUV += minorMajorOffset; // adjust UVs so major division lines are skipped
    float2 minorGrid2 = smoothstep(minorDrawWidth + minorLineAA, minorDrawWidth - minorLineAA, minorGridUV);
    minorGrid2 *= saturate(minorTargetWidth / minorDrawWidth);
    minorGrid2 = saturate(minorGrid2 - axisLines2); // hack
    minorGrid2 = lerp(minorGrid2, minorTargetWidth, saturate(uvDeriv * 2.0 - 1.0));
    minorGrid2 = minorInvertLine ? 1.0 - minorGrid2 : minorGrid2;
    minorGrid2 = abs(i_uv.zw) > 0.5 ? minorGrid2 : 0.0;

    half minorGrid = lerp(minorGrid2.x, 1.0, minorGrid2.y);
    half majorGrid = lerp(majorGrid2.x, 1.0, majorGrid2.y);

    float2 axisDashUV = abs(frac((i_uv.zw + axisLineWidth * 0.5) * _axisDashScale) * 2.0 - 1.0) - 0.5;
    float2 axisDashDeriv = uvDeriv * _axisDashScale * 1.5;
    float2 axisDash = smoothstep(-axisDashDeriv, axisDashDeriv, axisDashUV);
    axisDash = i_uv.zw < 0.0 ? axisDash : 1.0;
    
    half4 xAxisColor = _xAxisLineColor;
    half4 yAxisColor = _yAxisLineColor;
    half4 zAxisColor = _zAxisLineColor;
    half4 xAxisDashColor = _xAxisDashColor;
    half4 yAxisDashColor = _yAxisDashColor;
    half4 zAxisDashColor = _zAxisDashColor;
    half4 centerColor = _axisCenterColor;
    half4 majorLineColor = _majorLineColor;
    half4 minorLineColor = _minorLineColor;
    
    half4 aAxisColor = yAxisColor;
    half4 bAxisColor = zAxisColor;
    half4 aAxisDashColor = yAxisDashColor;
    half4 bAxisDashColor = zAxisDashColor;
    
    aAxisColor = lerp(aAxisDashColor, aAxisColor, axisDash.y);
    bAxisColor = lerp(bAxisDashColor, bAxisColor, axisDash.x);
    aAxisColor = lerp(aAxisColor, centerColor, axisLines2.y);

    half4 axisLines = lerp(bAxisColor * axisLines2.y, aAxisColor, axisLines2.x);

    half4 col = lerp(_baseColor, minorLineColor, minorGrid * minorLineColor.a);
    col = lerp(col, majorLineColor, majorGrid * majorLineColor.a);
    col = col * (1.0 - axisLines.a) + axisLines;
    
    o_color = col;
}
