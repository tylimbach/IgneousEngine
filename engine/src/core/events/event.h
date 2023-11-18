#pragma once

#include "../../defines.h"

#include <functional>

namespace bve
{
	enum class EventType
	{
		None = 0,
		MouseButtonPressed = 1,
		MouseButtonReleased = 2,
		MouseMoved = 3,
		MouseScrolled = 4,
		KeyPressed = 5,
		KeyReleased = 6,
		WindowClosed = 7,
		WindowResized = 8,
		WindowFocus = 9,
		WindowLostFocus = 10,
		WindowMoved = 11
	};

	KAPI class Event
	{
	public:
		virtual ~Event() = default;

		bool isHandled_ = false;

		virtual EventType getEventType() const = 0;
		virtual std::string toString() const = 0;
	};

	class EventDispatcher
	{
	public:
		EventDispatcher(Event& event) : event_(event) {}

		template<typename T, typename F>
		bool Dispatch(const F& func)
		{
			if (event_.getEventType() == T::GetStaticType()) {
				event_.isHandled_ |= func(static_cast<T&>(event_));
				return true;
			}
			return false;
		}

	private:
		Event& event_;
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.toString();
	}
}