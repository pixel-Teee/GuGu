#include <pch.h>

#include "HeaderRow.h"

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

	}

}