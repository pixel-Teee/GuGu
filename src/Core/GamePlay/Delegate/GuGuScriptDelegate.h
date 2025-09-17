#pragma once

#include <Core/Reflection/Type.h>
#include <Core/Reflection/Object.h>

namespace GuGu {
	class ScriptComponent;
	class GuGuScriptDelegate : public meta::Object {
	public:
		DECLARE_INITIAL
		DECLARE_INITIAL_FIELDS

		virtual meta::Type GetType() const override;

		GuGuScriptDelegate();

		virtual ~GuGuScriptDelegate();

		virtual void PostLoad();
		virtual void Update(float fElapsedTimeSeconds);
		virtual Object* Clone(void) const override;
		virtual void OnSerialize(nlohmann::json& output) const;
		virtual void OnDeserialize(const nlohmann::json& input);

		void invoke();

		void addFunction(std::shared_ptr<ScriptComponent> inScriptObject, const GuGuUtf8Str& inFunctionName);

		void removeFunction();
	private:
		std::weak_ptr<meta::Object> m_objectPtr;

		//------script------
		GuGuUtf8Str m_functionName;

		std::weak_ptr<ScriptComponent> m_scriptObject;
		//------script------
	};
}