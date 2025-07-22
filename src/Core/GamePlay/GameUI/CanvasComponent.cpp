#include <pch.h>

#include "CanvasComponent.h"
#include <Core/Reflection/TypeInfo.h>

#include <Core/GamePlay/GameObject.h>
#include <Core/GamePlay/GameUI/UITransformComponent.h>
#include <Core/GamePlay/World.h>
#include <Core/GamePlay/ViewportClient.h>

namespace GuGu {
	static bool registerGuGuCanvasomponent()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::CanvasComponent");
		auto& type = db.types[id];
		meta::TypeInfo<CanvasComponent>::Register(id, type, true, "A117C35B-4F4F-46A5-81A8-C428CD396F5C");

		auto typeID = typeidof(CanvasComponent);

		if (typeID != meta::InvalidTypeID && !meta::TypeInfo<CanvasComponent>::Defined)
		{
			auto& type = db.types[typeID];

			//array constructor
			type.SetArrayConstructor<CanvasComponent>();

			type.AddConstructor<CanvasComponent, false, false>({});

			type.AddConstructor<CanvasComponent, true, true>({});

			type.LoadBaseClasses(db, typeID, { typeof(Component) });

			meta::TypeInfo<CanvasComponent>::Defined = true;
		}

		{
			auto id = db.AllocateType("std::shared_ptr<GuGu::CanvasComponent>");
			auto& type = db.types[id];
			meta::TypeInfo<std::shared_ptr<CanvasComponent>>::Register(id, type, false, "59B11E97-948E-42F5-8A10-D3C6B0441576");
		}

		{
			auto id = db.AllocateType("std::weak_ptr<GuGu::CanvasComponent>");
			auto& type = db.types[id];
			meta::TypeInfo<std::weak_ptr<CanvasComponent>>::Register(id, type, false, "1F39C1F0-121E-4C88-A21B-F4CC96A98C22");
		}

		return true;
	}

	static bool registerGuGuCanvasComponentFields()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto& type = db.types[typeof(CanvasComponent).GetID()];

		type.AddField<CanvasComponent, float>("m_scaleFactor",
			(meta::FieldGetter<CanvasComponent, float&, true>::Signature) & CanvasComponent::getScaleFactor,
			(meta::FieldSetter<CanvasComponent, float, true>::Signature) & CanvasComponent::setScaleFactor, {});

		type.AddField<CanvasComponent, std::weak_ptr<GameObject>>("m_owner",
			(meta::FieldGetter<CanvasComponent, std::weak_ptr<GameObject>&, true>::Signature) & CanvasComponent::getParentGameObject,
			(meta::FieldSetter<CanvasComponent, std::weak_ptr<GameObject>&, true>::Signature) & CanvasComponent::setParentGameObject, {});

		return true;
	}

	IMPLEMENT_INITIAL_BEGIN(CanvasComponent)
		ADD_PRIORITY(Component)
		ADD_INITIAL_FUNCTION_WITH_PRIORITY(registerGuGuCanvasomponent)
		IMPLEMENT_INITIAL_END

		IMPLEMENT_INITIAL_FIELDS_BEGIN(CanvasComponent)
		ADD_INITIAL_FIELDS_FUNCTION_WITH_PRIORITY(registerGuGuCanvasComponentFields)
	IMPLEMENT_INITIAL_FIELDS_END

	CanvasComponent::CanvasComponent()
	{
		m_scaleFactor = 100.0f;
	}

	CanvasComponent::~CanvasComponent()
	{

	}

	meta::Object* CanvasComponent::Clone(void) const
	{
		CanvasComponent* canvasComponent = new CanvasComponent();
		
		return canvasComponent;
	}

	void CanvasComponent::Update(float fElapsedTimeSeconds)
	{
		std::shared_ptr<GameObject> owner = m_owner.lock();
		if (owner)
		{
			std::shared_ptr<UITransformComponent> rootTrans = owner->getComponent<UITransformComponent>();
			if (rootTrans)
			{
				rootTrans->setScaleFactor(m_scaleFactor);
				std::shared_ptr<ViewportClient> viewportClient = World::getWorld()->getViewportClient().lock();
				if (viewportClient)
				{
					rootTrans->setLocalPosition(math::float2(0.0f, 0.0f));
					rootTrans->setLocalSize(viewportClient->getViewportSize());
				}
				else
				{
					rootTrans->setLocalPosition(math::float2(0.0f, 0.0f));
					rootTrans->setLocalSize(math::float2(100.0f, 100.0f));
				}		
			}
		}
	}

	meta::Type CanvasComponent::GetType() const
	{
		return typeof(CanvasComponent);
	}

	float CanvasComponent::getScaleFactor() const
	{
		return m_scaleFactor;
	}

	float& CanvasComponent::getScaleFactor()
	{
		return m_scaleFactor;
	}

	void CanvasComponent::setScaleFactor(float inScaleFactor)
	{
		m_scaleFactor = inScaleFactor;
	}

}