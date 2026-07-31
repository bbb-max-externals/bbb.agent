#pragma once

#include <ixwebsocket/IXWebSocket.h>

#include <cstdint>
#include <deque>
#include <functional>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

namespace bbb::agent {

enum class websocket_event_type {
	open,
	close,
	message,
	error
};

struct websocket_event {
	websocket_event_type type{};
	std::string text;
	std::uint16_t close_code{};
};

enum class websocket_state {
	connecting,
	open,
	closing,
	closed
};

class websocket_client {
public:
	using notifier = std::function<void()>;

	websocket_client() {
		websocket_.setOnMessageCallback(
			[this](const ix::WebSocketMessagePtr &message) {
				handle_message(message);
			}
		);
	}

	~websocket_client() {
		clear_notifier();
		disconnect();
		websocket_.setOnMessageCallback(nullptr);
	}

	websocket_client(const websocket_client &) = delete;
	websocket_client(websocket_client &&) = delete;
	auto operator=(const websocket_client &) -> websocket_client & = delete;
	auto operator=(websocket_client &&) -> websocket_client & = delete;

	void set_notifier(notifier callback) {
		std::lock_guard<std::mutex> lock{event_mutex_};
		notifier_ = std::move(callback);
	}

	void clear_notifier() {
		std::lock_guard<std::mutex> lock{event_mutex_};
		notifier_ = {};
	}

	void connect(
		const std::string &url,
		bool automatically_reconnect,
		std::uint32_t reconnect_interval_milliseconds
	) {
		websocket_.stop();
		websocket_.setUrl(url);
		if(automatically_reconnect) {
			websocket_.enableAutomaticReconnection();
			websocket_.setMinWaitBetweenReconnectionRetries(
				reconnect_interval_milliseconds
			);
			websocket_.setMaxWaitBetweenReconnectionRetries(
				reconnect_interval_milliseconds
			);
		} else {
			websocket_.disableAutomaticReconnection();
		}
		websocket_.start();
	}

	void disconnect() {
		websocket_.disableAutomaticReconnection();
		websocket_.stop();
	}

	auto send(const std::string &text) -> bool {
		if(websocket_.getReadyState() != ix::ReadyState::Open) return false;
		return websocket_.sendText(text).success;
	}

	auto state() const -> websocket_state {
		switch(websocket_.getReadyState()) {
			case ix::ReadyState::Connecting:
				return websocket_state::connecting;
			case ix::ReadyState::Open:
				return websocket_state::open;
			case ix::ReadyState::Closing:
				return websocket_state::closing;
			case ix::ReadyState::Closed:
				return websocket_state::closed;
		}
		return websocket_state::closed;
	}

	auto drain_events() -> std::vector<websocket_event> {
		std::lock_guard<std::mutex> lock{event_mutex_};
		std::vector<websocket_event> events;
		events.reserve(events_.size());
		while(!events_.empty()) {
			events.push_back(std::move(events_.front()));
			events_.pop_front();
		}
		return events;
	}

private:
	ix::WebSocket websocket_;
	std::mutex event_mutex_;
	std::deque<websocket_event> events_;
	notifier notifier_;

	void enqueue(websocket_event event) {
		std::lock_guard<std::mutex> lock{event_mutex_};
		events_.push_back(std::move(event));
		if(notifier_) notifier_();
	}

	void handle_message(const ix::WebSocketMessagePtr &message) {
		switch(message->type) {
			case ix::WebSocketMessageType::Open:
				enqueue({websocket_event_type::open, {}, 0});
				break;
			case ix::WebSocketMessageType::Close:
				enqueue({
					websocket_event_type::close,
					message->closeInfo.reason,
					message->closeInfo.code
				});
				break;
			case ix::WebSocketMessageType::Message:
				enqueue({websocket_event_type::message, message->str, 0});
				break;
			case ix::WebSocketMessageType::Error:
				enqueue({
					websocket_event_type::error,
					message->errorInfo.reason,
					0
				});
				break;
			default:
				break;
		}
	}
};

} // namespace bbb::agent
