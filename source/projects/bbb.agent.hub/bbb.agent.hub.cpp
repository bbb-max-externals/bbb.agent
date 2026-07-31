#include "c74_min.h"
#include "bbb_agent_websocket_client.hpp"

#include <cstdint>
#include <map>
#include <string>

static c74::max::t_symbol *s_namespace = nullptr;

class bbb_agent_hub : public c74::min::object<bbb_agent_hub> {
public:
	MIN_DESCRIPTION{"WebSocket connection manager for bbb.agent helper service"};
	MIN_TAGS{"ai, network, connection"};
	MIN_AUTHOR{"2bit"};

	c74::min::inlet<> input{this, "(anything) commands"};
	c74::min::outlet<> status_out{this, "(symbol) status events"};
	c74::min::outlet<> data_out{this, "(anything) response data"};

	c74::min::attribute<c74::min::symbol> host{this, "host", "localhost",
		c74::min::description{"Helper service hostname"}
	};

	c74::min::attribute<int> port{this, "port", 8765,
		c74::min::description{"Helper service port"},
		c74::min::range{1, 65535}
	};

	c74::min::attribute<int> reconnect_attr{this, "reconnect", 1,
		c74::min::description{"Auto-reconnect on disconnect"},
		c74::min::range{0, 1}
	};

	c74::min::attribute<int> reconnect_interval{this, "reconnect_interval", 3000,
		c74::min::description{"Reconnect attempt interval in ms"},
		c74::min::range{100, 60000}
	};

	c74::min::message<> connect_msg{this, "connect", "Connect to helper service",
		MIN_FUNCTION {
			do_connect();
			return {};
		}
	};

	c74::min::message<> disconnect_msg{this, "disconnect", "Disconnect from helper service",
		MIN_FUNCTION {
			do_disconnect();
			return {};
		}
	};

	c74::min::message<> restart_msg{this, "restart", "Restart connection",
		MIN_FUNCTION {
			do_disconnect();
			do_connect();
			return {};
		}
	};

	c74::min::message<> status_msg{this, "status", "Query connection status",
		MIN_FUNCTION {
			status_out.send("status", status_label());
			return {};
		}
	};

	c74::min::message<> apikey_msg{this, "apikey", "Set API key. Usage: apikey <provider> <key>",
		MIN_FUNCTION {
			if(2 <= args.size()) {
				const auto provider = std::string(args[0]);
				const auto key = std::string(args[1]);
				api_keys_[provider] = key;
			}
			return {};
		}
	};

	c74::min::message<> config_msg{this, "config", "Send configuration dict to helper",
		MIN_FUNCTION {
			return {};
		}
	};

	c74::min::message<> send_msg{this, "send", "Send raw JSON string to helper",
		MIN_FUNCTION {
			if(!args.empty() && !websocket_client_.send(std::string(args[0]))) {
				report_error("WebSocket is not connected");
			}
			return {};
		}
	};

	bbb_agent_hub() {
		if(!s_namespace) s_namespace = c74::max::gensym("bbb.agent");
		websocket_client_.set_notifier([this]() {
			event_queue_.set();
		});
		init_timer_.delay(0);
	}

	~bbb_agent_hub() {
		websocket_client_.clear_notifier();
		websocket_client_.disconnect();
		auto *registered = c74::max::object_findregistered(
			s_namespace,
			c74::max::gensym("hub")
		);
		if(registered == maxobj()) c74::max::object_unregister(registered);
	}

private:
	std::map<std::string, std::string> api_keys_;

	c74::min::queue<> event_queue_{this, MIN_FUNCTION {
		deliver_events();
		return {};
	}};

	bbb::agent::websocket_client websocket_client_;

	c74::min::timer<c74::min::timer_options::defer_delivery> init_timer_{this,
		MIN_FUNCTION {
			c74::max::object_register(
				s_namespace,
				c74::max::gensym("hub"),
				maxobj()
			);
			return {};
		}
	};

	void do_connect() {
		status_out.send("status", "connecting");
		websocket_client_.connect(
			build_url(),
			reconnect_attr != 0,
			static_cast<std::uint32_t>(static_cast<int>(reconnect_interval))
		);
	}

	void do_disconnect() {
		websocket_client_.disconnect();
	}

	auto build_url() -> std::string {
		const c74::min::symbol host_symbol = host;
		return std::string{"ws://"} + host_symbol.c_str() + ":" +
			std::to_string(static_cast<int>(port));
	}

	auto status_label() const -> const char * {
		switch(websocket_client_.state()) {
			case bbb::agent::websocket_state::connecting:
				return "connecting";
			case bbb::agent::websocket_state::open:
				return "connected";
			case bbb::agent::websocket_state::closing:
			case bbb::agent::websocket_state::closed:
				return "disconnected";
		}
		return "disconnected";
	}

	void deliver_events() {
		for(auto &event : websocket_client_.drain_events()) {
			switch(event.type) {
				case bbb::agent::websocket_event_type::open:
					status_out.send("status", "connected");
					break;
				case bbb::agent::websocket_event_type::close:
					status_out.send("status", "disconnected");
					break;
				case bbb::agent::websocket_event_type::message:
					status_out.send("status", "message");
					data_out.send("data", event.text);
					break;
				case bbb::agent::websocket_event_type::error:
					report_error(event.text);
					break;
			}
		}
	}

	void report_error(const std::string &message) {
		cerr << message << c74::min::endl;
		status_out.send("error", message);
	}
};

MIN_EXTERNAL(bbb_agent_hub);
