#include "c74_min.h"
#include <map>
#include <string>
#include <ixwebsocket/IXWebSocket.h>

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
			auto state = m_ws.getReadyState();
			auto label = (state == ix::ReadyState::Open) ? "connected"
			           : (state == ix::ReadyState::Connecting) ? "connecting"
			           : "disconnected";
			status_out.send("status", label);
			return {};
		}
	};

	c74::min::message<> apikey_msg{this, "apikey", "Set API key. Usage: apikey <provider> <key>",
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

	c74::min::message<> send_msg{this, "send", "Send raw JSON string to helper",
		MIN_FUNCTION {
			if (args.size() >= 1) {
				auto text = std::string(args[0]);
				m_ws.send(text);
			}
			return {};
		}
	};

	bbb_agent_hub() {
		m_ws.setOnMessageCallback([this](const ix::WebSocketMessagePtr& msg) {
			if (msg->type == ix::WebSocketMessageType::Message) {
				m_last_received = msg->str;
				m_queue.set();
			} else if (msg->type == ix::WebSocketMessageType::Open) {
				m_was_connected = true;
				m_status_queue.set();
			} else if (msg->type == ix::WebSocketMessageType::Close) {
				if (m_was_connected && reconnect_attr) {
					m_reconnect_timer.delay(reconnect_interval);
				}
				m_was_connected = false;
				m_status_queue.set();
			} else if (msg->type == ix::WebSocketMessageType::Error) {
				m_last_error = msg->errorInfo.reason;
				m_error_queue.set();
			}
		});
		m_init_timer.delay(0);
	}

	~bbb_agent_hub() {
		m_ws.close();
	}

private:
	ix::WebSocket m_ws;
	std::map<std::string, std::string> m_api_keys;
	std::string m_last_received;
	std::string m_last_error;
	bool m_was_connected{false};

	void do_connect() {
		m_ws.setUrl(build_url());
		m_ws.start();
	}

	void do_disconnect() {
		m_was_connected = false;
		m_ws.stop();
	}

	std::string build_url() {
		c74::min::symbol h = host;
		return std::string("ws://") + std::string(h.c_str()) + ":" + std::to_string(static_cast<int>(port));
	}

	c74::min::queue<> m_queue{this, MIN_FUNCTION {
		status_out.send("status", "message");
		data_out.send("data", m_last_received);
		return {};
	}};

	c74::min::queue<> m_status_queue{this, MIN_FUNCTION {
		auto state = m_ws.getReadyState();
		auto label = (state == ix::ReadyState::Open) ? "connected"
		           : (state == ix::ReadyState::Connecting) ? "connecting"
		           : "disconnected";
		status_out.send("status", label);
		return {};
	}};

	c74::min::queue<> m_error_queue{this, MIN_FUNCTION {
		status_out.send("error", m_last_error);
		return {};
	}};

	c74::min::timer<c74::min::timer_options::defer_delivery> m_init_timer{this,
		MIN_FUNCTION {
			return {};
		}
	};

	c74::min::timer<c74::min::timer_options::defer_delivery> m_reconnect_timer{this,
		MIN_FUNCTION {
			if (reconnect_attr && m_ws.getReadyState() != ix::ReadyState::Open) {
				status_out.send("status", "reconnecting");
				m_ws.setUrl(build_url());
				m_ws.start();
			}
			return {};
		}
	};
};

MIN_EXTERNAL(bbb_agent_hub);
