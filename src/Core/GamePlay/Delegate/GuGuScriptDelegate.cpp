#include <pch.h>

#include "GuGuScriptDelegate.h"
#include <Core/Reflection/TypeInfo.h>

#include <Core/GamePlay/ScriptComponent.h>

namespace GuGu {
	static bool registerGuGuScriptDelegate()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		{
			auto id = db.AllocateType("GuGu::GuGuScriptDelegate");
			auto& type = db.types[id];
			meta::TypeInfo<GuGuScriptDelegate>::Register(id, type, true, "8503AABB-33A1-4DA6-BDEF-D91501865350");

			auto typeID = typeidof(GuGuScriptDelegate);
			if (typeID != meta::InvalidTypeID && !meta::TypeInfo<GuGuScriptDelegate>::Defined)
			{
				auto& type = db.types[typeID];

				//array constructor
				type.SetArrayConstructor<GuGuScriptDelegate>();

				type.AddConstructor<GuGuScriptDelegate, false, false>({});

				type.AddConstructor<GuGuScriptDelegate, true, true>({});

				type.LoadBaseClasses(db, id, { typeof(meta::Object) });

				meta::TypeInfo<GuGuScriptDelegate>::Defined = true;
			}

			{
				auto id = db.AllocateType("std::shared_ptr<GuGu::GuGuScriptDelegate>");
				auto& type = db.types[id];
				meta::TypeInfo<std::shared_ptr<GuGuScriptDelegate>>::Register(id, type, false, "24140DA4-32F6-492F-B83C-5BF3B54C6C87");
			}

			{
				auto id = db.AllocateType("std::weak_ptr<GuGu::GuGuScriptDelegate>");
				auto& type = db.types[id];
				meta::TypeInfo<std::weak_ptr<GuGuScriptDelegate>>::Register(id, type, false, "3F292B3E-E1C1-445B-A41B-DD4DDFA8FEB9");
			}
		}
		return true;
	}
	static bool registerGuGuGuGuScriptDelegateFields()
	{
		auto& db = meta::ReflectionDatabase::Instance();

		auto& type = db.types[typeof(GuGuScriptDelegate).GetID()];

		type.AddMethod("addFunction", &GuGuScriptDelegate::addFunction, {});

		type.AddMethod("removeFunction", &GuGuScriptDelegate::removeFunction, {});

		return true;
	}
	IMPLEMENT_INITIAL_BEGIN(GuGuScriptDelegate)
		ADD_PRIORITY(meta::Object)
		ADD_INITIAL_FUNCTION_WITH_PRIORITY(registerGuGuScriptDelegate)
	IMPLEMENT_INITIAL_END

	IMPLEMENT_INITIAL_FIELDS_BEGIN(GuGuScriptDelegate)
		ADD_INITIAL_FIELDS_FUNCTION_WITH_PRIORITY(registerGuGuGuGuScriptDelegateFields)
	IMPLEMENT_INITIAL_FIELDS_END
	GuGuScriptDelegate::GuGuScriptDelegate()
	{
	}

	GuGuScriptDelegate::~GuGuScriptDelegate()
	{
	}

	void GuGuScriptDelegate::PostLoad()
	{

	}

	void GuGuScriptDelegate::Update(float fElapsedTimeSeconds)
	{

	}

	meta::Type GuGuScriptDelegate::GetType() const
	{
		return typeof(GuGuScriptDelegate);
	}

	meta::Object* GuGuScriptDelegate::Clone(void) const
	{
		GuGuScriptDelegate* myDelegate = new GuGuScriptDelegate();

		return myDelegate;
	}

	void GuGuScriptDelegate::OnSerialize(nlohmann::json & output) const
	{

	}

	void GuGuScriptDelegate::OnDeserialize(const nlohmann::json & input)
	{

	}

	void GuGuScriptDelegate::invoke()
	{
		//invoke script function
		if (m_scriptObject.lock())
		{
			m_scriptObject.lock()->invoke(m_functionName);
		}
	}

	void GuGuScriptDelegate::addFunction(std::shared_ptr<ScriptComponent> inScriptObject, const GuGuUtf8Str& inFunctionName)
	{
		m_scriptObject = inScriptObject;
		m_functionName = inFunctionName;
	}

	void GuGuScriptDelegate::removeFunction()
	{
		m_scriptObject.reset();
		m_functionName = "";
	}

}