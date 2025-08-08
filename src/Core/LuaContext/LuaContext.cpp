#include <pch.h>

#include "LuaContext.h"

#include <Core/FileSystem/FileSystem.h>
#include <Application/Application.h>

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

	//lua reader
	static const char* customLuaReader(lua_State* L, void* data, size_t* size)
	{
		std::shared_ptr<LuaContext> luaContext = *static_cast<std::shared_ptr<LuaContext>*>(data);
		if (luaContext != nullptr)
		{
			int32_t numberOfBytesHaveReaded = 0;
			static char buffer[1024];
			memset(buffer, 0, sizeof(buffer));
			luaContext->getFileSystem()->ReadFile(buffer, sizeof(buffer), numberOfBytesHaveReaded);

			//convert to utf8
			//static GuGuUtf8Str utf8Str;
			//utf8Str = buffer;
			*size = numberOfBytesHaveReaded;

			return (numberOfBytesHaveReaded > 0) ? buffer : nullptr;
		}
	}

	static GuGuUtf8Str convertModuleToPath(const char* moduleName)
	{
		std::string path(moduleName);
		std::replace(path.begin(), path.end(), '.', '/');
		return path + ".lua";
	}

	static int32_t customLuaLoader(lua_State* L)
	{
		const char* moduleName = luaL_checkstring(L, 1);

		GuGuUtf8Str path = convertModuleToPath(moduleName);

		//read
		std::shared_ptr<LuaContext> luaContext = LuaContext::getLuaContext();
		luaContext->getFileSystem()->OpenFile(path, GuGuFile::FileMode::OnlyRead);
		
		int32_t status = lua_load(L, customLuaReader, &luaContext, path.getStr(), NULL);
		luaContext->getFileSystem()->CloseFile();

		if(status == LUA_OK)
			return 1;
		else
			return lua_error(L);
	}

	static void registerCustomLoader(lua_State* L)
	{
		lua_getglobal(L, "package");
		lua_getfield(L, -1, "searchers");

		lua_pushcfunction(L, customLuaLoader);
		for (int i = lua_rawlen(L, -2) + 1; i > 1; --i) {
			lua_rawgeti(L, -2, i - 1);
			lua_rawseti(L, -3, i);
		}
		lua_rawseti(L, -2, 1);

		//clear stack
		lua_pop(L, 2);
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
		GuGuUtf8Str contentPath = Application::GetContentDirectoryWithExecutable();

		std::shared_ptr<NativeFileSystem> nativeFileSystem = std::make_shared<NativeFileSystem>(contentPath);
		m_rootFileSystem = std::make_shared<RootFileSystem>();
		m_rootFileSystem->mount("content", nativeFileSystem);

		m_state = luaL_newstate();
		if(!m_state) return false;
		luaL_openlibs(m_state);

		registerCustomLoader(m_state);

		//register
		lua_pushcfunction(m_state, LuaLog);
		lua_setglobal(m_state, "LuaLog");

		if (luaL_dostring(m_state, "LuaLog('register first function success!')"))
		{
			GuGu_LOGE("register first function error");
		}

		//test read file
		lua_getglobal(m_state, "require");
		lua_pushstring(m_state, "content/testModule");
		if (lua_pcall(m_state, 1, 1, 0) != LUA_OK)
		{
			GuGu_LOGE("%s", lua_tostring(m_state, -1));
			lua_pop(m_state, 1);
		}

		lua_getfield(m_state, -1, "main");
		lua_remove(m_state, -2);

		if (lua_pcall(m_state, 0, 0, 0) != LUA_OK)
		{
			GuGu_LOGE("%s", lua_tostring(m_state, -1));
			lua_pop(m_state, 1);
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

	std::shared_ptr<GuGu::RootFileSystem> LuaContext::getFileSystem()
	{
		return m_rootFileSystem;
	}

}