#include <pch.h>

#include "gltfLoader.h"

#define CGLTF_IMPLEMENTATION
#include "cgltf.h"

#include <Core/GuGuUtf8Str.h>

#include <Core/SceneGraph/SceneTypes.h>

#include <unordered_map>

#include <Core/SceneGraph/SceneGraph.h>

#include <Core/FileSystem/FileSystem.h>

#include <Core/Animation/KeyframeAnimation.h>

namespace GuGu {
	static const char* cgltf_error_to_string(cgltf_result res)
	{
		switch (res)
		{
		case cgltf_result_success:
			return "Success";
		case cgltf_result_data_too_short:
			return "Data is too short";
		case cgltf_result_unknown_format:
			return "Unknown format";
		case cgltf_result_invalid_json:
			return "Invalid JSON";
		case cgltf_result_invalid_gltf:
			return "Invalid glTF";
		case cgltf_result_invalid_options:
			return "Invalid options";
		case cgltf_result_file_not_found:
			return "File not found";
		case cgltf_result_io_error:
			return "I/O error";
		case cgltf_result_out_of_memory:
			return "Out of memory";
		case cgltf_result_legacy_gltf:
			return "Legacy glTF";
		default:
			return "Unknown error";
		}
	}

	static std::pair<const uint8_t*, size_t> cgltf_buffer_iterator(const cgltf_accessor* accessor, size_t defaultStride)
	{
		//TODO: sparse accessor support
		const cgltf_buffer_view* view = accessor->buffer_view;
		const uint8_t* data = (uint8_t*)view->buffer->data + view->offset + accessor->offset;
		const size_t stride = view->stride ? view->stride : defaultStride;
		return std::make_pair(data, stride);
	}

	struct cgltf_vfs_context
	{
		std::shared_ptr<FileSystem> fs;
		std::vector<std::vector<uint8_t>> blobs;
	};

	static cgltf_result cgltf_read_file_vfs(const struct cgltf_memory_options* memory_options,
		const struct cgltf_file_options* file_options, const char* path, cgltf_size* size, void** data)
	{
		cgltf_vfs_context* context = (cgltf_vfs_context*)file_options->user_data;

		GuGuUtf8Str filePath = path;
		std::vector<uint8_t> fileContent;
		context->fs->OpenFile(filePath, GuGuFile::FileMode::OnlyRead);
		int32_t fileLength = context->fs->getFileSize();
		fileContent.resize(fileLength);
		int32_t haveReadedLength = 0;
		context->fs->ReadFile(fileContent.data(), fileLength, haveReadedLength);
		context->fs->CloseFile();

		if (fileContent.empty())
			return cgltf_result_file_not_found;

		context->blobs.push_back(fileContent);

		if (size) *size = context->blobs.back().size();
		if (data) *data = (void*)context->blobs.back().data();  // NOLINT(clang-diagnostic-cast-qual)

		return cgltf_result_success;
	}

	void cgltf_release_file_vfs(const struct cgltf_memory_options*, const struct cgltf_file_options*, void*)
	{
		// do nothing
	}

    bool Load(const GuGuUtf8Str& filePath, std::shared_ptr<SceneGraphNode>& outRoot, std::shared_ptr<FileSystem> fileSystem)
    {
		cgltf_vfs_context vfsContext;
		vfsContext.fs = fileSystem;

        cgltf_options options{};
		options.file.read = &cgltf_read_file_vfs;
		options.file.release = &cgltf_release_file_vfs;
		options.file.user_data = &vfsContext;

        cgltf_data* objects = nullptr;
        cgltf_result res = cgltf_parse_file(&options, filePath.getStr(), &objects);
        if (res != cgltf_result_success)
        {
            GuGu_LOGD("couldn't load gltf file '%s': %s", filePath.getStr(), cgltf_error_to_string(res));
        }
		res = cgltf_load_buffers(&options, objects, filePath.getStr());
		if (res != cgltf_result_success)
		{
			GuGu_LOGD("failed to load buffers for glTF file '%s': %s", filePath.getStr(), cgltf_error_to_string(res));
			return false;
		}

		size_t totalIndices = 0;
		size_t totalVertices = 0;
		bool hasJoints = false;
		//todo:load buffers
		for (size_t meshIndex = 0; meshIndex < objects->meshes_count; ++meshIndex)
		{
			const cgltf_mesh& mesh = objects->meshes[meshIndex];

			for (size_t primIndex = 0; primIndex < mesh.primitives_count; ++primIndex)
			{
				const cgltf_primitive& prim = mesh.primitives[primIndex];

				if (prim.type != cgltf_primitive_type_triangles || prim.attributes_count == 0)
					continue;

				if (prim.indices)
					totalIndices += prim.indices->count;
				else
					totalIndices += prim.attributes->data->count;
				totalVertices += prim.attributes->data->count;

				//todo:check joint and weights
				if (!hasJoints)
				{
					//detect if the primitive has joints or weights attributes
					for (size_t attrIndex = 0; attrIndex < prim.attributes_count; ++attrIndex)
					{
						const cgltf_attribute& attr = prim.attributes[attrIndex];
						if (attr.type == cgltf_attribute_type_joints || attr.type == cgltf_attribute_type_weights)
						{
							hasJoints = true;
							break;
						}
					}
				}
			}
		}

		auto buffers = std::make_shared<BufferGroup>();

		buffers->indexData.resize(totalIndices);
		buffers->positionData.resize(totalVertices);
		buffers->normalData.resize(totalVertices);
		buffers->tangentData.resize(totalVertices);
		buffers->texCoord1Data.resize(totalVertices);
		if (hasJoints)
		{
			buffers->jointData.resize(totalVertices);
			buffers->weightData.resize(totalVertices);
		}

		totalIndices = 0;
		totalVertices = 0;

		std::unordered_map<const cgltf_mesh*, std::shared_ptr<MeshInfo>> meshMap;
		std::vector<std::shared_ptr<MeshInfo>> meshes;
		for (size_t meshIndex = 0; meshIndex < objects->meshes_count; ++meshIndex)
		{
			const cgltf_mesh& mesh = objects->meshes[meshIndex];

			std::shared_ptr<MeshInfo> minfo = std::make_shared<MeshInfo>();
			if (mesh.name) minfo->name = mesh.name;
			minfo->buffers = buffers;
			minfo->indexOffset = (uint32_t)totalIndices;
			minfo->vertexOffset = (uint32_t)totalVertices;
			meshes.push_back(minfo);

			meshMap[&mesh] = minfo;

			for (size_t primIndex = 0; primIndex < mesh.primitives_count; ++primIndex)
			{
				const cgltf_primitive& prim = mesh.primitives[primIndex];

				if (prim.type != cgltf_primitive_type_triangles || prim.attributes_count == 0)
					continue;

				if (prim.indices)
				{
					assert(prim.indices->component_type == cgltf_component_type_r_32u ||
						prim.indices->component_type == cgltf_component_type_r_16u ||
						prim.indices->component_type == cgltf_component_type_r_8u);
					assert(prim.indices->type == cgltf_type_scalar);
				}

				const cgltf_accessor* positions = nullptr;
				const cgltf_accessor* normals = nullptr;
				const cgltf_accessor* tangents = nullptr;
				const cgltf_accessor* texcoords = nullptr;
				const cgltf_accessor* jointWeights = nullptr;
				const cgltf_accessor* jointIndices = nullptr;

				for (size_t attrIndex = 0; attrIndex < prim.attributes_count; ++attrIndex)
				{
					const cgltf_attribute& attr = prim.attributes[attrIndex];

					switch (attr.type)  // NOLINT(clang-diagnostic-switch)
					{
					case cgltf_attribute_type_position:
						assert(attr.data->type == cgltf_type_vec3);
						assert(attr.data->component_type == cgltf_component_type_r_32f);
						positions = attr.data;
						break;
					case cgltf_attribute_type_normal:
						assert(attr.data->type == cgltf_type_vec3);
						assert(attr.data->component_type == cgltf_component_type_r_32f);
						normals = attr.data;
						break;
					case cgltf_attribute_type_tangent:
						assert(attr.data->type == cgltf_type_vec4);
						assert(attr.data->component_type == cgltf_component_type_r_32f);
						tangents = attr.data;
						break;
					case cgltf_attribute_type_texcoord:
						assert(attr.data->type == cgltf_type_vec2);
						assert(attr.data->component_type == cgltf_component_type_r_32f);
						if (attr.index == 0)
							texcoords = attr.data;
						break;
					case cgltf_attribute_type_joints:
						assert(attr.data->type == cgltf_type_vec4);
						assert(attr.data->component_type == cgltf_component_type_r_8u || attr.data->component_type == cgltf_component_type_r_16u);
						jointIndices = attr.data;
						break;
					case cgltf_attribute_type_weights:
						assert(attr.data->type == cgltf_type_vec4);
						assert(attr.data->component_type == cgltf_component_type_r_8u || attr.data->component_type == cgltf_component_type_r_16u || attr.data->component_type == cgltf_component_type_r_32f);
						jointWeights = attr.data;
						break;
					}
				}

				assert(positions);

				size_t indexCount = 0;

				if (prim.indices)
				{
					indexCount = prim.indices->count;

					// copy the indices
					auto [indexSrc, indexStride] = cgltf_buffer_iterator(prim.indices, 0);

					uint32_t* indexDst = buffers->indexData.data() + totalIndices;

					switch (prim.indices->component_type)
					{
					case cgltf_component_type_r_8u:
						if (!indexStride) indexStride = sizeof(uint8_t);
						for (size_t i_idx = 0; i_idx < indexCount; i_idx++)
						{
							*indexDst = *(const uint8_t*)indexSrc;

							indexSrc += indexStride;
							indexDst++;
						}
						break;
					case cgltf_component_type_r_16u:
						if (!indexStride) indexStride = sizeof(uint16_t);
						for (size_t i_idx = 0; i_idx < indexCount; i_idx++)
						{
							*indexDst = *(const uint16_t*)indexSrc;

							indexSrc += indexStride;
							indexDst++;
						}
						break;
					case cgltf_component_type_r_32u:
						if (!indexStride) indexStride = sizeof(uint32_t);
						for (size_t i_idx = 0; i_idx < indexCount; i_idx++)
						{
							*indexDst = *(const uint32_t*)indexSrc;

							indexSrc += indexStride;
							indexDst++;
						}
						break;
					default:
						assert(false);
					}
				}

				if (positions)
				{
					auto [positionSrc, positionStride] = cgltf_buffer_iterator(positions, sizeof(float) * 3);
					math::float3* positionDst = buffers->positionData.data() + totalVertices;

					for (size_t vIndex = 0; vIndex < positions->count; ++vIndex)
					{
						*positionDst = (const float*)positionSrc;

						//bounds |= *positionDst;

						positionSrc += positionStride;
						++positionDst;
					}
				}

				if (normals)
				{
					assert(normals->count == positions->count);

					auto [normalSrc, normalStride] = cgltf_buffer_iterator(normals, sizeof(float) * 3);
					uint32_t* normalDst = buffers->normalData.data() + totalVertices;

					for (size_t vIndex = 0; vIndex < normals->count; ++vIndex)
					{
						math::float3 normal = (const float*)normalSrc;
						*normalDst = vectorToSnorm8(normal);

						normalSrc += normalStride;
						++normalDst;
					}
				}

				if (tangents)
				{
					assert(tangents->count == positions->count);

					auto [tangentSrc, tangentStride] = cgltf_buffer_iterator(tangents, sizeof(float) * 4);
					uint32_t* tangentDst = buffers->tangentData.data() + totalVertices;

					for (size_t vIndex = 0; vIndex < tangents->count; ++vIndex)
					{
						math::float4 tangent = (const float*)tangentSrc;
						*tangentDst = vectorToSnorm8(tangent);

						tangentSrc += tangentStride;
						++tangentDst;
					}
				}

				if (texcoords)
				{
					assert(texcoords->count == positions->count);

					auto [texcoordSrc, texcoordStride] = cgltf_buffer_iterator(texcoords, sizeof(float) * 2);
					math::float2* texcoordDst = buffers->texCoord1Data.data() + totalVertices;

					for (size_t v_idx = 0; v_idx < texcoords->count; v_idx++)
					{
						*texcoordDst = (const float*)texcoordSrc;

						texcoordSrc += texcoordStride;
						++texcoordDst;
					}
				}
				else
				{
					math::float2* texcoordDst = buffers->texCoord1Data.data() + totalVertices;
					for (size_t v_idx = 0; v_idx < positions->count; v_idx++)
					{
						*texcoordDst = math::float2(0.f);
						++texcoordDst;
					}
				}

				if (jointIndices)
				{
					assert(jointIndices->count == positions->count);

					auto [jointSrc, jointStride] = cgltf_buffer_iterator(jointIndices, 0);
					math::vector<uint16_t, 4>* jointDst = buffers->jointData.data() + totalVertices;

					if (jointIndices->component_type == cgltf_component_type_r_8u)
					{
						if (!jointStride) jointStride = sizeof(uint8_t) * 4;
						for (size_t v_idx = 0; v_idx < jointIndices->count; v_idx++)
						{
							*jointDst = dm::vector<uint16_t, 4>(jointSrc[0], jointSrc[1], jointSrc[2], jointSrc[3]);

 							jointSrc += jointStride;
							++jointDst;
						}
					}
					else
					{
						assert(jointIndices->component_type == cgltf_component_type_r_16u);
						for (size_t v_idx = 0; v_idx < jointIndices->count; v_idx++)
						{
							if (!jointStride) jointStride = sizeof(uint16_t) * 4;
							const uint16_t* jointSrcUshort = (const uint16_t*)jointSrc;
							*jointDst = dm::vector<uint16_t, 4>(jointSrcUshort[0], jointSrcUshort[1], jointSrcUshort[2], jointSrcUshort[3]);

							jointSrc += jointStride;
							++jointDst;
						}
					}
				}

				if (jointWeights)
				{
					assert(jointWeights->count == positions->count);

					auto [weightSrc, weightStride] = cgltf_buffer_iterator(jointWeights, 0);
					math::float4* weightDst = buffers->weightData.data() + totalVertices;

					if (jointWeights->component_type == cgltf_component_type_r_8u)
					{
						if (!weightStride) weightStride = sizeof(uint8_t) * 4;
						for (size_t v_idx = 0; v_idx < jointIndices->count; v_idx++)
						{
							*weightDst = math::float4(
								float(weightSrc[0]) / 255.f,
								float(weightSrc[1]) / 255.f,
								float(weightSrc[2]) / 255.f,
								float(weightSrc[3]) / 255.f);

							weightSrc += weightStride;
							++weightDst;
						}
					}
					else if (jointWeights->component_type == cgltf_component_type_r_16u)
					{
						if (!weightStride) weightStride = sizeof(uint16_t) * 4;
						for (size_t v_idx = 0; v_idx < jointIndices->count; v_idx++)
						{
							const uint16_t* weightSrcUshort = (const uint16_t*)weightSrc;
							*weightDst = dm::float4(
								float(weightSrcUshort[0]) / 65535.f,
								float(weightSrcUshort[1]) / 65535.f,
								float(weightSrcUshort[2]) / 65535.f,
								float(weightSrcUshort[3]) / 65535.f);

							weightSrc += weightStride;
							++weightDst;
						}
					}
					else
					{
						assert(jointWeights->component_type == cgltf_component_type_r_32f);
						if (!weightStride) weightStride = sizeof(uint32_t) * 4;
						for (size_t v_idx = 0; v_idx < jointIndices->count; v_idx++)
						{
							*weightDst = (const float*)weightSrc;

							weightSrc += weightStride;
							++weightDst;
						}
					}
				}

				auto geometry = std::make_shared<MeshGeometry>();
				geometry->indexOffsetInMesh = minfo->totalIndices;
				geometry->vertexOffsetInMesh = minfo->totalVertices;
				geometry->numIndices = (uint32_t)indexCount;
				geometry->numVertices = (uint32_t)positions->count;
				minfo->totalIndices += geometry->numIndices;
				minfo->totalVertices += geometry->numVertices;
				minfo->geometries.push_back(geometry);

				totalIndices += geometry->numIndices;
				totalVertices += geometry->numVertices;
			}
		}

		//build the scene graph
		std::shared_ptr<SceneGraph> graph = std::make_shared<SceneGraph>();
		std::shared_ptr<SceneGraphNode> root = std::make_shared<SceneGraphNode>();
		std::unordered_map<cgltf_node*, std::shared_ptr<SceneGraphNode>> nodeMap;
		std::vector<cgltf_node*> skinnedNodes;

		struct StackItem
		{
			std::shared_ptr<SceneGraphNode> dstParent;
			cgltf_node** srcNodes = nullptr;
			size_t srcCount = 0;
		};

		std::vector<StackItem> stack;
		root->SetName(filePath.getStr());

		StackItem context;
		context.dstParent = root;
		context.srcNodes = objects->scene->nodes;
		context.srcCount = objects->scene->nodes_count;

		while (context.srcCount > 0)
		{
			cgltf_node* src = *context.srcNodes;
			++context.srcNodes;
			--context.srcCount;

			auto dst = std::make_shared<SceneGraphNode>();

			nodeMap[src] = dst;

			if (src->has_matrix)
			{
				// decompose the matrix into TRS
				math::affine3 aff = math::affine3(&src->matrix[0],
					&src->matrix[4], &src->matrix[8], &src->matrix[12]);

				math::double3 translation;
				math::double3 scaling;
				math::dquat rotation;

				math::decomposeAffine(dm::daffine3(aff), &translation, &rotation, &scaling);

				dst->SetTransform(&translation, &rotation, &scaling);
			}
			else
			{
				if (src->has_scale)
					dst->SetScaling(dm::double3(dm::float3(src->scale)));
				if (src->has_rotation)
					dst->SetRotation(dm::dquat(dm::quat::fromXYZW(src->rotation)));
				if (src->has_translation)
					dst->SetTranslation(dm::double3(dm::float3(src->translation)));
			}

			if (src->name)
				dst->SetName(src->name);

			graph->Attach(context.dstParent, dst);

			if (src->skin)
			{
				//process the skinned nodes later, when the graph is constructed
				skinnedNodes.push_back(src);
			}
			else if (src->mesh)
			{
				auto found = meshMap.find(src->mesh);

				if (found != meshMap.end())
				{
					auto leaf = std::make_shared<MeshInstance>(found->second);
					dst->SetLeaf(leaf);
				}
			}

			if (src->children_count)
			{
				stack.push_back(context);
				context.dstParent = dst;
				context.srcNodes = src->children;
				context.srcCount = src->children_count;
			}
			else
			{
				// go up the stack until we find a node where some nodes are left
				while (context.srcCount == 0 && !stack.empty())
				{
					context.dstParent->ReverseChildren();
					context = stack.back();
					stack.pop_back();
				}
			}
		}

		for (auto* src : skinnedNodes)
		{
			assert(src->skin);
			assert(src->mesh);

			std::shared_ptr<MeshInfo> prototypeMesh;
			auto found = meshMap.find(src->mesh);
			if (found != meshMap.end())
			{
				prototypeMesh = found->second;

				auto skinnedInstance = std::make_shared<SkinnedMeshInstance>(prototypeMesh);
				skinnedInstance->joints.resize(src->skin->joints_count);

				for (size_t jointIndex = 0; jointIndex < src->skin->joints_count; ++jointIndex)
				{
					SkinnedMeshJoint& joint = skinnedInstance->joints[jointIndex];
					cgltf_accessor_read_float(src->skin->inverse_bind_matrices, jointIndex, joint.inverseBindMatrix.m_data, 16);
					joint.node = nodeMap[src->skin->joints[jointIndex]];

					auto jointNode = joint.node.lock();
					if (!jointNode->GetLeaf())
					{
						jointNode->SetLeaf(std::make_shared<SkinnedMeshReference>(skinnedInstance));
					}
				}

				auto dst = nodeMap[src];
				dst->SetLeaf(skinnedInstance);
			}
		}

		outRoot = root;

		auto animationContainer = root;
		if (objects->animations_count > 1)
		{
			animationContainer = std::make_shared<SceneGraphNode>();
			animationContainer->SetName("Animations");
			graph->Attach(root, animationContainer);
		}

		std::unordered_map<const cgltf_animation_sampler*, std::shared_ptr<animation::Sampler>> animationSamplers;
		for (size_t aIndex = 0; aIndex < objects->animations_count; ++aIndex)
		{
			const cgltf_animation* srcAnim = &objects->animations[aIndex];
			auto dstAnim = std::make_shared<SceneGraphAnimation>();

			animationSamplers.clear();

			for (size_t sIndex = 0; sIndex < srcAnim->samplers_count; ++sIndex)
			{
				const cgltf_animation_sampler* srcSampler = &srcAnim->samplers[sIndex];
				auto dstSampler = std::make_shared<animation::Sampler>();

				switch (srcSampler->interpolation)
				{
				case cgltf_interpolation_type_linear:
					dstSampler->SetInterpolationMode(animation::InterpolationMode::Linear);
					break;
				case cgltf_interpolation_type_step:
					dstSampler->SetInterpolationMode(animation::InterpolationMode::Step);
					break;
				case cgltf_interpolation_type_cubic_spline:
					dstSampler->SetInterpolationMode(animation::InterpolationMode::HermiteSpline);
					break;
				}

				const cgltf_accessor* times = srcSampler->input;
				const cgltf_accessor* values = srcSampler->output;
				assert(times->type == cgltf_type_scalar);
				for (size_t sample_idx = 0; sample_idx < times->count; sample_idx++)
				{
					animation::Keyframe keyframe;

					bool timeRead = cgltf_accessor_read_float(times, sample_idx, &keyframe.time, 1);

					bool valueRead;
					if (srcSampler->interpolation == cgltf_interpolation_type_cubic_spline)
					{
						valueRead = cgltf_accessor_read_float(values, sample_idx * 3 + 0, &keyframe.inTangent.x, 4);
						valueRead = cgltf_accessor_read_float(values, sample_idx * 3 + 1, &keyframe.value.x, 4);
						valueRead = cgltf_accessor_read_float(values, sample_idx * 3 + 2, &keyframe.outTangent.x, 4);
					}
					else
					{
						valueRead = cgltf_accessor_read_float(values, sample_idx, &keyframe.value.x, 4);
					}

					if (timeRead && valueRead)
						dstSampler->AddKeyframe(keyframe);
				}

				if (!dstSampler->GetKeyframes().empty())
					animationSamplers[srcSampler] = dstSampler;
				else
					GuGu_LOGD("Animation channel imported with no keyframes, ignoring.");
			}

			for (size_t channel_idx = 0; channel_idx < srcAnim->channels_count; channel_idx++)
			{
				const cgltf_animation_channel* srcChannel = &srcAnim->channels[channel_idx];

				auto dstNode = nodeMap[srcChannel->target_node];
				if (!dstNode)
					continue;

				AnimationAttribute attribute;
				switch (srcChannel->target_path)
				{
				case cgltf_animation_path_type_translation:
					attribute = AnimationAttribute::Translation;
					break;

				case cgltf_animation_path_type_rotation:
					attribute = AnimationAttribute::Rotation;
					break;

				case cgltf_animation_path_type_scale:
					attribute = AnimationAttribute::Scaling;
					break;

				case cgltf_animation_path_type_weights:
				case cgltf_animation_path_type_invalid:
				default:
					GuGu_LOGD("unsupported gltf animation target: %d", srcChannel->target_path);
					continue;
				}

				auto dstSampler = animationSamplers[srcChannel->sampler];
				if (!dstSampler)
					continue;

				auto dstTrack = std::make_shared<SceneGraphAnimationChannel>(dstSampler, dstNode, attribute);

				dstAnim->AddChannel(dstTrack);
			}

			if (dstAnim->IsValid())
			{
				auto animationNode = std::make_shared<SceneGraphNode>();
				animationNode->SetName(dstAnim->GetName());
				graph->Attach(animationContainer, animationNode);
				animationNode->SetLeaf(dstAnim);
				if (srcAnim->name)
					animationNode->SetName(srcAnim->name);
			}
		}

		animationContainer->ReverseChildren();

		cgltf_free(objects);

		return true;
    }
}