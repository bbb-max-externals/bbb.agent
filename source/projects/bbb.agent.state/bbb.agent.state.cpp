#include "c74_min.h"
#include "json_string.hpp"
#include <chrono>
#include <deque>
#include <map>
#include <string>

class bbb_agent_state : public c74::min::object<bbb_agent_state> {
public:
	MIN_DESCRIPTION{"Collect Max patch state as context for AI agent"};
	MIN_TAGS{"ai, state, context"};
	MIN_AUTHOR{"2bit"};

	c74::min::inlet<> update_in{this, "(anything) set / event / clear"};
	c74::min::outlet<> dict_out{this, "(anything) state key-value pairs"};
	c74::min::outlet<> json_out{this, "(anything) JSON string"};

	c74::min::attribute<int> max_events_attr{this, "max_events", 128,
		c74::min::description{"Max recent events to keep"},
		c74::min::range{1, 1024}
	};

	c74::min::message<> set_msg{this, "set", "Set state value. Usage: set <key> <value>",
		MIN_FUNCTION {
			if (args.size() >= 2) {
				auto key = std::string(args[0]);
				m_state[key] = args[1];
				dict_out.send("set", args[0], args[1]);
			}
			return {};
		}
	};

	c74::min::message<> unset_msg{this, "unset", "Remove state key. Usage: unset <key>",
		MIN_FUNCTION {
			if (args.size() >= 1) {
				m_state.erase(std::string(args[0]));
				dict_out.send("unset", args[0]);
			}
			return {};
		}
	};

	c74::min::message<> event_msg{this, "event", "Record event. Usage: event <type> <payload>",
		MIN_FUNCTION {
			auto now = std::chrono::steady_clock::now();
			auto ts = std::chrono::duration_cast<std::chrono::milliseconds>(
				now - m_epoch).count() / 1000.0;

			c74::min::atoms entry;
			entry.push_back(c74::min::symbol("event"));
			entry.push_back(ts);
			for (auto& a : args) entry.push_back(a);

			m_events.push_back(entry);
			while (static_cast<int>(m_events.size()) > max_events_attr) {
				m_events.pop_front();
			}

			dict_out.send(entry);
			return {};
		}
	};

	c74::min::message<> clear_msg{this, "clear", "Clear all state and events",
		MIN_FUNCTION {
			m_state.clear();
			m_events.clear();
			dict_out.send("cleared");
			return {};
		}
	};

	c74::min::message<> dump_msg{this, "dump", "Output all state as key-value pairs",
		MIN_FUNCTION {
			for (auto& [key, val] : m_state) {
				dict_out.send("set", c74::min::symbol(key.c_str()), val);
			}
			for (auto& ev : m_events) {
				dict_out.send(ev);
			}
			return {};
		}
	};

	c74::min::message<> json_msg{this, "json", "Output state as JSON string",
		MIN_FUNCTION {
			std::string j = "{";

			for (auto& [key, val] : m_state) {
				j += bbb::agent::json_string(key) + ":";
				j += value_to_json(val);
				j += ",";
			}

			if (!m_events.empty()) {
				j += "\"recent_events\":[";
				for (size_t i = 0; i < m_events.size(); ++i) {
					if (i > 0) j += ",";
					j += event_to_json(m_events[i]);
				}
				j += "]";
			} else {
				if (j.back() == ',') j.pop_back();
			}

			j += "}";
			json_out.send("json", c74::min::symbol(j.c_str()));
			return {};
		}
	};

private:
	std::map<std::string, c74::min::atom> m_state;
	std::deque<c74::min::atoms> m_events;
	std::chrono::steady_clock::time_point m_epoch{std::chrono::steady_clock::now()};


	std::string value_to_json(const c74::min::atom& val) {
		auto a_type = static_cast<long>(val.type());
		if (a_type == c74::max::A_LONG) {
			return std::to_string(static_cast<int>(val));
		} else if (a_type == c74::max::A_FLOAT) {
			return std::to_string(static_cast<double>(val));
		}
		return bbb::agent::json_string(std::string(val));
	}

	std::string event_to_json(const c74::min::atoms& ev) {
		std::string j = "{";
		if (ev.size() >= 2) j += "\"time\":" + std::to_string(static_cast<double>(ev[1]));
		if (ev.size() >= 3) j += ",\"type\":" + bbb::agent::json_string(std::string(ev[2]));
		if (ev.size() >= 4) {
			j += ",\"data\":";
			if (ev.size() == 4) {
				j += value_to_json(ev[3]);
			} else {
				j += "[";
				for (size_t i = 3; i < ev.size(); ++i) {
					if (i > 3) j += ",";
					j += value_to_json(ev[i]);
				}
				j += "]";
			}
		}
		j += "}";
		return j;
	}
};

MIN_EXTERNAL(bbb_agent_state);
