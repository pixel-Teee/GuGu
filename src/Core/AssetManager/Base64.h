#pragma once

namespace GuGu {
	//base 64
	GuGuUtf8Str Base64Encode(const std::vector<uint8_t>& data);

	std::vector<uint8_t> Base64Decode(const GuGuUtf8Str& encoded);
}