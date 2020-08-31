#pragma once
#include "Event.h"

namespace Doge
{
	class MouseButtonEvent : public Event
	{
	public:
		inline int GetMouseButton() const { return m_MouseCode; }

		EVENT_CLASS_CATEGORY(EventCategoryMouseButton | EventCategoryInput);
	protected:
		MouseButtonEvent(int mouseCode)
			: m_MouseCode(mouseCode) {}

		int m_MouseCode;
	};

	class MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(int mouseCode)
			: MouseButtonEvent(mouseCode) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << m_MouseCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonPressed)
	};

	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(int mouseCode)
			: MouseButtonEvent(mouseCode) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent: " << m_MouseCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonReleased)
	};

	class MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(float xpos, float ypos)
			: m_MousePos(xpos, ypos) {}


		inline const std::pair<float, float>& GetPos() const { return m_MousePos; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent: " << m_MousePos.first << ", " << m_MousePos.second;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseMoved)
			EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput);
	private:
		std::pair<float, float> m_MousePos;
	};

	class MouseScrolledEvent : public Event
	{
	public:
		MouseScrolledEvent(float xoffset, float yoffset)
			: m_MouseScrollOffset(xoffset, yoffset) {}

		inline const std::pair<float, float>& GetOffset() const { return m_MouseScrollOffset; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseScrolledEvent: " << m_MouseScrollOffset.first << ", " << m_MouseScrollOffset.second;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseScrolled)
			EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput);
	private:
		std::pair<float, float> m_MouseScrollOffset;
	};
}