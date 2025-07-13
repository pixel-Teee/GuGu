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

#include <Renderer/Color.h>

#include <Core/AssetManager/AssetData.h>

#include <Core/Reflection/Priority.h>
#include <Core/Reflection/ReflectionMain.h>
#include <Core/Reflection/ReflectionMacro.h>

namespace GuGu {
	void gamePlayerReflectionRegister()
	{
		auto& db = meta::ReflectionDatabase::Instance();

		//meta
		//{
		//	auto id = db.AllocateType("meta::DisplayName");
		//	auto& type = db.types[id];
		//	meta::TypeInfo<meta::DisplayName>::Register(id, type, true, "86AC4FFE-0C5A-46F2-BF40-AAE89636AE44");
		//}

		//数学
// 		{
// 			auto id = db.AllocateType("math::float2");
// 			auto& type = db.types[id];
// 			meta::TypeInfo<math::float2>::Register(id, type, true, "E73886F1-1745-4885-AC10-A6764668698E");
// 
// 			type.AddConstructor<math::float2, false, false>({});
// 
// 			type.AddConstructor<math::float2, true, false>({});
// 
// 			type.SetArrayConstructor<math::float2>();
// 
// 			type.AddField<math::float2, float>("x",
// 				(meta::FieldGetter<math::float2, float, false>::Signature) & math::float2::x,
// 				(meta::FieldSetter<math::float2, float, false>::Signature) & math::float2::x, {});
// 
// 			type.AddField<math::float2, float>("y",
// 				(meta::FieldGetter<math::float2, float, false>::Signature) & math::float2::y,
// 				(meta::FieldSetter<math::float2, float, false>::Signature) & math::float2::y, {});
// 		}
// 		{
// 			auto id = db.AllocateType("math::float3");
// 			auto& type = db.types[id];
// 			meta::TypeInfo<math::float3>::Register(id, type, true, "84172191-B81E-46A4-B06F-7E5B1F8712D0");
// 
// 			type.AddConstructor<math::float3, false, false>({});
// 
// 			type.AddConstructor<math::float3, true, false>({});
// 
// 			type.SetArrayConstructor<math::float3>();
// 
// 			type.AddField<math::float3, float>("x",
// 				(meta::FieldGetter<math::float3, float, false>::Signature) & math::float3::x,
// 				(meta::FieldSetter<math::float3, float, false>::Signature) & math::float3::x, {});
// 
// 			type.AddField<math::float3, float>("y",
// 				(meta::FieldGetter<math::float3, float, false>::Signature) & math::float3::y,
// 				(meta::FieldSetter<math::float3, float, false>::Signature) & math::float3::y, {});
// 
// 			type.AddField<math::float3, float>("z",
// 				(meta::FieldGetter<math::float3, float, false>::Signature) & math::float3::z,
// 				(meta::FieldSetter<math::float3, float, false>::Signature) & math::float3::z, {});
// 		}
		{
// 			auto id = db.AllocateType("math::float4");
// 			auto& type = db.types[id];
// 			meta::TypeInfo<math::float4>::Register(id, type, true, "50C5417E-8CD2-4F6A-A030-78FBA2C55F61");
// 
// 			type.AddConstructor<math::float4, false, false>({});
// 
// 			type.AddConstructor<math::float4, true, false>({});
// 
// 			type.SetArrayConstructor<math::float4>();
// 
// 			type.AddField<math::float4, float>("x",
// 				(meta::FieldGetter<math::float4, float, false>::Signature) & math::float4::x,
// 				(meta::FieldSetter<math::float4, float, false>::Signature) & math::float4::x, {});
// 
// 			type.AddField<math::float4, float>("y",
// 				(meta::FieldGetter<math::float4, float, false>::Signature) & math::float4::y,
// 				(meta::FieldSetter<math::float4, float, false>::Signature) & math::float4::y, {});
// 
// 			type.AddField<math::float4, float>("z",
// 				(meta::FieldGetter<math::float4, float, false>::Signature) & math::float4::z,
// 				(meta::FieldSetter<math::float4, float, false>::Signature) & math::float4::z, {});
// 
// 			type.AddField<math::float4, float>("w",
// 				(meta::FieldGetter<math::float4, float, false>::Signature) & math::float4::w,
// 				(meta::FieldSetter<math::float4, float, false>::Signature) & math::float4::w, {});
		}
		{
			//auto id = db.AllocateType("math::double3");
			//auto& type = db.types[id];
			//meta::TypeInfo<math::double3>::Register(id, type, true, "35EF324D-9D0A-4A20-82A4-E61BF62C5960");
			//
			//type.AddConstructor<math::double3, false, false>({});
			//
			//type.AddConstructor<math::double3, true, false>({});
			//
			//type.SetArrayConstructor<math::double3>();
			//
			//type.AddField<math::double3, double>("x",
			//	(meta::FieldGetter<math::double3, double, false>::Signature) & math::double3::x,
			//	(meta::FieldSetter<math::double3, double, false>::Signature) & math::double3::x, {});
			//
			//type.AddField<math::double3, double>("y",
			//	(meta::FieldGetter<math::double3, double, false>::Signature) & math::double3::y,
			//	(meta::FieldSetter<math::double3, double, false>::Signature) & math::double3::y, {});
			//
			//type.AddField<math::double3, double>("z",
			//	(meta::FieldGetter<math::double3, double, false>::Signature) & math::double3::z,
			//	(meta::FieldSetter<math::double3, double, false>::Signature) & math::double3::z, {});
		}

		{
			//auto id = db.AllocateType("math::dquat");
			//auto& type = db.types[id];
			//meta::TypeInfo<math::dquat>::Register(id, type, true, "A576462F-80EF-4132-9096-AF19E505671B");
			//
			//type.AddConstructor<math::dquat, false, false>({});
			//
			//type.AddConstructor<math::dquat, true, false>({});
			//
			//type.SetArrayConstructor<math::dquat>();
			//
			//type.AddField<math::dquat, double>("x",
			//	(meta::FieldGetter<math::dquat, double, false>::Signature) & math::dquat::x,
			//	(meta::FieldSetter<math::dquat, double, false>::Signature) & math::dquat::x, {});
			//
			//type.AddField<math::dquat, double>("y",
			//	(meta::FieldGetter<math::dquat, double, false>::Signature) & math::dquat::y,
			//	(meta::FieldSetter<math::dquat, double, false>::Signature) & math::dquat::y, {});
			//
			//type.AddField<math::dquat, double>("z",
			//	(meta::FieldGetter<math::dquat, double, false>::Signature) & math::dquat::z,
			//	(meta::FieldSetter<math::dquat, double, false>::Signature) & math::dquat::z, {});
			//
			//type.AddField<math::dquat, double>("w",
			//	(meta::FieldGetter<math::dquat, double, false>::Signature) & math::dquat::w,
			//	(meta::FieldSetter<math::dquat, double, false>::Signature) & math::dquat::w, {});
		}

		{
			//auto id = db.AllocateType("math::vector<uin16_t, 4>");
			//auto& type = db.types[id];
			//meta::TypeInfo<math::vector<uint16_t, 4>>::Register(id, type, true, "DD2D2988-9B15-41B1-82F2-2F32D095D4AA");
			//
			//type.AddConstructor<math::vector<uint16_t, 4>, false, false>({});
			//
			//type.AddConstructor<math::vector<uint16_t, 4>, true, false>({});
			//
			//type.SetArrayConstructor<math::vector<uint16_t, 4>>();
			//
			//type.AddField<math::vector<uint16_t, 4>, uint16_t>("x",
			//	(meta::FieldGetter<math::vector<uint16_t, 4>, uint16_t, false>::Signature)& math::vector<uint16_t, 4>::x,
			//	(meta::FieldSetter<math::vector<uint16_t, 4>, uint16_t, false>::Signature)& math::vector<uint16_t, 4>::x, {});
			//
			//type.AddField<math::vector<uint16_t, 4>, uint16_t>("y",
			//	(meta::FieldGetter<math::vector<uint16_t, 4>, uint16_t, false>::Signature)& math::vector<uint16_t, 4>::y,
			//	(meta::FieldSetter<math::vector<uint16_t, 4>, uint16_t, false>::Signature)& math::vector<uint16_t, 4>::y, {});
			//
			//type.AddField<math::vector<uint16_t, 4>, uint16_t>("z",
			//	(meta::FieldGetter<math::vector<uint16_t, 4>, uint16_t, false>::Signature)& math::vector<uint16_t, 4>::z,
			//	(meta::FieldSetter<math::vector<uint16_t, 4>, uint16_t, false>::Signature)& math::vector<uint16_t, 4>::z, {});
			//
			//type.AddField<math::vector<uint16_t, 4>, uint16_t>("w",
			//	(meta::FieldGetter<math::vector<uint16_t, 4>, uint16_t, false>::Signature)& math::vector<uint16_t, 4>::w,
			//	(meta::FieldSetter<math::vector<uint16_t, 4>, uint16_t, false>::Signature)& math::vector<uint16_t, 4>::w, {});
		}

		//color
		{
			//auto id = db.AllocateType("GuGu::Color");
			//auto& type = db.types[id];
			//meta::TypeInfo<Color>::Register(id, type, true, "87AC627B-14AE-4588-8E3F-80A36AF8C676");
			//
			//type.AddConstructor<Color, false, false>({});
			//
			//type.AddConstructor<Color, true, false>({});
			//
			//type.SetArrayConstructor<Color>();
			//
			//type.AddField<Color, float>("r",
			//	(meta::FieldGetter<Color, float, false>::Signature) & Color::r,
			//	(meta::FieldSetter<Color, float, false>::Signature) & Color::r, {});
			//
			//type.AddField<Color, float>("g",
			//	(meta::FieldGetter<Color, float, false>::Signature) & Color::g,
			//	(meta::FieldSetter<Color, float, false>::Signature) & Color::g, {});
			//
			//type.AddField<Color, float>("b",
			//	(meta::FieldGetter<Color, float, false>::Signature) & Color::b,
			//	(meta::FieldSetter<Color, float, false>::Signature) & Color::b, {});
			//
			//type.AddField<Color, float>("a",
			//	(meta::FieldGetter<Color, float, false>::Signature) & Color::a,
			//	(meta::FieldSetter<Color, float, false>::Signature) & Color::a, {});
		}

		//meta::object
		{
			//auto id = db.AllocateType("GuGu::meta::Object");
			//auto& type = db.types[id];
			//meta::TypeInfo<meta::Object>::Register(id, type, true, "E6023C42-0DA9-4F76-8290-CDB926C881E3");
			//
			//type.meta = {
			//	std::make_pair(typeof(meta::DisplayName), meta::MetaPropertyInitializer<meta::DisplayName>("Object"))
			//};

		}

		{
			//auto id = db.AllocateType("GuGu::Component");
			//auto& type = db.types[id];
			//meta::TypeInfo<Component>::Register(id, type, true, "0955EA47-6CB3-4F4E-929A-A19769B4DD93");
			//
			//type.AddField<Component, std::weak_ptr<GameObject>>("m_owner",
			//	(meta::FieldGetter<Component, std::weak_ptr<GameObject>&, true>::Signature)& Component::getParentGameObject,
			//	(meta::FieldSetter<Component, std::weak_ptr<GameObject>&, true>::Signature)& Component::setParentGameObject, {});
		}

// 		{
// 			auto id = db.AllocateType("std::shared_ptr<GuGu::Component>");
// 			auto& type = db.types[id];
// 			meta::TypeInfo<std::shared_ptr<Component>>::Register(id, type, false, "D7753E39-309C-4C52-BF1D-BCA0F90D6352");
// 
// 			type.SetArrayConstructor<std::shared_ptr<Component>>();
// 		}
// 
// 		{
// 			auto id = db.AllocateType("std::weak_ptr<GuGu::Component>");
// 			auto& type = db.types[id];
// 			meta::TypeInfo<std::weak_ptr<Component>>::Register(id, type, false, "1CA55F88-8D64-445F-AE1B-E6635525E0DA");
// 		}

		//game object
		{
// 			auto id = db.AllocateType("GuGu::GameObject");
// 			auto& type = db.types[id];
// 			meta::TypeInfo<GuGu::GameObject>::Register(id, type, true, "764220D9-31E0-448B-9612-79A47B570367");
// 
// 			type.meta = {
// 				std::make_pair(typeof(meta::DisplayName), meta::MetaPropertyInitializer<meta::DisplayName>("GameObject"))
// 			};
// 
// 			{
// 				auto id = db.AllocateType("std::shared_ptr<GuGu::GameObject>");
// 				auto& type = db.types[id];
// 				meta::TypeInfo<std::shared_ptr<GameObject>>::Register(id, type, false, "5126D2F9-FCF4-4020-8BBB-187389965FDE");
// 
// 				type.SetArrayConstructor<std::shared_ptr<GameObject>>();
// 			}
// 
// 			{
// 				auto id = db.AllocateType("std::weak_ptr<GuGu::GameObject>");
// 				auto& type = db.types[id];
// 				meta::TypeInfo<std::weak_ptr<GameObject>>::Register(id, type, false, "E3D8C076-BFD3-4FD1-B108-5FBB37169DE3");
// 			}
// 
// 			auto typeID = typeidof(GameObject);
// 			if (typeID != meta::InvalidTypeID && !meta::TypeInfo<GameObject>::Defined)
// 			{
// 				auto& type = db.types[typeID];
// 
// 				//array constructor
// 				type.SetArrayConstructor<GameObject>();
// 
// 				type.AddConstructor<GameObject, false, false>({});
// 
// 				type.AddConstructor<GameObject, true, true>({});
// 
// 				type.AddField<GameObject, Array<std::shared_ptr<Component>>>("m_components",
// 					(meta::FieldGetter<GameObject, Array<std::shared_ptr<Component>>, true>::Signature) & GameObject::getComponents,
// 					(meta::FieldSetter<GameObject, Array<std::shared_ptr<Component>>, true>::Signature) & GameObject::setComponents, {});
// 
// 				type.AddField<GameObject, Array<std::shared_ptr<GameObject>>>("m_childrens",
// 					(meta::FieldGetter<GameObject, Array<std::shared_ptr<GameObject>>, true>::Signature) & GameObject::getChildrens,
// 					(meta::FieldSetter<GameObject, Array<std::shared_ptr<GameObject>>, true>::Signature) & GameObject::setChildrens, {});
// 
// 				type.AddField<GameObject, std::weak_ptr<GameObject>>("m_parentGameObject",
// 					(meta::FieldGetter<GameObject, std::weak_ptr<GameObject>&, true>::Signature)& GameObject::getParentGameObject,
// 					(meta::FieldSetter<GameObject, std::weak_ptr<GameObject>&, true>::Signature)& GameObject::setParentGameObject, {});
// 
// 				type.LoadBaseClasses(db, typeID, { typeof(meta::Object) });
// 
// 				meta::TypeInfo<GameObject>::Defined = true;
// 			}
		}

		//level
		{
// 			auto id = db.AllocateType("GuGu::Level");
// 			auto& type = db.types[id];
// 			meta::TypeInfo<Level>::Register(id, type, true, "64CB643F-2C9C-4857-AA01-4F6F995A4CBE");
// 
// 			auto typeID = typeidof(Level);
// 			if (typeID != meta::InvalidTypeID && !meta::TypeInfo<Level>::Defined)
// 			{
// 				auto& type = db.types[typeID];
// 
// 				//array constructor
// 				type.SetArrayConstructor<Level>();
// 
// 				type.AddConstructor<Level, false, false>({});
// 
// 				type.AddConstructor<Level, true, true>({});
// 
// 				type.LoadBaseClasses(db, typeID, {typeof(meta::Object)});
// 
// 				type.AddField<Level, Array<std::shared_ptr<GameObject>>>("m_objects", 
// 					(meta::FieldGetter<Level, Array<std::shared_ptr<GameObject>>&, true>::Signature)&Level::getGameObjects,
// 					(meta::FieldSetter<Level, Array<std::shared_ptr<GameObject>>&, true>::Signature)&Level::setGameObjects, {});
// 
// 				meta::TypeInfo<Level>::Defined = true;
// 			}
		}

// 		{
// 			auto id = db.AllocateType("std::shared_ptr<GuGu::Level>");
// 			auto& type = db.types[id];
// 			meta::TypeInfo<std::shared_ptr<Level>>::Register(id, type, false, "5078441C-57E0-4322-9294-11D02360F9C9");
// 		}
// 
// 		{
// 			auto id = db.AllocateType("std::weak_ptr<GuGu::Level>");
// 			auto& type = db.types[id];
// 			meta::TypeInfo<std::weak_ptr<Level>>::Register(id, type, false, "A027395A-6B76-474C-89D3-1D56ACC8212F");
// 		}

		//资源
// 		{
// 			auto id = db.AllocateType("GuGu::AssetData");
// 			auto& type = db.types[id];
// 			meta::TypeInfo<AssetData>::Register(id, type, true, "5A9DF858-72C3-4EC2-856D-3DF59457D874");
// 
// 			type.LoadBaseClasses(db, id, { typeof(meta::Object) });
// 		}
// 		{
// 			auto id = db.AllocateType("std::shared_ptr<GuGu::AssetData>");
// 			auto& type = db.types[id];
// 			meta::TypeInfo<std::shared_ptr<AssetData>>::Register(id, type, false, "21C647F2-BF00-4919-85EF-53D77FB3BC07");
// 		}
// 		{
// 			auto id = db.AllocateType("std::weak_ptr<GuGu::AssetData>");
// 			auto& type = db.types[id];
// 			meta::TypeInfo<std::weak_ptr<AssetData>>::Register(id, type, false, "3B42C324-06E4-49D3-AC0F-ADAE86A2DF39");
// 		}

		//各种 component
// 		{
// 			auto id = db.AllocateType("GuGu::TransformComponent");
// 			auto& type = db.types[id];
// 			meta::TypeInfo<TransformComponent>::Register(id, type, true, "57B43954-87AF-495A-8B58-E15890E621D1");
// 
// 			auto typeID = typeidof(TransformComponent);
// 			if (typeID != meta::InvalidTypeID && !meta::TypeInfo<TransformComponent>::Defined)
// 			{
// 				auto& type = db.types[typeID];
// 
// 				//array constructor
// 				type.SetArrayConstructor<TransformComponent>();
// 
// 				type.AddConstructor<TransformComponent, false, false>({});
// 
// 				type.AddConstructor<TransformComponent, true, true>({});
// 
// 				type.AddField<TransformComponent, math::double3>("m_Translation",
// 					(meta::FieldGetter<TransformComponent, math::double3&, true>::Signature)&TransformComponent::getTranslation,
// 					(meta::FieldSetter<TransformComponent, math::double3, true>::Signature)&TransformComponent::SetTranslation, {});
// 				type.AddField<TransformComponent, math::dquat>("m_Rotation",
// 					(meta::FieldGetter<TransformComponent, math::dquat, true>::Signature)&TransformComponent::getRotation,
// 					(meta::FieldSetter<TransformComponent, math::dquat, true>::Signature)&TransformComponent::SetRotation, {});
// 				type.AddField<TransformComponent, math::double3>("m_Scaling",
// 					(meta::FieldGetter<TransformComponent, math::double3&, true>::Signature)&TransformComponent::getScaling,
// 					(meta::FieldSetter<TransformComponent, math::double3, true>::Signature)&TransformComponent::SetScaling, {});
// 
// 				type.AddField<TransformComponent, std::weak_ptr<GameObject>>("m_owner",
// 					(meta::FieldGetter<TransformComponent, std::weak_ptr<GameObject>&, true>::Signature)& TransformComponent::getParentGameObject,
// 					(meta::FieldSetter<TransformComponent, std::weak_ptr<GameObject>&, true>::Signature)& TransformComponent::setParentGameObject, {});
// 
// 				type.LoadBaseClasses(db, typeID, { typeof(Component) });
// 
// 				meta::TypeInfo<TransformComponent>::Defined = true;
// 
// 				type.meta = {
// 					std::make_pair(typeof(meta::DisplayName), meta::MetaPropertyInitializer<meta::DisplayName>("TransformComponent"))
// 				};
// 			}
// 
// 			{
// 				auto id = db.AllocateType("std::shared_ptr<GuGu::TransformComponent>");
// 				auto& type = db.types[id];
// 				meta::TypeInfo<std::shared_ptr<TransformComponent>>::Register(id, type, false, "311DBA9A-2885-4656-882D-81FF8EF66502");
// 			}
// 
// 			{
// 				auto id = db.AllocateType("std::weak_ptr<GuGu::TransformComponent>");
// 				auto& type = db.types[id];
// 				meta::TypeInfo<std::weak_ptr<TransformComponent>>::Register(id, type, false, "DC63130F-4532-4F84-9A37-11567967CEFA");
// 			}
// 		}

		{
// 			auto id = db.AllocateType("GuGu::StaticMeshComponent");
// 			auto& type = db.types[id];
// 			meta::TypeInfo<StaticMeshComponent>::Register(id, type, true, "303B0EAC-B07E-42E1-B6E9-A56F00E75814");
// 
// 			auto typeID = typeidof(StaticMeshComponent);
// 			if (typeID != meta::InvalidTypeID && !meta::TypeInfo<StaticMeshComponent>::Defined)
// 			{
// 				auto& type = db.types[typeID];
// 
// 				//array constructor
// 				type.SetArrayConstructor<StaticMeshComponent>();
// 
// 				type.AddConstructor<StaticMeshComponent, false, false>({});
// 
// 				type.AddConstructor<StaticMeshComponent, true, true>({});
// 
// 				type.LoadBaseClasses(db, typeID, { typeof(Component) });
// 				
// 				type.AddField<StaticMeshComponent, std::shared_ptr<AssetData>>("m_staticMeshAsset",
// 					(meta::FieldGetter<StaticMeshComponent, std::shared_ptr<AssetData>, true>::Signature)& StaticMeshComponent::getStaticMeshAsset,
// 					(meta::FieldSetter<StaticMeshComponent, std::shared_ptr<AssetData>, true>::Signature)& StaticMeshComponent::setStaticMeshAsset, {});
// 
// 				type.AddField<StaticMeshComponent, std::weak_ptr<GameObject>>("m_owner",
// 					(meta::FieldGetter<StaticMeshComponent, std::weak_ptr<GameObject>&, true>::Signature)& StaticMeshComponent::getParentGameObject,
// 					(meta::FieldSetter<StaticMeshComponent, std::weak_ptr<GameObject>&, true>::Signature)& StaticMeshComponent::setParentGameObject, {});
// 
// 				meta::TypeInfo<StaticMeshComponent>::Defined = true;
// 			}
// 
// 			{
// 				auto id = db.AllocateType("std::shared_ptr<GuGu::StaticMeshComponent>");
// 				auto& type = db.types[id];
// 				meta::TypeInfo<std::shared_ptr<StaticMeshComponent>>::Register(id, type, false, "81D20567-8580-45DF-9DC9-785225EC85FF");
// 			}
// 
// 			{
// 				auto id = db.AllocateType("std::weak_ptr<GuGu::StaticMeshComponent>");
// 				auto& type = db.types[id];
// 				meta::TypeInfo<std::weak_ptr<StaticMeshComponent>>::Register(id, type, false, "E07DC6B1-27A4-4875-B5FF-D49F82FAE211");
// 			}
		}

		{
// 			auto id = db.AllocateType("GuGu::LightComponent");
// 			auto& type = db.types[id];
// 			meta::TypeInfo<LightComponent>::Register(id, type, true, "3EF471CA-C9A2-4F2B-AEA9-326276AE4763");
// 
// 			auto typeID = typeidof(LightComponent);
// 			if (typeID != meta::InvalidTypeID && !meta::TypeInfo<LightComponent>::Defined)
// 			{
// 				auto& type = db.types[typeID];
// 
// 				//array constructor
// 				type.SetArrayConstructor<LightComponent>();
// 
// 				type.AddConstructor<LightComponent, false, false>({});
// 
// 				type.AddConstructor<LightComponent, true, true>({});
// 
// 				type.AddField<LightComponent, Color>("m_lightColor",
// 					(meta::FieldGetter<LightComponent, Color, false>::Signature) & LightComponent::m_lightColor,
// 					(meta::FieldSetter<LightComponent, Color, false>::Signature) & LightComponent::m_lightColor, {});
// 
// 				type.AddField<LightComponent, math::float4>("m_lightPosition",
// 					(meta::FieldGetter<LightComponent, math::float4, false>::Signature)& LightComponent::m_lightPosition,
// 					(meta::FieldSetter<LightComponent, math::float4, false>::Signature)& LightComponent::m_lightPosition, {});
// 
// 				type.AddField<LightComponent, std::weak_ptr<GameObject>>("m_owner",
// 					(meta::FieldGetter<LightComponent, std::weak_ptr<GameObject>&, true>::Signature)& LightComponent::getParentGameObject,
// 					(meta::FieldSetter<LightComponent, std::weak_ptr<GameObject>&, true>::Signature)& LightComponent::setParentGameObject, {});
// 
// 				type.LoadBaseClasses(db, typeID, { typeof(Component) });
// 
// 				meta::TypeInfo<LightComponent>::Defined = true;
// 			}
// 
// 			{
// 				auto id = db.AllocateType("std::shared_ptr<GuGu::LightComponent>");
// 				auto& type = db.types[id];
// 				meta::TypeInfo<std::shared_ptr<LightComponent>>::Register(id, type, false, "1E0975BC-3D79-466A-A383-C7EAAEB21605");
// 			}
// 
// 			{
// 				auto id = db.AllocateType("std::weak_ptr<GuGu::LightComponent>");
// 				auto& type = db.types[id];
// 				meta::TypeInfo<std::weak_ptr<LightComponent>>::Register(id, type, false, "77075968-2FB0-40BC-A431-C57C530A6724");
// 			}

		}

		{
// 			auto id = db.AllocateType("GuGu::MaterialComponent");
// 			auto& type = db.types[id];
// 			meta::TypeInfo<MaterialComponent>::Register(id, type, true, "5B3EA494-8F58-4A28-A623-CCF822CDF3A1");
// 
// 			auto typeID = typeidof(MaterialComponent);
// 			if (typeID != meta::InvalidTypeID && !meta::TypeInfo<MaterialComponent>::Defined)
// 			{
// 				auto& type = db.types[typeID];
// 
// 				//array constructor
// 				type.SetArrayConstructor<MaterialComponent>();
// 
// 				type.AddConstructor<MaterialComponent, false, false>({});
// 
// 				type.AddConstructor<MaterialComponent, true, true>({});
// 
// 				type.LoadBaseClasses(db, typeID, { typeof(Component) });
// 
// 				meta::TypeInfo<MaterialComponent>::Defined = true;
// 
// 				type.AddField<MaterialComponent, Color>("m_albedo",
// 					(meta::FieldGetter<MaterialComponent, Color, false>::Signature)& MaterialComponent::m_albedo,
// 					(meta::FieldSetter<MaterialComponent, Color, false>::Signature)& MaterialComponent::m_albedo, {});
// 
// 				type.AddField<MaterialComponent, float>("m_metallic",
// 					(meta::FieldGetter<MaterialComponent, float, false>::Signature)& MaterialComponent::m_metallic,
// 					(meta::FieldSetter<MaterialComponent, float, false>::Signature)& MaterialComponent::m_metallic, {});
// 
// 				type.AddField<MaterialComponent, float>("m_roughness",
// 					(meta::FieldGetter<MaterialComponent, float, false>::Signature)& MaterialComponent::m_roughness,
// 					(meta::FieldSetter<MaterialComponent, float, false>::Signature)& MaterialComponent::m_roughness, {});
// 
// 				type.AddField<MaterialComponent, float>("m_ao",
// 					(meta::FieldGetter<MaterialComponent, float, false>::Signature)& MaterialComponent::m_ao,
// 					(meta::FieldSetter<MaterialComponent, float, false>::Signature)& MaterialComponent::m_ao, {});
// 
// 				type.AddField<MaterialComponent, std::shared_ptr<AssetData>>("m_albedoTexture",
// 					(meta::FieldGetter<MaterialComponent, std::shared_ptr<AssetData>, true>::Signature)& MaterialComponent::getAlbeoTextureAsset,
// 					(meta::FieldSetter<MaterialComponent, std::shared_ptr<AssetData>, true>::Signature)& MaterialComponent::setAlbeoTextureAsset, {});
// 
// 				type.AddField<MaterialComponent, std::weak_ptr<GameObject>>("m_owner",
// 					(meta::FieldGetter<MaterialComponent, std::weak_ptr<GameObject>&, true>::Signature)& MaterialComponent::getParentGameObject,
// 					(meta::FieldSetter<MaterialComponent, std::weak_ptr<GameObject>&, true>::Signature)& MaterialComponent::setParentGameObject, {});
// 			}
// 
// 			{
// 				auto id = db.AllocateType("std::shared_ptr<GuGu::MaterialComponent>");
// 				auto& type = db.types[id];
// 				meta::TypeInfo<std::shared_ptr<MaterialComponent>>::Register(id, type, false, "B502E20C-C5B2-4118-8B34-BE7284185CE4");
// 			}
// 
// 			{
// 				auto id = db.AllocateType("std::weak_ptr<GuGu::MaterialComponent>");
// 				auto& type = db.types[id];
// 				meta::TypeInfo<std::weak_ptr<MaterialComponent>>::Register(id, type, false, "236A394A-0FD7-4339-95BA-C89B254474B9");
// 			}
		}

		{
// 			auto id = db.AllocateType("GuGu::TerrainComponent");
// 			auto& type = db.types[id];
// 			meta::TypeInfo<TerrainComponent>::Register(id, type, true, "D813E5D9-B950-4941-8577-4C9C78B9C9D1");
// 
// 			auto typeID = typeidof(TerrainComponent);
// 			if (typeID != meta::InvalidTypeID && !meta::TypeInfo<TerrainComponent>::Defined)
// 			{
// 				auto& type = db.types[typeID];
// 
// 				//array constructor
// 				type.SetArrayConstructor<TerrainComponent>();
// 
// 				type.AddConstructor<TerrainComponent, false, false>({});
// 
// 				type.AddConstructor<TerrainComponent, true, true>({});
// 
// 				type.LoadBaseClasses(db, typeID, { typeof(Component) });
// 
// 				meta::TypeInfo<TerrainComponent>::Defined = true;
// 
// 				type.AddField<TerrainComponent, std::shared_ptr<AssetData>>("m_heightTexture",
// 					(meta::FieldGetter<TerrainComponent, std::shared_ptr<AssetData>, false>::Signature) & TerrainComponent::m_heightTexture,
// 					(meta::FieldSetter<TerrainComponent, std::shared_ptr<AssetData>, false>::Signature) & TerrainComponent::m_heightTexture, {});
// 
// 				type.AddField<TerrainComponent, std::shared_ptr<AssetData>>("m_terrainTexture1",
// 					(meta::FieldGetter<TerrainComponent, std::shared_ptr<AssetData>, false>::Signature)& TerrainComponent::m_terrainTexture1,
// 					(meta::FieldSetter<TerrainComponent, std::shared_ptr<AssetData>, false>::Signature)& TerrainComponent::m_terrainTexture1, {});
// 
// 				type.AddField<TerrainComponent, std::shared_ptr<AssetData>>("m_terrainTexture2",
// 					(meta::FieldGetter<TerrainComponent, std::shared_ptr<AssetData>, false>::Signature)& TerrainComponent::m_terrainTexture2,
// 					(meta::FieldSetter<TerrainComponent, std::shared_ptr<AssetData>, false>::Signature)& TerrainComponent::m_terrainTexture2, {});
// 
// 				type.AddField<TerrainComponent, std::shared_ptr<AssetData>>("m_terrainTexture3",
// 					(meta::FieldGetter<TerrainComponent, std::shared_ptr<AssetData>, false>::Signature)& TerrainComponent::m_terrainTexture3,
// 					(meta::FieldSetter<TerrainComponent, std::shared_ptr<AssetData>, false>::Signature)& TerrainComponent::m_terrainTexture3, {});
// 
// 				type.AddField<TerrainComponent, std::shared_ptr<AssetData>>("m_terrainTexture4",
// 					(meta::FieldGetter<TerrainComponent, std::shared_ptr<AssetData>, false>::Signature)& TerrainComponent::m_terrainTexture4,
// 					(meta::FieldSetter<TerrainComponent, std::shared_ptr<AssetData>, false>::Signature)& TerrainComponent::m_terrainTexture4, {});
// 
// 				type.AddField<TerrainComponent, std::shared_ptr<AssetData>>("m_blendTexture",
// 					(meta::FieldGetter<TerrainComponent, std::shared_ptr<AssetData>, false>::Signature) & TerrainComponent::m_blendTexture,
// 					(meta::FieldSetter<TerrainComponent, std::shared_ptr<AssetData>, false>::Signature) & TerrainComponent::m_blendTexture, {});
// 
// 				type.AddField<TerrainComponent, uint32_t>("m_terrainRows",
// 					(meta::FieldGetter<TerrainComponent, uint32_t, false>::Signature)& TerrainComponent::m_terrainRows,
// 					(meta::FieldSetter<TerrainComponent, uint32_t, false>::Signature)& TerrainComponent::m_terrainRows, {});
// 
// 				type.AddField<TerrainComponent, uint32_t>("m_terrainCols",
// 					(meta::FieldGetter<TerrainComponent, uint32_t, false>::Signature)& TerrainComponent::m_terrainCols,
// 					(meta::FieldSetter<TerrainComponent, uint32_t, false>::Signature)& TerrainComponent::m_terrainCols, {});
// 
// 				type.AddField<TerrainComponent, uint32_t>("m_rows",
// 					(meta::FieldGetter<TerrainComponent, uint32_t, false>::Signature)& TerrainComponent::m_rows,
// 					(meta::FieldSetter<TerrainComponent, uint32_t, false>::Signature)& TerrainComponent::m_rows, {});
// 
// 				type.AddField<TerrainComponent, uint32_t>("m_cols",
// 					(meta::FieldGetter<TerrainComponent, uint32_t, false>::Signature)& TerrainComponent::m_cols,
// 					(meta::FieldSetter<TerrainComponent, uint32_t, false>::Signature)& TerrainComponent::m_cols, {});
// 
// 				type.AddField<TerrainComponent, uint32_t>("m_tileSize",
// 					(meta::FieldGetter<TerrainComponent, uint32_t, false>::Signature)& TerrainComponent::m_tileSize,
// 					(meta::FieldSetter<TerrainComponent, uint32_t, false>::Signature)& TerrainComponent::m_tileSize, {});
// 
// 				type.AddField<TerrainComponent, std::weak_ptr<GameObject>>("m_owner",
// 					(meta::FieldGetter<TerrainComponent, std::weak_ptr<GameObject>&, true>::Signature)& TerrainComponent::getParentGameObject,
// 					(meta::FieldSetter<TerrainComponent, std::weak_ptr<GameObject>&, true>::Signature)& TerrainComponent::setParentGameObject, {});
// 			}
// 
// 			{
// 				auto id = db.AllocateType("std::shared_ptr<GuGu::TerrainComponent>");
// 				auto& type = db.types[id];
// 				meta::TypeInfo<std::shared_ptr<TerrainComponent>>::Register(id, type, false, "39C665D1-522D-4321-83D4-C1946DC0D84F");
// 			}
// 
// 			{
// 				auto id = db.AllocateType("std::weak_ptr<GuGu::TerrainComponent>");
// 				auto& type = db.types[id];
// 				meta::TypeInfo<std::weak_ptr<TerrainComponent>>::Register(id, type, false, "0C93471F-0542-4DE4-9A38-9D6E7E977C4F");
// 			}
		}

		{
// 			auto id = db.AllocateType("GuGu::WaterComponent");
// 			auto& type = db.types[id];
// 			meta::TypeInfo<WaterComponent>::Register(id, type, true, "4BEAE916-760A-4142-9321-5DC622A4B640");
// 
// 			auto typeID = typeidof(WaterComponent);
// 			if (typeID != meta::InvalidTypeID && !meta::TypeInfo<WaterComponent>::Defined)
// 			{
// 				auto& type = db.types[typeID];
// 
// 				//array constructor
// 				type.SetArrayConstructor<WaterComponent>();
// 
// 				type.AddConstructor<WaterComponent, false, false>({});
// 
// 				type.AddConstructor<WaterComponent, true, true>({});
// 
// 				type.LoadBaseClasses(db, typeID, { typeof(Component) });
// 
// 				meta::TypeInfo<WaterComponent>::Defined = true;
// 
// 				type.AddField<WaterComponent, std::shared_ptr<AssetData>>("m_mixedColorTexture",
// 					(meta::FieldGetter<WaterComponent, std::shared_ptr<AssetData>, false>::Signature) & WaterComponent::m_mixedColorTexture,
// 					(meta::FieldSetter<WaterComponent, std::shared_ptr<AssetData>, false>::Signature) & WaterComponent::m_mixedColorTexture, {});
// 
// 				type.AddField<WaterComponent, std::shared_ptr<AssetData>>("m_colorTexture",
// 					(meta::FieldGetter<WaterComponent, std::shared_ptr<AssetData>, false>::Signature) & WaterComponent::m_colorTexture,
// 					(meta::FieldSetter<WaterComponent, std::shared_ptr<AssetData>, false>::Signature) & WaterComponent::m_colorTexture, {});
// 
// 				type.AddField<WaterComponent, std::shared_ptr<AssetData>>("m_dirTexture",
// 					(meta::FieldGetter<WaterComponent, std::shared_ptr<AssetData>, false>::Signature)& WaterComponent::m_dirTexture,
// 					(meta::FieldSetter<WaterComponent, std::shared_ptr<AssetData>, false>::Signature)& WaterComponent::m_dirTexture, {});
// 
// 				type.AddField<WaterComponent, uint32_t>("m_waterRows",
// 					(meta::FieldGetter<WaterComponent, uint32_t, false>::Signature) & WaterComponent::m_waterRows,
// 					(meta::FieldSetter<WaterComponent, uint32_t, false>::Signature) & WaterComponent::m_waterRows, {});
// 
// 				type.AddField<WaterComponent, uint32_t>("m_waterCols",
// 					(meta::FieldGetter<WaterComponent, uint32_t, false>::Signature) & WaterComponent::m_waterCols,
// 					(meta::FieldSetter<WaterComponent, uint32_t, false>::Signature) & WaterComponent::m_waterCols, {});
// 
// 				type.AddField<WaterComponent, uint32_t>("m_rows",
// 					(meta::FieldGetter<WaterComponent, uint32_t, false>::Signature) & WaterComponent::m_rows,
// 					(meta::FieldSetter<WaterComponent, uint32_t, false>::Signature) & WaterComponent::m_rows, {});
// 
// 				type.AddField<WaterComponent, uint32_t>("m_cols",
// 					(meta::FieldGetter<WaterComponent, uint32_t, false>::Signature) & WaterComponent::m_cols,
// 					(meta::FieldSetter<WaterComponent, uint32_t, false>::Signature) & WaterComponent::m_cols, {});
// 
// 				type.AddField<WaterComponent, uint32_t>("m_tileSize",
// 					(meta::FieldGetter<WaterComponent, uint32_t, false>::Signature) & WaterComponent::m_tileSize,
// 					(meta::FieldSetter<WaterComponent, uint32_t, false>::Signature) & WaterComponent::m_tileSize, {});
// 
// 				type.AddField<WaterComponent, std::weak_ptr<GameObject>>("m_owner",
// 					(meta::FieldGetter<WaterComponent, std::weak_ptr<GameObject>&, true>::Signature) & WaterComponent::getParentGameObject,
// 					(meta::FieldSetter<WaterComponent, std::weak_ptr<GameObject>&, true>::Signature) & WaterComponent::setParentGameObject, {});
// 			}
// 
// 			{
// 				auto id = db.AllocateType("std::shared_ptr<GuGu::WaterComponent>");
// 				auto& type = db.types[id];
// 				meta::TypeInfo<std::shared_ptr<WaterComponent>>::Register(id, type, false, "5942C0D7-9173-443E-98CC-0DBA19D3C9BA");
// 			}
// 
// 			{
// 				auto id = db.AllocateType("std::weak_ptr<GuGu::WaterComponent>");
// 				auto& type = db.types[id];
// 				meta::TypeInfo<std::weak_ptr<WaterComponent>>::Register(id, type, false, "E40553A2-248D-49DD-A284-FC35991219C1");
// 			}
		}

		{
// 			auto id = db.AllocateType("GuGu::CameraComponent");
// 			auto& type = db.types[id];
// 			meta::TypeInfo<CameraComponent>::Register(id, type, true, "DE850877-05B1-487C-A11F-CBABB546D177");
// 
// 			auto typeID = typeidof(CameraComponent);
// 			if (typeID != meta::InvalidTypeID && !meta::TypeInfo<CameraComponent>::Defined)
// 			{
// 				auto& type = db.types[typeID];
// 
// 				//array constructor
// 				type.SetArrayConstructor<CameraComponent>();
// 
// 				type.AddConstructor<CameraComponent, false, false>({});
// 
// 				type.AddConstructor<CameraComponent, true, true>({});
// 
// 				type.LoadBaseClasses(db, typeID, { typeof(Component) });
// 
// 				meta::TypeInfo<CameraComponent>::Defined = true;
// 
// 				type.AddField<CameraComponent, float>("m_fov",
// 					(meta::FieldGetter<CameraComponent, float, true>::Signature)& CameraComponent::getFov,
// 					(meta::FieldSetter<CameraComponent, float, true>::Signature)& CameraComponent::setFov, {});
// 
// 				type.AddField<CameraComponent, float>("m_nearPlane",
// 					(meta::FieldGetter<CameraComponent, float, true>::Signature)& CameraComponent::getNearPlane,
// 					(meta::FieldSetter<CameraComponent, float, true>::Signature)& CameraComponent::setNearPlane, {});
// 
// 				type.AddField<CameraComponent, float>("m_farPlane",
// 					(meta::FieldGetter<CameraComponent, float, true>::Signature) & CameraComponent::getFarPlane,
// 					(meta::FieldSetter<CameraComponent, float, true>::Signature) & CameraComponent::setFarPlane, {});
// 
// 				type.AddField<CameraComponent, std::weak_ptr<GameObject>>("m_owner",
// 					(meta::FieldGetter<CameraComponent, std::weak_ptr<GameObject>&, true>::Signature)& CameraComponent::getParentGameObject,
// 					(meta::FieldSetter<CameraComponent, std::weak_ptr<GameObject>&, true>::Signature)& CameraComponent::setParentGameObject, {});
// 			}
// 
// 			{
// 				auto id = db.AllocateType("std::shared_ptr<CameraComponent>");
// 				auto& type = db.types[id];
// 				meta::TypeInfo<std::shared_ptr<CameraComponent>>::Register(id, type, false, "D866D79C-DC28-4F6B-AF00-E6E592E3C8A3");
// 			}
// 
// 			{
// 				auto id = db.AllocateType("std::weak_ptr<CameraComponent>");
// 				auto& type = db.types[id];
// 				meta::TypeInfo<std::weak_ptr<CameraComponent>>::Register(id, type, false, "1E51EEDA-262E-4608-A995-864D1CB5B6C3");
// 			}
		}

		//资源
// 		{
// 			auto id = db.AllocateType("GuGu::GStaticMesh");
// 			auto& type = db.types[id];
// 			meta::TypeInfo<GStaticMesh>::Register(id, type, true, "C48E6CAE-2E8A-4CD9-A35D-56BF134431C7");
// 
// 			auto typeID = typeidof(GStaticMesh);
// 			if (typeID != meta::InvalidTypeID && !meta::TypeInfo<GStaticMesh>::Defined)
// 			{
// 				auto& type = db.types[typeID];
// 
// 				//array constructor
// 				type.SetArrayConstructor<GStaticMesh>();
// 
// 				type.AddConstructor<GStaticMesh, false, false>({});
// 
// 				type.AddConstructor<GStaticMesh, true, true>({});
// 
// 				type.LoadBaseClasses(db, id, { typeof(meta::Object) });
// 
// 				type.AddField<GStaticMesh, Array<uint32_t>>("m_indexData",
// 					(meta::FieldGetter<GStaticMesh, Array<uint32_t>, false>::Signature)& GStaticMesh::m_indexData,
// 					(meta::FieldSetter<GStaticMesh, Array<uint32_t>, false>::Signature)& GStaticMesh::m_indexData, {});
// 				type.AddField<GStaticMesh, Array<math::float3>>("m_positionData",
// 					(meta::FieldGetter<GStaticMesh, Array<math::float3>, false>::Signature)& GStaticMesh::m_positionData,
// 					(meta::FieldSetter<GStaticMesh, Array<math::float3>, false>::Signature)& GStaticMesh::m_positionData, {});
// 				type.AddField<GStaticMesh, Array<math::float2>>("m_texCoord1Data",
// 					(meta::FieldGetter<GStaticMesh, Array<math::float2>, false>::Signature)& GStaticMesh::m_texCoord1Data,
// 					(meta::FieldSetter<GStaticMesh, Array<math::float2>, false>::Signature)& GStaticMesh::m_texCoord1Data, {});
// 				type.AddField<GStaticMesh, Array<math::float2>>("m_texCoord2Data",
// 					(meta::FieldGetter<GStaticMesh, Array<math::float2>, false>::Signature)& GStaticMesh::m_texCoord2Data,
// 					(meta::FieldSetter<GStaticMesh, Array<math::float2>, false>::Signature)& GStaticMesh::m_texCoord2Data, {});
// 				type.AddField<GStaticMesh, Array<math::float3>>("m_normalData",
// 					(meta::FieldGetter<GStaticMesh, Array<math::float3>, false>::Signature)& GStaticMesh::m_normalData,
// 					(meta::FieldSetter<GStaticMesh, Array<math::float3>, false>::Signature)& GStaticMesh::m_normalData, {});
// 				type.AddField<GStaticMesh, Array<math::float3>>("m_tangentData",
// 					(meta::FieldGetter<GStaticMesh, Array<math::float3>, false>::Signature)& GStaticMesh::m_tangentData,
// 					(meta::FieldSetter<GStaticMesh, Array<math::float3>, false>::Signature)& GStaticMesh::m_tangentData, {});
// 				type.AddField<GStaticMesh, Array<math::float3>>("m_normalData",
// 					(meta::FieldGetter<GStaticMesh, Array<math::float3>, false>::Signature)& GStaticMesh::m_normalData,
// 					(meta::FieldSetter<GStaticMesh, Array<math::float3>, false>::Signature)& GStaticMesh::m_normalData, {});
// 				type.AddField<GStaticMesh, Array<math::vector<uint16_t, 4>>>("m_jointData",
// 					(meta::FieldGetter<GStaticMesh, Array<math::vector<uint16_t, 4>>, false>::Signature)& GStaticMesh::m_jointData,
// 					(meta::FieldSetter<GStaticMesh, Array<math::vector<uint16_t, 4>>, false>::Signature)& GStaticMesh::m_jointData, {});
// 				type.AddField<GStaticMesh, Array<math::float4>>("m_weightData",
// 					(meta::FieldGetter<GStaticMesh, Array<math::float4>, false>::Signature)& GStaticMesh::m_weightData,
// 					(meta::FieldSetter<GStaticMesh, Array<math::float4>, false>::Signature)& GStaticMesh::m_weightData, {});
// 
// 				meta::TypeInfo<GStaticMesh>::Defined = true;
// 			}
// 
// 			{
// 				auto id = db.AllocateType("std::shared_ptr<GuGu::GStaticMesh>");
// 				auto& type = db.types[id];
// 				meta::TypeInfo<std::shared_ptr<GStaticMesh>>::Register(id, type, false, "0D9E6859-B7CD-476C-AF4B-82462328432C");
// 			}
// 
// 			{
// 				auto id = db.AllocateType("std::weak_ptr<GuGu::GStaticMesh>");
// 				auto& type = db.types[id];
// 				meta::TypeInfo<std::weak_ptr<GStaticMesh>>::Register(id, type, false, "37092C3B-F3B7-44FA-9508-B0FE6B6D0920");
// 			}
// 		}

		{
// 			auto id = db.AllocateType("GuGu::GTexture");
// 			auto& type = db.types[id];
// 			meta::TypeInfo<GTexture>::Register(id, type, true, "026CB8ED-6633-4577-B32C-F3F4D9C2FC22");
// 
// 			auto typeID = typeidof(GTexture);
// 			if (typeID != meta::InvalidTypeID && !meta::TypeInfo<GTexture>::Defined)
// 			{
// 				auto& type = db.types[typeID];
// 
// 				//array constructor
// 				type.SetArrayConstructor<GTexture>();
// 
// 				type.AddConstructor<GTexture, false, false>({});
// 
// 				type.AddConstructor<GTexture, true, true>({});
// 
// 				type.LoadBaseClasses(db, id, { typeof(meta::Object) });
// 
// 				type.AddField<GTexture, Array<uint8_t>>("m_data",
// 					(meta::FieldGetter<GTexture, Array<uint8_t>, false>::Signature)& GTexture::m_data,
// 					(meta::FieldSetter<GTexture, Array<uint8_t>, false>::Signature)& GTexture::m_data, {});
// 				type.AddField<GTexture, uint32_t>("m_format",
// 					(meta::FieldGetter<GTexture, uint32_t, false>::Signature)& GTexture::m_format,
// 					(meta::FieldSetter<GTexture, uint32_t, false>::Signature)& GTexture::m_format, {});
// 				type.AddField<GTexture, uint32_t>("m_width",
// 					(meta::FieldGetter<GTexture, uint32_t, false>::Signature)& GTexture::m_width,
// 					(meta::FieldSetter<GTexture, uint32_t, false>::Signature)& GTexture::m_width, {});
// 				type.AddField<GTexture, uint32_t>("m_height",
// 					(meta::FieldGetter<GTexture, uint32_t, false>::Signature)& GTexture::m_height,
// 					(meta::FieldSetter<GTexture, uint32_t, false>::Signature)& GTexture::m_height, {});
// 				type.AddField<GTexture, uint32_t>("m_depth",
// 					(meta::FieldGetter<GTexture, uint32_t, false>::Signature)& GTexture::m_depth,
// 					(meta::FieldSetter<GTexture, uint32_t, false>::Signature)& GTexture::m_depth, {});
// 				type.AddField<GTexture, uint32_t>("m_arraySize",
// 					(meta::FieldGetter<GTexture, uint32_t, false>::Signature)& GTexture::m_arraySize,
// 					(meta::FieldSetter<GTexture, uint32_t, false>::Signature)& GTexture::m_arraySize, {});
// 				type.AddField<GTexture, uint32_t>("m_dimension",
// 					(meta::FieldGetter<GTexture, uint32_t, false>::Signature) & GTexture::m_dimension,
// 					(meta::FieldSetter<GTexture, uint32_t, false>::Signature) & GTexture::m_dimension, {});
// 				type.AddField<GTexture, uint32_t>("m_arraySize",
// 					(meta::FieldGetter<GTexture, uint32_t, false>::Signature)& GTexture::m_mipLevels,
// 					(meta::FieldSetter<GTexture, uint32_t, false>::Signature)& GTexture::m_mipLevels, {});
// 				type.AddField<GTexture, uint32_t>("m_bytesPerPixel",
// 					(meta::FieldGetter<GTexture, uint32_t, false>::Signature)& GTexture::m_bytesPerPixel,
// 					(meta::FieldSetter<GTexture, uint32_t, false>::Signature)& GTexture::m_bytesPerPixel, {});
// 
// 				meta::TypeInfo<GTexture>::Defined = true;
// 			}
// 
// 			{
// 				auto id = db.AllocateType("std::shared_ptr<GuGu::GTexture>");
// 				auto& type = db.types[id];
// 				meta::TypeInfo<std::shared_ptr<GTexture>>::Register(id, type, false, "7D876BB5-7FE5-4AAE-82B8-0BB529272FD0");
// 			}
// 
// 			{
// 				auto id = db.AllocateType("std::weak_ptr<GuGu::GTexture>");
// 				auto& type = db.types[id];
// 				meta::TypeInfo<std::weak_ptr<GTexture>>::Register(id, type, false, "B73F8372-6560-4AE9-806E-B475B8CD4B09");
// 			}
		}

		registerThirdParty();

		ReflectionMain::initialize();

		meta::Type::List types = meta::Type::GetTypes();
		for (const auto& type : types)
		{
			db.registerGuid(type.getGuid(), type.GetID());
		}
	}

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

	static bool registerGuGuGameObject()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::GameObject");
		auto& type = db.types[id];
		meta::TypeInfo<GuGu::GameObject>::Register(id, type, true, "764220D9-31E0-448B-9612-79A47B570367");

		type.meta = {
			std::make_pair(typeof(meta::DisplayName), meta::MetaPropertyInitializer<meta::DisplayName>("GameObject"))
		};

		{
			auto id = db.AllocateType("std::shared_ptr<GuGu::GameObject>");
			auto& type = db.types[id];
			meta::TypeInfo<std::shared_ptr<GameObject>>::Register(id, type, false, "5126D2F9-FCF4-4020-8BBB-187389965FDE");

			type.SetArrayConstructor<std::shared_ptr<GameObject>>();
		}

		{
			auto id = db.AllocateType("std::weak_ptr<GuGu::GameObject>");
			auto& type = db.types[id];
			meta::TypeInfo<std::weak_ptr<GameObject>>::Register(id, type, false, "E3D8C076-BFD3-4FD1-B108-5FBB37169DE3");
		}

		auto typeID = typeidof(GameObject);
		if (typeID != meta::InvalidTypeID && !meta::TypeInfo<GameObject>::Defined)
		{
			auto& type = db.types[typeID];

			//array constructor
			type.SetArrayConstructor<GameObject>();

			type.AddConstructor<GameObject, false, false>({});

			type.AddConstructor<GameObject, true, true>({});

			type.AddField<GameObject, Array<std::shared_ptr<Component>>>("m_components",
				(meta::FieldGetter<GameObject, Array<std::shared_ptr<Component>>, true>::Signature) & GameObject::getComponents,
				(meta::FieldSetter<GameObject, Array<std::shared_ptr<Component>>, true>::Signature) & GameObject::setComponents, {});

			type.AddField<GameObject, Array<std::shared_ptr<GameObject>>>("m_childrens",
				(meta::FieldGetter<GameObject, Array<std::shared_ptr<GameObject>>, true>::Signature) & GameObject::getChildrens,
				(meta::FieldSetter<GameObject, Array<std::shared_ptr<GameObject>>, true>::Signature) & GameObject::setChildrens, {});

			type.AddField<GameObject, std::weak_ptr<GameObject>>("m_parentGameObject",
				(meta::FieldGetter<GameObject, std::weak_ptr<GameObject>&, true>::Signature) & GameObject::getParentGameObject,
				(meta::FieldSetter<GameObject, std::weak_ptr<GameObject>&, true>::Signature) & GameObject::setParentGameObject, {});

			type.LoadBaseClasses(db, typeID, { typeof(meta::Object) });

			meta::TypeInfo<GameObject>::Defined = true;
		}
	}

	static bool registerGuGuLevel()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::Level");
		auto& type = db.types[id];
		meta::TypeInfo<Level>::Register(id, type, true, "64CB643F-2C9C-4857-AA01-4F6F995A4CBE");

		auto typeID = typeidof(Level);
		if (typeID != meta::InvalidTypeID && !meta::TypeInfo<Level>::Defined)
		{
			auto& type = db.types[typeID];

			//array constructor
			type.SetArrayConstructor<Level>();

			type.AddConstructor<Level, false, false>({});

			type.AddConstructor<Level, true, true>({});

			type.LoadBaseClasses(db, typeID, { typeof(meta::Object) });

			type.AddField<Level, Array<std::shared_ptr<GameObject>>>("m_objects",
				(meta::FieldGetter<Level, Array<std::shared_ptr<GameObject>>&, true>::Signature) & Level::getGameObjects,
				(meta::FieldSetter<Level, Array<std::shared_ptr<GameObject>>&, true>::Signature) & Level::setGameObjects, {});

			meta::TypeInfo<Level>::Defined = true;
		}

		{
			auto id = db.AllocateType("std::shared_ptr<GuGu::Level>");
			auto& type = db.types[id];
			meta::TypeInfo<std::shared_ptr<Level>>::Register(id, type, false, "5078441C-57E0-4322-9294-11D02360F9C9");
		}

		{
			auto id = db.AllocateType("std::weak_ptr<GuGu::Level>");
			auto& type = db.types[id];
			meta::TypeInfo<std::weak_ptr<Level>>::Register(id, type, false, "A027395A-6B76-474C-89D3-1D56ACC8212F");
		}
		return true;
	}

	static bool registerGuGuAssetData()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		{
			auto id = db.AllocateType("GuGu::AssetData");
			auto& type = db.types[id];
			meta::TypeInfo<AssetData>::Register(id, type, true, "5A9DF858-72C3-4EC2-856D-3DF59457D874");

			type.LoadBaseClasses(db, id, { typeof(meta::Object) });
		}
		{
			auto id = db.AllocateType("std::shared_ptr<GuGu::AssetData>");
			auto& type = db.types[id];
			meta::TypeInfo<std::shared_ptr<AssetData>>::Register(id, type, false, "21C647F2-BF00-4919-85EF-53D77FB3BC07");
		}
		{
			auto id = db.AllocateType("std::weak_ptr<GuGu::AssetData>");
			auto& type = db.types[id];
			meta::TypeInfo<std::weak_ptr<AssetData>>::Register(id, type, false, "3B42C324-06E4-49D3-AC0F-ADAE86A2DF39");
		}
		return true;
	}

	static bool registerTransformComponent()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		//各种 component
		{
			auto id = db.AllocateType("GuGu::TransformComponent");
			auto& type = db.types[id];
			meta::TypeInfo<TransformComponent>::Register(id, type, true, "57B43954-87AF-495A-8B58-E15890E621D1");

			auto typeID = typeidof(TransformComponent);
			if (typeID != meta::InvalidTypeID && !meta::TypeInfo<TransformComponent>::Defined)
			{
				auto& type = db.types[typeID];

				//array constructor
				type.SetArrayConstructor<TransformComponent>();

				type.AddConstructor<TransformComponent, false, false>({});

				type.AddConstructor<TransformComponent, true, true>({});

				type.AddField<TransformComponent, math::double3>("m_Translation",
					(meta::FieldGetter<TransformComponent, math::double3&, true>::Signature) & TransformComponent::getTranslation,
					(meta::FieldSetter<TransformComponent, math::double3, true>::Signature) & TransformComponent::SetTranslation, {});
				type.AddField<TransformComponent, math::dquat>("m_Rotation",
					(meta::FieldGetter<TransformComponent, math::dquat, true>::Signature) & TransformComponent::getRotationQuat,
					(meta::FieldSetter<TransformComponent, math::dquat, true>::Signature) & TransformComponent::SetRotationQuat, {});
				type.AddField<TransformComponent, math::double3>("m_Scaling",
					(meta::FieldGetter<TransformComponent, math::double3&, true>::Signature) & TransformComponent::getScaling,
					(meta::FieldSetter<TransformComponent, math::double3, true>::Signature) & TransformComponent::SetScaling, {});

				type.AddField<TransformComponent, std::weak_ptr<GameObject>>("m_owner",
					(meta::FieldGetter<TransformComponent, std::weak_ptr<GameObject>&, true>::Signature) & TransformComponent::getParentGameObject,
					(meta::FieldSetter<TransformComponent, std::weak_ptr<GameObject>&, true>::Signature) & TransformComponent::setParentGameObject, {});

				type.LoadBaseClasses(db, typeID, { typeof(Component) });

				meta::TypeInfo<TransformComponent>::Defined = true;

				type.meta = {
					std::make_pair(typeof(meta::DisplayName), meta::MetaPropertyInitializer<meta::DisplayName>("TransformComponent"))
				};
			}

			{
				auto id = db.AllocateType("std::shared_ptr<GuGu::TransformComponent>");
				auto& type = db.types[id];
				meta::TypeInfo<std::shared_ptr<TransformComponent>>::Register(id, type, false, "311DBA9A-2885-4656-882D-81FF8EF66502");
			}

			{
				auto id = db.AllocateType("std::weak_ptr<GuGu::TransformComponent>");
				auto& type = db.types[id];
				meta::TypeInfo<std::weak_ptr<TransformComponent>>::Register(id, type, false, "DC63130F-4532-4F84-9A37-11567967CEFA");
			}
		}
		return true;
	}

	static bool registerStaticMeshComponent()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::StaticMeshComponent");
		auto& type = db.types[id];
		meta::TypeInfo<StaticMeshComponent>::Register(id, type, true, "303B0EAC-B07E-42E1-B6E9-A56F00E75814");

		auto typeID = typeidof(StaticMeshComponent);
		if (typeID != meta::InvalidTypeID && !meta::TypeInfo<StaticMeshComponent>::Defined)
		{
			auto& type = db.types[typeID];

			//array constructor
			type.SetArrayConstructor<StaticMeshComponent>();

			type.AddConstructor<StaticMeshComponent, false, false>({});

			type.AddConstructor<StaticMeshComponent, true, true>({});

			type.LoadBaseClasses(db, typeID, { typeof(Component) });

			type.AddField<StaticMeshComponent, std::shared_ptr<AssetData>>("m_staticMeshAsset",
				(meta::FieldGetter<StaticMeshComponent, std::shared_ptr<AssetData>, true>::Signature) & StaticMeshComponent::getStaticMeshAsset,
				(meta::FieldSetter<StaticMeshComponent, std::shared_ptr<AssetData>, true>::Signature) & StaticMeshComponent::setStaticMeshAsset, {});

			type.AddField<StaticMeshComponent, std::weak_ptr<GameObject>>("m_owner",
				(meta::FieldGetter<StaticMeshComponent, std::weak_ptr<GameObject>&, true>::Signature) & StaticMeshComponent::getParentGameObject,
				(meta::FieldSetter<StaticMeshComponent, std::weak_ptr<GameObject>&, true>::Signature) & StaticMeshComponent::setParentGameObject, {});

			meta::TypeInfo<StaticMeshComponent>::Defined = true;
		}

		{
			auto id = db.AllocateType("std::shared_ptr<GuGu::StaticMeshComponent>");
			auto& type = db.types[id];
			meta::TypeInfo<std::shared_ptr<StaticMeshComponent>>::Register(id, type, false, "81D20567-8580-45DF-9DC9-785225EC85FF");
		}

		{
			auto id = db.AllocateType("std::weak_ptr<GuGu::StaticMeshComponent>");
			auto& type = db.types[id];
			meta::TypeInfo<std::weak_ptr<StaticMeshComponent>>::Register(id, type, false, "E07DC6B1-27A4-4875-B5FF-D49F82FAE211");
		}
		return true;
	}

	static bool registerGuGuLightComponent()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::LightComponent");
		auto& type = db.types[id];
		meta::TypeInfo<LightComponent>::Register(id, type, true, "3EF471CA-C9A2-4F2B-AEA9-326276AE4763");

		auto typeID = typeidof(LightComponent);
		if (typeID != meta::InvalidTypeID && !meta::TypeInfo<LightComponent>::Defined)
		{
			auto& type = db.types[typeID];

			//array constructor
			type.SetArrayConstructor<LightComponent>();

			type.AddConstructor<LightComponent, false, false>({});

			type.AddConstructor<LightComponent, true, true>({});

			type.AddField<LightComponent, Color>("m_lightColor",
				(meta::FieldGetter<LightComponent, Color, false>::Signature) & LightComponent::m_lightColor,
				(meta::FieldSetter<LightComponent, Color, false>::Signature) & LightComponent::m_lightColor, {});

			type.AddField<LightComponent, math::float4>("m_lightPosition",
				(meta::FieldGetter<LightComponent, math::float4, false>::Signature) & LightComponent::m_lightPosition,
				(meta::FieldSetter<LightComponent, math::float4, false>::Signature) & LightComponent::m_lightPosition, {});

			type.AddField<LightComponent, std::weak_ptr<GameObject>>("m_owner",
				(meta::FieldGetter<LightComponent, std::weak_ptr<GameObject>&, true>::Signature) & LightComponent::getParentGameObject,
				(meta::FieldSetter<LightComponent, std::weak_ptr<GameObject>&, true>::Signature) & LightComponent::setParentGameObject, {});

			type.LoadBaseClasses(db, typeID, { typeof(Component) });

			meta::TypeInfo<LightComponent>::Defined = true;
		}

		{
			auto id = db.AllocateType("std::shared_ptr<GuGu::LightComponent>");
			auto& type = db.types[id];
			meta::TypeInfo<std::shared_ptr<LightComponent>>::Register(id, type, false, "1E0975BC-3D79-466A-A383-C7EAAEB21605");
		}

		{
			auto id = db.AllocateType("std::weak_ptr<GuGu::LightComponent>");
			auto& type = db.types[id];
			meta::TypeInfo<std::weak_ptr<LightComponent>>::Register(id, type, false, "77075968-2FB0-40BC-A431-C57C530A6724");
		}
		return true;
	}

	static bool registerGuGuMaterialComponent()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::MaterialComponent");
		auto& type = db.types[id];
		meta::TypeInfo<MaterialComponent>::Register(id, type, true, "5B3EA494-8F58-4A28-A623-CCF822CDF3A1");

		auto typeID = typeidof(MaterialComponent);
		if (typeID != meta::InvalidTypeID && !meta::TypeInfo<MaterialComponent>::Defined)
		{
			auto& type = db.types[typeID];

			//array constructor
			type.SetArrayConstructor<MaterialComponent>();

			type.AddConstructor<MaterialComponent, false, false>({});

			type.AddConstructor<MaterialComponent, true, true>({});

			type.LoadBaseClasses(db, typeID, { typeof(Component) });

			meta::TypeInfo<MaterialComponent>::Defined = true;

			type.AddField<MaterialComponent, Color>("m_albedo",
				(meta::FieldGetter<MaterialComponent, Color, false>::Signature) & MaterialComponent::m_albedo,
				(meta::FieldSetter<MaterialComponent, Color, false>::Signature) & MaterialComponent::m_albedo, {});

			type.AddField<MaterialComponent, float>("m_metallic",
				(meta::FieldGetter<MaterialComponent, float, false>::Signature) & MaterialComponent::m_metallic,
				(meta::FieldSetter<MaterialComponent, float, false>::Signature) & MaterialComponent::m_metallic, {});

			type.AddField<MaterialComponent, float>("m_roughness",
				(meta::FieldGetter<MaterialComponent, float, false>::Signature) & MaterialComponent::m_roughness,
				(meta::FieldSetter<MaterialComponent, float, false>::Signature) & MaterialComponent::m_roughness, {});

			type.AddField<MaterialComponent, float>("m_ao",
				(meta::FieldGetter<MaterialComponent, float, false>::Signature) & MaterialComponent::m_ao,
				(meta::FieldSetter<MaterialComponent, float, false>::Signature) & MaterialComponent::m_ao, {});

			type.AddField<MaterialComponent, std::shared_ptr<AssetData>>("m_albedoTexture",
				(meta::FieldGetter<MaterialComponent, std::shared_ptr<AssetData>, true>::Signature) & MaterialComponent::getAlbeoTextureAsset,
				(meta::FieldSetter<MaterialComponent, std::shared_ptr<AssetData>, true>::Signature) & MaterialComponent::setAlbeoTextureAsset, {});

			type.AddField<MaterialComponent, std::weak_ptr<GameObject>>("m_owner",
				(meta::FieldGetter<MaterialComponent, std::weak_ptr<GameObject>&, true>::Signature) & MaterialComponent::getParentGameObject,
				(meta::FieldSetter<MaterialComponent, std::weak_ptr<GameObject>&, true>::Signature) & MaterialComponent::setParentGameObject, {});
		}

		{
			auto id = db.AllocateType("std::shared_ptr<GuGu::MaterialComponent>");
			auto& type = db.types[id];
			meta::TypeInfo<std::shared_ptr<MaterialComponent>>::Register(id, type, false, "B502E20C-C5B2-4118-8B34-BE7284185CE4");
		}

		{
			auto id = db.AllocateType("std::weak_ptr<GuGu::MaterialComponent>");
			auto& type = db.types[id];
			meta::TypeInfo<std::weak_ptr<MaterialComponent>>::Register(id, type, false, "236A394A-0FD7-4339-95BA-C89B254474B9");
		}
		return true;
	}

	static bool registerGuGuTerrainComponent()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::TerrainComponent");
		auto& type = db.types[id];
		meta::TypeInfo<TerrainComponent>::Register(id, type, true, "D813E5D9-B950-4941-8577-4C9C78B9C9D1");

		auto typeID = typeidof(TerrainComponent);
		if (typeID != meta::InvalidTypeID && !meta::TypeInfo<TerrainComponent>::Defined)
		{
			auto& type = db.types[typeID];

			//array constructor
			type.SetArrayConstructor<TerrainComponent>();

			type.AddConstructor<TerrainComponent, false, false>({});

			type.AddConstructor<TerrainComponent, true, true>({});

			type.LoadBaseClasses(db, typeID, { typeof(Component) });

			meta::TypeInfo<TerrainComponent>::Defined = true;

			type.AddField<TerrainComponent, std::shared_ptr<AssetData>>("m_heightTexture",
				(meta::FieldGetter<TerrainComponent, std::shared_ptr<AssetData>, false>::Signature) & TerrainComponent::m_heightTexture,
				(meta::FieldSetter<TerrainComponent, std::shared_ptr<AssetData>, false>::Signature) & TerrainComponent::m_heightTexture, {});

			type.AddField<TerrainComponent, std::shared_ptr<AssetData>>("m_terrainTexture1",
				(meta::FieldGetter<TerrainComponent, std::shared_ptr<AssetData>, false>::Signature) & TerrainComponent::m_terrainTexture1,
				(meta::FieldSetter<TerrainComponent, std::shared_ptr<AssetData>, false>::Signature) & TerrainComponent::m_terrainTexture1, {});

			type.AddField<TerrainComponent, std::shared_ptr<AssetData>>("m_terrainTexture2",
				(meta::FieldGetter<TerrainComponent, std::shared_ptr<AssetData>, false>::Signature) & TerrainComponent::m_terrainTexture2,
				(meta::FieldSetter<TerrainComponent, std::shared_ptr<AssetData>, false>::Signature) & TerrainComponent::m_terrainTexture2, {});

			type.AddField<TerrainComponent, std::shared_ptr<AssetData>>("m_terrainTexture3",
				(meta::FieldGetter<TerrainComponent, std::shared_ptr<AssetData>, false>::Signature) & TerrainComponent::m_terrainTexture3,
				(meta::FieldSetter<TerrainComponent, std::shared_ptr<AssetData>, false>::Signature) & TerrainComponent::m_terrainTexture3, {});

			type.AddField<TerrainComponent, std::shared_ptr<AssetData>>("m_terrainTexture4",
				(meta::FieldGetter<TerrainComponent, std::shared_ptr<AssetData>, false>::Signature) & TerrainComponent::m_terrainTexture4,
				(meta::FieldSetter<TerrainComponent, std::shared_ptr<AssetData>, false>::Signature) & TerrainComponent::m_terrainTexture4, {});

			type.AddField<TerrainComponent, std::shared_ptr<AssetData>>("m_blendTexture",
				(meta::FieldGetter<TerrainComponent, std::shared_ptr<AssetData>, false>::Signature) & TerrainComponent::m_blendTexture,
				(meta::FieldSetter<TerrainComponent, std::shared_ptr<AssetData>, false>::Signature) & TerrainComponent::m_blendTexture, {});

			type.AddField<TerrainComponent, uint32_t>("m_terrainRows",
				(meta::FieldGetter<TerrainComponent, uint32_t, false>::Signature) & TerrainComponent::m_terrainRows,
				(meta::FieldSetter<TerrainComponent, uint32_t, false>::Signature) & TerrainComponent::m_terrainRows, {});

			type.AddField<TerrainComponent, uint32_t>("m_terrainCols",
				(meta::FieldGetter<TerrainComponent, uint32_t, false>::Signature) & TerrainComponent::m_terrainCols,
				(meta::FieldSetter<TerrainComponent, uint32_t, false>::Signature) & TerrainComponent::m_terrainCols, {});

			type.AddField<TerrainComponent, uint32_t>("m_rows",
				(meta::FieldGetter<TerrainComponent, uint32_t, false>::Signature) & TerrainComponent::m_rows,
				(meta::FieldSetter<TerrainComponent, uint32_t, false>::Signature) & TerrainComponent::m_rows, {});

			type.AddField<TerrainComponent, uint32_t>("m_cols",
				(meta::FieldGetter<TerrainComponent, uint32_t, false>::Signature) & TerrainComponent::m_cols,
				(meta::FieldSetter<TerrainComponent, uint32_t, false>::Signature) & TerrainComponent::m_cols, {});

			type.AddField<TerrainComponent, uint32_t>("m_tileSize",
				(meta::FieldGetter<TerrainComponent, uint32_t, false>::Signature) & TerrainComponent::m_tileSize,
				(meta::FieldSetter<TerrainComponent, uint32_t, false>::Signature) & TerrainComponent::m_tileSize, {});

			type.AddField<TerrainComponent, std::weak_ptr<GameObject>>("m_owner",
				(meta::FieldGetter<TerrainComponent, std::weak_ptr<GameObject>&, true>::Signature) & TerrainComponent::getParentGameObject,
				(meta::FieldSetter<TerrainComponent, std::weak_ptr<GameObject>&, true>::Signature) & TerrainComponent::setParentGameObject, {});
		}

		{
			auto id = db.AllocateType("std::shared_ptr<GuGu::TerrainComponent>");
			auto& type = db.types[id];
			meta::TypeInfo<std::shared_ptr<TerrainComponent>>::Register(id, type, false, "39C665D1-522D-4321-83D4-C1946DC0D84F");
		}

		{
			auto id = db.AllocateType("std::weak_ptr<GuGu::TerrainComponent>");
			auto& type = db.types[id];
			meta::TypeInfo<std::weak_ptr<TerrainComponent>>::Register(id, type, false, "0C93471F-0542-4DE4-9A38-9D6E7E977C4F");
		}
		return true;
	}

	static bool registerGuGuWaterComponent()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::WaterComponent");
		auto& type = db.types[id];
		meta::TypeInfo<WaterComponent>::Register(id, type, true, "4BEAE916-760A-4142-9321-5DC622A4B640");

		auto typeID = typeidof(WaterComponent);
		if (typeID != meta::InvalidTypeID && !meta::TypeInfo<WaterComponent>::Defined)
		{
			auto& type = db.types[typeID];

			//array constructor
			type.SetArrayConstructor<WaterComponent>();

			type.AddConstructor<WaterComponent, false, false>({});

			type.AddConstructor<WaterComponent, true, true>({});

			type.LoadBaseClasses(db, typeID, { typeof(Component) });

			meta::TypeInfo<WaterComponent>::Defined = true;

			type.AddField<WaterComponent, std::shared_ptr<AssetData>>("m_mixedColorTexture",
				(meta::FieldGetter<WaterComponent, std::shared_ptr<AssetData>, false>::Signature) & WaterComponent::m_mixedColorTexture,
				(meta::FieldSetter<WaterComponent, std::shared_ptr<AssetData>, false>::Signature) & WaterComponent::m_mixedColorTexture, {});

			type.AddField<WaterComponent, std::shared_ptr<AssetData>>("m_colorTexture",
				(meta::FieldGetter<WaterComponent, std::shared_ptr<AssetData>, false>::Signature) & WaterComponent::m_colorTexture,
				(meta::FieldSetter<WaterComponent, std::shared_ptr<AssetData>, false>::Signature) & WaterComponent::m_colorTexture, {});

			type.AddField<WaterComponent, std::shared_ptr<AssetData>>("m_dirTexture",
				(meta::FieldGetter<WaterComponent, std::shared_ptr<AssetData>, false>::Signature) & WaterComponent::m_dirTexture,
				(meta::FieldSetter<WaterComponent, std::shared_ptr<AssetData>, false>::Signature) & WaterComponent::m_dirTexture, {});

			type.AddField<WaterComponent, uint32_t>("m_waterRows",
				(meta::FieldGetter<WaterComponent, uint32_t, false>::Signature) & WaterComponent::m_waterRows,
				(meta::FieldSetter<WaterComponent, uint32_t, false>::Signature) & WaterComponent::m_waterRows, {});

			type.AddField<WaterComponent, uint32_t>("m_waterCols",
				(meta::FieldGetter<WaterComponent, uint32_t, false>::Signature) & WaterComponent::m_waterCols,
				(meta::FieldSetter<WaterComponent, uint32_t, false>::Signature) & WaterComponent::m_waterCols, {});

			type.AddField<WaterComponent, uint32_t>("m_rows",
				(meta::FieldGetter<WaterComponent, uint32_t, false>::Signature) & WaterComponent::m_rows,
				(meta::FieldSetter<WaterComponent, uint32_t, false>::Signature) & WaterComponent::m_rows, {});

			type.AddField<WaterComponent, uint32_t>("m_cols",
				(meta::FieldGetter<WaterComponent, uint32_t, false>::Signature) & WaterComponent::m_cols,
				(meta::FieldSetter<WaterComponent, uint32_t, false>::Signature) & WaterComponent::m_cols, {});

			type.AddField<WaterComponent, uint32_t>("m_tileSize",
				(meta::FieldGetter<WaterComponent, uint32_t, false>::Signature) & WaterComponent::m_tileSize,
				(meta::FieldSetter<WaterComponent, uint32_t, false>::Signature) & WaterComponent::m_tileSize, {});

			type.AddField<WaterComponent, std::weak_ptr<GameObject>>("m_owner",
				(meta::FieldGetter<WaterComponent, std::weak_ptr<GameObject>&, true>::Signature) & WaterComponent::getParentGameObject,
				(meta::FieldSetter<WaterComponent, std::weak_ptr<GameObject>&, true>::Signature) & WaterComponent::setParentGameObject, {});
		}

		{
			auto id = db.AllocateType("std::shared_ptr<GuGu::WaterComponent>");
			auto& type = db.types[id];
			meta::TypeInfo<std::shared_ptr<WaterComponent>>::Register(id, type, false, "5942C0D7-9173-443E-98CC-0DBA19D3C9BA");
		}

		{
			auto id = db.AllocateType("std::weak_ptr<GuGu::WaterComponent>");
			auto& type = db.types[id];
			meta::TypeInfo<std::weak_ptr<WaterComponent>>::Register(id, type, false, "E40553A2-248D-49DD-A284-FC35991219C1");
		}
		return true;
	}

	static bool registerGuGuCameraComponent()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::CameraComponent");
		auto& type = db.types[id];
		meta::TypeInfo<CameraComponent>::Register(id, type, true, "DE850877-05B1-487C-A11F-CBABB546D177");

		auto typeID = typeidof(CameraComponent);
		if (typeID != meta::InvalidTypeID && !meta::TypeInfo<CameraComponent>::Defined)
		{
			auto& type = db.types[typeID];

			//array constructor
			type.SetArrayConstructor<CameraComponent>();

			type.AddConstructor<CameraComponent, false, false>({});

			type.AddConstructor<CameraComponent, true, true>({});

			type.LoadBaseClasses(db, typeID, { typeof(Component) });

			meta::TypeInfo<CameraComponent>::Defined = true;

			type.AddField<CameraComponent, float>("m_fov",
				(meta::FieldGetter<CameraComponent, float, true>::Signature) & CameraComponent::getFov,
				(meta::FieldSetter<CameraComponent, float, true>::Signature) & CameraComponent::setFov, {});

			type.AddField<CameraComponent, float>("m_nearPlane",
				(meta::FieldGetter<CameraComponent, float, true>::Signature) & CameraComponent::getNearPlane,
				(meta::FieldSetter<CameraComponent, float, true>::Signature) & CameraComponent::setNearPlane, {});

			type.AddField<CameraComponent, float>("m_farPlane",
				(meta::FieldGetter<CameraComponent, float, true>::Signature) & CameraComponent::getFarPlane,
				(meta::FieldSetter<CameraComponent, float, true>::Signature) & CameraComponent::setFarPlane, {});

			type.AddField<CameraComponent, std::weak_ptr<GameObject>>("m_owner",
				(meta::FieldGetter<CameraComponent, std::weak_ptr<GameObject>&, true>::Signature) & CameraComponent::getParentGameObject,
				(meta::FieldSetter<CameraComponent, std::weak_ptr<GameObject>&, true>::Signature) & CameraComponent::setParentGameObject, {});
		}

		{
			auto id = db.AllocateType("std::shared_ptr<CameraComponent>");
			auto& type = db.types[id];
			meta::TypeInfo<std::shared_ptr<CameraComponent>>::Register(id, type, false, "D866D79C-DC28-4F6B-AF00-E6E592E3C8A3");
		}

		{
			auto id = db.AllocateType("std::weak_ptr<CameraComponent>");
			auto& type = db.types[id];
			meta::TypeInfo<std::weak_ptr<CameraComponent>>::Register(id, type, false, "1E51EEDA-262E-4608-A995-864D1CB5B6C3");
		}
		return true;
	}

	static bool registerGuGuGStaticMesh()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		{
			auto id = db.AllocateType("GuGu::GStaticMesh");
			auto& type = db.types[id];
			meta::TypeInfo<GStaticMesh>::Register(id, type, true, "C48E6CAE-2E8A-4CD9-A35D-56BF134431C7");

			auto typeID = typeidof(GStaticMesh);
			if (typeID != meta::InvalidTypeID && !meta::TypeInfo<GStaticMesh>::Defined)
			{
				auto& type = db.types[typeID];

				//array constructor
				type.SetArrayConstructor<GStaticMesh>();

				type.AddConstructor<GStaticMesh, false, false>({});

				type.AddConstructor<GStaticMesh, true, true>({});

				type.LoadBaseClasses(db, id, { typeof(meta::Object) });

				type.AddField<GStaticMesh, Array<uint32_t>>("m_indexData",
					(meta::FieldGetter<GStaticMesh, Array<uint32_t>, false>::Signature) & GStaticMesh::m_indexData,
					(meta::FieldSetter<GStaticMesh, Array<uint32_t>, false>::Signature) & GStaticMesh::m_indexData, {});
				type.AddField<GStaticMesh, Array<math::float3>>("m_positionData",
					(meta::FieldGetter<GStaticMesh, Array<math::float3>, false>::Signature) & GStaticMesh::m_positionData,
					(meta::FieldSetter<GStaticMesh, Array<math::float3>, false>::Signature) & GStaticMesh::m_positionData, {});
				type.AddField<GStaticMesh, Array<math::float2>>("m_texCoord1Data",
					(meta::FieldGetter<GStaticMesh, Array<math::float2>, false>::Signature) & GStaticMesh::m_texCoord1Data,
					(meta::FieldSetter<GStaticMesh, Array<math::float2>, false>::Signature) & GStaticMesh::m_texCoord1Data, {});
				type.AddField<GStaticMesh, Array<math::float2>>("m_texCoord2Data",
					(meta::FieldGetter<GStaticMesh, Array<math::float2>, false>::Signature) & GStaticMesh::m_texCoord2Data,
					(meta::FieldSetter<GStaticMesh, Array<math::float2>, false>::Signature) & GStaticMesh::m_texCoord2Data, {});
				type.AddField<GStaticMesh, Array<math::float3>>("m_normalData",
					(meta::FieldGetter<GStaticMesh, Array<math::float3>, false>::Signature) & GStaticMesh::m_normalData,
					(meta::FieldSetter<GStaticMesh, Array<math::float3>, false>::Signature) & GStaticMesh::m_normalData, {});
				type.AddField<GStaticMesh, Array<math::float3>>("m_tangentData",
					(meta::FieldGetter<GStaticMesh, Array<math::float3>, false>::Signature) & GStaticMesh::m_tangentData,
					(meta::FieldSetter<GStaticMesh, Array<math::float3>, false>::Signature) & GStaticMesh::m_tangentData, {});
				type.AddField<GStaticMesh, Array<math::float3>>("m_normalData",
					(meta::FieldGetter<GStaticMesh, Array<math::float3>, false>::Signature) & GStaticMesh::m_normalData,
					(meta::FieldSetter<GStaticMesh, Array<math::float3>, false>::Signature) & GStaticMesh::m_normalData, {});
				type.AddField<GStaticMesh, Array<math::vector<uint16_t, 4>>>("m_jointData",
					(meta::FieldGetter<GStaticMesh, Array<math::vector<uint16_t, 4>>, false>::Signature) & GStaticMesh::m_jointData,
					(meta::FieldSetter<GStaticMesh, Array<math::vector<uint16_t, 4>>, false>::Signature) & GStaticMesh::m_jointData, {});
				type.AddField<GStaticMesh, Array<math::float4>>("m_weightData",
					(meta::FieldGetter<GStaticMesh, Array<math::float4>, false>::Signature) & GStaticMesh::m_weightData,
					(meta::FieldSetter<GStaticMesh, Array<math::float4>, false>::Signature) & GStaticMesh::m_weightData, {});

				meta::TypeInfo<GStaticMesh>::Defined = true;
			}

			{
				auto id = db.AllocateType("std::shared_ptr<GuGu::GStaticMesh>");
				auto& type = db.types[id];
				meta::TypeInfo<std::shared_ptr<GStaticMesh>>::Register(id, type, false, "0D9E6859-B7CD-476C-AF4B-82462328432C");
			}

			{
				auto id = db.AllocateType("std::weak_ptr<GuGu::GStaticMesh>");
				auto& type = db.types[id];
				meta::TypeInfo<std::weak_ptr<GStaticMesh>>::Register(id, type, false, "37092C3B-F3B7-44FA-9508-B0FE6B6D0920");
			}
		}
		return true;
	}

	static bool registerGuGuGTexture()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::GTexture");
		auto& type = db.types[id];
		meta::TypeInfo<GTexture>::Register(id, type, true, "026CB8ED-6633-4577-B32C-F3F4D9C2FC22");

		auto typeID = typeidof(GTexture);
		if (typeID != meta::InvalidTypeID && !meta::TypeInfo<GTexture>::Defined)
		{
			auto& type = db.types[typeID];

			//array constructor
			type.SetArrayConstructor<GTexture>();

			type.AddConstructor<GTexture, false, false>({});

			type.AddConstructor<GTexture, true, true>({});

			type.LoadBaseClasses(db, id, { typeof(meta::Object) });

			type.AddField<GTexture, Array<uint8_t>>("m_data",
				(meta::FieldGetter<GTexture, Array<uint8_t>, false>::Signature) & GTexture::m_data,
				(meta::FieldSetter<GTexture, Array<uint8_t>, false>::Signature) & GTexture::m_data, {});
			type.AddField<GTexture, uint32_t>("m_format",
				(meta::FieldGetter<GTexture, uint32_t, false>::Signature) & GTexture::m_format,
				(meta::FieldSetter<GTexture, uint32_t, false>::Signature) & GTexture::m_format, {});
			type.AddField<GTexture, uint32_t>("m_width",
				(meta::FieldGetter<GTexture, uint32_t, false>::Signature) & GTexture::m_width,
				(meta::FieldSetter<GTexture, uint32_t, false>::Signature) & GTexture::m_width, {});
			type.AddField<GTexture, uint32_t>("m_height",
				(meta::FieldGetter<GTexture, uint32_t, false>::Signature) & GTexture::m_height,
				(meta::FieldSetter<GTexture, uint32_t, false>::Signature) & GTexture::m_height, {});
			type.AddField<GTexture, uint32_t>("m_depth",
				(meta::FieldGetter<GTexture, uint32_t, false>::Signature) & GTexture::m_depth,
				(meta::FieldSetter<GTexture, uint32_t, false>::Signature) & GTexture::m_depth, {});
			type.AddField<GTexture, uint32_t>("m_arraySize",
				(meta::FieldGetter<GTexture, uint32_t, false>::Signature) & GTexture::m_arraySize,
				(meta::FieldSetter<GTexture, uint32_t, false>::Signature) & GTexture::m_arraySize, {});
			type.AddField<GTexture, uint32_t>("m_dimension",
				(meta::FieldGetter<GTexture, uint32_t, false>::Signature) & GTexture::m_dimension,
				(meta::FieldSetter<GTexture, uint32_t, false>::Signature) & GTexture::m_dimension, {});
			type.AddField<GTexture, uint32_t>("m_arraySize",
				(meta::FieldGetter<GTexture, uint32_t, false>::Signature) & GTexture::m_mipLevels,
				(meta::FieldSetter<GTexture, uint32_t, false>::Signature) & GTexture::m_mipLevels, {});
			type.AddField<GTexture, uint32_t>("m_bytesPerPixel",
				(meta::FieldGetter<GTexture, uint32_t, false>::Signature) & GTexture::m_bytesPerPixel,
				(meta::FieldSetter<GTexture, uint32_t, false>::Signature) & GTexture::m_bytesPerPixel, {});

			meta::TypeInfo<GTexture>::Defined = true;
		}

		{
			auto id = db.AllocateType("std::shared_ptr<GuGu::GTexture>");
			auto& type = db.types[id];
			meta::TypeInfo<std::shared_ptr<GTexture>>::Register(id, type, false, "7D876BB5-7FE5-4AAE-82B8-0BB529272FD0");
		}

		{
			auto id = db.AllocateType("std::weak_ptr<GuGu::GTexture>");
			auto& type = db.types[id];
			meta::TypeInfo<std::weak_ptr<GTexture>>::Register(id, type, false, "B73F8372-6560-4AE9-806E-B475B8CD4B09");
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

		//Priority guguColorPriority;
		//ReflectionMain::addInitialTypeFunction(registerGuGuColor, &guguColorPriority);

		//Priority guguMetaObjectPriority;
		//ReflectionMain::addInitialTypeFunction(registerGuGuMetaObject, &guguMetaObjectPriority);

		//Priority guguComponentPriority;
		//ReflectionMain::addInitialTypeFunction(registerGuGuComponent, &guguComponentPriority);

		//Priority guguGameObjectPriority;
		//ReflectionMain::addInitialTypeFunction(registerGuGuGameObject, &guguGameObjectPriority);

		//Priority guguLevelPriority;
		//ReflectionMain::addInitialTypeFunction(registerGuGuLevel, &guguLevelPriority);

		//Priority guguAssetDataPriority;
		//ReflectionMain::addInitialTypeFunction(registerGuGuAssetData, &guguAssetDataPriority);

		//Priority guguTransformComponentPriority;
		//ReflectionMain::addInitialTypeFunction(registerTransformComponent, &guguTransformComponentPriority);

		//Priority guguStaticMeshComponentPriority;
		//ReflectionMain::addInitialTypeFunction(registerStaticMeshComponent, &guguStaticMeshComponentPriority);

		//Priority guguLightComponentPriority;
		//ReflectionMain::addInitialTypeFunction(registerGuGuLightComponent, &guguLightComponentPriority);

		//Priority guguMaterialComponentPriority;
		//ReflectionMain::addInitialTypeFunction(registerGuGuMaterialComponent, &guguMaterialComponentPriority);

		//Priority guguTerrainComponentPriority;
		//ReflectionMain::addInitialTypeFunction(registerGuGuTerrainComponent, &guguTerrainComponentPriority);

		//Priority guguWaterComponentPriority;
		//ReflectionMain::addInitialTypeFunction(registerGuGuWaterComponent, &guguWaterComponentPriority);

		//Priority guguCameraComponentPriority;
		//ReflectionMain::addInitialTypeFunction(registerGuGuCameraComponent, &guguCameraComponentPriority);

		//Priority guguGStaticMeshPriority;
		//ReflectionMain::addInitialTypeFunction(registerGuGuGStaticMesh, &guguGStaticMeshPriority);

		//Priority guguGTexturePriority;
		//ReflectionMain::addInitialTypeFunction(registerGuGuGTexture, &guguGTexturePriority);

		//ReflectionMain::initialize();
	}

}