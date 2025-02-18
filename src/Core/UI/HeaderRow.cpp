#include <pch.h>

#include "HeaderRow.h"
#include <Core/UI/Splitter.h>
#include <Core/UI/BoxPanel.h>

namespace GuGu {
    HeaderRow::HeaderRow()
    {
    }
    HeaderRow::~HeaderRow()
    {
    }

	void HeaderRow::init(const BuilderArguments& inArgs)
	{
        m_scrollBarThickness = math::float2(0, 0);
        m_scrollBarVisibility = Visibility::Collapsed;
        m_style = inArgs.mStyle;
        m_resizeMode = inArgs.mresizeMode;

        Border::init(Border::BuilderArguments()
            .padding(0)
            .brush(m_style->m_backgroundBrush)
        );

        bool bHaveFillerColumn = false;
		for (int32_t slotIndex = 0; slotIndex < inArgs.m_slots.size(); ++slotIndex)
		{
			std::shared_ptr<GColumn> column = inArgs.m_slots[slotIndex];
			m_columns.push_back(column);
		}
	}

	void HeaderRow::regenerateWidgets()
	{
        const float splitterHandleDetectionSize = 5.0f;

        std::shared_ptr<Splitter> splitter;

        //std::shared_ptr<HorizontalBox> box = 
	}

}