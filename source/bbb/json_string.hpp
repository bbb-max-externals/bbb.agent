#pragma once

#include <string>
#include <string_view>

namespace bbb::agent {

inline std::string json_string(std::string_view text) {
	constexpr char hex[] = "0123456789abcdef";
	std::string result{"\""};
	for (unsigned char c : text) {
		switch (c) {
			case '"':  result += "\\\""; break;
			case '\\': result += "\\\\"; break;
			case '\b': result += "\\b"; break;
			case '\f': result += "\\f"; break;
			case '\n': result += "\\n"; break;
			case '\r': result += "\\r"; break;
			case '\t': result += "\\t"; break;
			default:
				if (c < 0x20) {
					result += "\\u00";
					result += hex[c >> 4];
					result += hex[c & 15];
				} else {
					result += static_cast<char>(c);
				}
		}
	}
	result += '"';
	return result;
}

}
