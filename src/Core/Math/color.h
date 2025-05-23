#pragma once

namespace GuGu {

	namespace math {
		namespace colors
		{
			constexpr float3 black = float3(0.f);
			constexpr float3 white = float3(1.f);
			constexpr float3 red = float3(1.f, 0.f, 0.f);
			constexpr float3 green = float3(0.f, 1.f, 0.f);
			constexpr float3 blue = float3(0.f, 0.f, 1.f);
		}

		// Rec. 709 luma coefficients for linear float3 space
		static const float3 lumaCoefficients = { 0.2126f, 0.7152f, 0.0722f };
		inline float luminance(const float3& c)
		{
			return dot(c, lumaCoefficients);
		}
		inline float luminance(const float4& c)
		{
			return dot(float3(c), lumaCoefficients);
		}

		// Composition operator for linear RGB space (premultiplied alpha)
		inline float4 over_premul(const float4& a, const float4& b)
		{
			return float4(float3(a) + (1.0f - a.w) * float3(b), 1.0f - ((1.0f - a.w) * (1.0f - b.w)));
		}

		// Composition operator for linear RGB space (non-premultiplied alpha)
		inline float4 over_nonpremul(const float4& a, const float4& b)
		{
			return float4(lerp(float3(b), float3(a), a.w), 1.0f - (1.0f - a.w) * (1.0f - b.w));
		}
		inline float3 over_nonpremul(const float4& a, const float3& b)
		{
			return lerp(b, float3(a), a.w);
		}



		// SRGB/linear color space conversions
		inline float toLinear(float c)
		{
			return (c <= 0.04045f) ? c / 12.92f : ::pow((c + 0.055f) / 1.055f, 2.4f);
		}
		inline float toSRGB(float c)
		{
			return (c <= 0.0031308f) ? c * 12.92f : 1.055f * ::pow(c, 1.0f / 2.4f) - 0.055f;
		}
		inline float3 toLinear(const float3& c)
		{
			return select(c <= 0.04045f, c / 12.92f, pow((c + 0.055f) / 1.055f, 2.4f));
		}
		inline float3 toSRGB(const float3& c)
		{
			return select(c <= 0.0031308f, c * 12.92f, 1.055f * pow(c, 1.0f / 2.4f) - 0.055f);
		}
		inline float4 toLinear(const float4& c)
		{
			return float4(toLinear(float3(c)), c.w);
		}
		inline float4 toSRGB(const float4& c)
		{
			return float4(toSRGB(float3(c)), c.w);
		}

		// float3/HSV conversions
		float3 RGBtoHSV(const float3& c);
		float3 HSVtoRGB(const float3& c);
		inline float4 RGBtoHSV(const float4& c)
		{
			return float4(RGBtoHSV(float3(c)), c.w);
		}
		inline float4 HSVtoRGB(const float4& c)
		{
			return float4(HSVtoRGB(float3(c)), c.w);
		}

		// float3/YCoCg conversions
		inline float3 RGBtoYCoCg(const float3& c)
		{
			return float3(0.25f * (c.x + 2.0f * c.y + c.z), c.x - c.z, c.y - 0.5f * (c.x + c.z));
		}
		inline float3 YCoCgtoRGB(const float3& c)
		{
			return float3(c.x + 0.5f * (c.y - c.z), c.x + 0.5f * c.z, c.x - 0.5f * (c.y + c.z));
		}
		inline float4 RGBtoYCoCg(const float4& c)
		{
			return float4(RGBtoYCoCg(float3(c)), c.w);
		}
		inline float4 YCoCgtoRGB(const float4& c)
		{
			return float4(YCoCgtoRGB(float3(c)), c.w);
		}

		// float3/CIELAB conversions
		float3 RGBtoCIELAB(const float3& c);
		float3 CIELABtoRGB(float3 c);
		inline float4 RGBtoCIELAB(const float4& c)
		{
			return float4(RGBtoCIELAB(float3(c)), c.w);
		}
		inline float4 CIELABtoRGB(const float4& c)
		{
			return float4(CIELABtoRGB(float3(c)), c.w);
		}
	}
}