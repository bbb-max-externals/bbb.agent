#include "json_string.hpp"
#include "json_number.hpp"
#include <iostream>

struct comma_decimal : std::numpunct<char> {
	char do_decimal_point() const override { return ','; }
};

int main() {
	std::locale::global(std::locale(std::locale::classic(), new comma_decimal));
	std::string text;
	for (char c = 0; c < 32; ++c) text += c;
	text += "\"\\";
	text += u8"\u65e5\u672c\u8a9e";
	std::cout << bbb::agent::json_string(text) << '\n';
	c74::max::t_atom atom{};
	atom.a_type = c74::max::A_LONG;
	for (auto value : {std::numeric_limits<c74::max::t_atom_long>::min(),
	                   std::numeric_limits<c74::max::t_atom_long>::max()}) {
		atom.a_w.w_long = value;
		std::cout << bbb::agent::json_number(atom) << '\n';
	}
	atom.a_type = c74::max::A_FLOAT;
	for (double value : {1e-7, 1.2345678901234567, std::numeric_limits<double>::quiet_NaN(),
	                     std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity()}) {
		atom.a_w.w_float = value;
		std::cout << bbb::agent::json_number(atom) << '\n';
	}
}
