#include <pch.h>

#include "SliderComponent.h"
#include <Core/AssetManager/AssetManager.h>
#include <Core/Texture/GTexture.h>
#include <Core/Reflection/TypeInfo.h>
#include <Core/GamePlay/GameObject.h>
#include "UITransformComponent.h"
#include "UIDrawInfo.h"

namespace GuGu {
	static bool registerGuGuSliderComponent()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::SliderComponent");
		auto& type = db.types[id];
		meta::TypeInfo<SliderComponent>::Register(id, type, true, "8537F282-05A7-46D0-A551-9424434C74D6");

		auto typeID = typeidof(SliderComponent);

		//array constructor
		type.SetArrayConstructor<SliderComponent>();

		type.AddConstructor<SliderComponent, false, false>({});

		type.AddConstructor<SliderComponent, true, true>({});

		type.LoadBaseClasses(db, typeID, { typeof(UIComponent) });

		{
			auto id = db.AllocateType("std::shared_ptr<GuGu::SliderComponent>");
			auto& type = db.types[id];
			meta::TypeInfo<std::shared_ptr<SliderComponent>>::Register(id, type, false, "C4FD982D-8097-41FA-A5B9-AC2302360D89");
		}

		{
			auto id = db.AllocateType("std::weak_ptr<GuGu::SliderComponent>");
			auto& type = db.types[id];
			meta::TypeInfo<std::weak_ptr<SliderComponent>>::Register(id, type, false, "617CC144-5877-463C-93E8-A7B212E6D6AA");
		}


		return true;
	}

	static bool registerGuGuSliderComponentFields()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto& type = db.types[typeof(SliderComponent).GetID()];


		return true;
	}

	IMPLEMENT_INITIAL_BEGIN(SliderComponent)
		ADD_PRIORITY(UIComponent)
		ADD_INITIAL_FUNCTION_WITH_PRIORITY(registerGuGuSliderComponent)
	IMPLEMENT_INITIAL_END

	IMPLEMENT_INITIAL_FIELDS_BEGIN(SliderComponent)

		ADD_INITIAL_FIELDS_FUNCTION_WITH_PRIORITY(registerGuGuSliderComponentFields)
	IMPLEMENT_INITIAL_FIELDS_END

	SliderComponent::SliderComponent()
	{
		
	}

	SliderComponent::~SliderComponent()
	{

	}

	meta::Object* SliderComponent::Clone(void) const
	{
		SliderComponent* sliderComponent = new SliderComponent();
		return sliderComponent;
	}

	void SliderComponent::Update(float fElapsedTimeSeconds)
	{

	}

	meta::Type SliderComponent::GetType() const
	{
		return typeof(SliderComponent);
	}

	std::shared_ptr<UIDrawInfo> SliderComponent::generateUIDrawInformation(bool bFlip)
	{
		std::shared_ptr<UIDrawInfo> drawInfo = std::make_shared<UIDrawInfo>();

		return drawInfo;
	}
}