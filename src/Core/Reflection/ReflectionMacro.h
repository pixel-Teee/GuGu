#pragma once

//namespace GuGu {

#define DECLARE_INITIAL \
	public:\
		static bool registerMainFactory();\
		static bool ms_bRegisterMainFactory;\
		static Priority ms_priority;\

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
//}