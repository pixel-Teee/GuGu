#include <pch.h>

#include "LuaContext.h"
#include "LuaTable.h"

#include <Core/FileSystem/FileSystem.h>
#include <Application/Application.h>
#include <Core/Reflection/Argument.h>
#include <Core/Reflection/ArgumentConfig.h>
#include <Core/Reflection/Method.h>
#include <Core/Reflection/Variant.h>

namespace GuGu {
	void LuaContext::debugStack(lua_State* L, const char* message)
	{
		int top = lua_gettop(L);
		GuGu_LOGD("message - stack top : %d\n", top);

		for (int i = 1; i <= top; i++) {
			int type = lua_type(L, i);
			GuGu_LOGD("%d typeName : %s", i, lua_typename(L, type));

			if (type == LUA_TSTRING) {
				//std::cout << " = '" << lua_tostring(L, i) << "'";
				GuGu_LOGD(" i : %s", lua_typename(L, type));
			}
			else if (type == LUA_TNUMBER) {
				//std::cout << " = " << lua_tonumber(L, i);
				GuGu_LOGD(" = %lf", lua_tonumber(L, i));
			}
			else if (type == LUA_TBOOLEAN) {
				GuGu_LOGD(" = %s", (lua_toboolean(L, i) ? "true" : "false"));
			}
			else if (type == LUA_TTABLE) {
				// 尝试获取表的元表
				if (lua_getmetatable(L, i)) {
					GuGu_LOGD("(has metatable)");
					lua_pop(L, 1); // 弹出元表
				}
				else {
					GuGu_LOGD("(no metatable)");
				}
			}

			GuGu_LOGD("\n");
		}
	}


	//------register function------
	meta::Variant LuaContext::luaToVariant(lua_State * L, int index)
	{
		int type = lua_type(L, index);
		switch (type)
		{
		case LUA_TNIL:
			return meta::Variant();//nothing
		case LUA_TBOOLEAN:
			return meta::Variant(lua_toboolean(L, index) != 0);
		case LUA_TNUMBER:
			return meta::Variant(lua_tonumber(L, index));
		case LUA_TSTRING:
			return meta::Variant(GuGuUtf8Str(lua_tostring(L, index)));
		case LUA_TUSERDATA:
		{
			void* ptr = lua_touserdata(L, index);//ptr
			return meta::Variant(*static_cast<std::shared_ptr<meta::Object>*>(ptr));
		}
		case LUA_TTABLE:
		{
			//将table推入注册表并获取引用
			lua_pushvalue(L, index);
			int ref = luaL_ref(L, LUA_REGISTRYINDEX);
			LuaTable table;
			table.refId = ref;
			return meta::Variant(table);
		}
		}

		return meta::Variant();
	}
	void LuaContext::pushVariantToLua(lua_State* L, const meta::Variant& value)
	{
		if (value.IsValid())
		{
			meta::Type type = value.GetType();
			if (type == typeof(int))
				lua_pushnumber(L, value.ToDouble());
			else if (type == typeof(unsigned int))
				lua_pushnumber(L, value.ToDouble());
			else if (type == typeof(float))
				lua_pushnumber(L, value.ToDouble());
			else if (type == typeof(double))
				lua_pushnumber(L, value);
			else if (type == typeof(uint8_t))
				lua_pushnumber(L, value.ToDouble());
			else if (type == typeof(bool))
				lua_pushboolean(L, value.ToBool());
			else if (type == typeof(GuGuUtf8Str))
				lua_pushstring(L, value.ToString().getStr());//is correct?
			else if (type.IsSharedPtr())
			{
				//lua_pushlightuserdata(L, value.getBase()->GetPtr());
				std::shared_ptr<meta::Object> object = *static_cast<std::shared_ptr<meta::Object>*>(value.getBase()->GetPtr());
				GuGuUtf8Str tableName = "MetaType_" + object->GetType().GetName();
				luaL_getmetatable(L, tableName.getStr());
				lua_setmetatable(L, -2);

				std::shared_ptr<meta::Object>** userData = static_cast<std::shared_ptr<meta::Object>**>(lua_newuserdata(L, sizeof(std::shared_ptr<meta::Object>*)));
				*userData = &object;
			}
			else if (type == typeof(LuaTable))
			{
				//从注册表中获取table
				lua_rawgeti(L, LUA_REGISTRYINDEX, value.GetValue<LuaTable>().refId);
			}
		}
		else
		{
			lua_pushnil(L);
		}
	}

	static int32_t universalMethodInvoker(lua_State* L)
	{
		//获取方法
		meta::Method method = *static_cast<meta::Method*>(lua_touserdata(L, lua_upvalueindex(1)));

		//获取对象实例
		meta::Variant instance = *static_cast<meta::Variant*>(lua_touserdata(L, 1));

		if (!instance)
		{
			lua_pushstring(L, "invalid object instance");
			return lua_error(L);
		}

		//收集参数
		int argc = lua_gettop(L) - 1;//减去self
		meta::ArgumentList args;
		for (int i = 1; i <= argc; ++i)
		{
			args.push_back(LuaContext::luaToVariant(L, i + 1));
		}

		//通过反射系统调用
		meta::Variant result = method.Invoke(instance, args);
		LuaContext::pushVariantToLua(L, result);
		return 1;
	}

	//通用构造函数
	static int universalConstructor(lua_State* L)
	{
		meta::TypeData& typeData = *static_cast<meta::TypeData*>(lua_touserdata(L, lua_upvalueindex(1)));

		//收集参数
		std::vector<meta::Type> types;
		int argc = lua_gettop(L);
		meta::ArgumentList args;
		for (int i = 1; i <= argc; ++i)
		{
			meta::Variant arg = LuaContext::luaToVariant(L, i + 1);
			args.push_back(arg);
			types.push_back(arg.GetType());
		}

		//创建实例
		meta::Constructor dynamicConstructor = typeData.GetDynamicConstructor(types);
		meta::Variant instance = dynamicConstructor.Invoke();
		meta::Object* instancePointer = static_cast<meta::Object*>(instance.getBase()->GetPtr());
		std::shared_ptr<meta::Object> instancePtr = std::shared_ptr<meta::Object>(instancePointer);

		std::shared_ptr<meta::Object>** userData = static_cast<std::shared_ptr<meta::Object>**>(lua_newuserdata(L, sizeof(std::shared_ptr<meta::Object>*)));
		*userData = &instancePtr;

		///设置元表
		GuGuUtf8Str tableName = "MetaType_" + typeData.name;
		luaL_getmetatable(L, tableName.getStr());
		lua_setmetatable(L, -2);

		return 1;
	}

	static int universalIndex(lua_State* L)
	{
		//获取对象实例
		meta::Variant instance = *static_cast<meta::Variant*>(lua_touserdata(L, 1));
		if (instance.IsValid() == false)
		{
			lua_pushstring(L, "invalid object instance");
			return lua_error(L);
		}

		//获取字段/方法名
		const char* key = lua_tostring(L, 2);

		meta::Type type = instance.GetType();
		if (instance.IsValid() == false)
		{
			lua_pushstring(L, "cannot determine object type");
			return lua_error(L);
		}

		//首先尝试查找字段
		auto& fields = meta::ReflectionDatabase::Instance().types[type.GetID()].fields;
		for (const auto& field : fields)
		{
			if (field.GetName() == key)
			{
				meta::Variant value = field.GetValue(instance);
				LuaContext::pushVariantToLua(L, value);
				return 1;
			}
		}

		//然后尝试查找方法
		auto& methods = meta::ReflectionDatabase::Instance().types[type.GetID()].methods;
		for (const auto& method : methods)
		{
			if (method.second.size() > 0)
			{
				//返回方法闭包
				lua_pushlightuserdata(L, (void*)(&method.second.begin()->second));//universalMethodInvoker会使用
				lua_pushcclosure(L, &universalMethodInvoker, 1);
				return 1;
			}
		}

		lua_pushnil(L);
		return 1;
	}

	//obj.name = 4(例子)
	static int universalNewIndex(lua_State* L)
	{
		//获取对象实例
		meta::Variant instance = *static_cast<meta::Variant*>(lua_touserdata(L, 1));
		if (instance.IsValid() == false)
		{
			lua_pushstring(L, "invalid object instance");
			return lua_error(L);
		}

		//获取字段名
		const char* key = lua_tostring(L, 2);

		//获取新值
		meta::Variant value = LuaContext::luaToVariant(L, 3);

		meta::Type type = instance.GetType();
		if (instance.IsValid() == false)
		{
			lua_pushstring(L, "cannot determine object type");
			return lua_error(L);
		}

		auto& fields = meta::ReflectionDatabase::Instance().types[type.GetID()].fields;
		for (const auto& field : fields)
		{
			if (field.GetName() == key)
			{
				field.SetValue(instance, value);
			}
		}

		//字段不存在
		lua_pushstring(L, "field not found or is read-only");
		return lua_error(L);
	}

	static void registerType(lua_State* L, meta::TypeData& typeData)
	{
		GuGuUtf8Str metaTableName = "MetaType_" + typeData.name;
		//创建元表，压入一个表，表名是类名，这个元表会作为通过构造函数创建的 user data 的元表
		luaL_newmetatable(L, metaTableName.getStr());

		//设置元方法，__index是键，函数是值
		lua_pushstring(L, "__index");
		lua_pushcfunction(L, &universalIndex);
		lua_settable(L, -3);

		lua_pushstring(L, "__newindex");
		lua_pushcfunction(L, &universalNewIndex);
		lua_settable(L, -3);

		lua_pop(L, 1);

		//注册全局构造函数
		lua_pushlightuserdata(L, &typeData);
		lua_pushcclosure(L, &universalConstructor, 1);
		lua_setglobal(L, typeData.name.getStr());
	}

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
		lua_getglobal(L, "package");//获取Lua的package全局变量，放入栈顶
		lua_getfield(L, -1, "searchers");//获取栈顶package的searchers键对应的值放入栈顶

		lua_pushcfunction(L, customLuaLoader);
		for (int i = lua_rawlen(L, -2) + 1; i > 1; --i) {
			lua_rawgeti(L, -2, i - 1);
			lua_rawseti(L, -3, i);
		}
		lua_rawseti(L, -2, 1);

		//clear stack
		lua_pop(L, 2);//弹出packages表和searches表
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

		////test read file
		//lua_getglobal(m_state, "require");
		//lua_pushstring(m_state, "content/testModule");
		//if (lua_pcall(m_state, 1, 1, 0) != LUA_OK)
		//{
		//	GuGu_LOGE("%s", lua_tostring(m_state, -1));
		//	lua_pop(m_state, 1);
		//}
		//
		//lua_getfield(m_state, -1, "init");//栈顶是require得到的表
		//lua_remove(m_state, -2);
		//
		//if (lua_pcall(m_state, 0, 0, 0) != LUA_OK)
		//{
		//	GuGu_LOGE("%s", lua_tostring(m_state, -1));
		//	lua_pop(m_state, 1);
		//}

		int32_t stackSize = lua_gettop(m_state);
		GuGu_LOGD("栈中元素个数%d", stackSize);//lua_pcall会自动清理栈

		//register function
		std::vector<meta::TypeData>& typeDatas = meta::ReflectionDatabase::Instance().types;
		for (auto& typeData : typeDatas)
		{
			registerType(m_state, typeData);
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

	lua_State* LuaContext::getLuaState()
	{
		return m_state;
	}

}