#include <pch.h>

#include "SceneGraph.h"

#include <unordered_map>

namespace GuGu {
	const GuGuUtf8Str& SceneGraphLeaf::GetName() const
	{
		auto node = GetNode();
		if (node)
			return node->GetName();

		static const GuGuUtf8Str emptyString = "";
		return emptyString;
	}

	std::shared_ptr<SceneGraphLeaf> MeshInstance::Clone()
	{
        return std::make_shared<MeshInstance>(m_mesh);
	}

	SkinnedMeshInstance::SkinnedMeshInstance(std::shared_ptr<MeshInfo> prototypeMesh)
        : MeshInstance(nullptr)
	{
        m_prototypeMesh = std::move(prototypeMesh);

        auto skinnedMesh = std::make_shared<MeshInfo>();
		skinnedMesh->skinPrototype = m_prototypeMesh;
		skinnedMesh->name = m_prototypeMesh->name;
		skinnedMesh->totalVertices = m_prototypeMesh->totalVertices;
		skinnedMesh->totalIndices = m_prototypeMesh->totalIndices;
		skinnedMesh->geometries.reserve(m_prototypeMesh->geometries.size());

		for (const auto& geometry : m_prototypeMesh->geometries)
		{
			std::shared_ptr<MeshGeometry> newGeometry = std::make_shared<MeshGeometry>();
			*newGeometry = *geometry;
			skinnedMesh->geometries.push_back(newGeometry);
		}

		m_mesh = skinnedMesh;
	}


	std::shared_ptr<SceneGraphLeaf> SkinnedMeshReference::Clone()
	{
        return std::make_shared<SkinnedMeshReference>(m_instance.lock());
	}

	bool SceneGraphAnimationChannel::IsValid() const
	{
        return !m_targetNode.expired();
	}

    bool SceneGraphAnimationChannel::Apply(float time) const
    {
		auto node = m_targetNode.lock();
		//auto material = m_TargetMaterial.lock();
		if ((!node && m_attribute != AnimationAttribute::LeafProperty))
			return false;

		auto valueOption = m_sampler->Evaluate(time, true);
		if (!valueOption.has_value())
			return false;

		auto value = valueOption.value();

		switch (m_attribute)
		{
		case AnimationAttribute::Scaling:
			node->SetScaling(dm::double3(value.xyz()));
			break;

		case AnimationAttribute::Rotation: {
			dm::dquat quat = dm::dquat::fromXYZW(dm::double4(value));
			double len = length(quat);
			if (len == 0.0)
			{
				GuGu_LOGD("Rotation quaternion interpolated to zero, ignoring.");
			}
			else
			{
				quat /= len;
				node->SetRotation(quat);
			}
			break;
		}

		case AnimationAttribute::Translation:
			node->SetTranslation(dm::double3(value.xyz()));
			break;

		case AnimationAttribute::Undefined:
		default:
			GuGu_LOGD("Unsupported animation target (%d), ignoring.", uint32_t(m_attribute));
			return false;
		}

		return true;
    }

	std::shared_ptr<SceneGraphLeaf> SceneGraphAnimation::Clone()
	{
		return std::shared_ptr<SceneGraphLeaf>();
	}

    void SceneGraphAnimation::AddChannel(const std::shared_ptr<SceneGraphAnimationChannel>& channel)
    {
        m_channels.push_back(channel);
        m_duration = std::max(m_duration, channel->GetSampler()->GetEndTime());
    }

    bool SceneGraphAnimation::IsValid() const
    {
		for (const auto& channel : m_channels)
		{
			if (!channel->IsValid())
				return false;
		}

		return true;
    }

    bool SceneGraphAnimation::Apply(float time) const
    {
		bool success = false;

		for (const auto& channel : m_channels)
		{
			success = channel->Apply(time) && success;
		}

		return success;
    }

    void SceneGraphNode::SetTransform(const math::double3* translation, const math::dquat* rotation, const math::double3* scaling)
    {
		if (scaling) m_Scaling = *scaling;
		if (rotation) m_Rotation = *rotation;
		if (translation) m_Translation = *translation;

        m_HasLocalTransform = true;
    }

    void SceneGraphNode::SetScaling(const math::double3& scaling)
    {
        SetTransform(nullptr, nullptr, &scaling);
    }

    void SceneGraphNode::SetRotation(const math::dquat& rotation)
    {
        SetTransform(nullptr, &rotation, nullptr);
    }

    void SceneGraphNode::SetTranslation(const math::double3& translation)
    {
        SetTransform(&translation, nullptr, nullptr);
    }

    void SceneGraphNode::SetLeaf(const std::shared_ptr<SceneGraphLeaf>& leaf)
	{
        auto graph = m_graph.lock();

        if (m_leaf)
        {
            m_leaf->m_node.reset();
            if (graph)
                graph->UnregisterLeaf(m_leaf);
        }

        m_leaf = leaf;
        leaf->m_node = weak_from_this();
        if (graph)
            graph->RegisterLeaf(leaf);
	}

    void SceneGraphNode::SetName(const GuGuUtf8Str& name)
    {
        m_name = name;
    }

    void SceneGraphNode::ReverseChildren()
	{
        // in-place linked list reverse algorithm
		std::shared_ptr<SceneGraphNode> current, prev, next;
		current = m_firstChild;

		while (current)
		{
			next = current->m_nextSibling;
			current->m_nextSibling = prev;
			prev = current;
			current = next;
		}

		m_firstChild = prev;
    }

    void SceneGraphNode::UpdateLocalTransform()
    {
		math::daffine3 transform = math::scaling(m_Scaling);
		transform *= m_Rotation.toAffine();
		transform *= math::translation(m_Translation);
		m_LocalTransform = transform;
    }

	int32_t SceneGraphWalker::Next(bool allowChildren)
	{
        if (!m_current)
            return 0;

        if (allowChildren)
        {
            auto firstChild = m_current->GetFirstChild();
            if (firstChild)
            {
                m_current = firstChild;
                return 1;
            }
        }

        int32_t depth = 0;

        while (m_current)
        {
            if (m_current == m_scope)
            {
                m_current = nullptr;
                return depth;
            }

            auto nextSibling = m_current->GetNextSibling();
            if (nextSibling)
            {
                m_current = nextSibling;
                return depth;
            }

            m_current = m_current->GetParent();
            --depth;
        }

        return depth;
	}
	int32_t SceneGraphWalker::Up()
	{
        if (!m_current)
            return 0;
        if (m_current == m_scope)
        {
            m_current = nullptr;
            return 0;
        }
        m_current = m_current->GetParent();
        return -1;
	}

    std::shared_ptr<SceneGraphNode> SceneGraph::SetRootNode(const std::shared_ptr<SceneGraphNode>& root)
    {
        auto oldRoot = m_root;
        if (m_root)
        {
            Detach(m_root);
        }
        Attach(nullptr, root);
        return oldRoot;
    }
    std::shared_ptr<SceneGraphNode> SceneGraph::Attach(const std::shared_ptr<SceneGraphNode>& parent, const std::shared_ptr<SceneGraphNode>& child)
    {
        auto parentGraph = parent ? parent->m_graph.lock() : shared_from_this();
        auto childGraph = child->m_graph.lock();

        if (!parentGraph && !childGraph)
        {
            //operating on an orphaned subgraph - do not copy or register anything
            assert(parent);
            child->m_nextSibling = parent->m_firstChild;
            child->m_parent = parent.get();
            parent->m_firstChild = child;
            return child;
        }

        assert(parentGraph.get() == this);
        std::shared_ptr<SceneGraphNode> attachedChild;

        if (childGraph)
        {
            //attaching a subgraph that already belongs to a graph - this one or another
            //copy the subgraph first

            //keep a mapping of old nodes to new nodes to patch the copied animations
            std::unordered_map<SceneGraphNode*, std::shared_ptr<SceneGraphNode>> nodeMap;

            SceneGraphNode* currentParent = parent.get();
            SceneGraphWalker walker(child.get());
            while (walker)
            {
                //make a copy of the current node
                std::shared_ptr<SceneGraphNode> copy = std::make_shared<SceneGraphNode>();
                nodeMap[walker.Get()] = copy;

                copy->m_name = walker->m_name;
                copy->m_parent = currentParent;
                copy->m_graph = weak_from_this();
               
                if (walker->m_leaf)
                {
                    //copy leaf
                    auto leafCopy = walker->m_leaf->Clone();
                    copy->SetLeaf(leafCopy);
                }

                //attach the copy to the new parent
                if (currentParent)
                {
                    copy->m_nextSibling = currentParent->m_firstChild;
                    currentParent->m_firstChild = copy;
                }
                else
                {
                    m_root = copy;
                }

                //if it's the first node we copied, store it as the new root
                if (!attachedChild)
                    attachedChild = copy;

                //go to the next node
                int32_t deltaDepth = walker.Next(true);

                if (deltaDepth > 0)
                {
                    currentParent = copy.get();
                }
                else
                {
                    while (deltaDepth++ < 0)
                    {
                        //reverse the children list of this parent to make them consistent with the original
                        currentParent->ReverseChildren();
                        //go up the new tree
                        currentParent = currentParent->m_parent;
                    }
                }
            }

            //go over the new nodes and patch the cloned animations and skinned groups to use the new nodes
			walker = SceneGraphWalker(attachedChild.get());
			while (walker)
			{
				if (auto animation = dynamic_cast<SceneGraphAnimation*>(walker->m_leaf.get()))
				{
					for (const auto& channel : animation->GetChannels())
					{
						auto newNode = nodeMap[channel->GetTargetNode().get()];
						if (newNode)
						{
							channel->SetTargetNode(newNode);
						}
					}
				}
				else if (auto skinnedInstance = dynamic_cast<SkinnedMeshInstance*>(walker->m_leaf.get()))
				{
					for (auto& joint : skinnedInstance->joints)
					{
						auto jointNode = joint.node.lock();
						auto newNode = nodeMap[jointNode.get()];
						if (newNode)
						{
							joint.node = newNode;
						}
					}
				}
				else if (auto meshReference = dynamic_cast<SkinnedMeshReference*>(walker->m_leaf.get()))
				{
					auto instance = meshReference->m_instance.lock();
					if (instance)
					{
						auto oldNode = instance->GetNode();
                
						auto newNode = nodeMap[oldNode];
						if (newNode)
							meshReference->m_instance = std::dynamic_pointer_cast<SkinnedMeshInstance>(newNode->m_leaf);
						else
							meshReference->m_instance.reset();
					}
				}
                
				walker.Next(true);
			}
        }
        else
        {
            //child node don't have graph pointer

            //attach a subgraph that has been detached from another graph (or never attached)
            SceneGraphWalker walker(child.get());

            while (walker)
            {
                walker->m_graph = weak_from_this();
                auto leaf = walker->GetLeaf();
                if (leaf)
                    RegisterLeaf(leaf);
                walker.Next(true);
            }

            child->m_parent = parent.get();

            if (parent)
            {
				child->m_nextSibling = parent->m_firstChild;
				parent->m_firstChild = child;
            }
            else
            {
				m_root = child;
            }

            attachedChild = child;
        }

        return attachedChild;
    }
    std::shared_ptr<SceneGraphNode> SceneGraph::Detach(const std::shared_ptr<SceneGraphNode>& node)
    {
        auto nodeGraph = node->m_graph.lock();

        if (nodeGraph)
        {
            assert(nodeGraph.get() == this);

            //unregister all leaves in the subgraph, detach all nodes from the graph
            SceneGraphWalker walker(node.get());
			while (walker)
			{
				walker->m_graph.reset();
				auto leaf = walker->GetLeaf();
				if (leaf)
					UnregisterLeaf(leaf);
				walker.Next(true);
			}
        }

        //remove the node from its parent
		if (node->m_parent)
		{
			//node->m_parent->PropagateDirtyFlags(SceneGraphNode::DirtyFlags::SubgraphStructure);

			std::shared_ptr<SceneGraphNode>* sibling = &node->m_parent->m_firstChild;
			while (*sibling && *sibling != node)
				sibling = &(*sibling)->m_nextSibling;
			if (*sibling)
				*sibling = node->m_nextSibling;
		}

		node->m_parent = nullptr;
		node->m_nextSibling.reset();

		if (m_root == node)
		{
			m_root.reset();
			m_root = std::make_shared<SceneGraphNode>();
		}

		return node;
    }

    void SceneGraph::RegisterLeaf(const std::shared_ptr<SceneGraphLeaf>& leaf)
    {
        if (!leaf)
            return;

        auto meshInstance = std::dynamic_pointer_cast<MeshInstance>(leaf);
        if (meshInstance)
        {
            const auto& mesh = meshInstance->GetMesh();
            if (mesh)
            {
                if (m_Meshes.AddRef(mesh))
                {
                    //m_GeometryCount += mesh->geometries.size();
                    //if (OnMeshAdded)
                    //    OnMeshAdded(mesh);
                }

				if (mesh->skinPrototype)
				{
					if (m_Meshes.AddRef(mesh->skinPrototype))
					{
						//m_GeometryCount += mesh->skinPrototype->geometries.size();
						//if (OnMeshAdded)
						//	OnMeshAdded(mesh->skinPrototype);
					}
				}
            }
            m_meshInstances.push_back(meshInstance);

			auto skinnedInstance = std::dynamic_pointer_cast<SkinnedMeshInstance>(meshInstance);
			if (skinnedInstance)
			{
				m_skinnedMeshInstances.push_back(skinnedInstance);
			}		
            return;
        }

		auto animation = std::dynamic_pointer_cast<SceneGraphAnimation>(leaf);
		if (animation)
		{
			m_animations.push_back(animation);
			return;
		}
    }

    void SceneGraph::UnregisterLeaf(const std::shared_ptr<SceneGraphLeaf>& leaf)
    {
        if (!leaf)
            return;

        auto meshInstance = std::dynamic_pointer_cast<MeshInstance>(leaf);
        if (meshInstance)
        {
			auto it = std::find(m_meshInstances.begin(), m_meshInstances.end(), meshInstance);
			if (it != m_meshInstances.end())
				m_meshInstances.erase(it);
			return;
        }

		auto skinnedInstance = std::dynamic_pointer_cast<SkinnedMeshInstance>(leaf);
		if (skinnedInstance)
		{
			auto it = std::find(m_skinnedMeshInstances.begin(), m_skinnedMeshInstances.end(), skinnedInstance);
			if (it != m_skinnedMeshInstances.end())
				m_skinnedMeshInstances.erase(it);
			return;
		}

		auto animation = std::dynamic_pointer_cast<SceneGraphAnimation>(leaf);
		if (animation)
		{
			auto it = std::find(m_animations.begin(), m_animations.end(), animation);
			if (it != m_animations.end())
				m_animations.erase(it);
			return;
		}
    }

}