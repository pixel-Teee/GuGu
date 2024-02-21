#include <pch.h>

#include "vulkan-backend.h"

#include <array>

namespace GuGu{
    namespace nvrhi::vulkan{
        struct FormatMapping
        {
            nvrhi::Format rhiFormat;
            VkFormat vkFormat;
        };
        static const std::array<FormatMapping, size_t(Format::COUNT)> c_FormatMap = { {
         { Format::UNKNOWN,           VK_FORMAT_UNDEFINED                },
         { Format::R8_UINT,           VK_FORMAT_R8_UINT                  },
         { Format::R8_SINT,           VK_FORMAT_R8_SINT                  },
         { Format::R8_UNORM,          VK_FORMAT_R8_UNORM                 },
         { Format::R8_SNORM,          VK_FORMAT_R8_SNORM                 },
         { Format::RG8_UINT,          VK_FORMAT_R8G8_UINT                },
         { Format::RG8_SINT,          VK_FORMAT_R8G8_SINT                },
         { Format::RG8_UNORM,         VK_FORMAT_R8G8_UNORM               },
         { Format::RG8_SNORM,         VK_FORMAT_R8G8_SNORM               },
         { Format::R16_UINT,          VK_FORMAT_R16_UINT                 },
         { Format::R16_SINT,          VK_FORMAT_R16_SINT                 },
         { Format::R16_UNORM,         VK_FORMAT_R16_UNORM                },
         { Format::R16_SNORM,         VK_FORMAT_R16_SNORM                },
         { Format::R16_FLOAT,         VK_FORMAT_R16_SFLOAT               },
         { Format::BGRA4_UNORM,       VK_FORMAT_B4G4R4A4_UNORM_PACK16    },
         { Format::B5G6R5_UNORM,      VK_FORMAT_B5G6R5_UNORM_PACK16      },
         { Format::B5G5R5A1_UNORM,    VK_FORMAT_B5G5R5A1_UNORM_PACK16    },
         { Format::RGBA8_UINT,        VK_FORMAT_R8G8B8A8_UINT            },
         { Format::RGBA8_SINT,        VK_FORMAT_R8G8B8A8_SINT            },
         { Format::RGBA8_UNORM,       VK_FORMAT_R8G8B8A8_UNORM           },
         { Format::RGBA8_SNORM,       VK_FORMAT_R8G8B8A8_SNORM           },
         { Format::BGRA8_UNORM,       VK_FORMAT_B8G8R8A8_UNORM           },
         { Format::SRGBA8_UNORM,      VK_FORMAT_R8G8B8A8_SRGB            },
         { Format::SBGRA8_UNORM,      VK_FORMAT_B8G8R8A8_SRGB            },
         { Format::R10G10B10A2_UNORM, VK_FORMAT_A2B10G10R10_UNORM_PACK32 },
         { Format::R11G11B10_FLOAT,   VK_FORMAT_B10G11R11_UFLOAT_PACK32  },
         { Format::RG16_UINT,         VK_FORMAT_R16G16_UINT              },
         { Format::RG16_SINT,         VK_FORMAT_R16G16_SINT              },
         { Format::RG16_UNORM,        VK_FORMAT_R16G16_UNORM             },
         { Format::RG16_SNORM,        VK_FORMAT_R16G16_SNORM             },
         { Format::RG16_FLOAT,        VK_FORMAT_R16G16_SFLOAT            },
         { Format::R32_UINT,          VK_FORMAT_R32_UINT                 },
         { Format::R32_SINT,          VK_FORMAT_R32_SINT                 },
         { Format::R32_FLOAT,         VK_FORMAT_R32_SFLOAT               },
         { Format::RGBA16_UINT,       VK_FORMAT_R16G16B16A16_UINT        },
         { Format::RGBA16_SINT,       VK_FORMAT_R16G16B16A16_SINT        },
         { Format::RGBA16_FLOAT,      VK_FORMAT_R16G16B16A16_SFLOAT      },
         { Format::RGBA16_UNORM,      VK_FORMAT_R16G16B16A16_UNORM       },
         { Format::RGBA16_SNORM,      VK_FORMAT_R16G16B16A16_SNORM       },
         { Format::RG32_UINT,         VK_FORMAT_R32G32_UINT              },
         { Format::RG32_SINT,         VK_FORMAT_R32G32_SINT              },
         { Format::RG32_FLOAT,        VK_FORMAT_R32G32_SFLOAT            },
         { Format::RGB32_UINT,        VK_FORMAT_R32G32B32_UINT           },
         { Format::RGB32_SINT,        VK_FORMAT_R32G32B32_SINT           },
         { Format::RGB32_FLOAT,       VK_FORMAT_R32G32B32_SFLOAT         },
         { Format::RGBA32_UINT,       VK_FORMAT_R32G32B32A32_UINT        },
         { Format::RGBA32_SINT,       VK_FORMAT_R32G32B32A32_SINT        },
         { Format::RGBA32_FLOAT,      VK_FORMAT_R32G32B32A32_SFLOAT      },
         { Format::D16,               VK_FORMAT_D16_UNORM                },
         { Format::D24S8,             VK_FORMAT_D24_UNORM_S8_UINT        },
         { Format::X24G8_UINT,        VK_FORMAT_D24_UNORM_S8_UINT        },
         { Format::D32,               VK_FORMAT_D32_SFLOAT               },
         { Format::D32S8,             VK_FORMAT_D32_SFLOAT_S8_UINT       },
         { Format::X32G8_UINT,        VK_FORMAT_D32_SFLOAT_S8_UINT       },
         { Format::BC1_UNORM,         VK_FORMAT_BC1_RGBA_UNORM_BLOCK     },
         { Format::BC1_UNORM_SRGB,    VK_FORMAT_BC1_RGBA_SRGB_BLOCK      },
         { Format::BC2_UNORM,         VK_FORMAT_BC2_UNORM_BLOCK          },
         { Format::BC2_UNORM_SRGB,    VK_FORMAT_BC2_SRGB_BLOCK           },
         { Format::BC3_UNORM,         VK_FORMAT_BC3_UNORM_BLOCK          },
         { Format::BC3_UNORM_SRGB,    VK_FORMAT_BC3_SRGB_BLOCK           },
         { Format::BC4_UNORM,         VK_FORMAT_BC4_UNORM_BLOCK          },
         { Format::BC4_SNORM,         VK_FORMAT_BC4_SNORM_BLOCK          },
         { Format::BC5_UNORM,         VK_FORMAT_BC5_UNORM_BLOCK          },
         { Format::BC5_SNORM,         VK_FORMAT_BC5_SNORM_BLOCK          },
         { Format::BC6H_UFLOAT,       VK_FORMAT_BC6H_UFLOAT_BLOCK        },
         { Format::BC6H_SFLOAT,       VK_FORMAT_BC6H_SFLOAT_BLOCK        },
         { Format::BC7_UNORM,         VK_FORMAT_BC7_UNORM_BLOCK          },
         { Format::BC7_UNORM_SRGB,    VK_FORMAT_BC7_SRGB_BLOCK           },} };
        VkFormat convertFormat(nvrhi::Format format) {
            assert(format < nvrhi::Format::COUNT);
            assert(c_FormatMap[uint32_t(format)].rhiFormat == format);

            return c_FormatMap[uint32_t(format)].vkFormat;
        }

        //DeviceHandle createDevice(const DeviceDesc &desc) {
        //    Device* device = new Device(desc);
        //    return DeviceHandle::Create(device); //attach
        //}
		VkSamplerAddressMode convertSamplerAddressMode(SamplerAddressMode mode) {
			switch (mode)
			{
			case SamplerAddressMode::ClampToEdge:
				return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

			case SamplerAddressMode::Repeat:
				return VK_SAMPLER_ADDRESS_MODE_REPEAT;

			case SamplerAddressMode::ClampToBorder:
				return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;

			case SamplerAddressMode::MirroredRepeat:
				return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;

			case SamplerAddressMode::MirrorClampToEdge:
				return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;

			default:
				utils::InvalidEnum();
				return VkSamplerAddressMode(0);
			}
		}

        VkShaderStageFlagBits convertShaderTypeToShaderStageFlagBits(ShaderType shaderType)
        {
            if (shaderType == ShaderType::All)
                return VK_SHADER_STAGE_ALL;

            if (shaderType == ShaderType::AllGraphics)
                return VK_SHADER_STAGE_ALL_GRAPHICS;

#if ENABLE_SHORTCUT_CONVERSIONS
        static_assert(uint32_t(ShaderType::Vertex)        == uint32_t(VK_SHADER_STAGE_VERTEX_BIT));
        static_assert(uint32_t(ShaderType::Hull)          == uint32_t(VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT));
        static_assert(uint32_t(ShaderType::Domain)        == uint32_t(VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT));
        static_assert(uint32_t(ShaderType::Geometry)      == uint32_t(VK_SHADER_STAGE_GEOMETRY_BIT));
        static_assert(uint32_t(ShaderType::Pixel)         == uint32_t(VK_SHADER_STAGE_FRAGMENT_BIT));
        static_assert(uint32_t(ShaderType::Compute)       == uint32_t(VK_SHADER_STAGE_COMPUTE_BIT));
        static_assert(uint32_t(ShaderType::Amplification) == uint32_t(VK_SHADER_STAGE_TASK_BIT_NV));
        static_assert(uint32_t(ShaderType::Mesh)          == uint32_t(VK_SHADER_STAGE_MESH_BIT_NV));
        static_assert(uint32_t(ShaderType::RayGeneration) == uint32_t(VK_SHADER_STAGE_RAYGEN_BIT_KHR));
        static_assert(uint32_t(ShaderType::ClosestHit)    == uint32_t(VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR));
        static_assert(uint32_t(ShaderType::AnyHit)        == uint32_t(VK_SHADER_STAGE_ANY_HIT_BIT_KHR));
        static_assert(uint32_t(ShaderType::Miss)          == uint32_t(VK_SHADER_STAGE_MISS_BIT_KHR));
        static_assert(uint32_t(ShaderType::Intersection)  == uint32_t(VK_SHADER_STAGE_INTERSECTION_BIT_KHR));
        static_assert(uint32_t(ShaderType::Callable)      == uint32_t(VK_SHADER_STAGE_CALLABLE_BIT_KHR));

        return vk::ShaderStageFlagBits(shaderType);
#else
            uint32_t result = 0;

            if ((shaderType & ShaderType::Compute) != 0)        result |= uint32_t(VK_SHADER_STAGE_COMPUTE_BIT);
            if ((shaderType & ShaderType::Vertex) != 0)         result |= uint32_t(VK_SHADER_STAGE_VERTEX_BIT);
            if ((shaderType & ShaderType::Hull) != 0)           result |= uint32_t(VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT);
            if ((shaderType & ShaderType::Domain) != 0)         result |= uint32_t(VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT);
            if ((shaderType & ShaderType::Geometry) != 0)       result |= uint32_t(VK_SHADER_STAGE_GEOMETRY_BIT);
            if ((shaderType & ShaderType::Pixel) != 0)          result |= uint32_t(VK_SHADER_STAGE_FRAGMENT_BIT);
            if ((shaderType & ShaderType::Amplification) != 0)  result |= uint32_t(VK_SHADER_STAGE_TASK_BIT_NV);
            if ((shaderType & ShaderType::Mesh) != 0)           result |= uint32_t(VK_SHADER_STAGE_MESH_BIT_NV);
            if ((shaderType & ShaderType::RayGeneration) != 0)  result |= uint32_t(VK_SHADER_STAGE_RAYGEN_BIT_KHR); // or eRaygenNV, they have the same value
            if ((shaderType & ShaderType::Miss) != 0)           result |= uint32_t(VK_SHADER_STAGE_MISS_BIT_KHR);   // same etc...
            if ((shaderType & ShaderType::ClosestHit) != 0)     result |= uint32_t(VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR);
            if ((shaderType & ShaderType::AnyHit) != 0)         result |= uint32_t(VK_SHADER_STAGE_ANY_HIT_BIT_KHR);
            if ((shaderType & ShaderType::Intersection) != 0)   result |= uint32_t(VK_SHADER_STAGE_INTERSECTION_BIT_KHR);

            return VkShaderStageFlagBits(result);
#endif
        }

        struct ResourceStateMappingInternal
        {
            ResourceStates nvrhiState;
            VkPipelineStageFlags2 stageFlags;
            VkAccessFlags2 accessMask;
            VkImageLayout imageLayout;

            ResourceStateMapping AsResourceStateMapping() const
            {
                // It's safe to cast vk::AccessFlags2 -> vk::AccessFlags and vk::PipelineStageFlags2 -> vk::PipelineStageFlags (as long as the enum exist in both versions!),
                // synchronization2 spec says: "The new flags are identical to the old values within the 32-bit range, with new stages and bits beyond that."
                // The below stages are exclustive to synchronization2
                //assert((stageFlags & VK_PIPELINE_STAGE_2_MICROMAP_BUILD_BIT_EXT) != VK_PIPELINE_STAGE_2_MICROMAP_BUILD_BIT_EXT);
                //assert((accessMask & VK_ACCESS_2_MICROMAP_WRITE_BIT_EXT) != VK_ACCESS_2_MICROMAP_WRITE_BIT_EXT);
                return
                        ResourceStateMapping(nvrhiState,
                                             reinterpret_cast<const VkPipelineStageFlags&>(stageFlags),
                                             reinterpret_cast<const VkAccessFlags&>(accessMask),
                                             imageLayout
                        );
            }

            ResourceStateMapping2 AsResourceStateMapping2() const
            {
                return ResourceStateMapping2(nvrhiState, stageFlags, accessMask, imageLayout);
            }
        };

        static const ResourceStateMappingInternal g_ResourceStateMap[] =
                {
                        { ResourceStates::Common,
                                VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
                                VkAccessFlagBits2(0),
                                VK_IMAGE_LAYOUT_UNDEFINED },
                        { ResourceStates::ConstantBuffer,
                                VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
                                VK_ACCESS_2_UNIFORM_READ_BIT,
                                VK_IMAGE_LAYOUT_UNDEFINED },
                        { ResourceStates::VertexBuffer,
                                VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT,
                                VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT,
                                VK_IMAGE_LAYOUT_UNDEFINED },
                        { ResourceStates::IndexBuffer,
                                VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT,
                                VK_ACCESS_2_INDEX_READ_BIT,
                                VK_IMAGE_LAYOUT_UNDEFINED },
                        { ResourceStates::IndirectArgument,
                                VK_PIPELINE_STAGE_2_DRAW_INDIRECT_BIT,
                                VK_ACCESS_2_INDIRECT_COMMAND_READ_BIT,
                                VK_IMAGE_LAYOUT_UNDEFINED },
                        { ResourceStates::ShaderResource,
                                VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
                                VK_ACCESS_2_SHADER_READ_BIT,
                                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL },
                        { ResourceStates::UnorderedAccess,
                                VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
                                VK_ACCESS_2_SHADER_READ_BIT | VK_ACCESS_2_SHADER_WRITE_BIT,
                                VK_IMAGE_LAYOUT_GENERAL },
                        { ResourceStates::RenderTarget,
                                VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
                                VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
                                VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL },
                        { ResourceStates::DepthWrite,
                                VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
                                VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                                VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL },
                        { ResourceStates::DepthRead,
                                VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
                                VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
                                VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL },
                        { ResourceStates::StreamOut,
                                VK_PIPELINE_STAGE_2_TRANSFORM_FEEDBACK_BIT_EXT,
                                VK_ACCESS_2_TRANSFORM_FEEDBACK_WRITE_BIT_EXT,
                                VK_IMAGE_LAYOUT_UNDEFINED },
                        { ResourceStates::CopyDest,
                                VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                                VK_ACCESS_2_TRANSFER_WRITE_BIT,
                                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL },
                        { ResourceStates::CopySource,
                                VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                                VK_ACCESS_2_TRANSFER_READ_BIT,
                                VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL },
                        { ResourceStates::ResolveDest,
                                VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                                VK_ACCESS_2_TRANSFER_WRITE_BIT,
                                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL },
                        { ResourceStates::ResolveSource,
                                VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                                VK_ACCESS_2_TRANSFER_READ_BIT,
                                VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL},
                        { ResourceStates::Present,
                                VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
                                VK_ACCESS_2_MEMORY_READ_BIT,
                                VK_IMAGE_LAYOUT_PRESENT_SRC_KHR },
                        { ResourceStates::AccelStructRead,
                                VK_PIPELINE_STAGE_2_RAY_TRACING_SHADER_BIT_KHR | VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
                                VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR,
                                VK_IMAGE_LAYOUT_UNDEFINED },
                        { ResourceStates::AccelStructWrite,
                                VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
                                VK_ACCESS_2_ACCELERATION_STRUCTURE_WRITE_BIT_KHR,
                                VK_IMAGE_LAYOUT_UNDEFINED },
                        { ResourceStates::AccelStructBuildInput,
                                VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
                                VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR,
                                VK_IMAGE_LAYOUT_UNDEFINED },
                        { ResourceStates::AccelStructBuildBlas,
                                VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
                                VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR,
                                VK_IMAGE_LAYOUT_UNDEFINED },
                        { ResourceStates::ShadingRateSurface,
                                VK_PIPELINE_STAGE_2_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR,
                                VK_ACCESS_2_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR,
                                VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR }
                        //{ ResourceStates::OpacityMicromapWrite,
                        //        VK_PIPELINE_STAGE_2_MICROMAP_BUILD_BIT_EXT,
                        //        VK_ACCESS_2_MICROMAP_READ_BIT_EXT,
                        //        VK_IMAGE_LAYOUT_UNDEFINED },
                        //{ ResourceStates::OpacityMicromapBuildInput,
                        //        VK_PIPELINE_STAGE_2_MICROMAP_BUILD_BIT_EXT,
                        //        VK_ACCESS_2_SHADER_READ_BIT,
                        //        VK_IMAGE_LAYOUT_UNDEFINED },
                };


        ResourceStateMappingInternal convertResourceStateInternal(ResourceStates state)
        {
            ResourceStateMappingInternal result = {};

            constexpr uint32_t numStateBits = sizeof(g_ResourceStateMap) / sizeof(g_ResourceStateMap[0]);

            uint32_t stateTmp = uint32_t(state);
            uint32_t bitIndex = 0;

            while (stateTmp != 0 && bitIndex < numStateBits)
            {
                uint32_t bit = (1 << bitIndex);

                if (stateTmp & bit)
                {
                    const ResourceStateMappingInternal& mapping = g_ResourceStateMap[bitIndex];

                    assert(uint32_t(mapping.nvrhiState) == bit);
                    assert(result.imageLayout == VK_IMAGE_LAYOUT_UNDEFINED || mapping.imageLayout == VK_IMAGE_LAYOUT_UNDEFINED || result.imageLayout == mapping.imageLayout);

                    result.nvrhiState = ResourceStates(result.nvrhiState | mapping.nvrhiState);
                    result.accessMask |= mapping.accessMask;
                    result.stageFlags |= mapping.stageFlags;
                    if (mapping.imageLayout != VK_IMAGE_LAYOUT_UNDEFINED)
                        result.imageLayout = mapping.imageLayout;

                    stateTmp &= ~bit;
                }

                bitIndex++;
            }

            assert(result.nvrhiState == state);

            return result;
        }

        ResourceStateMapping convertResourceState(ResourceStates state)
        {
            const ResourceStateMappingInternal mapping = convertResourceStateInternal(state);
            return mapping.AsResourceStateMapping();
        }

        ResourceStateMapping2 convertResourceState2(ResourceStates state)
        {
            const ResourceStateMappingInternal mapping = convertResourceStateInternal(state);
            return mapping.AsResourceStateMapping2();
        }

		VkPrimitiveTopology convertPrimitiveTopology(PrimitiveType topology)
		{
			switch (topology)
			{
			case PrimitiveType::PointList:
				return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;

			case PrimitiveType::LineList:
				return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;

			case PrimitiveType::TriangleList:
				return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

			case PrimitiveType::TriangleStrip:
				return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;

			case PrimitiveType::TriangleFan:
				return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;

			case PrimitiveType::TriangleListWithAdjacency:
				return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY;

			case PrimitiveType::TriangleStripWithAdjacency:
				return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY;

			case PrimitiveType::PatchList:
				return VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;

			default:
				assert(0);
				return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			}
		}

		VkPolygonMode convertFillMode(RasterFillMode mode)
		{
			switch (mode)
			{
			case RasterFillMode::Fill:
				return VK_POLYGON_MODE_FILL;

			case RasterFillMode::Line:
				return VK_POLYGON_MODE_LINE;

			default:
				assert(0);
				return VK_POLYGON_MODE_FILL;
			}
		}

		VkCullModeFlagBits convertCullMode(RasterCullMode mode)
		{
			switch (mode)
			{
			case RasterCullMode::Back:
				return VK_CULL_MODE_BACK_BIT;

			case RasterCullMode::Front:
				return VK_CULL_MODE_FRONT_BIT;

			case RasterCullMode::None:
				return VK_CULL_MODE_NONE;

			default:
				assert(0);
				return VK_CULL_MODE_NONE;
			}
		}

		VkCompareOp convertCompareOp(ComparisonFunc op)
		{
			switch (op)
			{
			case ComparisonFunc::Never:
				return VK_COMPARE_OP_NEVER;

			case ComparisonFunc::Less:
				return VK_COMPARE_OP_LESS;

			case ComparisonFunc::Equal:
				return VK_COMPARE_OP_EQUAL;

			case ComparisonFunc::LessOrEqual:
				return VK_COMPARE_OP_LESS_OR_EQUAL;

			case ComparisonFunc::Greater:
				return VK_COMPARE_OP_GREATER;

			case ComparisonFunc::NotEqual:
				return VK_COMPARE_OP_NOT_EQUAL;

			case ComparisonFunc::GreaterOrEqual:
				return VK_COMPARE_OP_GREATER_OR_EQUAL;

			case ComparisonFunc::Always:
				return VK_COMPARE_OP_ALWAYS;

			default:
				utils::InvalidEnum();
				return VK_COMPARE_OP_ALWAYS;
			}
		}

		VkStencilOp convertStencilOp(StencilOp op)
		{
			switch (op)
			{
			case StencilOp::Keep:
				return VK_STENCIL_OP_KEEP;

			case StencilOp::Zero:
				return VK_STENCIL_OP_ZERO;

			case StencilOp::Replace:
				return VK_STENCIL_OP_REPLACE;

			case StencilOp::IncrementAndClamp:
				return VK_STENCIL_OP_INCREMENT_AND_CLAMP;

			case StencilOp::DecrementAndClamp:
				return VK_STENCIL_OP_DECREMENT_AND_CLAMP;

			case StencilOp::Invert:
				return VK_STENCIL_OP_INVERT;

			case StencilOp::IncrementAndWrap:
				return VK_STENCIL_OP_INCREMENT_AND_WRAP;

			case StencilOp::DecrementAndWrap:
				return VK_STENCIL_OP_DECREMENT_AND_WRAP;

			default:
				utils::InvalidEnum();
				return VK_STENCIL_OP_KEEP;
			}
		}

		VkStencilOpState convertStencilState(const DepthStencilState& depthStencilState, const DepthStencilState::StencilOpDesc& desc)
		{
			//return vk::StencilOpState()
			//        .setFailOp(convertStencilOp(desc.failOp))
			//        .setPassOp(convertStencilOp(desc.passOp))
			//        .setDepthFailOp(convertStencilOp(desc.depthFailOp))
			//        .setCompareOp(convertCompareOp(desc.stencilFunc))
			//        .setCompareMask(depthStencilState.stencilReadMask)
			//        .setWriteMask(depthStencilState.stencilWriteMask)
			//        .setReference(depthStencilState.stencilRefValue);

			VkStencilOpState stencilOpState = {};
			stencilOpState.failOp = convertStencilOp(desc.failOp);
			stencilOpState.passOp = convertStencilOp(desc.passOp);
			stencilOpState.depthFailOp = convertStencilOp(desc.depthFailOp);
			stencilOpState.compareOp = convertCompareOp(desc.stencilFunc);
			stencilOpState.compareMask = depthStencilState.stencilReadMask;
			stencilOpState.writeMask = depthStencilState.stencilWriteMask;
			stencilOpState.reference = depthStencilState.stencilRefValue;

			return stencilOpState;
		}

		VkBlendFactor convertBlendValue(BlendFactor value)
		{
			switch (value)
			{
			case BlendFactor::Zero:
				return VK_BLEND_FACTOR_ZERO;

			case BlendFactor::One:
				return VK_BLEND_FACTOR_ONE;

			case BlendFactor::SrcColor:
				return VK_BLEND_FACTOR_SRC_COLOR;

			case BlendFactor::OneMinusSrcColor:
				return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;

			case BlendFactor::SrcAlpha:
				return VK_BLEND_FACTOR_SRC_ALPHA;

			case BlendFactor::OneMinusSrcAlpha:
				return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;

			case BlendFactor::DstAlpha:
				return VK_BLEND_FACTOR_DST_ALPHA;

			case BlendFactor::OneMinusDstAlpha:
				return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;

			case BlendFactor::DstColor:
				return VK_BLEND_FACTOR_DST_COLOR;

			case BlendFactor::OneMinusDstColor:
				return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;

			case BlendFactor::SrcAlphaSaturate:
				return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;

			case BlendFactor::ConstantColor:
				return VK_BLEND_FACTOR_CONSTANT_COLOR;

			case BlendFactor::OneMinusConstantColor:
				return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;

			case BlendFactor::Src1Color:
				return VK_BLEND_FACTOR_SRC1_COLOR;

			case BlendFactor::OneMinusSrc1Color:
				return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;

			case BlendFactor::Src1Alpha:
				return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;

			case BlendFactor::OneMinusSrc1Alpha:
				return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;

			default:
				assert(0);
				return VK_BLEND_FACTOR_ZERO;
			}
		}

		VkBlendOp convertBlendOp(BlendOp op)
		{
			switch (op)
			{
			case BlendOp::Add:
				return VK_BLEND_OP_ADD;

			case BlendOp::Subrtact:
				return VK_BLEND_OP_SUBTRACT;

			case BlendOp::ReverseSubtract:
				return VK_BLEND_OP_REVERSE_SUBTRACT;

			case BlendOp::Min:
				return VK_BLEND_OP_MIN;

			case BlendOp::Max:
				return VK_BLEND_OP_MAX;

			default:
				assert(0);
				return VK_BLEND_OP_ADD;
			}
		}

		VkColorComponentFlags convertColorMask(ColorMask mask)
		{
			VkColorComponentFlagBits colorComponentFlagBits = {};
			colorComponentFlagBits = VkColorComponentFlagBits(uint8_t(mask));
			return colorComponentFlagBits;
		}

		VkPipelineColorBlendAttachmentState convertBlendState(const BlendState::RenderTarget& state)
		{
			//return vk::PipelineColorBlendAttachmentState()
			//        .setBlendEnable(state.blendEnable)
			//        .setSrcColorBlendFactor(convertBlendValue(state.srcBlend))
			//        .setDstColorBlendFactor(convertBlendValue(state.destBlend))
			//        .setColorBlendOp(convertBlendOp(state.blendOp))
			//        .setSrcAlphaBlendFactor(convertBlendValue(state.srcBlendAlpha))
			//        .setDstAlphaBlendFactor(convertBlendValue(state.destBlendAlpha))
			//        .setAlphaBlendOp(convertBlendOp(state.blendOpAlpha))
			//        .setColorWriteMask(convertColorMask(state.colorWriteMask));
			VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState = {};
			pipelineColorBlendAttachmentState.blendEnable = state.blendEnable;
			pipelineColorBlendAttachmentState.srcColorBlendFactor = convertBlendValue(state.srcBlend);
			pipelineColorBlendAttachmentState.dstColorBlendFactor = convertBlendValue(state.destBlend);
			pipelineColorBlendAttachmentState.colorBlendOp = convertBlendOp(state.blendOp);
			pipelineColorBlendAttachmentState.srcAlphaBlendFactor = convertBlendValue(state.srcBlendAlpha);
			pipelineColorBlendAttachmentState.dstAlphaBlendFactor = convertBlendValue(state.destBlendAlpha);
			pipelineColorBlendAttachmentState.alphaBlendOp = convertBlendOp(state.blendOpAlpha);
			pipelineColorBlendAttachmentState.colorWriteMask = convertColorMask(state.colorWriteMask);

			return pipelineColorBlendAttachmentState;
		}
		
		VkExtent2D convertFragmentShadingRate(VariableShadingRate shadingRate)
		{
			switch (shadingRate)
			{
			case VariableShadingRate::e1x2:
			{
				VkExtent2D extent2D = {};
				extent2D.width = 1;
				extent2D.height = 2;
				return extent2D;
			}
			case VariableShadingRate::e2x1:
			{
				VkExtent2D extent2D = {};
				extent2D.width = 2;
				extent2D.height = 1;
				return extent2D;
			}
			case VariableShadingRate::e2x2:
			{
				VkExtent2D extent2D = {};
				extent2D.width = 2;
				extent2D.height = 2;
				return extent2D;
			}
			case VariableShadingRate::e2x4:
			{
				VkExtent2D extent2D = {};
				extent2D.width = 2;
				extent2D.height = 4;
				return extent2D;
			}
			case VariableShadingRate::e4x2:
			{
				VkExtent2D extent2D = {};
				extent2D.width = 4;
				extent2D.height = 2;
				return extent2D;
			}
			case VariableShadingRate::e4x4:
			{
				VkExtent2D extent2D = {};
				extent2D.width = 4;
				extent2D.height = 4;
				return extent2D;
			}
			case VariableShadingRate::e1x1:
			default:
			{
				VkExtent2D extent2D = {};
				extent2D.width = 1;
				extent2D.height = 1;
				return extent2D;
			}
			}
		}		

		VkFragmentShadingRateCombinerOpKHR convertShadingRateCombiner(ShadingRateCombiner combiner)
		{
			switch (combiner)
			{
			case ShadingRateCombiner::Override:
				return VK_FRAGMENT_SHADING_RATE_COMBINER_OP_REPLACE_KHR;
			case ShadingRateCombiner::Min:
				return VK_FRAGMENT_SHADING_RATE_COMBINER_OP_MIN_KHR;
			case ShadingRateCombiner::Max:
				return VK_FRAGMENT_SHADING_RATE_COMBINER_OP_MAX_KHR;
			case ShadingRateCombiner::ApplyRelative:
				return VK_FRAGMENT_SHADING_RATE_COMBINER_OP_MUL_KHR;
			case ShadingRateCombiner::Passthrough:
			default:
				return VK_FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP_KHR;
			}
		}	
    }
}