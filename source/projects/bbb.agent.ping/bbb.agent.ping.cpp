#include "c74_min.h"
#include <chrono>
#include <string>

static c74::max::t_symbol* s_namespace = nullptr;

class bbb_agent_ping : public c74::min::object<bbb_agent_ping> {
public:
	MIN_DESCRIPTION{"Ping the helper service to measure latency"};
	MIN_TAGS{"ai, network, diagnostic"};
	MIN_AUTHOR{"2bit"};

	c74::min::inlet<> input{this, "(anything) commands"};
	c74::min::outlet<> output{this, "(anything) pong / latency / status"};

	c74::min::attribute<int> interval{this, "interval", 0,
		c74::min::description{"Auto-ping interval in ms. 0 = manual only."},
		c74::min::range{0, 60000}
	};

	c74::min::message<> ping_msg{this, "ping", "Send ping and measure round-trip time",
		MIN_FUNCTION {
			m_ping_start = std::chrono::steady_clock::now();
			auto* hub_obj = find_hub();
			if (hub_obj) {
				c74::max::t_atom argv[1];
				c74::max::atom_setsym(&argv[0], c74::max::gensym("{\"type\":\"ping\",\"id\":\"ping\"}"));
				c74::max::object_method_typed(hub_obj, c74::max::gensym("send"), 1, argv, nullptr);
			} else {
				output.send("status", "no hub");
			}
			return {};
		}
	};

	c74::min::message<> bang_msg{this, "bang", "Same as ping",
		MIN_FUNCTION {
			return ping_msg(c74::min::atoms{}, -1);
		}
	};

	c74::min::message<> pong_msg{this, "pong", "Receive pong response with latency",
		MIN_FUNCTION {
			auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::steady_clock::now() - m_ping_start).count();
			output.send("pong", static_cast<int>(elapsed));
			output.send("latency", static_cast<int>(elapsed));
			return {};
		}
	};

	c74::min::timer<c74::min::timer_options::defer_delivery> m_ping_timer{this,
		MIN_FUNCTION {
			if (interval > 0) {
				ping_msg(c74::min::atoms{}, -1);
				m_ping_timer.delay(interval);
			}
			return {};
		}
	};

	c74::min::timer<c74::min::timer_options::defer_delivery> m_init_timer{this,
		MIN_FUNCTION {
			if (!s_namespace) s_namespace = c74::max::gensym("bbb.agent");
			if (interval > 0) {
				m_ping_timer.delay(interval);
			}
			return {};
		}
	};

	bbb_agent_ping() {
		m_init_timer.delay(0);
	}

private:
	std::chrono::steady_clock::time_point m_ping_start{std::chrono::steady_clock::now()};

	c74::max::t_object* find_hub() {
		if (!s_namespace) s_namespace = c74::max::gensym("bbb.agent");
		return static_cast<c74::max::t_object*>(
			c74::max::object_findregistered(s_namespace, c74::max::gensym("hub")));
	}
};

MIN_EXTERNAL(bbb_agent_ping);
