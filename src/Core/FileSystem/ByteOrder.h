#pragma once

namespace GuGu {
	bool IsLittleEndian();

	//uint16_t
	uint16_t ByteSwap16(uint16_t value);

	uint32_t ByteSwap32(uint32_t value);

	uint64_t ByteSwap64(uint64_t value);

	uint16_t HostToLittle16(uint16_t x);

	uint32_t HostToLittle32(uint32_t x);

	uint64_t HostToLittle64(uint64_t x);

	uint16_t LittleToHost16(uint16_t x);

	uint32_t LittleToHost32(uint32_t x);

	uint64_t LittleToHost64(uint64_t x);

	float HostToLittleFloat(float x);

	float LittleToHostFloat(float x);

	double HostToLittleDouble(double x);

	double LittleToHostDouble(double x);
}