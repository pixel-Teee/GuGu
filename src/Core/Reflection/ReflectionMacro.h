#pragma once

//namespace GuGu {
#define STRINGIFY(x)   #x
#define CONCAT_SUFFIX(X) STRINGIFY(x) "_Fields"

#define DECLARE_INITIAL \
	public:\
		static bool registerMainFactory();\
		static bool ms_bRegisterMainFactory;\
		static Priority ms_priority;\

#define DECLARE_INITIAL_FIELDS \
	public:\
		static bool registerMainFactory2();\
		static bool ms_bRegisterMainFactory2;\
		static Priority ms_priority2;\

#define IMPLEMENT_INITIAL_BEGIN(classname)\
	bool classname::ms_bRegisterMainFactory = false;\
	Priority classname::ms_priority;\
	static bool gs_bStreamRegistered_##classname = classname::registerMainFactory(); \
	bool classname::registerMainFactory()\
	{\
		if(!ms_bRegisterMainFactory)\
		{\
			classname::ms_priority.setDebugName(#classname);
#define IMPLEMENT_INITIAL_END\
			ms_bRegisterMainFactory = true;\
		}\
		return ms_bRegisterMainFactory;\
	}
#define ADD_INITIAL_FUNCTION_WITH_PRIORITY(function_name)\
	ReflectionMain::addInitialTypeFunction(function_name, &ms_priority);
#define ADD_PRIORITY(classname)\
	if(!ms_priority.addPriorityThan(&classname::ms_priority))\
		return 0;

#define ADD_INITIAL_FIELDS_FUNCTION_WITH_PRIORITY(function_name)\
	ReflectionMain::addInitialTypeFunction(function_name, &ms_priority2);
#define ADD_PRIORITY_FIELDS(classname)\
	if(!ms_priority2.addPriorityThan(&classname::ms_priority))\
		return 0;

#define IMPLEMENT_INITIAL_FIELDS_BEGIN(classname)\
	bool classname::ms_bRegisterMainFactory2 = false;\
	Priority classname::ms_priority2;\
	static bool gs_bStreamRegistered_##classname2 = classname::registerMainFactory2(); \
	bool classname::registerMainFactory2()\
	{\
		if(!ms_bRegisterMainFactory2)\
		{\
			classname::ms_priority2.addPriorityThan(&classname::ms_priority);\
			classname::ms_priority2.setDebugName(CONCAT_SUFFIX(classname));
#define IMPLEMENT_INITIAL_FIELDS_END\
			ms_bRegisterMainFactory2 = true;\
		}\
		return ms_bRegisterMainFactory2;\
	}

