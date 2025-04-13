#pragma once

#include "DetailCategoryBuilder.h"//IDetailLayoutRow
#include <Core/UI/BasicElement.h>
#include <Core/UI/NullWidget.h>

namespace GuGu {
	class Widget;
	class DetailWidgetRow;
	class DetailWidgetDecl
	{
	public:
		DetailWidgetDecl(DetailWidgetRow& inParentDecl, float inMinWidth, float inMaxWidth, HorizontalAlignment inHAlign, VerticalAlignment inVAlign)
			: m_widget(NullWidget::getNullWidget())
			, m_horizontalAlignment(inHAlign)
			, m_verticalAlignment(inVAlign)
			, m_minWidth(inMinWidth)
			, m_maxWidth(inMaxWidth)
			, m_parentDecl(&inParentDecl)
		{}

		DetailWidgetRow& operator()(std::shared_ptr<Widget> inWidget)
		{
			m_widget = inWidget;
			return *m_parentDecl;
		}

		DetailWidgetDecl& setVerticalAlignment(VerticalAlignment inAlignment)
		{
			m_verticalAlignment = inAlignment;
			return *this;
		}

		DetailWidgetDecl& setHorizontalAlignment(HorizontalAlignment inAlignment)
		{
			m_horizontalAlignment = inAlignment;
			return *this;
		}

		DetailWidgetDecl& minFixedWidth(std::optional<float> minWidth)
		{
			m_minWidth = minWidth;
			return *this;
		}

		DetailWidgetDecl& maxFixedWidth(std::optional<float> maxWidth)
		{
			m_maxWidth = maxWidth;
			return *this;
		}
	public:
		std::shared_ptr<Widget> m_widget;
		HorizontalAlignment m_horizontalAlignment;
		VerticalAlignment m_verticalAlignment;
		std::optional<float> m_minWidth;
		std::optional<float> m_maxWidth;
	private:
		class DetailWidgetRow* m_parentDecl;
	};
	class DetailWidgetRow : public IDetailLayoutRow
	{
	public:
		DetailWidgetRow();

		virtual ~DetailWidgetRow();

		virtual GuGuUtf8Str getRowName() const override { return m_rowTagName; }

		DetailWidgetDecl& nameContent() { return m_nameWidget; }

		DetailWidgetDecl& valueContent() { return m_valueWidget; }
	public:
		//属性名
		DetailWidgetDecl m_nameWidget;
		//属性值
		DetailWidgetDecl m_valueWidget;

		GuGuUtf8Str m_rowTagName;
	};
}