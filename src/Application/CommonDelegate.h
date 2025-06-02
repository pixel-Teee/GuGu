#pragma once

#include <Core/UI/Reply.h>
#include <Core/UI/Events.h>

namespace GuGu {
	using OnKeyEvent = std::function<Reply(const KeyEvent&)>;
}