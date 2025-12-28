#include <pch.h>

#include "Prefab.h"

#include <Core/Reflection/TypeInfo.h>
#include <Core/GamePlay/GameObject.h>

namespace GuGu {
	static bool registerGuGuPrefab()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		{
			auto id = db.AllocateType("GuGu::Prefab");
			auto& type = db.types[id];
			meta::TypeInfo<Prefab>::Register(id, type, true, "27FA1875-C32A-411C-9B51-AABEC9E28152");

			auto typeID = typeidof(Prefab);
			if (typeID != meta::InvalidTypeID && !meta::TypeInfo<Prefab>::Defined)
			{
				auto& type = db.types[typeID];

				//array constructor
				type.SetArrayConstructor<Prefab>();

				type.AddConstructor<Prefab, false, false>({});

				type.AddConstructor<Prefab, true, true>({});

				type.LoadBaseClasses(db, id, { typeof(meta::Object) });

				meta::TypeInfo<Prefab>::Defined = true;
			}

			{
				auto id = db.AllocateType("std::shared_ptr<GuGu::Prefab>");
				auto& type = db.types[id];
				meta::TypeInfo<std::shared_ptr<Prefab>>::Register(id, type, false, "EA18A292-4574-4960-BEC1-8356D103C296");
			}

			{
				auto id = db.AllocateType("std::weak_ptr<GuGu::Prefab>");
				auto& type = db.types[id];
				meta::TypeInfo<std::weak_ptr<Prefab>>::Register(id, type, false, "71B81439-0F43-4911-A807-CF062DC62A19");
			}
		}

		return true;
	}
	static bool registerGuGuPrefabFields()
	{
		auto& db = meta::ReflectionDatabase::Instance();

		auto& type = db.types[typeof(Prefab).GetID()];

		type.AddField<Prefab, Array<std::shared_ptr<GameObject>>>("m_objects",
			(meta::FieldGetter<Prefab, Array<std::shared_ptr<GameObject>>&, true>::Signature) & Prefab::getGameObjects,
			(meta::FieldSetter<Prefab, Array<std::shared_ptr<GameObject>>&, true>::Signature) & Prefab::setGameObjects, {});

		return true;
	}
	IMPLEMENT_INITIAL_BEGIN(Prefab)
		ADD_PRIORITY(meta::Object)
		ADD_INITIAL_FUNCTION_WITH_PRIORITY(registerGuGuPrefab)
	IMPLEMENT_INITIAL_END

	IMPLEMENT_INITIAL_FIELDS_BEGIN(Prefab)
		ADD_PRIORITY(GameObject)
		ADD_INITIAL_FIELDS_FUNCTION_WITH_PRIORITY(registerGuGuPrefabFields)
	IMPLEMENT_INITIAL_FIELDS_END
	Prefab::Prefab()
	{
	}

	Prefab::~Prefab()
	{
	}

	void Prefab::PostLoad()
	{

	}

	void Prefab::Update(float fElapsedTimeSeconds)
	{

	}

	meta::Type Prefab::GetType() const
	{
		return typeof(Prefab);
	}

	meta::Object* Prefab::Clone(void) const
	{	
		Prefab* prefab = new Prefab();
	
		for (int32_t i = 0; i < m_objects.size(); ++i)
		{
			GameObject* gameObject = static_cast<GameObject*>(m_objects[i]->Clone());
			
			std::shared_ptr<GameObject> gameObjectPtr(gameObject);
			prefab->m_objects.push_back(gameObjectPtr);
		}
		return prefab;
	}

	void Prefab::OnSerialize(nlohmann::json& output) const
	{

	}

	void Prefab::OnDeserialize(const nlohmann::json& input)
	{

	}

	void Prefab::setGameObjects(const Array<std::shared_ptr<GameObject>>& inObjects)
	{
		m_objects = inObjects;
	}

	Array<std::shared_ptr<GameObject>>& Prefab::getGameObjects()
	{
		return m_objects;
	}

}