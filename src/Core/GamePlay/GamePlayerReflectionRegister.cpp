#include <pch.h>

#include "GamePlayerReflectionRegister.h"

#include <Core/Reflection/ReflectionDatabase.h>
#include <Core/Reflection/TypeInfo.h>
#include <Core/Reflection/Array.h>

#include <Core/Model/StaticMesh.h>
#include <Core/GamePlay/Level.h>
#include <Core/GamePlay/TransformComponent.h>
#include <Core/GamePlay/StaticMeshComponent.h>
#include <Core/GamePlay/LightComponent.h>
#include <Core/GamePlay/MaterialComponent.h>
#include <Core/GamePlay/CameraComponent.h>

#include <Renderer/Color.h>

#include <Core/AssetManager/AssetData.h>

namespace GuGu {
	void gamePlayerReflectionRegister()
	{
		auto& db = meta::ReflectionDatabase::Instance();

		//数学
		{
			auto id = db.AllocateType("math::float2");
			auto& type = db.types[id];
			meta::TypeInfo<math::float2>::Register(id, type, true);

			type.AddConstructor<math::float2, false, false>();

			type.AddConstructor<math::float2, true, false>();

			type.SetArrayConstructor<math::float2>();

			type.AddField<math::float2, float>("x",
				(meta::FieldGetter<math::float2, float, false>::Signature) & math::float2::x,
				(meta::FieldSetter<math::float2, float, false>::Signature) & math::float2::x);

			type.AddField<math::float2, float>("y",
				(meta::FieldGetter<math::float2, float, false>::Signature) & math::float2::y,
				(meta::FieldSetter<math::float2, float, false>::Signature) & math::float2::y);
		}
		{
			auto id = db.AllocateType("math::float3");
			auto& type = db.types[id];
			meta::TypeInfo<math::float3>::Register(id, type, true);

			type.AddConstructor<math::float3, false, false>();

			type.AddConstructor<math::float3, true, false>();

			type.SetArrayConstructor<math::float3>();

			type.AddField<math::float3, float>("x",
				(meta::FieldGetter<math::float3, float, false>::Signature) & math::float3::x,
				(meta::FieldSetter<math::float3, float, false>::Signature) & math::float3::x);

			type.AddField<math::float3, float>("y",
				(meta::FieldGetter<math::float3, float, false>::Signature) & math::float3::y,
				(meta::FieldSetter<math::float3, float, false>::Signature) & math::float3::y);

			type.AddField<math::float3, float>("z",
				(meta::FieldGetter<math::float3, float, false>::Signature) & math::float3::z,
				(meta::FieldSetter<math::float3, float, false>::Signature) & math::float3::z);
		}
		{
			auto id = db.AllocateType("math::float4");
			auto& type = db.types[id];
			meta::TypeInfo<math::float4>::Register(id, type, true);

			type.AddConstructor<math::float4, false, false>();

			type.AddConstructor<math::float4, true, false>();

			type.SetArrayConstructor<math::float4>();

			type.AddField<math::float4, float>("x",
				(meta::FieldGetter<math::float4, float, false>::Signature) & math::float4::x,
				(meta::FieldSetter<math::float4, float, false>::Signature) & math::float4::x);

			type.AddField<math::float4, float>("y",
				(meta::FieldGetter<math::float4, float, false>::Signature) & math::float4::y,
				(meta::FieldSetter<math::float4, float, false>::Signature) & math::float4::y);

			type.AddField<math::float4, float>("z",
				(meta::FieldGetter<math::float4, float, false>::Signature) & math::float4::z,
				(meta::FieldSetter<math::float4, float, false>::Signature) & math::float4::z);

			type.AddField<math::float4, float>("w",
				(meta::FieldGetter<math::float4, float, false>::Signature) & math::float4::w,
				(meta::FieldSetter<math::float4, float, false>::Signature) & math::float4::w);
		}
		{
			auto id = db.AllocateType("math::double3");
			auto& type = db.types[id];
			meta::TypeInfo<math::double3>::Register(id, type, true);

			type.AddConstructor<math::double3, false, false>();

			type.AddConstructor<math::double3, true, false>();

			type.SetArrayConstructor<math::double3>();

			type.AddField<math::double3, double>("x",
				(meta::FieldGetter<math::double3, double, false>::Signature) & math::double3::x,
				(meta::FieldSetter<math::double3, double, false>::Signature) & math::double3::x);

			type.AddField<math::double3, double>("y",
				(meta::FieldGetter<math::double3, double, false>::Signature) & math::double3::y,
				(meta::FieldSetter<math::double3, double, false>::Signature) & math::double3::y);

			type.AddField<math::double3, double>("z",
				(meta::FieldGetter<math::double3, double, false>::Signature) & math::double3::z,
				(meta::FieldSetter<math::double3, double, false>::Signature) & math::double3::z);
		}

		{
			auto id = db.AllocateType("math::dquat");
			auto& type = db.types[id];
			meta::TypeInfo<math::dquat>::Register(id, type, true);

			type.AddConstructor<math::dquat, false, false>();

			type.AddConstructor<math::dquat, true, false>();

			type.SetArrayConstructor<math::dquat>();

			type.AddField<math::dquat, double>("x",
				(meta::FieldGetter<math::dquat, double, false>::Signature) & math::dquat::x,
				(meta::FieldSetter<math::dquat, double, false>::Signature) & math::dquat::x);

			type.AddField<math::dquat, double>("y",
				(meta::FieldGetter<math::dquat, double, false>::Signature) & math::dquat::y,
				(meta::FieldSetter<math::dquat, double, false>::Signature) & math::dquat::y);

			type.AddField<math::dquat, double>("z",
				(meta::FieldGetter<math::dquat, double, false>::Signature) & math::dquat::z,
				(meta::FieldSetter<math::dquat, double, false>::Signature) & math::dquat::z);

			type.AddField<math::dquat, double>("w",
				(meta::FieldGetter<math::dquat, double, false>::Signature) & math::dquat::w,
				(meta::FieldSetter<math::dquat, double, false>::Signature) & math::dquat::w);
		}

		{
			auto id = db.AllocateType("math::vector<uin16_t, 4>");
			auto& type = db.types[id];
			meta::TypeInfo<math::vector<uint16_t, 4>>::Register(id, type, true);

			type.AddConstructor<math::vector<uint16_t, 4>, false, false>();

			type.AddConstructor<math::vector<uint16_t, 4>, true, false>();

			type.SetArrayConstructor<math::vector<uint16_t, 4>>();

			type.AddField<math::vector<uint16_t, 4>, uint16_t>("x",
				(meta::FieldGetter<math::vector<uint16_t, 4>, uint16_t, false>::Signature)& math::vector<uint16_t, 4>::x,
				(meta::FieldSetter<math::vector<uint16_t, 4>, uint16_t, false>::Signature)& math::vector<uint16_t, 4>::x);

			type.AddField<math::vector<uint16_t, 4>, uint16_t>("y",
				(meta::FieldGetter<math::vector<uint16_t, 4>, uint16_t, false>::Signature)& math::vector<uint16_t, 4>::y,
				(meta::FieldSetter<math::vector<uint16_t, 4>, uint16_t, false>::Signature)& math::vector<uint16_t, 4>::y);

			type.AddField<math::vector<uint16_t, 4>, uint16_t>("z",
				(meta::FieldGetter<math::vector<uint16_t, 4>, uint16_t, false>::Signature)& math::vector<uint16_t, 4>::z,
				(meta::FieldSetter<math::vector<uint16_t, 4>, uint16_t, false>::Signature)& math::vector<uint16_t, 4>::z);

			type.AddField<math::vector<uint16_t, 4>, uint16_t>("w",
				(meta::FieldGetter<math::vector<uint16_t, 4>, uint16_t, false>::Signature)& math::vector<uint16_t, 4>::w,
				(meta::FieldSetter<math::vector<uint16_t, 4>, uint16_t, false>::Signature)& math::vector<uint16_t, 4>::w);
		}

		//color
		{
			auto id = db.AllocateType("Color");
			auto& type = db.types[id];
			meta::TypeInfo<Color>::Register(id, type, true);

			type.AddConstructor<Color, false, false>();

			type.AddConstructor<Color, true, false>();

			type.SetArrayConstructor<Color>();

			type.AddField<Color, float>("r",
				(meta::FieldGetter<Color, float, false>::Signature) & Color::r,
				(meta::FieldSetter<Color, float, false>::Signature) & Color::r);

			type.AddField<Color, float>("g",
				(meta::FieldGetter<Color, float, false>::Signature) & Color::g,
				(meta::FieldSetter<Color, float, false>::Signature) & Color::g);

			type.AddField<Color, float>("b",
				(meta::FieldGetter<Color, float, false>::Signature) & Color::b,
				(meta::FieldSetter<Color, float, false>::Signature) & Color::b);

			type.AddField<Color, float>("a",
				(meta::FieldGetter<Color, float, false>::Signature) & Color::a,
				(meta::FieldSetter<Color, float, false>::Signature) & Color::a);
		}

		//meta::object
		{
			auto id = db.AllocateType("meta::object");
			auto& type = db.types[id];
			meta::TypeInfo<meta::Object>::Register(id, type, true);
		}

		{
			auto id = db.AllocateType("Component");
			auto& type = db.types[id];
			meta::TypeInfo<Component>::Register(id, type, true);

			type.AddField<Component, std::weak_ptr<GameObject>>("m_owner",
				(meta::FieldGetter<Component, std::weak_ptr<GameObject>&, true>::Signature)& Component::getParentGameObject,
				(meta::FieldSetter<Component, std::weak_ptr<GameObject>&, true>::Signature)& Component::setParentGameObject);
		}

		{
			auto id = db.AllocateType("ComponentSharedPtr");
			auto& type = db.types[id];
			meta::TypeInfo<std::shared_ptr<Component>>::Register(id, type, false);

			type.SetArrayConstructor<std::shared_ptr<Component>>();
		}

		{
			auto id = db.AllocateType("ComponentWeakPtr");
			auto& type = db.types[id];
			meta::TypeInfo<std::weak_ptr<Component>>::Register(id, type, false);
		}

		//game object
		{
			auto id = db.AllocateType("GameObject");
			auto& type = db.types[id];
			meta::TypeInfo<GameObject>::Register(id, type, true);

			{
				auto id = db.AllocateType("GameObjectSharedPtr");
				auto& type = db.types[id];
				meta::TypeInfo<std::shared_ptr<GameObject>>::Register(id, type, false);

				type.SetArrayConstructor<std::shared_ptr<GameObject>>();
			}

			{
				auto id = db.AllocateType("GameObjectWeakPtr");
				auto& type = db.types[id];
				meta::TypeInfo<std::weak_ptr<GameObject>>::Register(id, type, false);
			}

			auto typeID = typeidof(GameObject);
			if (typeID != meta::InvalidTypeID && !meta::TypeInfo<GameObject>::Defined)
			{
				auto& type = db.types[typeID];

				//array constructor
				type.SetArrayConstructor<GameObject>();

				type.AddConstructor<GameObject, false, false>();

				type.AddConstructor<GameObject, true, true>();

				type.AddField<GameObject, Array<std::shared_ptr<Component>>>("m_components",
					(meta::FieldGetter<GameObject, Array<std::shared_ptr<Component>>, true>::Signature) & GameObject::getComponents,
					(meta::FieldSetter<GameObject, Array<std::shared_ptr<Component>>, true>::Signature) & GameObject::setComponents);

				type.AddField<GameObject, Array<std::shared_ptr<GameObject>>>("m_childrens",
					(meta::FieldGetter<GameObject, Array<std::shared_ptr<GameObject>>, true>::Signature) & GameObject::getChildrens,
					(meta::FieldSetter<GameObject, Array<std::shared_ptr<GameObject>>, true>::Signature) & GameObject::setChildrens);

				type.AddField<GameObject, std::weak_ptr<GameObject>>("m_parentGameObject",
					(meta::FieldGetter<GameObject, std::weak_ptr<GameObject>&, true>::Signature)& GameObject::getParentGameObject,
					(meta::FieldSetter<GameObject, std::weak_ptr<GameObject>&, true>::Signature)& GameObject::setParentGameObject);

				type.LoadBaseClasses(db, typeID, { typeof(meta::Object) });

				meta::TypeInfo<GameObject>::Defined = true;
			}
		}

		//level
		{
			auto id = db.AllocateType("Level");
			auto& type = db.types[id];
			meta::TypeInfo<Level>::Register(id, type, true);

			auto typeID = typeidof(Level);
			if (typeID != meta::InvalidTypeID && !meta::TypeInfo<Level>::Defined)
			{
				auto& type = db.types[typeID];

				//array constructor
				type.SetArrayConstructor<Level>();

				type.AddConstructor<Level, false, false>();

				type.AddConstructor<Level, true, true>();

				type.LoadBaseClasses(db, typeID, {typeof(meta::Object)});

				type.AddField<Level, Array<std::shared_ptr<GameObject>>>("m_objects", 
					(meta::FieldGetter<Level, Array<std::shared_ptr<GameObject>>&, true>::Signature)&Level::getGameObjects,
					(meta::FieldSetter<Level, Array<std::shared_ptr<GameObject>>&, true>::Signature)&Level::setGameObjects);

				meta::TypeInfo<Level>::Defined = true;
			}
		}

		{
			auto id = db.AllocateType("LevelSharedPtr");
			auto& type = db.types[id];
			meta::TypeInfo<std::shared_ptr<Level>>::Register(id, type, false);
		}

		{
			auto id = db.AllocateType("LevelWeakPtr");
			auto& type = db.types[id];
			meta::TypeInfo<std::weak_ptr<Level>>::Register(id, type, false);
		}

		//资源
		{
			auto id = db.AllocateType("AssetData");
			auto& type = db.types[id];
			meta::TypeInfo<AssetData>::Register(id, type, true);

			type.LoadBaseClasses(db, id, { typeof(meta::Object) });
		}
		{
			auto id = db.AllocateType("AssetDataSharedPtr");
			auto& type = db.types[id];
			meta::TypeInfo<std::shared_ptr<AssetData>>::Register(id, type, false);
		}
		{
			auto id = db.AllocateType("AssetDataWeakPtr");
			auto& type = db.types[id];
			meta::TypeInfo<std::weak_ptr<AssetData>>::Register(id, type, false);
		}

		//各种 component
		{
			auto id = db.AllocateType("TransformComponent");
			auto& type = db.types[id];
			meta::TypeInfo<TransformComponent>::Register(id, type, true);

			auto typeID = typeidof(TransformComponent);
			if (typeID != meta::InvalidTypeID && !meta::TypeInfo<TransformComponent>::Defined)
			{
				auto& type = db.types[typeID];

				//array constructor
				type.SetArrayConstructor<TransformComponent>();

				type.AddConstructor<TransformComponent, false, false>();

				type.AddConstructor<TransformComponent, true, true>();

				type.AddField<TransformComponent, math::double3>("m_Translation",
					(meta::FieldGetter<TransformComponent, math::double3&, true>::Signature)&TransformComponent::getTranslation,
					(meta::FieldSetter<TransformComponent, math::double3, true>::Signature)&TransformComponent::SetTranslation);
				type.AddField<TransformComponent, math::dquat>("m_Rotation",
					(meta::FieldGetter<TransformComponent, math::dquat, true>::Signature)&TransformComponent::getRotation,
					(meta::FieldSetter<TransformComponent, math::dquat, true>::Signature)&TransformComponent::SetRotation);
				type.AddField<TransformComponent, math::double3>("m_Scaling",
					(meta::FieldGetter<TransformComponent, math::double3&, true>::Signature)&TransformComponent::getScaling,
					(meta::FieldSetter<TransformComponent, math::double3, true>::Signature)&TransformComponent::SetScaling);

				type.AddField<TransformComponent, std::weak_ptr<GameObject>>("m_owner",
					(meta::FieldGetter<TransformComponent, std::weak_ptr<GameObject>&, true>::Signature)& TransformComponent::getParentGameObject,
					(meta::FieldSetter<TransformComponent, std::weak_ptr<GameObject>&, true>::Signature)& TransformComponent::setParentGameObject);

				type.LoadBaseClasses(db, typeID, { typeof(Component) });

				meta::TypeInfo<TransformComponent>::Defined = true;
			}

			{
				auto id = db.AllocateType("TransformComponentSharedPtr");
				auto& type = db.types[id];
				meta::TypeInfo<std::shared_ptr<TransformComponent>>::Register(id, type, false);
			}

			{
				auto id = db.AllocateType("TransformComponentWeakPtr");
				auto& type = db.types[id];
				meta::TypeInfo<std::weak_ptr<TransformComponent>>::Register(id, type, false);
			}
		}

		{
			auto id = db.AllocateType("StaticMeshComponent");
			auto& type = db.types[id];
			meta::TypeInfo<StaticMeshComponent>::Register(id, type, true);

			auto typeID = typeidof(StaticMeshComponent);
			if (typeID != meta::InvalidTypeID && !meta::TypeInfo<StaticMeshComponent>::Defined)
			{
				auto& type = db.types[typeID];

				//array constructor
				type.SetArrayConstructor<StaticMeshComponent>();

				type.AddConstructor<StaticMeshComponent, false, false>();

				type.AddConstructor<StaticMeshComponent, true, true>();

				type.LoadBaseClasses(db, typeID, { typeof(Component) });
				
				type.AddField<StaticMeshComponent, std::shared_ptr<AssetData>>("m_staticMeshAsset",
					(meta::FieldGetter<StaticMeshComponent, std::shared_ptr<AssetData>, true>::Signature)& StaticMeshComponent::getStaticMeshAsset,
					(meta::FieldSetter<StaticMeshComponent, std::shared_ptr<AssetData>, true>::Signature)& StaticMeshComponent::setStaticMeshAsset);

				type.AddField<StaticMeshComponent, std::weak_ptr<GameObject>>("m_owner",
					(meta::FieldGetter<StaticMeshComponent, std::weak_ptr<GameObject>&, true>::Signature)& StaticMeshComponent::getParentGameObject,
					(meta::FieldSetter<StaticMeshComponent, std::weak_ptr<GameObject>&, true>::Signature)& StaticMeshComponent::setParentGameObject);

				meta::TypeInfo<StaticMeshComponent>::Defined = true;
			}

			{
				auto id = db.AllocateType("StaticMeshComponentSharedPtr");
				auto& type = db.types[id];
				meta::TypeInfo<std::shared_ptr<StaticMeshComponent>>::Register(id, type, false);
			}

			{
				auto id = db.AllocateType("StaticMeshComponentWeakPtr");
				auto& type = db.types[id];
				meta::TypeInfo<std::weak_ptr<StaticMeshComponent>>::Register(id, type, false);
			}
		}

		{
			auto id = db.AllocateType("LightComponent");
			auto& type = db.types[id];
			meta::TypeInfo<LightComponent>::Register(id, type, true);

			auto typeID = typeidof(LightComponent);
			if (typeID != meta::InvalidTypeID && !meta::TypeInfo<LightComponent>::Defined)
			{
				auto& type = db.types[typeID];

				//array constructor
				type.SetArrayConstructor<LightComponent>();

				type.AddConstructor<LightComponent, false, false>();

				type.AddConstructor<LightComponent, true, true>();

				type.AddField<LightComponent, math::float4>("m_lightColor",
					(meta::FieldGetter<LightComponent, math::float4, false>::Signature) & LightComponent::m_lightColor,
					(meta::FieldSetter<LightComponent, math::float4, false>::Signature) & LightComponent::m_lightColor);

				type.AddField<LightComponent, math::float4>("m_lightPosition",
					(meta::FieldGetter<LightComponent, math::float4, false>::Signature)& LightComponent::m_lightPosition,
					(meta::FieldSetter<LightComponent, math::float4, false>::Signature)& LightComponent::m_lightPosition);

				type.AddField<LightComponent, std::weak_ptr<GameObject>>("m_owner",
					(meta::FieldGetter<LightComponent, std::weak_ptr<GameObject>&, true>::Signature)& LightComponent::getParentGameObject,
					(meta::FieldSetter<LightComponent, std::weak_ptr<GameObject>&, true>::Signature)& LightComponent::setParentGameObject);

				type.LoadBaseClasses(db, typeID, { typeof(Component) });

				meta::TypeInfo<LightComponent>::Defined = true;
			}

			{
				auto id = db.AllocateType("LightComponentSharedPtr");
				auto& type = db.types[id];
				meta::TypeInfo<std::shared_ptr<LightComponent>>::Register(id, type, false);
			}

			{
				auto id = db.AllocateType("LightComponentWeakPtr");
				auto& type = db.types[id];
				meta::TypeInfo<std::weak_ptr<LightComponent>>::Register(id, type, false);
			}

		}

		{
			auto id = db.AllocateType("MaterialComponent");
			auto& type = db.types[id];
			meta::TypeInfo<MaterialComponent>::Register(id, type, true);

			auto typeID = typeidof(MaterialComponent);
			if (typeID != meta::InvalidTypeID && !meta::TypeInfo<MaterialComponent>::Defined)
			{
				auto& type = db.types[typeID];

				//array constructor
				type.SetArrayConstructor<MaterialComponent>();

				type.AddConstructor<MaterialComponent, false, false>();

				type.AddConstructor<MaterialComponent, true, true>();

				type.LoadBaseClasses(db, typeID, { typeof(Component) });

				meta::TypeInfo<MaterialComponent>::Defined = true;

				type.AddField<MaterialComponent, Color>("m_albedo",
					(meta::FieldGetter<MaterialComponent, Color, false>::Signature)& MaterialComponent::m_albedo,
					(meta::FieldSetter<MaterialComponent, Color, false>::Signature)& MaterialComponent::m_albedo);

				type.AddField<MaterialComponent, float>("m_metallic",
					(meta::FieldGetter<MaterialComponent, float, false>::Signature)& MaterialComponent::m_metallic,
					(meta::FieldSetter<MaterialComponent, float, false>::Signature)& MaterialComponent::m_metallic);

				type.AddField<MaterialComponent, float>("m_roughness",
					(meta::FieldGetter<MaterialComponent, float, false>::Signature)& MaterialComponent::m_roughness,
					(meta::FieldSetter<MaterialComponent, float, false>::Signature)& MaterialComponent::m_roughness);

				type.AddField<MaterialComponent, float>("m_ao",
					(meta::FieldGetter<MaterialComponent, float, false>::Signature)& MaterialComponent::m_ao,
					(meta::FieldSetter<MaterialComponent, float, false>::Signature)& MaterialComponent::m_ao);

				type.AddField<MaterialComponent, std::weak_ptr<GameObject>>("m_owner",
					(meta::FieldGetter<MaterialComponent, std::weak_ptr<GameObject>&, true>::Signature)& MaterialComponent::getParentGameObject,
					(meta::FieldSetter<MaterialComponent, std::weak_ptr<GameObject>&, true>::Signature)& MaterialComponent::setParentGameObject);
			}

			{
				auto id = db.AllocateType("MaterialComponentSharedPtr");
				auto& type = db.types[id];
				meta::TypeInfo<std::shared_ptr<MaterialComponent>>::Register(id, type, false);
			}

			{
				auto id = db.AllocateType("MaterialComponentWeakPtr");
				auto& type = db.types[id];
				meta::TypeInfo<std::weak_ptr<MaterialComponent>>::Register(id, type, false);
			}
		}

		{
			auto id = db.AllocateType("CameraComponent");
			auto& type = db.types[id];
			meta::TypeInfo<CameraComponent>::Register(id, type, true);

			auto typeID = typeidof(CameraComponent);
			if (typeID != meta::InvalidTypeID && !meta::TypeInfo<CameraComponent>::Defined)
			{
				auto& type = db.types[typeID];

				//array constructor
				type.SetArrayConstructor<CameraComponent>();

				type.AddConstructor<CameraComponent, false, false>();

				type.AddConstructor<CameraComponent, true, true>();

				type.LoadBaseClasses(db, typeID, { typeof(Component) });

				meta::TypeInfo<CameraComponent>::Defined = true;

				type.AddField<CameraComponent, float>("m_fov",
					(meta::FieldGetter<CameraComponent, float, true>::Signature)& CameraComponent::getFov,
					(meta::FieldSetter<CameraComponent, float, true>::Signature)& CameraComponent::setFov);

				type.AddField<CameraComponent, float>("m_nearPlane",
					(meta::FieldGetter<CameraComponent, float, true>::Signature)& CameraComponent::getNearPlane,
					(meta::FieldSetter<CameraComponent, float, true>::Signature)& CameraComponent::setNearPlane);

				type.AddField<CameraComponent, float>("m_farPlane",
					(meta::FieldGetter<CameraComponent, float, true>::Signature) & CameraComponent::getFarPlane,
					(meta::FieldSetter<CameraComponent, float, true>::Signature) & CameraComponent::setFarPlane);

				type.AddField<CameraComponent, std::weak_ptr<GameObject>>("m_owner",
					(meta::FieldGetter<CameraComponent, std::weak_ptr<GameObject>&, true>::Signature)& CameraComponent::getParentGameObject,
					(meta::FieldSetter<CameraComponent, std::weak_ptr<GameObject>&, true>::Signature)& CameraComponent::setParentGameObject);
			}

			{
				auto id = db.AllocateType("CameraComponentSharedPtr");
				auto& type = db.types[id];
				meta::TypeInfo<std::shared_ptr<CameraComponent>>::Register(id, type, false);
			}

			{
				auto id = db.AllocateType("CameraComponentWeakPtr");
				auto& type = db.types[id];
				meta::TypeInfo<std::weak_ptr<CameraComponent>>::Register(id, type, false);
			}
		}

		//资源
		{
			auto id = db.AllocateType("GStaticMesh");
			auto& type = db.types[id];
			meta::TypeInfo<GStaticMesh>::Register(id, type, true);

			auto typeID = typeidof(GStaticMesh);
			if (typeID != meta::InvalidTypeID && !meta::TypeInfo<GStaticMesh>::Defined)
			{
				auto& type = db.types[typeID];

				//array constructor
				type.SetArrayConstructor<GStaticMesh>();

				type.AddConstructor<GStaticMesh, false, false>();

				type.AddConstructor<GStaticMesh, true, true>();

				type.LoadBaseClasses(db, id, { typeof(meta::Object) });

				type.AddField<GStaticMesh, Array<uint32_t>>("m_indexData",
					(meta::FieldGetter<GStaticMesh, Array<uint32_t>, false>::Signature)& GStaticMesh::m_indexData,
					(meta::FieldSetter<GStaticMesh, Array<uint32_t>, false>::Signature)& GStaticMesh::m_indexData);
				type.AddField<GStaticMesh, Array<math::float3>>("m_positionData",
					(meta::FieldGetter<GStaticMesh, Array<math::float3>, false>::Signature)& GStaticMesh::m_positionData,
					(meta::FieldSetter<GStaticMesh, Array<math::float3>, false>::Signature)& GStaticMesh::m_positionData);
				type.AddField<GStaticMesh, Array<math::float2>>("m_texCoord1Data",
					(meta::FieldGetter<GStaticMesh, Array<math::float2>, false>::Signature)& GStaticMesh::m_texCoord1Data,
					(meta::FieldSetter<GStaticMesh, Array<math::float2>, false>::Signature)& GStaticMesh::m_texCoord1Data);
				type.AddField<GStaticMesh, Array<math::float2>>("m_texCoord2Data",
					(meta::FieldGetter<GStaticMesh, Array<math::float2>, false>::Signature)& GStaticMesh::m_texCoord2Data,
					(meta::FieldSetter<GStaticMesh, Array<math::float2>, false>::Signature)& GStaticMesh::m_texCoord2Data);
				type.AddField<GStaticMesh, Array<math::float3>>("m_normalData",
					(meta::FieldGetter<GStaticMesh, Array<math::float3>, false>::Signature)& GStaticMesh::m_normalData,
					(meta::FieldSetter<GStaticMesh, Array<math::float3>, false>::Signature)& GStaticMesh::m_normalData);
				type.AddField<GStaticMesh, Array<math::float3>>("m_tangentData",
					(meta::FieldGetter<GStaticMesh, Array<math::float3>, false>::Signature)& GStaticMesh::m_tangentData,
					(meta::FieldSetter<GStaticMesh, Array<math::float3>, false>::Signature)& GStaticMesh::m_tangentData);
				type.AddField<GStaticMesh, Array<math::float3>>("m_normalData",
					(meta::FieldGetter<GStaticMesh, Array<math::float3>, false>::Signature)& GStaticMesh::m_normalData,
					(meta::FieldSetter<GStaticMesh, Array<math::float3>, false>::Signature)& GStaticMesh::m_normalData);
				type.AddField<GStaticMesh, Array<math::vector<uint16_t, 4>>>("m_jointData",
					(meta::FieldGetter<GStaticMesh, Array<math::vector<uint16_t, 4>>, false>::Signature)& GStaticMesh::m_jointData,
					(meta::FieldSetter<GStaticMesh, Array<math::vector<uint16_t, 4>>, false>::Signature)& GStaticMesh::m_jointData);
				type.AddField<GStaticMesh, Array<math::float4>>("m_weightData",
					(meta::FieldGetter<GStaticMesh, Array<math::float4>, false>::Signature)& GStaticMesh::m_weightData,
					(meta::FieldSetter<GStaticMesh, Array<math::float4>, false>::Signature)& GStaticMesh::m_weightData);

				meta::TypeInfo<GStaticMesh>::Defined = true;
			}

			{
				auto id = db.AllocateType("GStaticMeshPtr");
				auto& type = db.types[id];
				meta::TypeInfo<std::shared_ptr<GStaticMesh>>::Register(id, type, false);
			}

			{
				auto id = db.AllocateType("GStaticMeshWeakPtr");
				auto& type = db.types[id];
				meta::TypeInfo<std::weak_ptr<GStaticMesh>>::Register(id, type, false);
			}
		}
	}
}