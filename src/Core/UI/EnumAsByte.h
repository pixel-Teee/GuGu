#pragma once

namespace GuGu {
	//模板去储存枚举值作为字节在一个类型安全的方式
	template<class Enum>
	class EnumAsByte
	{
	public:
		typedef Enum EnumType;

		EnumAsByte() = default;
		EnumAsByte(const EnumAsByte&) = default;
		EnumAsByte& operator=(const EnumAsByte&) = default;

		EnumAsByte(Enum inValue)
			: m_value(static_cast<uint8_t>(inValue))
		{}

		EnumAsByte(int32_t inValue)
			: m_value(static_cast<uint8_t>(inValue))
		{}

		EnumAsByte(uint8_t inValue)
			: m_value(inValue)
		{}

	public:
		bool operator==(Enum inValue) const
		{
			return static_cast<Enum>(m_value) == inValue;
		}

		bool operator==(EnumAsByte inValue) const
		{
			return m_value == inValue.m_value;
		}

		operator Enum() const
		{
			return (Enum)m_value;
		}

		Enum getValue() const
		{
			return (Enum)m_value;
		}

	private:
		uint8_t m_value;
	};
}