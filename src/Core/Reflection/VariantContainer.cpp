#include <pch.h>

#include "VariantContainer.h"

#include <algorithm>

namespace GuGu {
	namespace meta {
		#pragma region void

		VariantContainer<void>::VariantContainer(void) {}

		Type VariantContainer<void>::GetType(void) const
		{
			return typeof(void);
		}

		void* VariantContainer<void>::GetPtr() const
		{
			return nullptr;
		}

		int VariantContainer<void>::ToInt(void) const
		{
			return int();
		}

		bool VariantContainer<void>::ToBool(void) const
		{
			return bool();
		}

		float VariantContainer<void>::ToFloat(void) const
		{
			return float();
		}

		double VariantContainer<void>::ToDouble(void) const
		{
			return double();
		}

		GuGuUtf8Str VariantContainer<void>::ToString(void) const
		{
			return std::string();
		}

		VariantBase* VariantContainer<void>::Clone(void) const
		{
			return new VariantContainer<void>();
		}

		#pragma endregion

		#pragma region int
		VariantContainer<int>::VariantContainer(const int& value)
			: m_value(value) { }

		VariantContainer<int>::VariantContainer(const int&& value)
			: m_value(std::move(value)) { }

		Type VariantContainer<int>::GetType(void) const
		{
			return typeof(int);
		}

		void* VariantContainer<int>::GetPtr(void) const
		{
			return const_cast<void*>(
				reinterpret_cast<const void*>(
					std::addressof(m_value)
					)
				);
		}

		int VariantContainer<int>::ToInt(void) const
		{
			return m_value;
		}

		bool VariantContainer<int>::ToBool(void) const
		{
			return m_value == 0 ? false : true;
		}

		float VariantContainer<int>::ToFloat(void) const
		{
			return static_cast<float>(m_value);
		}

		double VariantContainer<int>::ToDouble(void) const
		{
			return static_cast<double>(m_value);
		}

		GuGuUtf8Str VariantContainer<int>::ToString(void) const
		{
			return std::to_string(m_value);
		}

		VariantBase* VariantContainer<int>::Clone(void) const
		{
			return new VariantContainer<int>(m_value);
		}

		#pragma endregion

		#pragma region unsigned unsigned
		VariantContainer<unsigned>::VariantContainer(const unsigned& value)
			: m_value(value) { }

		VariantContainer<unsigned>::VariantContainer(const unsigned&& value)
			: m_value(std::move(value)) { }

		Type VariantContainer<unsigned>::GetType(void) const
		{
			return typeof(unsigned);
		}

		void* VariantContainer<unsigned>::GetPtr(void) const
		{
			return const_cast<void*>(
				reinterpret_cast<const void*>(
					std::addressof(m_value)
					)
				);
		}

		int VariantContainer<unsigned>::ToInt(void) const
		{
			return static_cast<int>(m_value);
		}

		bool VariantContainer<unsigned>::ToBool(void) const
		{
			return m_value == 0 ? false : true;
		}

		float VariantContainer<unsigned>::ToFloat(void) const
		{
			return static_cast<float>(m_value);
		}

		double VariantContainer<unsigned>::ToDouble(void) const
		{
			return static_cast<double>(m_value);
		}

		GuGuUtf8Str VariantContainer<unsigned>::ToString(void) const
		{
			return std::to_string(m_value);
		}

		VariantBase* VariantContainer<unsigned>::Clone(void) const
		{
			return new VariantContainer<unsigned>(m_value);
		}
		#pragma endregion

		#pragma region bool
		VariantContainer<bool>::VariantContainer(const bool& value)
			: m_value(value) { }

		VariantContainer<bool>::VariantContainer(const bool&& value)
			: m_value(std::move(value)) { }

		Type VariantContainer<bool>::GetType(void) const
		{
			return typeof(bool);
		}

		void* VariantContainer<bool>::GetPtr(void) const
		{
			return const_cast<void*>(
				reinterpret_cast<const void*>(
					std::addressof(m_value)
					)
				);
		}

		int VariantContainer<bool>::ToInt(void) const
		{
			return m_value ? 1 : 0;
		}

		bool VariantContainer<bool>::ToBool(void) const
		{
			return m_value;
		}

		float VariantContainer<bool>::ToFloat(void) const
		{
			return m_value ? 1.0f : 0.0f;
		}

		double VariantContainer<bool>::ToDouble(void) const
		{
			return m_value ? 1.0 : 0.0;
		}

		GuGuUtf8Str VariantContainer<bool>::ToString(void) const
		{
			return m_value ? "true" : "false";
		}

		VariantBase* VariantContainer<bool>::Clone(void) const
		{
			return new VariantContainer<bool>(m_value);
		}
		#pragma endregion

		#pragma region float
		VariantContainer<float>::VariantContainer(const float& value)
			: m_value(value) { }

		VariantContainer<float>::VariantContainer(const float&& value)
			: m_value(std::move(value)) { }

		Type VariantContainer<float>::GetType(void) const
		{
			return typeof(float);
		}

		void* VariantContainer<float>::GetPtr(void) const
		{
			return const_cast<void*>(
				reinterpret_cast<const void*>(
					std::addressof(m_value)
					)
				);
		}

		int VariantContainer<float>::ToInt(void) const
		{
			return static_cast<int>(m_value);
		}

		bool VariantContainer<float>::ToBool(void) const
		{
			return m_value == 0.0f ? false : true;
		}

		float VariantContainer<float>::ToFloat(void) const
		{
			return m_value;
		}

		double VariantContainer<float>::ToDouble(void) const
		{
			return static_cast<double>(m_value);
		}

		GuGuUtf8Str VariantContainer<float>::ToString(void) const
		{
			return std::to_string(m_value);
		}

		VariantBase* VariantContainer<float>::Clone(void) const
		{
			return new VariantContainer<float>(m_value);
		}
		#pragma endregion

		#pragma region double
		VariantContainer<double>::VariantContainer(const double& value)
			: m_value(value) { }

		VariantContainer<double>::VariantContainer(const double&& value)
			: m_value(std::move(value)) { }

		Type VariantContainer<double>::GetType(void) const
		{
			return typeof(double);
		}

		void* VariantContainer<double>::GetPtr(void) const
		{
			return const_cast<void*>(
				reinterpret_cast<const void*>(
					std::addressof(m_value)
					)
				);
		}

		int VariantContainer<double>::ToInt(void) const
		{
			return static_cast<int>(m_value);
		}

		bool VariantContainer<double>::ToBool(void) const
		{
			return m_value == 0.0 ? false : true;
		}

		float VariantContainer<double>::ToFloat(void) const
		{
			return static_cast<float>(m_value);
		}

		double VariantContainer<double>::ToDouble(void) const
		{
			return m_value;
		}

		GuGuUtf8Str VariantContainer<double>::ToString(void) const
		{
			return std::to_string(m_value);
		}

		VariantBase* VariantContainer<double>::Clone(void) const
		{
			return new VariantContainer<double>(m_value);
		}
		#pragma endregion

		#pragma region string
		VariantContainer<GuGuUtf8Str>::VariantContainer(
			const GuGuUtf8Str& value
		)
		: m_value(value) { }

		VariantContainer<GuGuUtf8Str>::VariantContainer(
			const GuGuUtf8Str&& value
		)
			: m_value(std::move(value)) { }

		Type VariantContainer<GuGuUtf8Str>::GetType(void) const
		{
			return typeof(GuGuUtf8Str);
		}

		void* VariantContainer<GuGuUtf8Str>::GetPtr(void) const
		{
			return const_cast<void*>(
				reinterpret_cast<const void*>(
					std::addressof(m_value)
					)
				);
		}

		int VariantContainer<GuGuUtf8Str>::ToInt(void) const
		{
			return std::stoi(m_value.getStr());
		}

		bool VariantContainer<GuGuUtf8Str>::ToBool(void) const
		{
			// 0 -> false
			// 1 -> true
			// "true" -> true (case insensitive)
			// "false" -> false (case insensitive)

			if (m_value == "0")
				return false;

			if (m_value == "1")
				return true;

			auto copy = std::string(m_value.getStr());//todo:这里要修复
		
			// convert to lowercase
			std::transform(copy.begin(), copy.end(), copy.begin(), tolower);

			if (copy == "true")
				return true;

			return false;
		}

		float VariantContainer<GuGuUtf8Str>::ToFloat(void) const
		{
			return std::stof(m_value.getStr());
		}

		GuGuUtf8Str VariantContainer<GuGuUtf8Str>::ToString(void) const
		{
			return m_value;
		}

		double VariantContainer<GuGuUtf8Str>::ToDouble(void) const
		{
			return std::stod(m_value.getStr());
		}

		VariantBase* VariantContainer<GuGuUtf8Str>::Clone(void) const
		{
			return new VariantContainer<GuGuUtf8Str>(m_value);
		}
		#pragma endregion

		#pragma region uint8_t
		VariantContainer<uint8_t>::VariantContainer(const uint8_t& value)
			: m_value(value) { }

		VariantContainer<uint8_t>::VariantContainer(const uint8_t&& value)
			: m_value(std::move(value)) { }

		Type VariantContainer<uint8_t>::GetType(void) const
		{
			return typeof(uint8_t);
		}

		void* VariantContainer<uint8_t>::GetPtr(void) const
		{
			return const_cast<void*>(
				reinterpret_cast<const void*>(
					std::addressof(m_value)
					)
				);
		}

		int VariantContainer<uint8_t>::ToInt(void) const
		{
			return m_value;
		}

		bool VariantContainer<uint8_t>::ToBool(void) const
		{
			return m_value == 0 ? false : true;
		}

		float VariantContainer<uint8_t>::ToFloat(void) const
		{
			return static_cast<float>(m_value);
		}

		double VariantContainer<uint8_t>::ToDouble(void) const
		{
			return static_cast<double>(m_value);
		}

		GuGuUtf8Str VariantContainer<uint8_t>::ToString(void) const
		{
			return std::to_string(m_value);
		}

		VariantBase* VariantContainer<uint8_t>::Clone(void) const
		{
			return new VariantContainer<uint8_t>(m_value);
		}

		#pragma endregion
	}
}