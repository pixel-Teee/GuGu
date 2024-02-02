#pragma once

#include <Core/GuGuUtf8Str.h>

#include <Renderer/nvrhi.h>

#include <vector>

namespace GuGu{
    struct ShaderMacro
    {
        GuGuUtf8Str name;
        GuGuUtf8Str definition;

        ShaderMacro(const std::string& _name, const std::string& _definition)
                : name(_name)
                , definition(_definition)
        { }
    };

    class ShaderFactory
    {
    private:
        nvrhi::DeviceHandle m_Device;
        //std::unordered_map<std::string, std::shared_ptr<vfs::IBlob>> m_BytecodeCache;
        //std::shared_ptr<vfs::IFileSystem> m_fs;
        //std::filesystem::path m_basePath;

    public:
        ShaderFactory(
                nvrhi::DeviceHandle rendererInterface);
                //std::shared_ptr<vfs::IFileSystem> fs,
                //const std::filesystem::path& basePath);

        void ClearCache();

        nvrhi::ShaderHandle CreateShader(const char* fileName, const char* entryName, const std::vector<ShaderMacro>* pDefines, nvrhi::ShaderType shaderType);
        nvrhi::ShaderHandle CreateShader(const char* fileName, const char* entryName, const std::vector<ShaderMacro>* pDefines, const nvrhi::ShaderDesc& desc);
        //nvrhi::ShaderLibraryHandle CreateShaderLibrary(const char* fileName, const std::vector<ShaderMacro>* pDefines);

        //std::shared_ptr<vfs::IBlob> GetBytecode(const char* fileName, const char* entryName);
    };
}