#include <pch.h>

#include "LuaContext.h"

extern "C" {
	#include "lua.h"
	#include "lauxlib.h"
	#include "lualib.h"
}

namespace GuGu {

	int32_t LuaLog(lua_State* L) {
		const char* str = lua_tostring(L, -1);
		if (str != nullptr)
		{
			GuGu_LOGD("lua debug : %s", str);
		}
		else
		{
			GuGu_LOGD("lua debug error");
		}
		return 0;
	}

	LuaContext::LuaContext()
	{
		
	}

	LuaContext::~LuaContext()
	{
		shutDown();
	}

	bool LuaContext::initialize()
	{
		m_state = luaL_newstate();
		if(!m_state) return false;
		luaL_openlibs(m_state);

		//register
		lua_pushcfunction(m_state, LuaLog);
		lua_setglobal(m_state, "LuaLog");

		if (luaL_dostring(m_state, "LuaLog('register first function success!')"))
		{
			GuGu_LOGE("register first function error");
		}

		return true;
	}

	void LuaContext::shutDown()
	{
		if (m_state)
		{
			lua_close(m_state);
			m_state = nullptr;
		}
	}

	bool LuaContext::executeString(const GuGuUtf8Str& code)
	{
		if(luaL_dostring(m_state, code.getStr()))
			return false;
		return true;
	}

	std::shared_ptr<LuaContext> LuaContext::getLuaContext()
	{
		static std::shared_ptr<LuaContext> luaContext = std::make_shared<LuaContext>();
		return luaContext;
	}

}