#include <pch.h>

#include "Base64.h"

namespace GuGu {

	GuGuUtf8Str Base64Encode(const std::vector<uint8_t>& data)
	{
		//encode base64
		static const char* base64Chars =
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ" \
			"abcdefghijklmnopqrstuvwxyz" \
			"0123456789+/";

		std::string result;
		int32_t val = 0;
		int32_t valB = -6;
		for (uint8_t c : data) {
			val = (val << 8) + c;
			valB += 8;
			while (valB >= 0) {
				result.push_back(base64Chars[(val >> valB) & 0x3F]);
				valB -= 6;
			}
		}
		if (valB > -6) {
			result.push_back(base64Chars[((val << 8) >> (valB + 8)) & 0x3F]);
		}
		while (result.size() % 4) {
			result.push_back('=');
		}
		return result;
	}

	std::vector<uint8_t> Base64Decode(const GuGuUtf8Str& encoded)
	{
		static const std::string base64Chars =
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ" \
			"abcdefghijklmnopqrstuvwxyz" \
			"0123456789+/";

		// 创建解码映射表
		int32_t decodingTable[256];
		for (int32_t i = 0; i < 256; i++)
			decodingTable[i] = -1;
		for (int32_t i = 0; i < 64; i++)
			decodingTable[base64Chars[i]] = i;

		std::vector<uint8_t> decoded;
		int32_t val = 0, valB = -8;

		for (int32_t i = 0; i < encoded.getTotalByteCount(); ++i) {
			char c = encoded.getStr()[i];
			if (c == '=') break; // 遇到填充字符，结束

			if (decodingTable[c] == -1)
				continue; // 跳过无效字符

			val = (val << 6) + decodingTable[c];
			valB += 6;

			if (valB >= 0) {
				decoded.push_back((val >> valB) & 0xFF);
				valB -= 8;
			}
		}

		return decoded;
	}

}