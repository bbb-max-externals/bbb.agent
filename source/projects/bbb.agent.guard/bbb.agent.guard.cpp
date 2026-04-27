#include "c74_min.h"
#include <chrono>
#include <deque>
#include <map>
#include <string>

class bbb_agent_guard : public c74::min::object<bbb_agent_guard> {
public:
	MIN_DESCRIPTION{"Safety guard for AI agent output — clip, reject, rate limit"};
	MIN_TAGS{"ai, safety, guard"};
	MIN_AUTHOR{"2bit"};

	c74::min::inlet<> msg_in{this, "(anything) messages to guard"};
	c74::min::inlet<> rules_in{this, "(anything) rule / bypass / approve / deny"};
	c74::min::outlet<> pass_out{this, "(anything) allowed messages"};
	c74::min::outlet<> block_out{this, "(anything) blocked messages"};
	c74::min::outlet<> status_out{this, "(anything) status / warning"};

	c74::min::attribute<int> bypass_attr{this, "bypass", 0,
		c74::min::description{"Bypass guard (pass all messages through)"},
		c74::min::range{0, 1}
	};

	c74::min::attribute<int> mode_attr{this, "mode", 0,
		c74::min::description{"Guard mode: 0=reject, 1=clip, 2=dryrun"},
		c74::min::range{0, 2}
	};

	c74::min::attribute<int> default_rate_limit{this, "rate_limit", 50,
		c74::min::description{"Default rate limit in ms between same selector"},
		c74::min::range{0, 10000}
	};

	struct Rule {
		bool deny{false};
		double arg_min{0.0};
		double arg_max{0.0};
		bool has_range{false};
		bool clip{false};
		int rate_limit_ms{0};
		bool has_rate_limit{false};
	};

	c74::min::message<> anything_msg{this, "anything", "Guard incoming messages",
		MIN_FUNCTION {
			if (bypass_attr) {
				pass_out.send(args);
				return {};
			}

			auto selector = std::string(args[0]);
			auto now = std::chrono::steady_clock::now();

			if (m_deny_all.count(selector) && m_deny_all[selector]) {
				block_out.send("blocked", "denied_selector", args[0]);
				return {};
			}

			auto it = m_rules.find(selector);
			if (it != m_rules.end()) {
				auto& rule = it->second;

				if (rule.has_rate_limit && rule.rate_limit_ms > 0) {
					auto last = m_last_fire.find(selector);
					if (last != m_last_fire.end()) {
						auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
							now - last->second).count();
						if (elapsed < rule.rate_limit_ms) {
							block_out.send("blocked", "rate_limited", args[0]);
							return {};
						}
					}
				}

				if (rule.has_range && args.size() > 1) {
					c74::min::atoms checked = args;
					bool modified = false;

					for (size_t i = 1; i < checked.size(); ++i) {
						double val = static_cast<double>(checked[i]);
						if (val < rule.arg_min) {
							if (rule.clip && mode_attr >= 1) {
								checked[i] = rule.arg_min;
								modified = true;
							} else {
								block_out.send("blocked", "range_low", args[0], args[i]);
								return {};
							}
						} else if (val > rule.arg_max) {
							if (rule.clip && mode_attr >= 1) {
								checked[i] = rule.arg_max;
								modified = true;
							} else {
								block_out.send("blocked", "range_high", args[0], args[i]);
								return {};
							}
						}
					}

					if (mode_attr == 2) {
						status_out.send("dryrun", modified ? "would_clip" : "pass", args[0]);
						return {};
					}

					m_last_fire[selector] = now;
					pass_out.send(checked);
					return {};
				}
			}

			if (mode_attr == 2) {
				status_out.send("dryrun", "pass", args[0]);
				return {};
			}

			m_last_fire[selector] = now;
			pass_out.send(args);
			return {};
		}
	};

	c74::min::message<> rule_msg{this, "rule", "Add safety rule. Usage: rule <selector> <min> <max> @clip 1 @rate 50 @deny 0",
		MIN_FUNCTION {
			if (args.size() < 1) return {};

			auto selector = std::string(args[0]);
			Rule rule;

			if (args.size() >= 3) {
				rule.arg_min = static_cast<double>(args[1]);
				rule.arg_max = static_cast<double>(args[2]);
				rule.has_range = true;
			}

			for (size_t i = 1; i < args.size(); ++i) {
				auto key = std::string(args[i]);
				if (key == "@clip" && i + 1 < args.size()) {
					rule.clip = static_cast<int>(args[++i]) != 0;
				} else if (key == "@rate" && i + 1 < args.size()) {
					rule.rate_limit_ms = static_cast<int>(args[++i]);
					rule.has_rate_limit = true;
				} else if (key == "@deny" && i + 1 < args.size()) {
					rule.deny = static_cast<int>(args[++i]) != 0;
					m_deny_all[selector] = rule.deny;
				}
			}

			m_rules[selector] = rule;
			status_out.send("status", "rule_added", args[0]);
			return {};
		}
	};

	c74::min::message<> deny_msg{this, "deny", "Deny selector. Usage: deny <name>",
		MIN_FUNCTION {
			if (args.size() >= 1) {
				m_deny_all[std::string(args[0])] = true;
				status_out.send("status", "denied", args[0]);
			}
			return {};
		}
	};

	c74::min::message<> approve_msg{this, "approve", "Approve previously denied selector",
		MIN_FUNCTION {
			if (args.size() >= 1) {
				m_deny_all.erase(std::string(args[0]));
				status_out.send("status", "approved", args[0]);
			}
			return {};
		}
	};

	c74::min::message<> bypass_msg{this, "bypass", "Toggle bypass. Usage: bypass <0|1>",
		MIN_FUNCTION {
			if (args.size() >= 1) {
				bypass_attr = static_cast<int>(args[0]);
				status_out.send("status", bypass_attr ? "bypassed" : "active");
			}
			return {};
		}
	};

	c74::min::message<> clear_msg{this, "clear", "Clear all rules",
		MIN_FUNCTION {
			m_rules.clear();
			m_deny_all.clear();
			m_last_fire.clear();
			status_out.send("status", "cleared");
			return {};
		}
	};

private:
	std::map<std::string, Rule> m_rules;
	std::map<std::string, bool> m_deny_all;
	std::map<std::string, std::chrono::steady_clock::time_point> m_last_fire;
};

MIN_EXTERNAL(bbb_agent_guard);
