#pragma once

#include "../../defines.h"

#include <functional>
#include <concepts>

namespace bve
{
	enum class EventType
	{
		None = 0,
		MouseButtonPressed,
		MouseButtonReleased,
		MouseMoved,
		MouseScrolled,
		KeyPressed,
		KeyReleased,
		WindowClosed,
		WindowResized,
		WindowFocus,
		WindowLostFocus,
		WindowMoved
	};

	IG_API class Event
	{
	public:
		virtual ~Event() = default;

		bool isHandled_ = false;

		virtual std::string toString() const = 0;
	};

	template <typename T>
	concept IsEvent = std::is_base_of_v<Event, T>;

	template <IsEvent T>
	class EventManager
	{
	public:
		EventManager() {}

		// todo: implement all

		static void enqueue(T event);
		static void handle(T event);
		static void	listen();
		static void unlisten();
		static void popFront();
		static void popBack();

	private:
		static constexpr int MAX_QUEUE = 100;
	};

	template <IsEvent T>
	void EventManager<T>::enqueue(T event) {
		// Implementation
	}

	template <IsEvent T>
	void EventManager<T>::handle(T event) {
		// Implementation
	}

	//template <typename T>
	//IG_API class EventDispatcherHz
	//{
	//public:
	//	EventDispatcherHz(Event& event) : event_(event) {}

	//	template<typename T, typename F>
	//	bool Dispatch(const F& func)
	//	{
	//		if (event_.getEventType() == T::GetStaticType()) {
	//			event_.isHandled_ |= func(static_cast<T&>(event_));
	//			return true;
	//		}
	//		return false;
	//	}

	//private:
	//	Event& event_;
	//};



	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.toString();
	}
}