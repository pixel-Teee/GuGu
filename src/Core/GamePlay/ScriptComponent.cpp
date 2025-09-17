#include <pch.h>

#include "ScriptComponent.h"
#include <Core/Reflection/TypeInfo.h>
#include <Core/GamePlay/GameObject.h>
#include <Core/GamePlay/GamePlayerReflectionRegister.h>
#include <Core/LuaContext/LuaContext.h>

namespace GuGu {
	static bool registerGuGuScriptComponent()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::ScriptComponent");
		auto& type = db.types[id];
		meta::TypeInfo<ScriptComponent>::Register(id, type, true, "671B819A-F8C7-4921-8295-46BF1D2D857F");

		auto typeID = typeidof(ScriptComponent);

		if (typeID != meta::InvalidTypeID && !meta::TypeInfo<ScriptComponent>::Defined)
		{
			auto& type = db.types[typeID];

			//array constructor
			type.SetArrayConstructor<ScriptComponent>();

			type.AddConstructor<ScriptComponent, false, false>({});

			type.AddConstructor<ScriptComponent, true, true>({});

			type.LoadBaseClasses(db, typeID, { typeof(Component) });

			meta::TypeInfo<ScriptComponent>::Defined = true;
		}

		{
			auto id = db.AllocateType("std::shared_ptr<GuGu::ScriptComponent>");
			auto& type = db.types[id];
			meta::TypeInfo<std::shared_ptr<ScriptComponent>>::Register(id, type, false, "7882B450-E4EA-4B40-BB06-DB928C1ADF41");
		}

		{
			auto id = db.AllocateType("std::weak_ptr<GuGu::ScriptComponent>");
			auto& type = db.types[id];
			meta::TypeInfo<std::weak_ptr<ScriptComponent>>::Register(id, type, false, "E2D97C02-377D-4200-8C8F-42BC113ED7BD");
		}

		return true;
	}

	static bool registerGuGuScriptComponentFields()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto& type = db.types[typeof(ScriptComponent).GetID()];

		type.AddField<ScriptComponent, GuGuUtf8Str>("m_scriptPath",
			(meta::FieldGetter<ScriptComponent, GuGuUtf8Str&, true>::Signature) & ScriptComponent::getScriptPath,
			(meta::FieldSetter<ScriptComponent, GuGuUtf8Str, true>::Signature) & ScriptComponent::setScriptPath, {});

		type.AddField<ScriptComponent, std::weak_ptr<GameObject>>("m_owner",
			(meta::FieldGetter<ScriptComponent, std::weak_ptr<GameObject>&, true>::Signature) & ScriptComponent::getParentGameObject,
			(meta::FieldSetter<ScriptComponent, std::weak_ptr<GameObject>&, true>::Signature) & ScriptComponent::setParentGameObject, {});

		return true;
	}

	IMPLEMENT_INITIAL_BEGIN(ScriptComponent)
		ADD_PRIORITY(Component)
		ADD_INITIAL_FUNCTION_WITH_PRIORITY(registerGuGuScriptComponent)
	IMPLEMENT_INITIAL_END

	IMPLEMENT_INITIAL_FIELDS_BEGIN(ScriptComponent)
		ADD_PRIORITY_FIELDS(GameObject)
		ADD_INITIAL_FIELDS_FUNCTION_WITH_PRIORITY(registerGuGuScriptComponentFields)
	IMPLEMENT_INITIAL_FIELDS_END

	ScriptComponent::ScriptComponent()
	{
		m_scriptPath = "content/testModule";
	}

	ScriptComponent::~ScriptComponent()
	{

	}

	meta::Object* ScriptComponent::Clone(void) const
	{
		ScriptComponent* scriptComponent = new ScriptComponent();
		scriptComponent->m_scriptPath = m_scriptPath;
		return scriptComponent;
	}

	void ScriptComponent::Update(float fElapsedTimeSeconds)
	{
		std::shared_ptr<LuaContext> luaContext = LuaContext::getLuaContext();
		lua_State* L = luaContext->getLuaState();
		if (L && luaRef != LUA_REFNIL) {
			// 获取脚本实例
			lua_rawgeti(L, LUA_REGISTRYINDEX, luaRef);

			// 调用 update 方法
			lua_getfield(L, -1, "update");
			if (lua_isfunction(L, -1)) 
			{
				lua_pushvalue(L, -2); // 将脚本实例作为第一个参数
				lua_pushnumber(L, fElapsedTimeSeconds); //  deltaTime 作为第二个参数

				if (lua_pcall(L, 2, 0, 0) != LUA_OK) {
					GuGu_LOGE("error in script update: %s", lua_tostring(L, -1));
					lua_pop(L, 1); // 弹出错误信息
				}
			}
			else 
			{
				lua_pop(L, 1); //弹出非函数的 update
			}

			lua_pop(L, 1); //弹出脚本实例
		}
	}

	meta::Type ScriptComponent::GetType() const
	{
		return typeof(ScriptComponent);
	}

	GuGuUtf8Str ScriptComponent::getScriptPath() const
	{
		return m_scriptPath;
	}

	GuGuUtf8Str& ScriptComponent::getScriptPath()
	{
		return m_scriptPath;
	}

	void ScriptComponent::setScriptPath(const GuGuUtf8Str& inScriptPath)
	{
		m_scriptPath = inScriptPath;
	}

	void ScriptComponent::initialize()
	{
		//dont have path
		if (m_scriptPath == "")
			return;

		std::shared_ptr<LuaContext> luaContext = LuaContext::getLuaContext();
		lua_State* L = luaContext->getLuaState();

		lua_getglobal(L, "require");
		lua_pushstring(L, m_scriptPath.getStr());
		if (lua_pcall(L, 1, 1, 0) != LUA_OK)
		{
			GuGu_LOGE("error executing script : %s", lua_tostring(L, -1));
			lua_pop(L, 1);
		}

		if (!lua_istable(L, -1))
		{
			GuGu_LOGE("script must return a table");
		}
			
		//if (!lua_getfield(L, -1, "new"))
		//{
		//	luaContext->debugStack(L, "dont have new func");
		//	lua_pop(L, 1);
		//}

		lua_getfield(L, -1, "new");

		if (!lua_isfunction(L, -1))
		{
			GuGu_LOGE("script must have a new function");
		}

		lua_pushvalue(L, -2);

		meta::Variant obj = ObjectVariant(getParentGameObject().lock().get());

		luaContext->pushVariantToLua(L, obj);

		luaContext->debugStack(L, "print owner");

		//调用new，将owner作为其参数
		if (lua_pcall(L, 2, 1, 0) != LUA_OK)
		{
			GuGu_LOGE("error creating script instance %s", lua_tostring(L, -1));
		}

		//脚本实例存储到注册表中
		luaRef = luaL_ref(L, LUA_REGISTRYINDEX);
	}

	void ScriptComponent::close()
	{
		std::shared_ptr<LuaContext> luaContext = LuaContext::getLuaContext();
		lua_State* L = luaContext->getLuaState();

		if (L && luaRef != LUA_REFNIL)
		{
			//获取脚本实例
			lua_rawgeti(L, LUA_REGISTRYINDEX, luaRef);

			//调用脚本的销毁方法
			if (lua_istable(L, -1)) {
				lua_getfield(L, -1, "deInit");
				if (lua_isfunction(L, -1)) {
					//调用 deInit 方法
					lua_pushvalue(L, -2); //将脚本实例作为第一个参数
					if (lua_pcall(L, 1, 0, 0) != LUA_OK) {
						GuGu_LOGE("error in script deInit: %s", lua_tostring(L, -1));
						lua_pop(L, 1); //弹出错误信息
					}
				}
				else
				{
					lua_pop(L, 1); //弹出非函数的 deInit
				}
			}

			lua_pop(L, 1); //弹出脚本实例

			// 释放 Lua 引用
			luaL_unref(L, LUA_REGISTRYINDEX, luaRef);
			luaRef = LUA_REFNIL;
		}

	}

	void ScriptComponent::invoke(const GuGuUtf8Str& inFunctionName)
	{
		std::shared_ptr<LuaContext> luaContext = LuaContext::getLuaContext();
		lua_State* L = luaContext->getLuaState();

		if (L && luaRef != LUA_REFNIL)
		{
			//获取脚本实例
			lua_rawgeti(L, LUA_REGISTRYINDEX, luaRef);

			//调用脚本的方法
			if (lua_istable(L, -1)) {
				lua_getfield(L, -1, inFunctionName.getStr());
				if (lua_isfunction(L, -1)) {
					//调用 deInit 方法
					lua_pushvalue(L, -2); //将脚本实例作为第一个参数
					if (lua_pcall(L, 1, 0, 0) != LUA_OK) {
						GuGu_LOGE("error in script function %s: %s", inFunctionName.getStr(), lua_tostring(L, -1));
						lua_pop(L, 1); //弹出错误信息
					}
				}
				else
				{
					lua_pop(L, 1); //弹出非函数
				}
			}

			lua_pop(L, 1); //弹出脚本实例
		}
	}

}