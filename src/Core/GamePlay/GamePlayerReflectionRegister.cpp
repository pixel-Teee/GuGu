#include <pch.h>

#include "GamePlayerReflectionRegister.h"

#include <Core/Reflection/ReflectionDatabase.h>
#include <Core/Reflection/TypeInfo.h>
#include <Core/Reflection/MetaProperty/DisplayName.h>
#include <Core/Reflection/Array.h>

#include <Core/Model/StaticMesh.h>
#include <Core/Texture/GTexture.h>
#include <Core/GamePlay/Level.h>
#include <Core/GamePlay/TransformComponent.h>
#include <Core/GamePlay/StaticMeshComponent.h>
#include <Core/GamePlay/LightComponent.h>
#include <Core/GamePlay/MaterialComponent.h>
#include <Core/GamePlay/CameraComponent.h>
#include <Core/GamePlay/TerrainComponent.h>
#include <Core/GamePlay/WaterComponent.h>

#include <Core/GamePlay/GameUI/UIComponent.h>
#include <Core/GamePlay/GameUI/ImageComponent.h>
#include <Core/GamePlay/GameUI/CanvasComponent.h>
#include <Core/GamePlay/GameUI/ButtonComponent.h>

#include <Renderer/Color.h>

#include <Core/AssetManager/AssetData.h>

#include <Core/Reflection/Priority.h>
#include <Core/Reflection/ReflectionMain.h>
#include <Core/Reflection/ReflectionMacro.h>
#include <Core/Reflection/MetaProperty/Range.h>

#include <Core/GamePlay/GameUI/UIAnchors.h>
#include <Core/GamePlay/GameUI/UIPadding.h>
#include <Core/GamePlay/GameUI/UIAnchorData.h>

namespace GuGu {

	static bool registermetaDisplayName()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("meta::DisplayName");
		auto& type = db.types[id];
		meta::TypeInfo<meta::DisplayName>::Register(id, type, true, "86AC4FFE-0C5A-46F2-BF40-AAE89636AE44");
		return true;
	}

	static bool registermathfloat2()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("math::float2");
		auto& type = db.types[id];
		meta::TypeInfo<math::float2>::Register(id, type, true, "E73886F1-1745-4885-AC10-A6764668698E");

		type.AddConstructor<math::float2, false, false>({});

		type.AddConstructor<math::float2, true, false>({});

		type.SetArrayConstructor<math::float2>();

		type.AddField<math::float2, float>("x",
			(meta::FieldGetter<math::float2, float, false>::Signature) & math::float2::x,
			(meta::FieldSetter<math::float2, float, false>::Signature) & math::float2::x, {});

		type.AddField<math::float2, float>("y",
			(meta::FieldGetter<math::float2, float, false>::Signature) & math::float2::y,
			(meta::FieldSetter<math::float2, float, false>::Signature) & math::float2::y, {});
		return true;
	}

	static bool registermathfloat3()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("math::float3");
		auto& type = db.types[id];
		meta::TypeInfo<math::float3>::Register(id, type, true, "84172191-B81E-46A4-B06F-7E5B1F8712D0");

		type.AddConstructor<math::float3, false, false>({});

		type.AddConstructor<math::float3, true, false>({});

		type.SetArrayConstructor<math::float3>();

		type.AddField<math::float3, float>("x",
			(meta::FieldGetter<math::float3, float, false>::Signature) & math::float3::x,
			(meta::FieldSetter<math::float3, float, false>::Signature) & math::float3::x, {});

		type.AddField<math::float3, float>("y",
			(meta::FieldGetter<math::float3, float, false>::Signature) & math::float3::y,
			(meta::FieldSetter<math::float3, float, false>::Signature) & math::float3::y, {});

		type.AddField<math::float3, float>("z",
			(meta::FieldGetter<math::float3, float, false>::Signature) & math::float3::z,
			(meta::FieldSetter<math::float3, float, false>::Signature) & math::float3::z, {});
		return true;
	}

	static bool registermathfloat4()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("math::float4");
		auto& type = db.types[id];
		meta::TypeInfo<math::float4>::Register(id, type, true, "50C5417E-8CD2-4F6A-A030-78FBA2C55F61");

		type.AddConstructor<math::float4, false, false>({});

		type.AddConstructor<math::float4, true, false>({});

		type.SetArrayConstructor<math::float4>();

		type.AddField<math::float4, float>("x",
			(meta::FieldGetter<math::float4, float, false>::Signature) & math::float4::x,
			(meta::FieldSetter<math::float4, float, false>::Signature) & math::float4::x, {});

		type.AddField<math::float4, float>("y",
			(meta::FieldGetter<math::float4, float, false>::Signature) & math::float4::y,
			(meta::FieldSetter<math::float4, float, false>::Signature) & math::float4::y, {});

		type.AddField<math::float4, float>("z",
			(meta::FieldGetter<math::float4, float, false>::Signature) & math::float4::z,
			(meta::FieldSetter<math::float4, float, false>::Signature) & math::float4::z, {});

		type.AddField<math::float4, float>("w",
			(meta::FieldGetter<math::float4, float, false>::Signature) & math::float4::w,
			(meta::FieldSetter<math::float4, float, false>::Signature) & math::float4::w, {});
		return true;
	}

	static bool registermathdouble2()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("math::double2");
		auto& type = db.types[id];
		meta::TypeInfo<math::double2>::Register(id, type, true, "12FCA7F0-E223-4184-9857-96B740A335A9");

		type.AddConstructor<math::double2, false, false>({});

		type.AddConstructor<math::double2, true, false>({});

		type.SetArrayConstructor<math::double2>();

		type.AddField<math::double2, double>("x",
			(meta::FieldGetter<math::double2, double, false>::Signature) & math::double2::x,
			(meta::FieldSetter<math::double2, double, false>::Signature) & math::double2::x, {});

		type.AddField<math::double2, double>("y",
			(meta::FieldGetter<math::double2, double, false>::Signature) & math::double2::y,
			(meta::FieldSetter<math::double2, double, false>::Signature) & math::double2::y, {});

		return true;
	}

	static bool registermathdouble3()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("math::double3");
		auto& type = db.types[id];
		meta::TypeInfo<math::double3>::Register(id, type, true, "35EF324D-9D0A-4A20-82A4-E61BF62C5960");

		type.AddConstructor<math::double3, false, false>({});

		type.AddConstructor<math::double3, true, false>({});

		type.SetArrayConstructor<math::double3>();

		type.AddField<math::double3, double>("x",
			(meta::FieldGetter<math::double3, double, false>::Signature) & math::double3::x,
			(meta::FieldSetter<math::double3, double, false>::Signature) & math::double3::x, {});

		type.AddField<math::double3, double>("y",
			(meta::FieldGetter<math::double3, double, false>::Signature) & math::double3::y,
			(meta::FieldSetter<math::double3, double, false>::Signature) & math::double3::y, {});

		type.AddField<math::double3, double>("z",
			(meta::FieldGetter<math::double3, double, false>::Signature) & math::double3::z,
			(meta::FieldSetter<math::double3, double, false>::Signature) & math::double3::z, {});
		return true;
	}

	static bool registermathdquat()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("math::dquat");
		auto& type = db.types[id];
		meta::TypeInfo<math::dquat>::Register(id, type, true, "A576462F-80EF-4132-9096-AF19E505671B");

		type.AddConstructor<math::dquat, false, false>({});

		type.AddConstructor<math::dquat, true, false>({});

		type.SetArrayConstructor<math::dquat>();

		type.AddField<math::dquat, double>("x",
			(meta::FieldGetter<math::dquat, double, false>::Signature) & math::dquat::x,
			(meta::FieldSetter<math::dquat, double, false>::Signature) & math::dquat::x, {});

		type.AddField<math::dquat, double>("y",
			(meta::FieldGetter<math::dquat, double, false>::Signature) & math::dquat::y,
			(meta::FieldSetter<math::dquat, double, false>::Signature) & math::dquat::y, {});

		type.AddField<math::dquat, double>("z",
			(meta::FieldGetter<math::dquat, double, false>::Signature) & math::dquat::z,
			(meta::FieldSetter<math::dquat, double, false>::Signature) & math::dquat::z, {});

		type.AddField<math::dquat, double>("w",
			(meta::FieldGetter<math::dquat, double, false>::Signature) & math::dquat::w,
			(meta::FieldSetter<math::dquat, double, false>::Signature) & math::dquat::w, {});
		return true;
	}

	static bool registermathuint16_4()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("math::vector<uin16_t, 4>");
		auto& type = db.types[id];
		meta::TypeInfo<math::vector<uint16_t, 4>>::Register(id, type, true, "DD2D2988-9B15-41B1-82F2-2F32D095D4AA");

		type.AddConstructor<math::vector<uint16_t, 4>, false, false>({});

		type.AddConstructor<math::vector<uint16_t, 4>, true, false>({});

		type.SetArrayConstructor<math::vector<uint16_t, 4>>();

		type.AddField<math::vector<uint16_t, 4>, uint16_t>("x",
			(meta::FieldGetter<math::vector<uint16_t, 4>, uint16_t, false>::Signature) & math::vector<uint16_t, 4>::x,
			(meta::FieldSetter<math::vector<uint16_t, 4>, uint16_t, false>::Signature) & math::vector<uint16_t, 4>::x, {});

		type.AddField<math::vector<uint16_t, 4>, uint16_t>("y",
			(meta::FieldGetter<math::vector<uint16_t, 4>, uint16_t, false>::Signature) & math::vector<uint16_t, 4>::y,
			(meta::FieldSetter<math::vector<uint16_t, 4>, uint16_t, false>::Signature) & math::vector<uint16_t, 4>::y, {});

		type.AddField<math::vector<uint16_t, 4>, uint16_t>("z",
			(meta::FieldGetter<math::vector<uint16_t, 4>, uint16_t, false>::Signature) & math::vector<uint16_t, 4>::z,
			(meta::FieldSetter<math::vector<uint16_t, 4>, uint16_t, false>::Signature) & math::vector<uint16_t, 4>::z, {});

		type.AddField<math::vector<uint16_t, 4>, uint16_t>("w",
			(meta::FieldGetter<math::vector<uint16_t, 4>, uint16_t, false>::Signature) & math::vector<uint16_t, 4>::w,
			(meta::FieldSetter<math::vector<uint16_t, 4>, uint16_t, false>::Signature) & math::vector<uint16_t, 4>::w, {});
		return true;
	}

	static bool registerRotator()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("math::Rotator");
		auto& type = db.types[id];
		meta::TypeInfo<math::Rotator>::Register(id, type, true, "0AD77E66-681B-4D8E-9D45-AEF7872EF3D3");

		type.AddConstructor<math::Rotator, false, false>({});

		type.AddConstructor<math::Rotator, true, false>({});

		type.SetArrayConstructor<math::Rotator>();

		type.AddField<math::Rotator, float>("pitch",
			(meta::FieldGetter<math::Rotator, float, false>::Signature) & math::Rotator::pitch,
			(meta::FieldSetter<math::Rotator, float, false>::Signature) & math::Rotator::pitch, {});

		type.AddField<math::Rotator, float>("yaw",
			(meta::FieldGetter<math::Rotator, float, false>::Signature) & math::Rotator::yaw,
			(meta::FieldSetter<math::Rotator, float, false>::Signature) & math::Rotator::yaw, {});

		type.AddField<math::Rotator, float>("roll",
			(meta::FieldGetter<math::Rotator, float, false>::Signature) & math::Rotator::roll,
			(meta::FieldSetter<math::Rotator, float, false>::Signature) & math::Rotator::roll, {});

		return true;
	}

	static bool registerGuGuColor()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::Color");
		auto& type = db.types[id];
		meta::TypeInfo<Color>::Register(id, type, true, "87AC627B-14AE-4588-8E3F-80A36AF8C676");

		type.AddConstructor<Color, false, false>({});

		type.AddConstructor<Color, true, false>({});

		type.SetArrayConstructor<Color>();

		type.AddField<Color, float>("r",
			(meta::FieldGetter<Color, float, false>::Signature) & Color::r,
			(meta::FieldSetter<Color, float, false>::Signature) & Color::r, {});

		type.AddField<Color, float>("g",
			(meta::FieldGetter<Color, float, false>::Signature) & Color::g,
			(meta::FieldSetter<Color, float, false>::Signature) & Color::g, {});

		type.AddField<Color, float>("b",
			(meta::FieldGetter<Color, float, false>::Signature) & Color::b,
			(meta::FieldSetter<Color, float, false>::Signature) & Color::b, {});

		type.AddField<Color, float>("a",
			(meta::FieldGetter<Color, float, false>::Signature) & Color::a,
			(meta::FieldSetter<Color, float, false>::Signature) & Color::a, {});
		return true;
	}

	static bool registerUIPadding()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::UIPadding");
		auto& type = db.types[id];
		meta::TypeInfo<UIPadding>::Register(id, type, true, "F3A91F05-42B3-432F-87D7-7D9925C50EEC");

		type.AddConstructor<UIPadding, false, false>({});

		type.AddConstructor<UIPadding, true, false>({});

		type.SetArrayConstructor<UIPadding>();

		type.AddField<UIPadding, float>("left",
			(meta::FieldGetter<UIPadding, float, false>::Signature) & UIPadding::left,
			(meta::FieldSetter<UIPadding, float, false>::Signature) & UIPadding::left,
			{
				std::make_pair(typeof(meta::Range), meta::MetaPropertyInitializer<meta::Range>(-2000.0f, 2000.0f)) //meta
			});

		type.AddField<UIPadding, float>("top",
			(meta::FieldGetter<UIPadding, float, false>::Signature) & UIPadding::top,
			(meta::FieldSetter<UIPadding, float, false>::Signature) & UIPadding::top,
			{
				std::make_pair(typeof(meta::Range), meta::MetaPropertyInitializer<meta::Range>(-2000.0f, 2000.0f)) //meta
			});

		type.AddField<UIPadding, float>("right",
			(meta::FieldGetter<UIPadding, float, false>::Signature) & UIPadding::right,
			(meta::FieldSetter<UIPadding, float, false>::Signature) & UIPadding::right,
			{
				std::make_pair(typeof(meta::Range), meta::MetaPropertyInitializer<meta::Range>(-2000.0f, 2000.0f)) //meta
			});

		type.AddField<UIPadding, float>("bottom",
			(meta::FieldGetter<UIPadding, float, false>::Signature) & UIPadding::bottom,
			(meta::FieldSetter<UIPadding, float, false>::Signature) & UIPadding::bottom,
			{
				std::make_pair(typeof(meta::Range), meta::MetaPropertyInitializer<meta::Range>(-2000.0f, 2000.0f)) //meta
			});
		return true;
	}

	static bool registerUIAnchors()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::UIAnchors");
		auto& type = db.types[id];
		meta::TypeInfo<UIAnchors>::Register(id, type, true, "740049DA-CFDE-4D2D-BD66-2477308876FC");

		type.AddConstructor<UIAnchors, false, false>({});

		type.AddConstructor<UIAnchors, true, false>({});

		type.SetArrayConstructor<UIAnchors>();

		type.AddField<UIAnchors, math::float2>("m_minimum",
			(meta::FieldGetter<UIAnchors, math::float2, false>::Signature) & UIAnchors::m_minimum,
			(meta::FieldSetter<UIAnchors, math::float2, false>::Signature) & UIAnchors::m_minimum,
			{
				std::make_pair(typeof(meta::Range), meta::MetaPropertyInitializer<meta::Range>(-100.0f, 100.0f)) //meta
			});

		type.AddField<UIAnchors, math::float2>("m_maximum",
			(meta::FieldGetter<UIAnchors, math::float2, false>::Signature) & UIAnchors::m_maximum,
			(meta::FieldSetter<UIAnchors, math::float2, false>::Signature) & UIAnchors::m_maximum, 
			{
				std::make_pair(typeof(meta::Range), meta::MetaPropertyInitializer<meta::Range>(-100.0f, 100.0f)) //meta
			});

		return true;
	}

	static bool registerUIAnchorData()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::UIAnchorData");
		auto& type = db.types[id];
		meta::TypeInfo<UIAnchorData>::Register(id, type, true, "32449923-5F79-42D1-BC31-3A32D770CDE7");

		type.AddConstructor<UIAnchorData, false, false>({});

		type.AddConstructor<UIAnchorData, true, false>({});

		type.SetArrayConstructor<UIAnchorData>();

		type.AddField<UIAnchorData, UIAnchors>("m_anchors",
			(meta::FieldGetter<UIAnchorData, UIAnchors, false>::Signature) & UIAnchorData::m_anchors,
			(meta::FieldSetter<UIAnchorData, UIAnchors, false>::Signature) & UIAnchorData::m_anchors, {});

		type.AddField<UIAnchorData, UIPadding>("m_offset",
			(meta::FieldGetter<UIAnchorData, UIPadding, false>::Signature) & UIAnchorData::m_offset,
			(meta::FieldSetter<UIAnchorData, UIPadding, false>::Signature) & UIAnchorData::m_offset, {});

		type.AddField<UIAnchorData, math::float2>("m_alignment",
			(meta::FieldGetter<UIAnchorData, math::float2, false>::Signature) & UIAnchorData::m_alignment,
			(meta::FieldSetter<UIAnchorData, math::float2, false>::Signature) & UIAnchorData::m_alignment, {});

		return true;
	}

	static bool registerGuGuMetaObject()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::meta::Object");
		auto& type = db.types[id];
		meta::TypeInfo<meta::Object>::Register(id, type, true, "E6023C42-0DA9-4F76-8290-CDB926C881E3");

		type.meta = {
			std::make_pair(typeof(meta::DisplayName), meta::MetaPropertyInitializer<meta::DisplayName>("Object"))
		};
		return true;
	}

	static bool registerGuGuComponent()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::Component");
		auto& type = db.types[id];
		meta::TypeInfo<Component>::Register(id, type, true, "0955EA47-6CB3-4F4E-929A-A19769B4DD93");

		type.AddField<Component, std::weak_ptr<GameObject>>("m_owner",
			(meta::FieldGetter<Component, std::weak_ptr<GameObject>&, true>::Signature) & Component::getParentGameObject,
			(meta::FieldSetter<Component, std::weak_ptr<GameObject>&, true>::Signature) & Component::setParentGameObject, {});

		{
			auto id = db.AllocateType("std::shared_ptr<GuGu::Component>");
			auto& type = db.types[id];
			meta::TypeInfo<std::shared_ptr<Component>>::Register(id, type, false, "D7753E39-309C-4C52-BF1D-BCA0F90D6352");

			type.SetArrayConstructor<std::shared_ptr<Component>>();
		}

		{
			auto id = db.AllocateType("std::weak_ptr<GuGu::Component>");
			auto& type = db.types[id];
			meta::TypeInfo<std::weak_ptr<Component>>::Register(id, type, false, "1CA55F88-8D64-445F-AE1B-E6635525E0DA");
		}
		return true;
	}

	void registerThirdParty()
	{
		//Priority metaDisplayNamePriority;
		//metaDisplayNamePriority.setDebugName("meta::DisplayName");
		//ReflectionMain::addInitialTypeFunction(registermetaDisplayName, &metaDisplayNamePriority);

		//Priority mathfloat2Priority;
		mathfloat2Priority.setDebugName("math::float2");
		ReflectionMain::addInitialTypeFunction(registermathfloat2, &mathfloat2Priority);

		//Priority mathfloat3Priority;
		mathfloat3Priority.setDebugName("math::float3");
		ReflectionMain::addInitialTypeFunction(registermathfloat3, &mathfloat3Priority);

		//Priority mathfloat4Priority;
		mathfloat4Priority.setDebugName("math::float4");
		ReflectionMain::addInitialTypeFunction(registermathfloat4, &mathfloat4Priority);

		mathdouble2Priority.setDebugName("math::double2");
		ReflectionMain::addInitialTypeFunction(registermathdouble2, &mathdouble2Priority);

		//Priority mathdouble3Priority;
		mathdouble3Priority.setDebugName("math::double3");
		ReflectionMain::addInitialTypeFunction(registermathdouble3, &mathdouble3Priority);

		//Priority mathdquatPriority;
		mathdquatPriority.setDebugName("math::dquat");
		ReflectionMain::addInitialTypeFunction(registermathdquat, &mathdquatPriority);

		//Priority mathuint16_4Priority;
		mathuint16_4Priority.setDebugName("math::uint16_4");
		ReflectionMain::addInitialTypeFunction(registermathuint16_4, &mathuint16_4Priority);

		mathrotatorPriority.setDebugName("math::Rotator");
		ReflectionMain::addInitialTypeFunction(registerRotator, &mathrotatorPriority);

		//uiPaddingPriority.setDebugName("GuGu::UIPadding");
		//ReflectionMain::addInitialTypeFunction(registerUIPadding, &uiPaddingPriority);
		//
		//uiAnchorsPriority.setDebugName("GuGu:UIAnchors");
		//ReflectionMain::addInitialTypeFunction(registerUIAnchors, &uiAnchorsPriority);
	}

	void gamePlayerReflectionRegister()
	{
		auto& db = meta::ReflectionDatabase::Instance();

		registerThirdParty();

		meta::Range::registerMainFactory();
		meta::Range::registerMainFactory2();

		meta::DisplayName::registerMainFactory();
		meta::DisplayName::registerMainFactory2();

		ReflectionMain::addInitialTypeFunction(registerUIPadding, &uiPaddingPriority);
		ReflectionMain::addInitialTypeFunction(registerUIAnchors, &uiAnchorsPriority);
		ReflectionMain::addInitialTypeFunction(registerUIAnchorData, &uiAnchorDataPrority);
		uiPaddingPriority.addPriorityThan(&meta::Range::ms_priority);
		uiAnchorsPriority.addPriorityThan(&meta::Range::ms_priority);
		uiAnchorDataPrority.addPriorityThan(&uiPaddingPriority); //padding
		uiAnchorDataPrority.addPriorityThan(&uiAnchorsPriority); //anchor
		uiAnchorDataPrority.addPriorityThan(&mathfloat2Priority); //alignment

		//UIComponent register
		UIComponent::registerMainFactory();
		UIComponent::registerMainFactory2();

		//ImageComponent register
		ImageComponent::registerMainFactory();
		ImageComponent::registerMainFactory2();

		CanvasComponent::registerMainFactory();
		CanvasComponent::registerMainFactory2();

		ButtonComponent::registerMainFactory();
		ButtonComponent::registerMainFactory2();

		ReflectionMain::initialize();

		meta::Type::List types = meta::Type::GetTypes();
		for (const auto& type : types)
		{
			db.registerGuid(type.getGuid(), type.GetID());
		}
	}
}