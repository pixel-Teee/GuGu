#pragma once

#include <memory>
#include <functional>

#include "Clipping.h"
#include "Visibility.h"

namespace GuGu {
	//clang 支持不加 typename，但是 msvc 需要，如果WidgetType是个模板类
#if !defined(__clang__)
	#define TYPENAME_OUTSIDE_TEMPLATE
#endif

#ifndef TYPENAME_OUTSIDE_TEMPLATE
	#define TYPENAME_OUTSIDE_TEMPLATE typename
#endif

	template<typename...>
	struct functionTraits;

	template<typename R, typename Class, typename... Args, typename... Vars>
	struct functionTraits<Class, std::function<R(Args...)>, Vars...> {
		using ConstMethod = R(Class::*)(Args..., Vars...) const;
		using Method = R(Class::*)(Args..., Vars...);
		static const uint32_t ParamCount = sizeof...(Args) + sizeof...(Vars);
	};

	template<typename...>
	struct staticFunctionTraits;

	template<typename R, typename... Args, typename... Vars>
	struct staticFunctionTraits<std::function<R(Args...)>, Vars...> {
		using StaticFunc = R(Args..., Vars...);
		static const uint32_t ParamCount = sizeof...(Args) + sizeof...(Vars);
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

		template<typename Arg0Type, typename Arg1Type>
		struct T2RequiredArgs
		{
			T2RequiredArgs(Arg0Type&& inArg0, Arg1Type&& inArg1)
				: m_arg0(inArg0)
				, m_arg1(inArg1)
			{}

			template<class WidgetType>
			void callConstruct(const std::shared_ptr<WidgetType>& onWidget, const typename WidgetType::BuilderArguments& withNamedArgs) const
			{
				onWidget->init(withNamedArgs, std::forward<Arg0Type>(m_arg0), std::forward<Arg1Type>(m_arg1));
			}
			Arg0Type& m_arg0;
			Arg1Type& m_arg1;
		};

		template<typename Arg0Type, typename Arg1Type>
		T2RequiredArgs<Arg0Type&&, Arg1Type&&> makeRequiredArgs(Arg0Type&& inArg0, Arg1Type&& inArg1)
		{
			return T2RequiredArgs<Arg0Type&&, Arg1Type&&>(std::forward<Arg0Type>(inArg0), std::forward<Arg1Type>(inArg1));
		}

		template<typename Arg0Type, typename Arg1Type, typename Arg2Type>
		struct T3RequiredArgs
		{
			T3RequiredArgs(Arg0Type&& inArg0, Arg1Type&& inArg1, Arg2Type&& inArg2)
				: m_arg0(inArg0)
				, m_arg1(inArg1)
				, m_arg2(inArg2)
			{}

			template<class WidgetType>
			void callConstruct(const std::shared_ptr<WidgetType>& onWidget, const typename WidgetType::BuilderArguments& withNamedArgs) const
			{
				onWidget->init(withNamedArgs, std::forward<Arg0Type>(m_arg0), std::forward<Arg1Type>(m_arg1), std::forward<Arg2Type>(m_arg2));
			}
			Arg0Type& m_arg0;
			Arg1Type& m_arg1;
			Arg2Type& m_arg2;
		};

		template<typename Arg0Type, typename Arg1Type, typename Arg2Type>
		T3RequiredArgs<Arg0Type&&, Arg1Type&&, Arg2Type&&> makeRequiredArgs(Arg0Type&& inArg0, Arg1Type&& inArg1, Arg2Type&& inArg2)
		{
			return T3RequiredArgs<Arg0Type&&, Arg1Type&&, Arg2Type&&>(std::forward<Arg0Type>(inArg0), std::forward<Arg1Type>(inArg1), std::forward<Arg2Type>(inArg2));
		}

		template<typename Arg0Type, typename Arg1Type, typename Arg2Type, typename Arg3Type>
		struct T4RequiredArgs
		{
			T4RequiredArgs(Arg0Type&& inArg0, Arg1Type&& inArg1, Arg2Type&& inArg2, Arg3Type&& inArg3)
				: m_arg0(inArg0)
				, m_arg1(inArg1)
				, m_arg2(inArg2)
				, m_arg3(inArg3)
			{}

			template<class WidgetType>
			void callConstruct(const std::shared_ptr<WidgetType>& onWidget, const typename WidgetType::BuilderArguments& withNamedArgs) const
			{
				onWidget->init(withNamedArgs, std::forward<Arg0Type>(m_arg0), std::forward<Arg1Type>(m_arg1), std::forward<Arg2Type>(m_arg2), std::forward<Arg3Type>(m_arg3));
			}
			Arg0Type& m_arg0;
			Arg1Type& m_arg1;
			Arg2Type& m_arg2;
			Arg3Type& m_arg3;
		};

		template<typename Arg0Type, typename Arg1Type, typename Arg2Type, typename Arg3Type>
		T4RequiredArgs<Arg0Type&&, Arg1Type&&, Arg2Type&&, Arg3Type&&> makeRequiredArgs(Arg0Type&& inArg0, Arg1Type&& inArg1, Arg2Type&& inArg2, Arg3Type&& inArg3)
		{
			return T4RequiredArgs<Arg0Type&&, Arg1Type&&, Arg2Type&&, Arg3Type&&>(std::forward<Arg0Type>(inArg0), std::forward<Arg1Type>(inArg1), std::forward<Arg2Type>(inArg2), std::forward<Arg3Type>(inArg3));
		}

		template<typename Arg0Type, typename Arg1Type, typename Arg2Type, typename Arg3Type, typename Arg4Type>
		struct T5RequiredArgs
		{
			T5RequiredArgs(Arg0Type&& inArg0, Arg1Type&& inArg1, Arg2Type&& inArg2, Arg3Type&& inArg3, Arg4Type&& inArg4)
				: m_arg0(inArg0)
				, m_arg1(inArg1)
				, m_arg2(inArg2)
				, m_arg3(inArg3)
				, m_arg4(inArg4)
			{}

			template<class WidgetType>
			void callConstruct(const std::shared_ptr<WidgetType>& onWidget, const typename WidgetType::BuilderArguments& withNamedArgs) const
			{
				onWidget->init(withNamedArgs, std::forward<Arg0Type>(m_arg0), std::forward<Arg1Type>(m_arg1), std::forward<Arg2Type>(m_arg2), std::forward<Arg3Type>(m_arg3), std::forward<Arg4Type>(m_arg4));
			}
			Arg0Type& m_arg0;
			Arg1Type& m_arg1;
			Arg2Type& m_arg2;
			Arg3Type& m_arg3;
			Arg4Type& m_arg4;
		};

		template<typename Arg0Type, typename Arg1Type, typename Arg2Type, typename Arg3Type, typename Arg4Type>
		T5RequiredArgs<Arg0Type&&, Arg1Type&&, Arg2Type&&, Arg3Type&&, Arg4Type&&> makeRequiredArgs(Arg0Type&& inArg0, Arg1Type&& inArg1, Arg2Type&& inArg2, Arg3Type&& inArg3, Arg4Type&& inArg4)
		{
			return T5RequiredArgs<Arg0Type&&, Arg1Type&&, Arg2Type&&, Arg3Type&&, Arg4Type&&>(std::forward<Arg0Type>(inArg0), std::forward<Arg1Type>(inArg1), std::forward<Arg2Type>(inArg2), std::forward<Arg3Type>(inArg3), std::forward<Arg4Type>(inArg4));
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
//这个相对于ARGUMENT_SLOT，ARGUMENT_SLOT是槽参数，槽参数存放了槽指针，而这个则是直接构造，将参数作为槽的参数放入
#define ARGUMENT_SUPPORTS_SLOT_WITH_ARGS(Type) \
	std::vector<std::shared_ptr<Type>> m_slots;\
	BuilderArguments& operator+(const typename Type::BuilderArguments& argumentsForNewSlot) \
	{ \
		m_slots.push_back(std::make_shared<Type>(argumentsForNewSlot));\
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
	template<class Class>\
	BuilderArguments& Name(const Class* inObject, Type(Class::*inConstMethodPtr)()const) \
	{\
		m##Name = Attribute<Type>::CreateSP(inObject, inConstMethodPtr);\
		return static_cast<BuilderArguments*>(this)->Me();\
	}\
	template<class Class, typename Var1Type>\
	BuilderArguments& Name(Class* inObject, Type(Class::*inConstMethodPtr)(Var1Type)const, Var1Type value1) \
	{\
		m##Name = Attribute<Type>::CreateSP(inObject, inConstMethodPtr, value1);\
		return static_cast<BuilderArguments*>(this)->Me();\
	}\
	template<class Class, typename Var1Type>\
	BuilderArguments& Name(const Class* inObject, Type(Class::*inConstMethodPtr)(Var1Type)const, Var1Type value1) \
	{\
		m##Name = Attribute<Type>::CreateSP(inObject, inConstMethodPtr, value1);\
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
	template<typename StaticFuncPtr, typename... VarTypes>\
	BuilderArguments& EventName##Static(StaticFuncPtr inFunc, VarTypes... vars)\
	{\
		constexpr int32_t paramCount = staticFunctionTraits<Type, VarTypes...>::ParamCount - sizeof...(VarTypes);\
		if constexpr (paramCount == 0) \
			m##EventName = std::bind(inFunc, std::forward<VarTypes>(vars)...); \
		else if constexpr (paramCount == 1) \
			m##EventName = std::bind(inFunc, std::placeholders::_1, std::forward<VarTypes>(vars)...); \
		else if constexpr (paramCount == 2) \
			m##EventName = std::bind(inFunc, std::placeholders::_1, std::placeholders::_2, std::forward<VarTypes>(vars)...); \
		return static_cast<BuilderArguments*>(this)->Me(); \
	}\
	template<class Class, typename... VarTypes>\
	BuilderArguments& EventName(Class* inObject, typename functionTraits<Class, Type, VarTypes...>::ConstMethod inConstMethodPtr, VarTypes... vars) \
	{\
		constexpr int32_t paramCount = functionTraits<Class, Type, VarTypes...>::ParamCount - sizeof...(VarTypes);\
		if constexpr (paramCount == 0) \
			m##EventName = std::bind(inConstMethodPtr, inObject, std::forward<VarTypes>(vars)...); \
		else if constexpr (paramCount == 1) \
			m##EventName = std::bind(inConstMethodPtr, inObject, std::placeholders::_1, std::forward<VarTypes>(vars)...); \
		else if constexpr (paramCount == 2) \
			m##EventName = std::bind(inConstMethodPtr, inObject, std::placeholders::_1, std::placeholders::_2, std::forward<VarTypes>(vars)...); \
		else if constexpr (paramCount == 3) \
			m##EventName = std::bind(inConstMethodPtr, inObject, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::forward<VarTypes>(vars)...); \
		else if constexpr (paramCount == 4) \
			m##EventName = std::bind(inConstMethodPtr, inObject, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::forward<VarTypes>(vars)...); \
		return static_cast<BuilderArguments*>(this)->Me(); \
	}\
	template<class Class, typename... VarTypes>\
	BuilderArguments& EventName(Class* inObject, typename functionTraits<Class, Type, VarTypes...>::Method inMethodPtr, VarTypes... vars) \
	{\
		constexpr int32_t paramCount = functionTraits<Class, Type, VarTypes...>::ParamCount - sizeof...(VarTypes);\
		if constexpr (paramCount == 0) \
			m##EventName = std::bind(inMethodPtr, inObject, std::forward<VarTypes>(vars)...); \
		else if constexpr (paramCount == 1) \
			m##EventName = std::bind(inMethodPtr, inObject, std::placeholders::_1, std::forward<VarTypes>(vars)...); \
		else if constexpr (paramCount == 2) \
			m##EventName = std::bind(inMethodPtr, inObject, std::placeholders::_1, std::placeholders::_2, std::forward<VarTypes>(vars)...); \
		else if constexpr (paramCount == 3) \
			m##EventName = std::bind(inMethodPtr, inObject, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::forward<VarTypes>(vars)...); \
		else if constexpr (paramCount == 4) \
			m##EventName = std::bind(inMethodPtr, inObject, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::forward<VarTypes>(vars)...); \
		return static_cast<BuilderArguments*>(this)->Me(); \
	}
#define SLOT_CONTENT(SlotType, Name) \
	std::shared_ptr<SlotType> m##Name;

#define WIDGET_NEW(WidgetType, ...) \
	makeDecl<WidgetType>(#WidgetType, __FILE__, __LINE__, RequiredArgs::makeRequiredArgs(__VA_ARGS__)) << TYPENAME_OUTSIDE_TEMPLATE WidgetType::BuilderArguments()

#define WIDGET_ASSIGN_NEW(WidgetType, OutValue, ...) \
	OutValue = makeDecl<WidgetType>(#WidgetType, __FILE__, __LINE__, RequiredArgs::makeRequiredArgs(__VA_ARGS__)) << TYPENAME_OUTSIDE_TEMPLATE WidgetType::BuilderArguments()

	template<typename WidgetType, typename RequiredArgsPayloadType>
	WidgetConstruct<WidgetType, RequiredArgsPayloadType> makeDecl(const GuGuUtf8Str& typeStr, const GuGuUtf8Str& createLocation, int32_t line, RequiredArgsPayloadType&& inRequiredArgs)
	{
		return WidgetConstruct<WidgetType, RequiredArgsPayloadType>(typeStr, createLocation, line, std::forward<RequiredArgsPayloadType>(inRequiredArgs));
	}

	template<typename WidgetType>
	struct Arguments
	{
		typedef typename WidgetType::BuilderArguments WidgetArgsType;
		WidgetArgsType& Me()
		{
			return *(static_cast<WidgetArgsType*>(this));
		}

		WidgetArgsType& Clip(WidgetClipping inValue) { //todo:fix this
			mClip = inValue; return Me();
		} 

		WidgetArgsType& visibility(Attribute<Visibility> inVisibility)
		{
			mVisibility = inVisibility; return Me();
		}
		WidgetClipping mClip = WidgetClipping::Inherit;

		Attribute<Visibility> mVisibility = Visibility::SelfHitTestInvisible;
	};
}