#include "c74_min.h"

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
			auto now = std::chrono::steady_clock::now();
			output.send("pong", std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::steady_clock::now() - now).count());
			return {};
		}
	};

	c74::min::message<> bang_msg{this, "bang", "Same as ping",
		MIN_FUNCTION {
			return ping_msg(c74::min::atoms{}, -1);
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
			if (interval > 0) {
				m_ping_timer.delay(interval);
			}
			return {};
		}
	};

	bbb_agent_ping() {
		m_init_timer.delay(0);
	}
};

MIN_EXTERNAL(bbb_agent_ping);
