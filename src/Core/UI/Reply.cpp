#include <pch.h>

#include "Reply.h"

namespace GuGu {
    Reply& Reply::setFocus(std::shared_ptr<Widget> giveMeFocus)
    {
        this->m_bSetUserFocus = true;
        this->m_focusRecipient = giveMeFocus;
        return Me();
    }
}