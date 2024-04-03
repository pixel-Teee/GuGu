#pragma once

#include <memory>
#include <functional>

#include "Clipping.h"

namespace GuGu {
	template<typename...>
	struct functionTraits;

	template<typename R, typename Class, typename... Args>
	struct functionTraits<Class, std::function<R(Args...)>> {
		using ConstMethod = R(Class::*)(Args...) const;
		using Method = R(Class::*)(Args...);
		static const uint32_t ParamCount = sizeof...(Args);
	};

	template<typename WidgetType>
	struct WidgetConstruct
	{
		WidgetConstruct() { m_widget = std::make_shared<WidgetType>(); }
		~WidgetConstruct() = default;
		std::shared_ptr<WidgetType> operator<<(const typename WidgetType::BuilderArguments& arguments)
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
		return static_cast<BuilderArguments*>(this)->Me();\
	}\
	std::shared_ptr<Type> m##Name;
#define ARGUMENT_VALUE(Type, Name)\
	BuilderArguments& Name(Type inValue) \
 	{\
		m##Name = inValue;\
		return static_cast<BuilderArguments*>(this)->Me();\
	}\
	Type m##Name;
#define ARGUMENT_NAMED_SLOT(Type, Name)\
	BuilderArguments& Name(std::shared_ptr<Widget> inValue) \
 	{\
		m##Name->setChildWidget(inValue);\
		return static_cast<BuilderArguments*>(this)->Me();\
	}\
	std::shared_ptr<Type> m##Name = std::make_shared<Type>();
#define ARGUMENT_SLOT(Type, Name)\
	std::vector<typename Type::SlotBuilderArguments> m##Name;\
	BuilderArguments& operator+(typename Type::SlotBuilderArguments& inValue) \
 	{\
		m##Name.push_back(inValue);\
		return static_cast<BuilderArguments*>(this)->Me();\
	}\
	BuilderArguments& operator+(typename Type::SlotBuilderArguments&& inValue) \
 	{\
		m##Name.push_back(std::move(inValue));\
		return static_cast<BuilderArguments*>(this)->Me();\
	}
#define ARGUMENT_ATTRIBUTE(Type, Name)\
	Attribute<Type> m##Name;\
	BuilderArguments& Name(Attribute<Type> inValue)\
	{\
		m##Name = inValue;\
		return static_cast<BuilderArguments*>(this)->Me(); \
	}\
	template<class Class>\
	BuilderArguments& Name(std::shared_ptr<Class> inObject, Type(Class::*inConstMethodPtr)()const) \
	{\
		m##Name = Attribute<Type>::CreateSP(inObject.get(), inConstMethodPtr);\
		return static_cast<BuilderArguments*>(this)->Me();\
	}\
	template<class Class>\
	BuilderArguments& Name(Class* inObject, Type(Class::*inConstMethodPtr)()const) \
	{\
		m##Name = Attribute<Type>::CreateSP(inObject, inConstMethodPtr);\
		return static_cast<BuilderArguments*>(this)->Me();\
	}\
	BuilderArguments& Name##Lambda(std::function<Type(void)>&& inFunctor) \
	{\
		m##Name = Attribute<Type>::CreateLambda(inFunctor); \
		return static_cast<BuilderArguments*>(this)->Me(); \
	}
#define UI_EVENT(Type, EventName)\
	Type m##EventName;\
	BuilderArguments& EventName##Lambda(Type&& inFunctor) \
	{\
		m##EventName = inFunctor; \
		return static_cast<BuilderArguments*>(this)->Me(); \
	}\
	template<class Class>\
	BuilderArguments& EventName(Class* inObject, typename functionTraits<Class, Type>::ConstMethod inConstMethodPtr) \
	{\
		if constexpr (functionTraits<Class, Type>::ParamCount == 0) \
			m##EventName = std::bind(inConstMethodPtr, inObject); \
		else if constexpr (functionTraits<Class, Type>::ParamCount == 1) \
			m##EventName = std::bind(inConstMethodPtr, inObject, std::placeholders::_1); \
		else if constexpr (functionTraits<Class, Type>::ParamCount == 2) \
			m##EventName = std::bind(inConstMethodPtr, inObject, std::placeholders::_1, std::placeholders::_2); \
		return static_cast<BuilderArguments*>(this)->Me(); \
	}\
	template<class Class>\
	BuilderArguments& EventName(Class* inObject, typename functionTraits<Class, Type>::Method inMethodPtr) \
	{\
		if constexpr (functionTraits<Class, Type>::ParamCount == 0) \
			m##EventName = std::bind(inMethodPtr, inObject); \
		else if constexpr (functionTraits<Class, Type>::ParamCount == 1) \
			m##EventName = std::bind(inMethodPtr, inObject, std::placeholders::_1); \
		else if constexpr (functionTraits<Class, Type>::ParamCount == 2) \
			m##EventName = std::bind(inMethodPtr, inObject, std::placeholders::_1, std::placeholders::_2); \
			return static_cast<BuilderArguments*>(this)->Me(); \
	}
#define SLOT_CONTENT(SlotType, Name) \
	std::shared_ptr<SlotType> m##Name;

#define WIDGET_NEW(WidgetType) \
	WidgetConstruct<WidgetType>() << WidgetType::BuilderArguments()

#define WIDGET_ASSIGN_NEW(WidgetType, OutValue) \
	OutValue = WidgetConstruct<WidgetType>() << WidgetType::BuilderArguments()


	template<typename WidgetType>
	struct Arguments
	{
		typename WidgetType::BuilderArguments& Me()
		{
			return *(static_cast<typename WidgetType::BuilderArguments*>(this));
		}

		typename WidgetType::BuilderArguments& Clip(WidgetClipping inValue) { //todo:fix this
			mClip = inValue; return Me();
		} 
		WidgetClipping mClip = WidgetClipping::Inherit;
	};
}