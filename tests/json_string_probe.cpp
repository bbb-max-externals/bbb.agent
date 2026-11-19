#include "json_string.hpp"
#include <iostream>

int main() {
	std::string text;
	for (char c = 0; c < 32; ++c) text += c;
	text += "\"\\";
	text += u8"\u65e5\u672c\u8a9e";
	std::cout << bbb::agent::json_string(text) << '\n';
}
