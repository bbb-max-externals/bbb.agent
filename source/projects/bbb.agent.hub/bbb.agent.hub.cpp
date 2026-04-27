#include "c74_min.h"
#include <map>

class bbb_agent_hub : public c74::min::object<bbb_agent_hub> {
public:
	MIN_DESCRIPTION{"WebSocket connection manager for bbb.agent helper service"};
	MIN_TAGS{"ai, network, connection"};
	MIN_AUTHOR{"2bit"};

	c74::min::inlet<> input{this, "(anything) commands: connect, disconnect, restart, status, apikey, config"};
	c74::min::outlet<> status_out{this, "(symbol) status events"};
	c74::min::outlet<> data_out{this, "(anything) response data"};

	c74::min::attribute<c74::min::symbol> host{this, "host", "localhost",
		c74::min::description{"Helper service hostname"}
	};

	c74::min::attribute<int> port{this, "port", 8765,
		c74::min::description{"Helper service port"},
		c74::min::range{1, 65535}
	};

	c74::min::attribute<int> reconnect{this, "reconnect", 1,
		c74::min::description{"Auto-reconnect on disconnect"},
		c74::min::range{0, 1}
	};

	c74::min::attribute<int> reconnect_interval{this, "reconnect_interval", 3000,
		c74::min::description{"Reconnect attempt interval in ms"},
		c74::min::range{100, 60000}
	};

	c74::min::message<> connect_msg{this, "connect", "Connect to helper service",
		MIN_FUNCTION {
			status_out.send("status", "connecting");
			status_out.send("status", "connected");
			return {};
		}
	};

	c74::min::message<> disconnect_msg{this, "disconnect", "Disconnect from helper service",
		MIN_FUNCTION {
			status_out.send("status", "disconnected");
			return {};
		}
	};

	c74::min::message<> restart_msg{this, "restart", "Restart connection",
		MIN_FUNCTION {
			status_out.send("status", "disconnected");
			status_out.send("status", "connecting");
			status_out.send("status", "connected");
			return {};
		}
	};

	c74::min::message<> status_msg{this, "status", "Query connection status",
		MIN_FUNCTION {
			status_out.send("status", "disconnected");
			return {};
		}
	};

	c74::min::message<> apikey_msg{this, "apikey", "Set API key for a provider. Usage: apikey <provider> <key>",
		MIN_FUNCTION {
			if (args.size() >= 2) {
				auto provider = std::string(args[0]);
				auto key = std::string(args[1]);
				m_api_keys[provider] = key;
			}
			return {};
		}
	};

	c74::min::message<> config_msg{this, "config", "Send configuration dict to helper",
		MIN_FUNCTION {
			return {};
		}
	};

private:
	std::map<std::string, std::string> m_api_keys;

	c74::min::timer<c74::min::timer_options::defer_delivery> m_init_timer{this,
		MIN_FUNCTION {
			return {};
		}
	};
};

MIN_EXTERNAL(bbb_agent_hub);
