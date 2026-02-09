#include <pch.h>

#include "GamePlayerReflectionRegister.h"

#include <Core/Reflection/ReflectionDatabase.h>
#include <Core/Reflection/TypeInfo.h>
#include <Core/Reflection/MetaProperty/DisplayName.h>
#include <Core/Reflection/Array.h>

#include <Core/Model/StaticMesh.h>
#include <Core/Model/BoneInfo.h>
#include <Core/Texture/GTexture.h>
#include <Core/GamePlay/Level.h>
#include <Core/GamePlay/TransformComponent.h>
#include <Core/GamePlay/StaticMeshComponent.h>
#include <Core/GamePlay/LightComponent.h>
#include <Core/GamePlay/MaterialComponent.h>
#include <Core/GamePlay/CameraComponent.h>
#include <Core/GamePlay/TerrainComponent.h>
#include <Core/GamePlay/WaterComponent.h>
#include <Core/GamePlay/GrassComponent.h>

#include <Core/GamePlay/GameUI/UIComponent.h>
#include <Core/GamePlay/GameUI/ImageComponent.h>
#include <Core/GamePlay/GameUI/CanvasComponent.h>
#include <Core/GamePlay/GameUI/ButtonComponent.h>
#include <Core/GamePlay/ScriptComponent.h>
#include <Core/GamePlay/TerrainVegetationComponent.h>
#include <Core/GamePlay/Physics/Collision3DComponent.h>
#include <Core/GamePlay/Physics/CollisionResult.h>
#include <Core/GamePlay/InputManager.h>

#include <Renderer/Color.h>

#include <Core/AssetManager/AssetData.h>

#include <Core/Reflection/Priority.h>
#include <Core/Reflection/ReflectionMain.h>
#include <Core/Reflection/ReflectionMacro.h>
#include <Core/Reflection/MetaProperty/Range.h>

#include <Core/GamePlay/GameUI/UIAnchors.h>
#include <Core/GamePlay/GameUI/UIPadding.h>
#include <Core/GamePlay/GameUI/UIAnchorData.h>

#include <Core/Animation/Keyframe.h>
#include <Core/Animation/Animator.h>
#include <Core/Animation/GAnimation.h>

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
		auto id = db.AllocateType("GuGu::math::float2");
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
		auto id = db.AllocateType("GuGu::math::float3");
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
		auto id = db.AllocateType("GuGu::math::float4");
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
		auto id = db.AllocateType("GuGu::math::double2");
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
		auto id = db.AllocateType("GuGu::math::double3");
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

	static bool registermathquat()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::math::quat");
		auto& type = db.types[id];
		meta::TypeInfo<math::quat>::Register(id, type, true, "3922F75E-C0DC-44B7-B8B3-94548A8EBDE2");

		type.AddConstructor<math::quat, false, false>({});

		type.AddConstructor<math::quat, true, false>({});

		type.SetArrayConstructor<math::quat>();

		type.AddField<math::quat, float>("x",
			(meta::FieldGetter<math::quat, float, false>::Signature) & math::quat::x,
			(meta::FieldSetter<math::quat, float, false>::Signature) & math::quat::x, {});

		type.AddField<math::quat, float>("y",
			(meta::FieldGetter<math::quat, float, false>::Signature) & math::quat::y,
			(meta::FieldSetter<math::quat, float, false>::Signature) & math::quat::y, {});

		type.AddField<math::quat, float>("z",
			(meta::FieldGetter<math::quat, float, false>::Signature) & math::quat::z,
			(meta::FieldSetter<math::quat, float, false>::Signature) & math::quat::z, {});

		type.AddField<math::quat, float>("w",
			(meta::FieldGetter<math::quat, float, false>::Signature) & math::quat::w,
			(meta::FieldSetter<math::quat, float, false>::Signature) & math::quat::w, {});
		return true;
	}

	static bool registermathdquat()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::math::dquat");
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
		auto id = db.AllocateType("GuGu::math::vector<uin16_t, 4>");
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

	static bool registermathint16_4()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::math::vector<int16_t, 4>");
		auto& type = db.types[id];
		meta::TypeInfo<math::vector<int16_t, 4>>::Register(id, type, true, "466A1C4E-51C7-4B31-9B4E-86117E87E162");

		type.AddConstructor<math::vector<int16_t, 4>, false, false>({});

		type.AddConstructor<math::vector<int16_t, 4>, true, false>({});

		type.SetArrayConstructor<math::vector<int16_t, 4>>();

		type.AddField<math::vector<int16_t, 4>, int16_t>("x",
			(meta::FieldGetter<math::vector<int16_t, 4>, int16_t, false>::Signature) & math::vector<int16_t, 4>::x,
			(meta::FieldSetter<math::vector<int16_t, 4>, int16_t, false>::Signature) & math::vector<int16_t, 4>::x, {});

		type.AddField<math::vector<int16_t, 4>, int16_t>("y",
			(meta::FieldGetter<math::vector<int16_t, 4>, int16_t, false>::Signature) & math::vector<int16_t, 4>::y,
			(meta::FieldSetter<math::vector<int16_t, 4>, int16_t, false>::Signature) & math::vector<int16_t, 4>::y, {});

		type.AddField<math::vector<int16_t, 4>, int16_t>("z",
			(meta::FieldGetter<math::vector<int16_t, 4>, int16_t, false>::Signature) & math::vector<int16_t, 4>::z,
			(meta::FieldSetter<math::vector<int16_t, 4>, int16_t, false>::Signature) & math::vector<int16_t, 4>::z, {});

		type.AddField<math::vector<int16_t, 4>, int16_t>("w",
			(meta::FieldGetter<math::vector<int16_t, 4>, int16_t, false>::Signature) & math::vector<int16_t, 4>::w,
			(meta::FieldSetter<math::vector<int16_t, 4>, int16_t, false>::Signature) & math::vector<int16_t, 4>::w, {});
		return true;
	}

	static bool registerRotator()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::math::Rotator");
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

	static bool registerfloat4x4()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::math::float4x4");
		auto& type = db.types[id];
		meta::TypeInfo<math::float4x4>::Register(id, type, true, "91B562CC-EF4F-4044-B77C-DC2EEAF1D571");

		type.AddConstructor<math::float4x4, false, false>({});

		type.AddConstructor<math::float4x4, true, false>({});

		type.AddField<math::float4x4, math::float4>("row0",
			(meta::FieldGetter<math::float4x4, math::float4, false > ::Signature) & math::float4x4::row0,
			(meta::FieldSetter<math::float4x4, math::float4, false > ::Signature) & math::float4x4::row0, {});

		type.AddField<math::float4x4, math::float4>("row1",
			(meta::FieldGetter<math::float4x4, math::float4, false > ::Signature) & math::float4x4::row1,
			(meta::FieldSetter<math::float4x4, math::float4, false > ::Signature) & math::float4x4::row1, {});

		type.AddField<math::float4x4, math::float4>("row2",
			(meta::FieldGetter<math::float4x4, math::float4, false > ::Signature) & math::float4x4::row2,
			(meta::FieldSetter<math::float4x4, math::float4, false > ::Signature) & math::float4x4::row2, {});

		type.AddField<math::float4x4, math::float4>("row3",
			(meta::FieldGetter<math::float4x4, math::float4, false > ::Signature) & math::float4x4::row3,
			(meta::FieldSetter<math::float4x4, math::float4, false > ::Signature) & math::float4x4::row3, {});

		type.SetArrayConstructor<math::float4x4>();
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
			(meta::FieldGetter<UIPadding, float&, true>::Signature) & UIPadding::getRight,
			(meta::FieldSetter<UIPadding, float, true>::Signature) & UIPadding::setRight,
			{
				std::make_pair(typeof(meta::Range), meta::MetaPropertyInitializer<meta::Range>(-2000.0f, 2000.0f)) //meta
			});

		type.AddField<UIPadding, float>("bottom",
			(meta::FieldGetter<UIPadding, float&, true>::Signature) & UIPadding::getBottom,
			(meta::FieldSetter<UIPadding, float, true>::Signature) & UIPadding::setBottom,
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
			(meta::FieldGetter<UIAnchorData, UIPadding&, true>::Signature) & UIAnchorData::getOffset,
			(meta::FieldSetter<UIAnchorData, UIPadding, true>::Signature) & UIAnchorData::setOffset, {});

		type.AddField<UIAnchorData, math::float2>("m_alignment",
			(meta::FieldGetter<UIAnchorData, math::float2, false>::Signature) & UIAnchorData::m_alignment,
			(meta::FieldSetter<UIAnchorData, math::float2, false>::Signature) & UIAnchorData::m_alignment, {});

		return true;
	}

	static bool registerBoneInfo()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::BoneInfo");
		auto& type = db.types[id];
		meta::TypeInfo<BoneInfo>::Register(id, type, true, "02CD624C-86AF-4467-88AD-4C7EB93F84F2");

		type.AddConstructor<BoneInfo, false, false>({});

		type.AddConstructor<BoneInfo, true, false>({});

		type.SetArrayConstructor<BoneInfo>();

		type.AddField<BoneInfo, GuGuUtf8Str>("m_boneName",
			(meta::FieldGetter<BoneInfo, GuGuUtf8Str, false>::Signature) & BoneInfo::m_boneName,
			(meta::FieldSetter<BoneInfo, GuGuUtf8Str, false>::Signature) & BoneInfo::m_boneName, {});

		type.AddField<BoneInfo, math::float4x4>("m_offsetMatrix",
			(meta::FieldGetter<BoneInfo, math::float4x4, false>::Signature) & BoneInfo::m_offsetMatrix,
			(meta::FieldSetter<BoneInfo, math::float4x4, false>::Signature) & BoneInfo::m_offsetMatrix, {});

		type.AddField<BoneInfo, int32_t>("m_boneId",
			(meta::FieldGetter<BoneInfo, int32_t, false>::Signature) & BoneInfo::m_boneId,
			(meta::FieldSetter<BoneInfo, int32_t, false>::Signature) & BoneInfo::m_boneId, {});

		return true;
	}

	static bool registerKeyPosition()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::KeyPosition");
		auto& type = db.types[id];
		meta::TypeInfo<KeyPosition>::Register(id, type, true, "FF29E0B9-C9BC-4454-86B0-27B91B555333");

		type.AddConstructor<KeyPosition, false, false>({});

		type.AddConstructor<KeyPosition, true, false>({});

		type.SetArrayConstructor<KeyPosition>();

		type.AddField<KeyPosition, math::float3>("m_position",
			(meta::FieldGetter<KeyPosition, math::float3, false>::Signature) & KeyPosition::m_position,
			(meta::FieldSetter<KeyPosition, math::float3, false>::Signature) & KeyPosition::m_position, {});

		type.AddField<KeyPosition, float>("m_timestamp",
			(meta::FieldGetter<KeyPosition, float, false>::Signature) & KeyPosition::m_timestamp,
			(meta::FieldSetter<KeyPosition, float, false>::Signature) & KeyPosition::m_timestamp, {});

		return true;
	}

	static bool registerKeyRotation()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::KeyRotation");
		auto& type = db.types[id];
		meta::TypeInfo<KeyRotation>::Register(id, type, true, "F45ED69C-B17A-4EB6-A1C2-FE0C0CABB198");

		type.AddConstructor<KeyRotation, false, false>({});

		type.AddConstructor<KeyRotation, true, false>({});

		type.SetArrayConstructor<KeyRotation>();

		type.AddField<KeyRotation, math::quat>("m_orientation",
			(meta::FieldGetter<KeyRotation, math::quat, false>::Signature) & KeyRotation::m_orientation,
			(meta::FieldSetter<KeyRotation, math::quat, false>::Signature) & KeyRotation::m_orientation, {});

		type.AddField<KeyRotation, float>("m_timestamp",
			(meta::FieldGetter<KeyRotation, float, false>::Signature) & KeyRotation::m_timestamp,
			(meta::FieldSetter<KeyRotation, float, false>::Signature) & KeyRotation::m_timestamp, {});

		return true;
	}

	static bool registerKeyScale()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::KeyScale");
		auto& type = db.types[id];
		meta::TypeInfo<KeyScale>::Register(id, type, true, "96235FB4-5D6C-451B-B63E-8B2BECC623E6");

		type.AddConstructor<KeyScale, false, false>({});

		type.AddConstructor<KeyScale, true, false>({});

		type.SetArrayConstructor<KeyScale>();

		type.AddField<KeyScale, math::float3>("m_orientation",
			(meta::FieldGetter<KeyScale, math::float3, false>::Signature) & KeyScale::m_scale,
			(meta::FieldSetter<KeyScale, math::float3, false>::Signature) & KeyScale::m_scale, {});

		type.AddField<KeyScale, float>("m_timestamp",
			(meta::FieldGetter<KeyScale, float, false>::Signature) & KeyScale::m_timestamp,
			(meta::FieldSetter<KeyScale, float, false>::Signature) & KeyScale::m_timestamp, {});

		return true;
	}

	static bool registerChannel()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::Channel");
		auto& type = db.types[id];
		meta::TypeInfo<Channel>::Register(id, type, true, "38C37CFC-2F73-422D-8AFB-35AA54FBCC56");

		type.AddConstructor<Channel, false, false>({});

		type.AddConstructor<Channel, true, false>({});

		type.SetArrayConstructor<Channel>();

		type.AddField<Channel, Array<KeyPosition>>("m_positions",
			(meta::FieldGetter<Channel, Array<KeyPosition>, false>::Signature) & Channel::m_positions,
			(meta::FieldSetter<Channel, Array<KeyPosition>, false>::Signature) & Channel::m_positions, {});

		type.AddField<Channel, Array<KeyRotation>>("m_rotations",
			(meta::FieldGetter<Channel, Array<KeyRotation>, false>::Signature) & Channel::m_rotations,
			(meta::FieldSetter<Channel, Array<KeyRotation>, false>::Signature) & Channel::m_rotations, {});

		type.AddField<Channel, Array<KeyScale>>("m_scales",
			(meta::FieldGetter<Channel, Array<KeyScale>, false>::Signature) & Channel::m_scales,
			(meta::FieldSetter<Channel, Array<KeyScale>, false>::Signature) & Channel::m_scales, {});

		type.AddField<Channel, GuGuUtf8Str>("m_name",
			(meta::FieldGetter<Channel, GuGuUtf8Str, false>::Signature) & Channel::m_name,
			(meta::FieldSetter<Channel, GuGuUtf8Str, false>::Signature) & Channel::m_name, {});

		return true;
	}

	static bool registerMeshGeometry()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::GMeshGeometry");
		auto& type = db.types[id];
		meta::TypeInfo<GMeshGeometry>::Register(id, type, true, "D7C9AF99-BE14-4DB4-8ACE-D951C04D5DFA");

		type.AddConstructor<GMeshGeometry, false, false>({});

		type.AddConstructor<GMeshGeometry, true, false>({});

		type.SetArrayConstructor<GMeshGeometry>();

		type.AddField<GMeshGeometry, GuGuUtf8Str>("m_nodeName",
			(meta::FieldGetter<GMeshGeometry, GuGuUtf8Str, false>::Signature) & GMeshGeometry::m_nodeName,
			(meta::FieldSetter<GMeshGeometry, GuGuUtf8Str, false>::Signature) & GMeshGeometry::m_nodeName, {});

		type.AddField<GMeshGeometry, uint32_t>("m_indexOffsetInMesh",
			(meta::FieldGetter<GMeshGeometry, uint32_t, false>::Signature) & GMeshGeometry::m_indexOffsetInMesh,
			(meta::FieldSetter<GMeshGeometry, uint32_t, false>::Signature) & GMeshGeometry::m_indexOffsetInMesh, {});

		type.AddField<GMeshGeometry, uint32_t>("m_vertexOffsetInMesh",
			(meta::FieldGetter<GMeshGeometry, uint32_t, false>::Signature) & GMeshGeometry::m_vertexOffsetInMesh,
			(meta::FieldSetter<GMeshGeometry, uint32_t, false>::Signature) & GMeshGeometry::m_vertexOffsetInMesh, {});

		type.AddField<GMeshGeometry, uint32_t>("m_numIndices",
			(meta::FieldGetter<GMeshGeometry, uint32_t, false>::Signature) & GMeshGeometry::m_numIndices,
			(meta::FieldSetter<GMeshGeometry, uint32_t, false>::Signature) & GMeshGeometry::m_numIndices, {});

		type.AddField<GMeshGeometry, uint32_t>("m_numVertices",
			(meta::FieldGetter<GMeshGeometry, uint32_t, false>::Signature) & GMeshGeometry::m_numVertices,
			(meta::FieldSetter<GMeshGeometry, uint32_t, false>::Signature) & GMeshGeometry::m_numVertices, {});

		type.AddField<GMeshGeometry, int32_t>("m_id",
			(meta::FieldGetter<GMeshGeometry, int32_t, false>::Signature) & GMeshGeometry::m_id,
			(meta::FieldSetter<GMeshGeometry, int32_t, false>::Signature) & GMeshGeometry::m_id, {});

		type.AddField<GMeshGeometry, int32_t>("m_parentId",
			(meta::FieldGetter<GMeshGeometry, int32_t, false>::Signature) & GMeshGeometry::m_parentId,
			(meta::FieldSetter<GMeshGeometry, int32_t, false>::Signature) & GMeshGeometry::m_parentId, {});

		type.AddField<GMeshGeometry, Array<int32_t>>("m_childrens",
			(meta::FieldGetter<GMeshGeometry, Array<int32_t>, false>::Signature) & GMeshGeometry::m_childrens,
			(meta::FieldSetter<GMeshGeometry, Array<int32_t>, false>::Signature) & GMeshGeometry::m_childrens, {});

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

	static bool registerCollisionResult()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::CollisionResult");
		auto& type = db.types[id];
		meta::TypeInfo<CollisionResult>::Register(id, type, true, "5B835168-14F8-43DE-A823-3DB4AB386F3E");

		type.AddConstructor<CollisionResult, false, false>({});

		type.AddConstructor<CollisionResult, true, false>({});

		type.SetArrayConstructor<CollisionResult>();

		type.AddField<CollisionResult, math::float3>("m_hitNormal",
			(meta::FieldGetter<CollisionResult, math::float3, false>::Signature) & CollisionResult::m_hitNormal,
			(meta::FieldSetter<CollisionResult, math::float3, false>::Signature) & CollisionResult::m_hitNormal, {});

		type.AddField<CollisionResult, math::float3>("m_hitPosition",
			(meta::FieldGetter<CollisionResult, math::float3, false>::Signature) & CollisionResult::m_hitPosition,
			(meta::FieldSetter<CollisionResult, math::float3, false>::Signature) & CollisionResult::m_hitPosition, {});

		return true;
	}

	static bool registerKeys()
	{
		//key
		{
			auto& db = meta::ReflectionDatabase::Instance();
			auto id = db.AllocateType("GuGu::Key");
			auto& type = db.types[id];
			meta::TypeInfo<Key>::Register(id, type, true, "FAD2C2EA-6B75-410D-9708-59C264CAE7D7");

			type.AddConstructor<Key, false, false>({});

			type.AddConstructor<Key, true, false>({});

			type.SetArrayConstructor<Key>();
		}

		//keys
		{
			auto& db = meta::ReflectionDatabase::Instance();
			auto id = db.AllocateType("GuGu::Keys");
			auto& type = db.types[id];
			meta::TypeInfo<Keys>::Register(id, type, true, "0FDD1FEB-2F82-475A-9CC2-B74DC1109A3B");

			type.AddConstructor<Keys, false, false>({});

			type.AddConstructor<Keys, true, false>({});

			type.SetArrayConstructor<Keys>();

			//TODO:add static field
			type.AddStaticField<Keys, Key>("W",
				(Key*)&Keys::W,
				(Key*)&Keys::W, {});

			type.AddStaticField<Keys, Key>("S",
				(Key*)&Keys::S,
				(Key*)&Keys::S, {});

			type.AddStaticField<Keys, Key>("A",
				(Key*)&Keys::A,
				(Key*)&Keys::A, {});

			type.AddStaticField<Keys, Key>("D",
				(Key*)&Keys::D,
				(Key*)&Keys::D, {});

			type.AddStaticField<Keys, Key>("SpaceBar",
				(Key*)&Keys::SpaceBar,
				(Key*)&Keys::SpaceBar, {});
		}
		
		return true;
	}

	static bool registerInputManager()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::InputManager");
		auto& type = db.types[id];
		meta::TypeInfo<InputManager>::Register(id, type, true, "0464021E-7145-468E-94CD-A302D25D2DF3");

		type.AddConstructor<InputManager, false, false>({});

		type.AddConstructor<InputManager, true, false>({});

		type.SetArrayConstructor<InputManager>();

		type.AddMethod("isKeyDown", &InputManager::isKeyDown, {});

		type.AddMethod("getMouseDelta", &InputManager::getMouseDelta, {});

		type.AddStaticMethod<InputManager>("getInputManager", &InputManager::getInputManager, {});

		return true;
	}

	void registerThirdParty()
	{
		//Priority metaDisplayNamePriority;
		//metaDisplayNamePriority.setDebugName("meta::DisplayName");
		//ReflectionMain::addInitialTypeFunction(registermetaDisplayName, &metaDisplayNamePriority);

		//Priority mathfloat2Priority;
		mathfloat2Priority.setDebugName("GuGu::math::float2");
		ReflectionMain::addInitialTypeFunction(registermathfloat2, &mathfloat2Priority);

		//Priority mathfloat3Priority;
		mathfloat3Priority.setDebugName("GuGu::math::float3");
		ReflectionMain::addInitialTypeFunction(registermathfloat3, &mathfloat3Priority);

		//Priority mathfloat4Priority;
		mathfloat4Priority.setDebugName("GuGu::math::float4");
		ReflectionMain::addInitialTypeFunction(registermathfloat4, &mathfloat4Priority);

		mathdouble2Priority.setDebugName("GuGu::math::double2");
		ReflectionMain::addInitialTypeFunction(registermathdouble2, &mathdouble2Priority);

		//Priority mathdouble3Priority;
		mathdouble3Priority.setDebugName("GuGu::math::double3");
		ReflectionMain::addInitialTypeFunction(registermathdouble3, &mathdouble3Priority);

		mathquatPriority.setDebugName("GuGu::math::quat");
		ReflectionMain::addInitialTypeFunction(registermathquat, &mathquatPriority);

		//Priority mathdquatPriority;
		mathdquatPriority.setDebugName("GuGu::math::dquat");
		ReflectionMain::addInitialTypeFunction(registermathdquat, &mathdquatPriority);

		//Priority mathuint16_4Priority;
		mathuint16_4Priority.setDebugName("GuGu::math::uint16_4");
		ReflectionMain::addInitialTypeFunction(registermathuint16_4, &mathuint16_4Priority);

		mathint16_4Priority.setDebugName("GuGu::math::int16_4");
		ReflectionMain::addInitialTypeFunction(registermathint16_4, &mathint16_4Priority);

		mathrotatorPriority.setDebugName("GuGu::math::Rotator");
		ReflectionMain::addInitialTypeFunction(registerRotator, &mathrotatorPriority);

		mathfloat4x4Priority.setDebugName("GuGu::math::float4x4");
		ReflectionMain::addInitialTypeFunction(registerfloat4x4, &mathfloat4x4Priority);
        mathfloat4x4Priority.addPriorityThan(&mathfloat4Priority);

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
		ReflectionMain::addInitialTypeFunction(registerBoneInfo, &boneInfoPriority);
		uiPaddingPriority.addPriorityThan(&meta::Range::ms_priority);
		uiAnchorsPriority.addPriorityThan(&meta::Range::ms_priority);
		uiAnchorDataPrority.addPriorityThan(&uiPaddingPriority); //padding
		uiAnchorDataPrority.addPriorityThan(&uiAnchorsPriority); //anchor
		uiAnchorDataPrority.addPriorityThan(&mathfloat2Priority); //alignment

		boneInfoPriority.addPriorityThan(&mathfloat4x4Priority);//bone info

		ReflectionMain::addInitialTypeFunction(registerKeyPosition, &keyPositionPriority);
		ReflectionMain::addInitialTypeFunction(registerKeyRotation, &keyRotationPriority);
		ReflectionMain::addInitialTypeFunction(registerKeyScale, &keyScalePriority);
		ReflectionMain::addInitialTypeFunction(registerChannel, &channelPriority);
		ReflectionMain::addInitialTypeFunction(registerMeshGeometry, &meshGeometryPriority);
		ReflectionMain::addInitialTypeFunction(registerCollisionResult, &collisionResultPriority);
		ReflectionMain::addInitialTypeFunction(registerKeys, &keysPriority);
		ReflectionMain::addInitialTypeFunction(registerInputManager, &inputManagerPriority);
		keyPositionPriority.addPriorityThan(&mathfloat3Priority);
		keyRotationPriority.addPriorityThan(&mathquatPriority);
		keyScalePriority.addPriorityThan(&mathfloat3Priority);
		channelPriority.addPriorityThan(&keyPositionPriority);
		channelPriority.addPriorityThan(&keyRotationPriority);
		channelPriority.addPriorityThan(&keyScalePriority);
		collisionResultPriority.addPriorityThan(&mathfloat3Priority);
		inputManagerPriority.addPriorityThan(&keysPriority);

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

		ScriptComponent::registerMainFactory();
		ScriptComponent::registerMainFactory2();

		GAnimator::registerMainFactory();
		GAnimator::registerMainFactory2();

		GAnimation::ms_priority2.addPriorityThan(&channelPriority);
		GAnimation::registerMainFactory();
		GAnimation::registerMainFactory2();

		GStaticMesh::ms_priority2.addPriorityThan(&boneInfoPriority);
		GStaticMesh::registerMainFactory();
		GStaticMesh::registerMainFactory2();

		GuGuScriptDelegate::registerMainFactory();
		GuGuScriptDelegate::registerMainFactory2();

		GrassComponent::registerMainFactory();
		GrassComponent::registerMainFactory2();

		TerrainVegetationComponent::registerMainFactory();
		TerrainVegetationComponent::registerMainFactory2();

        Collision3DComponent::registerMainFactory();
        Collision3DComponent::registerMainFactory2();
		ReflectionMain::initialize();

		meta::Type::List types = meta::Type::GetTypes();
		for (const auto& type : types)
		{
			meta::TypeData& typeData = meta::ReflectionDatabase::Instance().types[type.GetID()];
			db.registerGuid(typeData.typeGuid, type.GetID());
		}

		meta::ReflectionDatabase::Instance().checkHaveSameTypeGuid();
	}
}