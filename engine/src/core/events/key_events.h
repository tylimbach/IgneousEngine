#pragma once

#include "key_codes.h"
#include "event.h"

#include <string>

namespace bve
{
	class KeyPressedEvent : public Event
	{
	public:
		KeyPressedEvent(const KeyCode keyCode, bool isRepeated = false)
			: keyCode_(keyCode), isRepeated_(isRepeated) {}

		EventType getEventType() const override { return EventType::KeyPressed; }
		bool isRepeated() const { return isRepeated_; }

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << keyCode_ << " (repeat = " << isRepeated_ << ")";
			return ss.str();
		}

	private:
		KeyCode keyCode_;
		bool isRepeated_;
	};


	GunSystem::equip(Gun gun)
	{
		// todo: how can i generically support passing a filter into the listen function (optionally)?
		EventHandle handle;
		EventManager<KeyPressedEvent>::listen(handle&);
	}

	GunSystem::unequip(Gun gun)
	{
		EventManager<KeyPressedEvent>::unlisten();
	}

	GunSystem::checkReload()
	{
		EventManager<KeyPressedEvent>::process
	}

}
