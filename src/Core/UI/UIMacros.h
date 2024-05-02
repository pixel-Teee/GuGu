#pragma once

#include <memory>
#include <functional>

#include "Clipping.h"
#include "Visibility.h"

namespace GuGu {
	template<typename...>
	struct functionTraits;

	template<typename R, typename Class, typename... Args>
	struct functionTraits<Class, std::function<R(Args...)>> {
		using ConstMethod = R(Class::*)(Args...) const;
		using Method = R(Class::*)(Args...);
		static const uint32_t ParamCount = sizeof...(Args);
	};

	namespace RequiredArgs
	{
		struct T0RequiredArgs
		{
			T0RequiredArgs()
			{

			}

			template<class WidgetType>
			void callConstruct(const std::shared_ptr<WidgetType>& onWidget, const typename WidgetType::BuilderArguments& withNamedArgs) const
			{
				onWidget->init(withNamedArgs);
			}
		};

		template<typename Arg0Type>
		struct T1RequiredArgs
		{
			T1RequiredArgs(Arg0Type&& inArg0)
				: m_arg0(inArg0)
			{}

			template<class WidgetType>
			void callConstruct(const std::shared_ptr<WidgetType>& onWidget, const typename WidgetType::BuilderArguments& withNamedArgs) const
			{
				onWidget->init(withNamedArgs, std::forward<Arg0Type>(m_arg0));
			}
			Arg0Type& m_arg0;
		};

		inline T0RequiredArgs makeRequiredArgs() //这个inline很重要
		{
			return T0RequiredArgs();
		}

		template<typename Arg0Type>
		T1RequiredArgs<Arg0Type&&> makeRequiredArgs(Arg0Type&& inArg0)
		{
			return T1RequiredArgs<Arg0Type&&>(std::forward<Arg0Type>(inArg0));
		}
	}

	template<typename WidgetType, typename RequiredArgsPayloadType>
	struct WidgetConstruct
	{
		WidgetConstruct(const GuGuUtf8Str& typeString, const GuGuUtf8Str& createLocation, int32_t onLine, RequiredArgsPayloadType&& inRequiredArgs)
			: m_requiredArgs(inRequiredArgs)	
		{ 
			m_widget = std::make_shared<WidgetType>(); 
			m_widget->setDebugInfo(typeString, createLocation, onLine);
		}
		~WidgetConstruct() = default;
		std::shared_ptr<WidgetType> operator<<(const typename WidgetType::BuilderArguments& arguments)
		{
			//m_widget->init(arguments);
			m_requiredArgs.callConstruct(m_widget, arguments);
			return m_widget;
		}
		std::shared_ptr<WidgetType> m_widget;
		RequiredArgsPayloadType& m_requiredArgs;
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
	BuilderArguments& EventName(const Type& inFunctor) \
	{\
		m##EventName = inFunctor; \
		return static_cast<BuilderArguments*>(this)->Me(); \
	}\
	BuilderArguments& EventName##Lambda(Type&& inFunctor) \
	{\
		m##EventName = std::move(inFunctor); \
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

#define WIDGET_NEW(WidgetType, ...) \
	makeDecl<WidgetType>(#WidgetType, __FILE__, __LINE__, RequiredArgs::makeRequiredArgs(__VA_ARGS__)) << typename WidgetType::BuilderArguments()

#define WIDGET_ASSIGN_NEW(WidgetType, OutValue, ...) \
	OutValue = makeDecl<WidgetType>(#WidgetType, __FILE__, __LINE__, RequiredArgs::makeRequiredArgs(__VA_ARGS__)) << typename WidgetType::BuilderArguments()

	template<typename WidgetType, typename RequiredArgsPayloadType>
	WidgetConstruct<WidgetType, RequiredArgsPayloadType> makeDecl(const GuGuUtf8Str& typeStr, const GuGuUtf8Str& createLocation, int32_t line, RequiredArgsPayloadType&& inRequiredArgs)
	{
		return WidgetConstruct<WidgetType, RequiredArgsPayloadType>(typeStr, createLocation, line, std::forward<RequiredArgsPayloadType>(inRequiredArgs));
	}

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

		typename WidgetType::BuilderArguments& visibility(Visibility inVisibility)
		{
			mVisibility = inVisibility; return Me();
		}
		WidgetClipping mClip = WidgetClipping::Inherit;

		Visibility mVisibility = Visibility::Visible;
	};
}