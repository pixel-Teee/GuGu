#include <pch.h>

#include "Reply.h"

namespace GuGu {
    Reply& Reply::setFocus(std::shared_ptr<Widget> giveMeFocus)
    {
        this->m_bSetUserFocus = true;
        this->m_focusRecipient = giveMeFocus;
        return Me();
    }
    Reply& Reply::beginDragDrop(std::shared_ptr<DragDropOperation> inDragDropContent)
    {
		this->m_dragDropContent = inDragDropContent;
		return Me();
    }
    Reply& Reply::endDragDrop()
    {
        this->m_bEndDragDrop = true;
        return Me();
    }
}