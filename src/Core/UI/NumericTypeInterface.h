#pragma once

#include <Core/UI/Attribute.h> //Attribute
#include <Core/GuGuUtf8Str.h>

#include <string>
#include <cerrno>

namespace GuGu {
	template<typename NumericType>
	struct INumericTypeInterface
	{
		virtual ~INumericTypeInterface() {}

		virtual int32_t getMinFractionalDigits() const = 0;
		virtual int32_t getMaxFractionalDigits() const = 0;

		virtual void setMinFractionalDigits(const Attribute<std::optional<int32_t>>& newValue) = 0;
		virtual void setMaxFractionalDigits(const Attribute<std::optional<int32_t>>& newValue) = 0;

		virtual GuGuUtf8Str toString(const NumericType& value) const = 0;
		virtual std::optional<NumericType> fromString(const GuGuUtf8Str& inString, const NumericType& existingValue) = 0;

		/*检查输入的字符是否有效*/
		virtual bool isCharacterValid(GuGuUtf8Str inChar) const = 0;
	};

	template<typename NumericType>
	struct DefaultNumericTypeInterface : INumericTypeInterface<NumericType>
	{
		//最小的小数位数
		static const int32_t m_defaultMinFractionalDigits = 1;

		//最大的小数位数
		static const int32_t m_defaultMaxFractionalDigits = 6;

		//现在最小的小数位数
		int32_t minFractionalDigits = m_defaultMaxFractionalDigits;

		//现在最大的小数位数
		int32_t maxFractionalDigits = m_defaultMaxFractionalDigits;

		virtual int32_t getMinFractionalDigits() const override
		{
			return minFractionalDigits;
		}
		virtual int32_t getMaxFractionalDigits() const override
		{
			return maxFractionalDigits;
		}
		virtual void setMinFractionalDigits(const Attribute<std::optional<int32_t>>& newValue) override
		{
			minFractionalDigits = (newValue.Get().has_value()) ? std::max(0, newValue.Get().value()) : m_defaultMinFractionalDigits;
		}
		virtual void setMaxFractionalDigits(const Attribute<std::optional<int32_t>>& newValue) override
		{
			maxFractionalDigits = (newValue.Get().has_value()) ? std::max(0, newValue.Get().value()) : m_defaultMaxFractionalDigits;
		}

		virtual GuGuUtf8Str toString(const NumericType& value) const override
		{
			//暂时只处理小数
			double newValue = value;
			std::string strNum = std::to_string(newValue);
			int32_t pos = strNum.find(".");
			std::string decimals = strNum.substr(pos + 1);
			int32_t count = decimals.length();

			if (count <= minFractionalDigits)
			{
				GuGuUtf8Str formatStr = GuGuUtf8Str("%.*f");
				return strFormat(formatStr, minFractionalDigits,newValue);
			}
			else if (count >= maxFractionalDigits)
			{
				GuGuUtf8Str formatStr = GuGuUtf8Str("%.*f");
				return strFormat(formatStr, maxFractionalDigits, newValue);
			}
			else
			{
				return strFormat("%f", newValue);
			}
		}

		virtual std::optional<NumericType> fromString(const GuGuUtf8Str& inString, const NumericType& existingValue) override
		{
			errno = 0;
			//existing value是默认值，如果inString无法转换为数字，那么就使用existing value
			double value = std::strtod(inString.getStr(), nullptr);

			if (errno == 0)
			{
				std::string strNum = std::to_string(value);
				int32_t pos = strNum.find(".");
				std::string decimals = strNum.substr(pos + 1);
				int32_t count = decimals.length();

				if (count <= minFractionalDigits)
				{
					GuGuUtf8Str formatStr = GuGuUtf8Str("%.*f");
					return std::strtod(strFormat(formatStr, minFractionalDigits, value).getStr(), nullptr);
				}
				else if (count >= maxFractionalDigits)
				{
					GuGuUtf8Str formatStr = GuGuUtf8Str("%.*f");
					return std::strtod(strFormat(formatStr, maxFractionalDigits, value).getStr(), nullptr);
				}
				else
				{
					GuGuUtf8Str formatStr = GuGuUtf8Str("%f");
					return std::strtod(strFormat(formatStr, value).getStr(), nullptr);
				}

			}
			else
			{
				std::string strNum = std::to_string(existingValue);
				int32_t pos = strNum.find(".");
				std::string decimals = strNum.substr(pos + 1);
				int32_t count = decimals.length();

				if (count <= minFractionalDigits)
				{
					GuGuUtf8Str formatStr = GuGuUtf8Str("%.*f");
					return std::strtod(strFormat(formatStr, minFractionalDigits, existingValue).getStr(), nullptr);
				}
				else if (count >= maxFractionalDigits)
				{
					GuGuUtf8Str formatStr = GuGuUtf8Str("%.*f");
					return std::strtod(strFormat(formatStr, maxFractionalDigits, existingValue).getStr(), nullptr);
				}
				else
				{
					GuGuUtf8Str formatStr = GuGuUtf8Str("%f");
					return std::strtod(strFormat(formatStr, existingValue).getStr(), nullptr);
				}
			}
		}

		virtual bool isCharacterValid(GuGuUtf8Str inChar) const override
		{
			static const GuGuUtf8Str validChars = u8"1234567890()-+=\\/.,*^%%";

			return inChar != GuGuUtf8Str("\0") && validChars.find(inChar.getStr()) != -1;
		}
	};
}