#pragma once

struct lua_State;
namespace GuGu {
	class RootFileSystem;
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
	private:
		lua_State* m_state = nullptr;

		std::shared_ptr<RootFileSystem> m_rootFileSystem;
	};
}