#pragma once

struct lua_State;
namespace GuGu {
	class LuaContext 
	{
	public:
		LuaContext();

		~LuaContext();

		bool initialize();

		void shutDown();

		bool executeString(const GuGuUtf8Str& code);

		static std::shared_ptr<LuaContext> getLuaContext();
	private:
		lua_State* m_state = nullptr;
	};
}