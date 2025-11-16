#include <pch.h>

#include "Animator.h"
#include <Core/Reflection/TypeInfo.h>
#include <Core/GamePlay/GamePlayerReflectionRegister.h>
#include "GAnimation.h"
#include <Core/Model/ModelBasic.h>
#include <Core/Model/StaticMesh.h>
#include <Core/AssetManager/AssetData.h>

namespace GuGu {
	static bool registerGuGuGAnimator()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		{
			auto id = db.AllocateType("GuGu::GAnimator");
			auto& type = db.types[id];
			meta::TypeInfo<GAnimator>::Register(id, type, true, "42DEF0A6-A497-4243-8D7F-586F000EFB4B");

			auto typeID = typeidof(GAnimator);
			if (typeID != meta::InvalidTypeID && !meta::TypeInfo<GAnimator>::Defined)
			{
				auto& type = db.types[typeID];

				//array constructor
				type.SetArrayConstructor<GAnimator>();

				type.AddConstructor<GAnimator, false, false>({});

				type.AddConstructor<GAnimator, true, true>({});

				type.LoadBaseClasses(db, id, { typeof(meta::Object) });

				meta::TypeInfo<GAnimator>::Defined = true;
			}

			{
				auto id = db.AllocateType("std::shared_ptr<GuGu::GAnimator>");
				auto& type = db.types[id];
				meta::TypeInfo<std::shared_ptr<GAnimator>>::Register(id, type, false, "051759DC-2B3A-4003-BB67-CA6775E23F53");
			}

			{
				auto id = db.AllocateType("std::weak_ptr<GuGu::GAnimator>");
				auto& type = db.types[id];
				meta::TypeInfo<std::weak_ptr<GAnimator>>::Register(id, type, false, "F05E1B84-72B4-4027-A07F-84BC69F6A96A");
			}
		}
		return true;
	}
	static bool registerGuGuGAnimatorFields()
	{
		auto& db = meta::ReflectionDatabase::Instance();

		auto& type = db.types[typeof(GAnimator).GetID()];

		type.AddMethod("playAnimation", &GAnimator::playAnimation, {});

		type.AddMethod("playAnimationWithAsset", &GAnimator::playAnimationWithAsset, {});

		type.AddMethod("Update", &GAnimator::Update, {});

		type.AddMethod("stopAnimation", &GAnimator::stopAnimation, {});

		type.AddMethod("isRunning", &GAnimator::isRunning, {});

		return true;
	}
	IMPLEMENT_INITIAL_BEGIN(GAnimator)
		ADD_PRIORITY(meta::Object)
		ADD_INITIAL_FUNCTION_WITH_PRIORITY(registerGuGuGAnimator)
	IMPLEMENT_INITIAL_END

	IMPLEMENT_INITIAL_FIELDS_BEGIN(GAnimator)
		ADD_INITIAL_FIELDS_FUNCTION_WITH_PRIORITY(registerGuGuGAnimatorFields)
	IMPLEMENT_INITIAL_FIELDS_END
	GAnimator::GAnimator()
	{
	}
	GAnimator::~GAnimator()
	{
	}

	void GAnimator::PostLoad()
	{

	}

	void GAnimator::Update(float fElapsedTimeSeconds)
	{
		if (m_isRunning)
		{
			if (m_anim.lock())
			{
				//每秒的帧数 * 时间秒
				m_currentTime += m_anim.lock()->m_ticksPerSecond * fElapsedTimeSeconds;
				//计算得到帧数
				m_currentTime = std::fmod(m_currentTime, m_anim.lock()->m_duration);

				if (m_skeletonMesh.lock())
				{
					std::shared_ptr<GStaticMesh> skeletonMesh = m_skeletonMesh.lock();
					if (skeletonMesh)
					{
						for (int32_t i = 0; i < skeletonMesh->m_geometries.size(); ++i)
						{
							if(skeletonMesh->m_geometries[i].m_parentId == -1)
								calculateBoneTransform(skeletonMesh->m_geometries[i], math::float4x4::identity());
						}		
					}	
					skeletonMesh->m_finalBoneMatrices = m_finalBoneMatrices;
				}	
			}
		}
	}

	meta::Type GAnimator::GetType() const
	{
		return typeof(GAnimator);
	}

	meta::Object* GAnimator::Clone(void) const
	{
		GAnimator* animator = new GAnimator();
		//animator do not clone
		return animator;
	}

	void GAnimator::OnSerialize(nlohmann::json& output) const
	{

	}

	void GAnimator::OnDeserialize(const nlohmann::json& input)
	{

	}

	void GAnimator::playAnimation(std::shared_ptr<GAnimation> anim, std::shared_ptr<GStaticMesh> inStaticMesh)
	{
		m_currentTime = 0.0f;
		m_anim = anim;
		m_skeletonMesh = inStaticMesh;//todo:change name

		m_finalBoneMatrices.resize(256);//256 max bones
		for (int32_t i = 0; i < 256; ++i)
		{
			m_finalBoneMatrices[i] = math::float4x4::identity();
		}

		m_isRunning = true;
	}

	void GAnimator::playAnimationWithAsset(std::shared_ptr<AssetData> inAnim, std::shared_ptr<AssetData> inStaticMesh)
	{
		m_currentTime = 0.0f;
		m_anim = std::static_pointer_cast<GAnimation>(inAnim->m_loadedResource);
		m_skeletonMesh = std::static_pointer_cast<GStaticMesh>(inStaticMesh->m_loadedResource);

		m_finalBoneMatrices.resize(256);//256 max bones
		for (int32_t i = 0; i < 256; ++i)
		{
			m_finalBoneMatrices[i] = math::float4x4::identity();
		}

		m_isRunning = true;
	}

	void GAnimator::stopAnimation()
	{
		m_isRunning = false;
	}

	void GAnimator::calculateBoneTransform(const GMeshGeometry& inCurrentNode, math::float4x4 parentTransform)
	{
		GuGuUtf8Str nodeName = inCurrentNode.m_nodeName;

		math::float4x4 globalTransform = parentTransform;

		//find bone info
		std::shared_ptr<GStaticMesh> gSkeletonMesh = m_skeletonMesh.lock();
		if (gSkeletonMesh)
		{
			if (m_anim.lock())
			{
				//获取当前帧当前骨骼的变换矩阵
				globalTransform = m_anim.lock()->getBoneLocalTransform(nodeName, m_currentTime) * globalTransform;

				auto it = gSkeletonMesh->m_boneInfos.find(nodeName);
				if (it != gSkeletonMesh->m_boneInfos.end())
				{
					BoneInfo boneInfo = it->second;
					//bone info
					math::float4x4 offsetMatrix = boneInfo.m_offsetMatrix;
					int32_t boneId = boneInfo.m_boneId;

					//if (boneId == 41 || boneId == 42)
					//{
					//	m_anim.lock()->getBoneLocalTransform(nodeName, m_currentTime);
					//}

					//m_finalBoneMatrices[boneId] = offsetMatrix * globalTransform;
					m_finalBoneMatrices[boneId] = offsetMatrix * globalTransform;


					//if (std::isnan(m_finalBoneMatrices[boneId].m00))
					//{
					//	auto m1 = m_anim.lock()->getBoneLocalTransform(nodeName, m_currentTime);
					//	auto m2 = m1 * globalTransform;
					//	auto m3 = offsetMatrix * m2;
					//
					//	int32_t a = 4 + 5;
					//}
				}
			}	
		}
		
		for (int32_t i = 0; i < inCurrentNode.m_childrens.size(); ++i)
		{
			calculateBoneTransform(gSkeletonMesh->m_geometries[inCurrentNode.m_childrens[i]], globalTransform);
		}
	}

	bool GAnimator::isRunning() const
	{
		return m_isRunning;
	}

}