#pragma once

#include <Core/GamePlay/Component.h>

namespace GuGu {

	//lua
	class ScriptComponent : public Component
	{
	public:
		DECLARE_INITIAL
		DECLARE_INITIAL_FIELDS

		ScriptComponent();

		virtual ~ScriptComponent();

		virtual Object* Clone(void) const override;

		void Update(float fElapsedTimeSeconds) override;

		virtual meta::Type GetType() const override;

		GuGuUtf8Str getScriptPath() const;
		GuGuUtf8Str& getScriptPath();
		void setScriptPath(const GuGuUtf8Str& inScriptPath);

		void initialize();

		void close();
	private:
		GuGuUtf8Str m_scriptPath;

		meta::Variant m_scriptInstance;

		int32_t luaRef = -1;//Lua 注册表的引用

	};
}