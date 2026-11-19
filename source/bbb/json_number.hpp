#pragma once

#include "c74_max.h"
#include <cmath>
#include <iomanip>
#include <limits>
#include <locale>
#include <sstream>
#include <string>

namespace bbb::agent {

// Called for A_LONG and A_FLOAT atoms.
inline std::string json_number(const c74::max::t_atom& atom) {
	if (atom.a_type == c74::max::A_LONG) return std::to_string(atom.a_w.w_long);
	const double value = atom.a_w.w_float;
	if (!std::isfinite(value)) return "null";

	std::ostringstream output;
	output.imbue(std::locale::classic());
	output << std::setprecision(std::numeric_limits<double>::max_digits10) << value;
	return output.str();
}

}
