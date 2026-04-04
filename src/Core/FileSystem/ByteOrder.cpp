#include <pch.h>

#include "ByteOrder.h"

namespace GuGu {
	bool gIsLittleEndian = IsLittleEndian();

	bool IsLittleEndian()
	{
		static const uint16_t test = 0x0001;  // 低字节为 0x01，高字节为 0x00
		return *reinterpret_cast<const uint8_t*>(&test) == 0x01;  // 如果低地址存的是 0x01，则是小端
	}

	uint16_t ByteSwap16(uint16_t value)
	{
		return (value << 8) | (value >> 8);
	}

	uint32_t ByteSwap32(uint32_t value)
	{
		return (value << 24) |
			((value << 8) & 0x00FF0000) |
			((value >> 8) & 0x0000FF00) |
			(value >> 24);
	}

	uint64_t ByteSwap64(uint64_t value)
	{
		value = ((value << 8) & 0xFF00FF00FF00FF00ULL) | ((value >> 8) & 0x00FF00FF00FF00FFULL);
		value = ((value << 16) & 0xFFFF0000FFFF0000ULL) | ((value >> 16) & 0x0000FFFF0000FFFFULL);
		return (value << 32) | (value >> 32);
	}

	uint16_t HostToLittle16(uint16_t x)
	{
		return gIsLittleEndian ? x : ByteSwap16(x);
	}

	uint32_t HostToLittle32(uint32_t x)
	{
		return gIsLittleEndian ? x : ByteSwap32(x);
	}

	uint64_t HostToLittle64(uint64_t x)
	{
		return gIsLittleEndian ? x : ByteSwap64(x);
	}

	uint16_t LittleToHost16(uint16_t x)
	{
		return gIsLittleEndian ? x : ByteSwap16(x);
	}

	uint32_t LittleToHost32(uint32_t x)
	{
		return gIsLittleEndian ? x : ByteSwap32(x);
	}

	uint64_t LittleToHost64(uint64_t x)
	{
		return gIsLittleEndian ? x : ByteSwap64(x);
	}

	float HostToLittleFloat(float x)
	{
		uint32_t bits;
		std::memcpy(&bits, &x, sizeof(bits));
		bits = HostToLittle32(bits);
		float result;
		std::memcpy(&result, &bits, sizeof(result));
		return result;
	}

	float LittleToHostFloat(float x)
	{
		uint32_t bits;
		std::memcpy(&bits, &x, sizeof(bits));
		bits = LittleToHost32(bits);
		float result;
		std::memcpy(&result, &bits, sizeof(result));
		return result;
	}

	double HostToLittleDouble(double x)
	{
		uint64_t bits;
		std::memcpy(&bits, &x, sizeof(bits));
		bits = HostToLittle64(bits);   // 将主机序的位模式转为小端
		double result;
		std::memcpy(&result, &bits, sizeof(result));
		return result;
	}

	double LittleToHostDouble(double x)
	{
		uint64_t bits;
		std::memcpy(&bits, &x, sizeof(bits));
		bits = LittleToHost64(bits);   // 将小端的位模式转为主机序
		double result;
		std::memcpy(&result, &bits, sizeof(result));
		return result;
	}

	void HostToLittle(meta::Type type, meta::Variant& instance)
	{
		if (type == typeof(float))
		{
			float& val = instance.GetValue<float>();
			val = HostToLittleFloat(val);
		}
		else if (type == typeof(double))
		{
			double& val = instance.GetValue<double>();
			val = HostToLittleDouble(val);
		}
		else if (type == typeof(int16_t))
		{
			int16_t& val = instance.GetValue<int16_t>();
			val = HostToLittle16(val);
		}
		else if (type == typeof(uint16_t))
		{
			uint16_t& val = instance.GetValue<uint16_t>();
			val = HostToLittle16(val);
		}
		else if (type == typeof(int32_t))
		{
			int32_t& val = instance.GetValue<int32_t>();
			val = HostToLittle32(val);
		}
		else if (type == typeof(uint32_t))
		{
			uint32_t& val = instance.GetValue<uint32_t>();
			val = HostToLittle32(val);
		}
		else if (type == typeof(int64_t))
		{
			int64_t& val = instance.GetValue<int64_t>();
			val = HostToLittle64(val);
		}
		else if (type == typeof(uint64_t))
		{
			uint64_t& val = instance.GetValue<uint64_t>();
			val = HostToLittle64(val);
		}

		auto& fields = meta::ReflectionDatabase::Instance().types[type.GetID()].fields;

		for (auto& field : fields)
		{
			auto value = field.GetValueReference(instance);//variant

			HostToLittle(field.GetType(), value);
		}
	}

	void LittleToHost(meta::Type type, meta::Variant& instance)
	{
		if (type == typeof(float))
		{
			float& val = instance.GetValue<float>();
			val = LittleToHostFloat(val);
		}
		else if (type == typeof(double))
		{
			double& val = instance.GetValue<double>();
			val = LittleToHostDouble(val);
		}
		else if (type == typeof(int16_t))
		{
			int16_t& val = instance.GetValue<int16_t>();
			val = LittleToHost16(val);
		}
		else if (type == typeof(uint16_t))
		{
			uint16_t& val = instance.GetValue<uint16_t>();
			val = LittleToHost16(val);
		}
		else if (type == typeof(int32_t))
		{
			int32_t& val = instance.GetValue<int32_t>();
			val = LittleToHost32(val);
		}
		else if (type == typeof(uint32_t))
		{
			uint32_t& val = instance.GetValue<uint32_t>();
			val = LittleToHost32(val);
		}
		else if (type == typeof(int64_t))
		{
			int64_t& val = instance.GetValue<int64_t>();
			val = LittleToHost64(val);
		}
		else if (type == typeof(uint64_t))
		{
			uint64_t& val = instance.GetValue<uint64_t>();
			val = LittleToHost64(val);
		}

		auto& fields = meta::ReflectionDatabase::Instance().types[type.GetID()].fields;

		for (auto& field : fields)
		{
			auto value = field.GetValueReference(instance);//variant

			HostToLittle(field.GetType(), value);
		}
	}

}