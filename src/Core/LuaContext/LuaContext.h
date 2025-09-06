#pragma once

#include <Core/Reflection/Variant.h>

extern "C" {
	#include "lua.h"
	#include "lauxlib.h"
	#include "lualib.h"
}

struct lua_State;
namespace GuGu {
	class RootFileSystem;
	//class meta::Variant;
	class LuaContext 
	{
	public:
		LuaContext();

		~LuaContext();

		bool initialize();

		void shutDown();

		bool executeString(const GuGuUtf8Str& code);

		static std::shared_ptr<LuaContext> getLuaContext();

		std::shared_ptr<RootFileSystem> getFileSystem();

		lua_State* getLuaState();

		static void pushVariantToLua(lua_State* L, const meta::Variant& value);

		static meta::Variant luaToVariant(lua_State* L, int index);

		static void debugStack(lua_State* L, const char* message);
	private:
		lua_State* m_state = nullptr;

		std::shared_ptr<RootFileSystem> m_rootFileSystem;
	};
}