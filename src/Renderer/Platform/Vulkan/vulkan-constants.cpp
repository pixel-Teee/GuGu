#include <pch.h>

#include "vulkan-constants.h"

#include <array>

//#include "vulkan-backend.h"

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
    }
}