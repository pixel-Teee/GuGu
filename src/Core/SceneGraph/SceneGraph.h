#pragma once

#include <memory>
#include <unordered_map>

#include <Core/Math/MyMath.h>

#include "SceneTypes.h"

#include <Core/Animation/KeyframeAnimation.h>

namespace GuGu {
    class SceneGraphNode;
    class SceneGraph;
	class SceneGraphLeaf
	{
	public:
		SceneGraphLeaf() = default;
		virtual ~SceneGraphLeaf() = default;

		const GuGuUtf8Str& GetName() const;
		SceneGraphNode* GetNode() const { return m_node.lock().get(); }
		virtual dm::box3 getLocalBoundingBox() { return dm::box3::empty(); }
		virtual std::shared_ptr<SceneGraphLeaf> Clone() = 0;
		
		friend class SceneGraphNode;
	private:
		std::weak_ptr<SceneGraphNode> m_node;
	};

	class MeshInstance : public SceneGraphLeaf
	{
	public:
		explicit MeshInstance(std::shared_ptr<MeshInfo> mesh)
			: m_mesh(std::move(mesh))
		{}

		const std::shared_ptr<MeshInfo>& GetMesh() const { return m_mesh; }
		std::shared_ptr<SceneGraphLeaf> Clone() override;
		dm::box3 getLocalBoundingBox() override { return m_mesh->objectSpaceBounds; }

		const std::shared_ptr<MeshInfo>& getMesh() const { return m_mesh; }

		friend class SceneGraph;
	protected:
		std::shared_ptr<MeshInfo> m_mesh;
	};

	struct SkinnedMeshJoint
	{
		std::weak_ptr<SceneGraphNode> node;
		math::float4x4 inverseBindMatrix;
	};

	class SkinnedMeshInstance : public MeshInstance
	{
	public:
		explicit SkinnedMeshInstance(std::shared_ptr<MeshInfo> prototypeMesh);

		std::vector<SkinnedMeshJoint> joints;
		nvrhi::BufferHandle jointBuffer;
		nvrhi::BindingSetHandle skinnedBindingSet;

		friend class SceneGraph;

		friend class Demo;//todo:remove this

		const std::shared_ptr<MeshInfo>& GetPrototypeMesh() const { return m_prototypeMesh; }
	private:
		std::shared_ptr<MeshInfo> m_prototypeMesh;
	};

	class SkinnedMeshReference : public SceneGraphLeaf
	{
	public:

		explicit SkinnedMeshReference(std::shared_ptr<SkinnedMeshInstance> instance) : m_instance(instance) {}
		std::shared_ptr<SceneGraphLeaf> Clone() override;
		friend class SceneGraph;
	private:
		std::weak_ptr<SkinnedMeshInstance> m_instance;
	};

	class SceneGraphNode final : public std::enable_shared_from_this<SceneGraphNode>
	{
	public:
		SceneGraphNode() = default;
		~SceneGraphNode() = default;

		SceneGraphNode* GetParent() const { return m_parent; }
		SceneGraphNode* GetFirstChild() const { return m_firstChild.get(); }
		SceneGraphNode* GetNextSibling() const { return m_nextSibling.get(); }
		const std::shared_ptr<SceneGraphLeaf>& GetLeaf() const { return m_leaf; }
		const math::affine3& GetLocalToWorldTransformFloat() const { return m_GlobalTransformFloat; }
		const math::daffine3& GetLocalToWorldTransform() const { return m_GlobalTransform; }

		const math::double3& getTranslation() const { return m_Translation; }

		void SetTransform(const math::double3* translation, const math::dquat* rotation, const math::double3* scaling);
		void SetScaling(const math::double3& scaling);
		void SetRotation(const math::dquat& rotation);
		void SetTranslation(const math::double3& translation);
		void SetLeaf(const std::shared_ptr<SceneGraphLeaf>& leaf);
		void SetName(const GuGuUtf8Str& name);

		const GuGuUtf8Str& GetName() const { return m_name; }

		void ReverseChildren();
		void UpdateLocalTransform();

		friend class SceneGraph;
	private:
		std::weak_ptr<SceneGraph> m_graph;
		SceneGraphNode* m_parent = nullptr;
		std::shared_ptr<SceneGraphNode> m_firstChild;
		std::shared_ptr<SceneGraphNode> m_nextSibling;
		std::shared_ptr<SceneGraphLeaf> m_leaf;

		GuGuUtf8Str m_name;
		math::dquat m_Rotation = math::dquat::identity();
		math::double3 m_Scaling = 1.0;
		math::double3 m_Translation = 0.0;
		math::daffine3 m_LocalTransform = math::daffine3::identity();
		math::daffine3 m_GlobalTransform = math::daffine3::identity();
		math::affine3 m_GlobalTransformFloat = math::affine3::identity();
		bool m_HasLocalTransform = false;

		friend class Demo;//todo:remove this
	};

	class SceneGraphWalker final
	{
	public:
		SceneGraphWalker() = default;

		explicit SceneGraphWalker(SceneGraphNode* scope)
			: m_current(scope)
			, m_scope(scope)
		{}

		SceneGraphWalker(SceneGraphNode* current, SceneGraphNode* scope)
			: m_current(current)
			, m_scope(scope)
		{}

		SceneGraphNode* Get() const { return m_current; }
		operator bool() const { return m_current != nullptr; }
		SceneGraphNode* operator->() const { return m_current; }

		int32_t Next(bool allowChildren);

		int32_t Up();
	private:
		SceneGraphNode* m_current;
		SceneGraphNode* m_scope;
	};

	enum class AnimationAttribute : uint32_t
	{
		Undefined,
		Scaling,
		Rotation,
		Translation,
		LeafProperty
	};

	class SceneGraphAnimationChannel
	{
	public:
		SceneGraphAnimationChannel(std::shared_ptr<animation::Sampler> sampler, const std::shared_ptr<SceneGraphNode>& targetNode, AnimationAttribute attribute)
			: m_sampler(std::move(sampler))
			, m_targetNode(targetNode)
			, m_attribute(attribute)
		{}

		const std::shared_ptr<animation::Sampler>& GetSampler() const { return m_sampler; }
		bool IsValid() const;
		std::shared_ptr<SceneGraphNode> GetTargetNode() const { return m_targetNode.lock(); }
		void SetTargetNode(const std::shared_ptr<SceneGraphNode>& node) { m_targetNode = node; }
		bool Apply(float time) const;
	private:
		std::shared_ptr<animation::Sampler> m_sampler;
		std::weak_ptr<SceneGraphNode> m_targetNode;
		AnimationAttribute m_attribute;
		GuGuUtf8Str m_leafPropertyName;
	};

	class SceneGraphAnimation : public SceneGraphLeaf
	{
	public:
		SceneGraphAnimation() = default;

		std::shared_ptr<SceneGraphLeaf> Clone() override;

		const std::vector<std::shared_ptr<SceneGraphAnimationChannel>>& GetChannels() const { return m_channels; }
		void AddChannel(const std::shared_ptr<SceneGraphAnimationChannel>& channel);
		bool IsValid() const;
		float GetDuration() const { return m_duration; }
		bool Apply(float time) const;
	private:
		std::vector<std::shared_ptr<SceneGraphAnimationChannel>> m_channels;
		float m_duration = 0.0f;
	};

	template<typename T>
	class ResourceTracker
	{
	private:
		std::unordered_map<std::shared_ptr<T>, uint32_t> m_Map;
		using UnderlyingConstIterator = typename std::unordered_map<std::shared_ptr<T>, uint32_t>::const_iterator;

	public:
		class ConstIterator
		{
		private:
			UnderlyingConstIterator m_Iter;
		public:
			ConstIterator(UnderlyingConstIterator iter) : m_Iter(std::move(iter)) {}
			ConstIterator& operator++() { ++m_Iter; return *this; }
			ConstIterator operator++(int) { ConstIterator res = *this; ++m_Iter; return res; }
			bool operator==(ConstIterator other) const { return m_Iter == other.m_Iter; }
			bool operator!=(ConstIterator other) const { return !(*this == other); }
			const std::shared_ptr<T>& operator*() { return m_Iter->first; }
		};

		// Adds a reference to the specified resource.
		// Returns true if this is the first reference, i.e. if the resource has just been added to the tracker.
		bool AddRef(const std::shared_ptr<T>& resource)
		{
			if (!resource) return false;
			uint32_t refCount = ++m_Map[resource];
			return (refCount == 1);
		}

		// Removes a reference from the specified resource.
		// Returns true if this was the last reference, i.e. if the resource has just been removed from the tracker.
		bool Release(const std::shared_ptr<T>& resource)
		{
			if (!resource) return false;
			auto it = m_Map.find(resource);
			if (it == m_Map.end())
			{
				assert(false); // trying to release an object not owned by this tracker
				return false;
			}

			if (it->second == 0)
				assert(false); // zero-reference entries should not be possible; might indicate concurrency issues
			else
				--it->second;

			if (it->second == 0)
			{
				m_Map.erase(it);
				return true;
			}
			return false;
		}

		[[nodiscard]] ConstIterator begin() const { return ConstIterator(m_Map.cbegin()); }
		[[nodiscard]] ConstIterator end() const { return ConstIterator(m_Map.cend()); }
		[[nodiscard]] bool empty() const { return m_Map.empty(); }
		[[nodiscard]] size_t size() const { return m_Map.size(); }
		[[nodiscard]] const std::shared_ptr<T>& operator[](size_t i) { return m_Map[i].first; }
	};

	class SceneGraph : public std::enable_shared_from_this<SceneGraph>
	{
	public:
		SceneGraph() = default;
		virtual ~SceneGraph() = default;

		const std::shared_ptr<SceneGraphNode>& GetRootNode() const { return m_root; }
		const ResourceTracker<MeshInfo>& GetMeshes() const { return m_Meshes; }
		const std::vector<std::shared_ptr<SkinnedMeshInstance>>& GetSkinnedMeshInstances() const { return m_skinnedMeshInstances; }
		const std::vector<std::shared_ptr<SceneGraphAnimation>>& GetAnimations() const { return m_animations; }
		std::shared_ptr<SceneGraphNode> SetRootNode(const std::shared_ptr<SceneGraphNode>& root);
		std::shared_ptr<SceneGraphNode> Attach(const std::shared_ptr<SceneGraphNode>& parent, const std::shared_ptr<SceneGraphNode>& child);
		std::shared_ptr<SceneGraphNode> Detach(const std::shared_ptr<SceneGraphNode>& node);

		std::shared_ptr<SceneGraphNode> findNode(GuGuUtf8Str name, SceneGraphNode* context = nullptr) const;
		friend class SceneGraphNode;
	protected:
		virtual void RegisterLeaf(const std::shared_ptr<SceneGraphLeaf>& leaf);
		virtual void UnregisterLeaf(const std::shared_ptr<SceneGraphLeaf>& leaf);

	private:
		std::shared_ptr<SceneGraphNode> m_root;
		ResourceTracker<MeshInfo> m_Meshes;
		std::vector<std::shared_ptr<MeshInstance>> m_meshInstances;
		std::vector<std::shared_ptr<SkinnedMeshInstance>> m_skinnedMeshInstances;
		std::vector<std::shared_ptr<SceneGraphAnimation>> m_animations;
	};
}