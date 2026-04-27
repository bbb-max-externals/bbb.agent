#include "c74_min.h"
#include <chrono>
#include <string>

static c74::max::t_symbol* s_namespace = nullptr;

class bbb_agent_chat : public c74::min::object<bbb_agent_chat> {
public:
	MIN_DESCRIPTION{"LLM chat with streaming response via bbb.agent.hub"};
	MIN_TAGS{"ai, chat, streaming"};
	MIN_AUTHOR{"2bit"};

	c74::min::inlet<> input{this, "(anything) commands"};
	c74::min::outlet<> response_out{this, "(anything) delta / done text"};
	c74::min::outlet<> tool_out{this, "(anything) tool calls"};
	c74::min::outlet<> status_out{this, "(anything) status / error"};

	c74::min::attribute<c74::min::symbol> model_attr{this, "model", "gpt-4.1",
		c74::min::description{"LLM model name"}
	};

	c74::min::attribute<double> temperature_attr{this, "temperature", 0.7,
		c74::min::description{"Sampling temperature"},
		c74::min::range{0.0, 2.0}
	};

	c74::min::attribute<int> stream_attr{this, "stream", 1,
		c74::min::description{"Enable streaming responses"},
		c74::min::range{0, 1}
	};

	c74::min::attribute<int> history_attr{this, "history", 1,
		c74::min::description{"Keep conversation history on helper side"},
		c74::min::range{0, 1}
	};

	c74::min::attribute<int> max_history_attr{this, "max_history", 20,
		c74::min::description{"Max conversation turns to keep"},
		c74::min::range{1, 100}
	};

	c74::min::attribute<int> timeout_attr{this, "timeout", 30,
		c74::min::description{"Request timeout in seconds"},
		c74::min::range{5, 120}
	};

	c74::min::message<> prompt_msg{this, "prompt", "Set prompt text. Usage: prompt <text>",
		MIN_FUNCTION {
			m_prompt = join_args(args);
			return {};
		}
	};

	c74::min::message<> system_msg{this, "system", "Set system prompt. Usage: system <text>",
		MIN_FUNCTION {
			m_system = join_args(args);
			return {};
		}
	};

	c74::min::message<> context_msg{this, "context", "Set context (reserved for future dict support)",
		MIN_FUNCTION {
			return {};
		}
	};

	c74::min::message<> send_msg{this, "send", "Send chat request to LLM via hub",
		MIN_FUNCTION {
			auto* hub_obj = find_hub();
			if (!hub_obj) {
				status_out.send("error", "no hub");
				return {};
			}

			m_request_id++;
			m_full_response.clear();
			m_is_streaming = true;
			status_out.send("status", "streaming");

			std::string json = build_chat_request();
			c74::max::t_atom argv[1];
			c74::max::atom_setsym(&argv[0], c74::max::gensym(json.c_str()));
			c74::max::object_method_typed(hub_obj, c74::max::gensym("send"), 1, argv, nullptr);
			return {};
		}
	};

	c74::min::message<> clear_msg{this, "clear", "Clear prompt, system prompt, and accumulated response",
		MIN_FUNCTION {
			m_prompt.clear();
			m_system.clear();
			m_full_response.clear();
			m_is_streaming = false;
			status_out.send("status", "cleared");
			return {};
		}
	};

	c74::min::message<> cancel_msg{this, "cancel", "Cancel current streaming request",
		MIN_FUNCTION {
			if (m_is_streaming) {
				auto* hub_obj = find_hub();
				if (hub_obj) {
					std::string json = "{\"type\":\"chat.cancel\",\"id\":\"" + std::to_string(m_request_id) + "\"}";
					c74::max::t_atom argv[1];
					c74::max::atom_setsym(&argv[0], c74::max::gensym(json.c_str()));
					c74::max::object_method_typed(hub_obj, c74::max::gensym("send"), 1, argv, nullptr);
				}
				m_is_streaming = false;
				status_out.send("status", "cancelled");
			}
			return {};
		}
	};

	c74::min::message<> delta_msg{this, "delta", "Receive streaming text delta",
		MIN_FUNCTION {
			if (args.size() >= 1) {
				auto text = std::string(args[0]);
				m_full_response += text;
				response_out.send("delta", args[0]);
			}
			return {};
		}
	};

	c74::min::message<> done_msg{this, "done", "Streaming complete",
		MIN_FUNCTION {
			m_is_streaming = false;
			response_out.send("done", c74::min::symbol(m_full_response.c_str()));
			status_out.send("status", "done");
			return {};
		}
	};

	c74::min::message<> tool_msg{this, "tool", "Receive tool call from LLM",
		MIN_FUNCTION {
			tool_out.send(args);
			return {};
		}
	};

	c74::min::message<> error_msg{this, "error", "Receive error from helper",
		MIN_FUNCTION {
			m_is_streaming = false;
			status_out.send(args);
			return {};
		}
	};

	bbb_agent_chat() {
		m_init_timer.delay(0);
	}

private:
	std::string m_prompt;
	std::string m_system;
	std::string m_full_response;
	int m_request_id{0};
	bool m_is_streaming{false};

	c74::min::timer<c74::min::timer_options::defer_delivery> m_init_timer{this,
		MIN_FUNCTION {
			if (!s_namespace) s_namespace = c74::max::gensym("bbb.agent");
			return {};
		}
	};

	c74::max::t_object* find_hub() {
		if (!s_namespace) s_namespace = c74::max::gensym("bbb.agent");
		return static_cast<c74::max::t_object*>(
			c74::max::object_findregistered(s_namespace, c74::max::gensym("hub")));
	}

	static std::string join_args(const c74::min::atoms& args) {
		std::string result;
		for (size_t i = 0; i < args.size(); ++i) {
			if (i > 0) result += " ";
			result += std::string(args[i]);
		}
		return result;
	}

	std::string escape_json(const std::string& s) {
		std::string out;
		out.reserve(s.size());
		for (char c : s) {
			switch (c) {
				case '"':  out += "\\\""; break;
				case '\\': out += "\\\\"; break;
				case '\n': out += "\\n"; break;
				case '\r': out += "\\r"; break;
				case '\t': out += "\\t"; break;
				default:   out += c; break;
			}
		}
		return out;
	}

	std::string build_chat_request() {
		std::string json = "{";
		json += "\"type\":\"chat.request\"";
		json += ",\"id\":\"" + std::to_string(m_request_id) + "\"";
		c74::min::symbol m = model_attr;
		json += ",\"model\":\"" + std::string(m.c_str()) + "\"";
		json += ",\"temperature\":" + std::to_string(static_cast<double>(temperature_attr));
		json += ",\"stream\":" + std::to_string(static_cast<int>(stream_attr));
		json += ",\"history\":" + std::to_string(static_cast<int>(history_attr));
		json += ",\"max_history\":" + std::to_string(static_cast<int>(max_history_attr));
		if (!m_system.empty()) {
			json += ",\"system\":\"" + escape_json(m_system) + "\"";
		}
		json += ",\"prompt\":\"" + escape_json(m_prompt) + "\"";
		json += "}";
		return json;
	}
};

MIN_EXTERNAL(bbb_agent_chat);
