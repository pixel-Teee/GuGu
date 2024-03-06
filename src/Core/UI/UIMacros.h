#pragma once

#include <memory>

namespace GuGu {

	template<typename WidgetType>
	struct WidgetConstruct
	{
		WidgetConstruct() { m_widget = std::make_shared<WidgetType>(); }
		~WidgetConstruct() = default;
		std::shared_ptr<WidgetType> operator<<(typename WidgetType::BuilderArguments& arguments)
		{
			m_widget->init(arguments);
			return m_widget;
		}
		std::shared_ptr<WidgetType> m_widget;
	};
#define ARGUMENT_MEMBER(Type, Name)\
	BuilderArguments& Name(std::shared_ptr<Type> inValue) \
 	{\
		m##Name = inValue;\
		return *this;\
	}\
	std::shared_ptr<Type> m##Name;
#define ARGUMENT_VALUE(Type, Name)\
	BuilderArguments& Name(Type inValue) \
 	{\
		m##Name = inValue;\
		return *this;\
	}\
	Type m##Name;
#define ARGUMENT_SLOT(Type, Name)\
	BuilderArguments& Name(std::shared_ptr<Widget> inValue) \
 	{\
		m##Name->setChildWidget(inValue);\
		return *this;\
	}\
	std::shared_ptr<Type> m##Name = std::make_shared<Type>();
#define SLOT_CONTENT(SlotType, Name) \
	std::shared_ptr<SlotType> m##Name;

#define WIDGET_NEW(WidgetType) \
	WidgetConstruct<WidgetType>() << WidgetType::BuilderArguments()

#define WIDGET_ASSIGN_NEW(WidgetType, OutValue) \
	OutValue = WidgetConstruct<WidgetType>() << WidgetType::BuilderArguments()


}