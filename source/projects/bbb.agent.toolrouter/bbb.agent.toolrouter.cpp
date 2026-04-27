#include "c74_min.h"
#include <map>
#include <string>

class bbb_agent_toolrouter : public c74::min::object<bbb_agent_toolrouter> {
public:
	MIN_DESCRIPTION{"Route validated tool calls from LLM to Max messages"};
	MIN_TAGS{"ai, tool, routing"};
	MIN_AUTHOR{"2bit"};

	c74::min::inlet<> tool_in{this, "(anything) tool calls from chat"};
	c74::min::inlet<> control_in{this, "(anything) enable / allow / deny"};
	c74::min::outlet<> route_out{this, "(anything) routed tool messages"};
	c74::min::outlet<> reject_out{this, "(anything) rejected tool calls"};
	c74::min::outlet<> status_out{this, "(anything) status"};

	c74::min::attribute<int> strict_attr{this, "strict", 0,
		c74::min::description{"Reject tools not in allowlist"},
		c74::min::range{0, 1}
	};

	c74::min::message<> tool_msg{this, "tool", "Route tool call. Usage: tool <name> <args_json>",
		MIN_FUNCTION {
			if (args.size() < 2) {
				reject_out.send("reject", "missing_args");
				return {};
			}

			auto tool_name = std::string(args[0]);
			auto it = m_tools.find(tool_name);

			if (it == m_tools.end()) {
				reject_out.send("reject", "unknown_tool", args[0]);
				return {};
			}

			if (!it->second.enabled) {
				reject_out.send("reject", "disabled", args[0]);
				return {};
			}

			route_out.send(args);
			return {};
		}
	};

	c74::min::message<> allow_msg{this, "allow", "Add tool to allowlist. Usage: allow <name>",
		MIN_FUNCTION {
			if (args.size() >= 1) {
				auto name = std::string(args[0]);
				auto& entry = m_tools[name];
				entry.name = name;
				entry.enabled = true;
				status_out.send("status", "allowed", args[0]);
			}
			return {};
		}
	};

	c74::min::message<> deny_msg{this, "deny", "Remove tool from allowlist. Usage: deny <name>",
		MIN_FUNCTION {
			if (args.size() >= 1) {
				auto name = std::string(args[0]);
				m_tools.erase(name);
				status_out.send("status", "denied", args[0]);
			}
			return {};
		}
	};

	c74::min::message<> enable_msg{this, "enable", "Enable/disable tool. Usage: enable <name> <0|1>",
		MIN_FUNCTION {
			if (args.size() >= 2) {
				auto name = std::string(args[0]);
				int val = static_cast<int>(args[1]);
				if (m_tools.count(name)) {
					m_tools[name].enabled = val != 0;
					status_out.send("status", val ? "enabled" : "disabled", args[0]);
				} else {
					reject_out.send("reject", "unknown_tool", args[0]);
				}
			}
			return {};
		}
	};

	c74::min::message<> list_msg{this, "list", "List registered tools",
		MIN_FUNCTION {
			for (auto& [name, entry] : m_tools) {
				status_out.send("tool", c74::min::symbol(name.c_str()), entry.enabled ? 1 : 0);
			}
			return {};
		}
	};

	c74::min::message<> clear_msg{this, "clear", "Clear all registered tools",
		MIN_FUNCTION {
			m_tools.clear();
			status_out.send("status", "cleared");
			return {};
		}
	};

	c74::min::message<> schema_msg{this, "schema", "Load tool schema (reserved for future)",
		MIN_FUNCTION {
			return {};
		}
	};

	c74::min::message<> loadschema_msg{this, "loadschema", "Load tool schema from file (reserved for future)",
		MIN_FUNCTION {
			return {};
		}
	};

private:
	struct ToolEntry {
		std::string name;
		bool enabled{true};
	};

	std::map<std::string, ToolEntry> m_tools;
};

MIN_EXTERNAL(bbb_agent_toolrouter);
