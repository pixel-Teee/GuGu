#include <pch.h>

#include "ShaderFactory.h"

//#include <vector>

#include <Core/GuGuFile.h>

//todo:fix this
#if ANDROID
    #include <Core/Platform/Android/AndroidGuGuFile.h>
#else
    #ifdef WIN32

        #include <Core/Platform/Windows/WindowsGuGuFile.h>
    #endif
#endif

namespace GuGu{
    static std::vector<uint8_t> LoadBinaryFileToVector(const char *file_path){
#if ANDROID
        std::vector<uint8_t> file_content;

        GuGuUtf8Str filePath(file_path);
        AndroidGuGuFile file;
        file.OpenFile(filePath, GuGuFile::FileMode::OnlyRead);

        int32_t fileLength = file.getFileSize();
        file_content.resize(fileLength);

        int32_t haveReadedLength = 0;
        file.ReadFile(file_content.data(), fileLength, haveReadedLength);
        //assert(assetManager);
        //AAsset *file =
        //        AAssetManager_open(assetManager, file_path, AASSET_MODE_BUFFER);
        //size_t file_length = AAsset_getLength(file);
//
        //file_content.resize(file_length);
//
        //AAsset_read(file, file_content.data(), file_length);
        //AAsset_close(file);
        file.CloseFile();
        return file_content;
#else 
#if WIN32
        std::vector<uint8_t> file_content;

        GuGuUtf8Str filePath(file_path);
        WindowsGuGuFile file;
        file.OpenFile(filePath, GuGuFile::FileMode::OnlyRead);

        int32_t fileLength = file.getFileSize();
        file_content.resize(fileLength);

        int32_t haveReadedLength = 0;
        file.ReadFile(file_content.data(), fileLength, haveReadedLength);
        //assert(assetManager);
        //AAsset *file =
        //        AAssetManager_open(assetManager, file_path, AASSET_MODE_BUFFER);
        //size_t file_length = AAsset_getLength(file);
    //
        //file_content.resize(file_length);
    //
        //AAsset_read(file, file_content.data(), file_length);
        //AAsset_close(file);
        file.CloseFile();
        
        return file_content;
#endif
#endif
    }

    ShaderFactory::ShaderFactory(nvrhi::DeviceHandle rendererInterface)
    : m_Device(rendererInterface){

    }

    nvrhi::ShaderHandle ShaderFactory::CreateShader(const char* fileName, const char* entryName, const std::vector<ShaderMacro>* pDefines, nvrhi::ShaderType shaderType)
    {
        nvrhi::ShaderDesc desc = nvrhi::ShaderDesc(shaderType);
        desc.debugName = fileName;
        return CreateShader(fileName, entryName, pDefines, desc);
    }

    nvrhi::ShaderHandle ShaderFactory::CreateShader(const char* fileName, const char* entryName, const std::vector<ShaderMacro>* pDefines, const nvrhi::ShaderDesc& desc)
    {
        std::vector<uint8_t> buffer = LoadBinaryFileToVector(fileName);

        //std::shared_ptr<IBlob> byteCode = GetBytecode(fileName, entryName);
//
        //if(!byteCode)
        //    return nullptr;
//
        //vector<ShaderMake::ShaderConstant> constants;
        //if (pDefines)
        //{
        //    for (const ShaderMacro& define : *pDefines)
        //        constants.push_back(ShaderMake::ShaderConstant{ define.name.c_str(), define.definition.c_str() });
        //}
//
        //nvrhi::ShaderDesc descCopy = desc;
        //descCopy.entryName = entryName;
//
        //const void* permutationBytecode = nullptr;
        //size_t permutationSize = 0;
        //if (!ShaderMake::FindPermutationInBlob(byteCode->data(), byteCode->size(), constants.data(), uint32_t(constants.size()), &permutationBytecode, &permutationSize))
        //{
        //    const std::string message = ShaderMake::FormatShaderNotFoundMessage(byteCode->data(), byteCode->size(), constants.data(), uint32_t(constants.size()));
        //    log::error("%s", message.c_str());
//
        //    return nullptr;
        //}
//
        //todo:fix this function
        nvrhi::ShaderDesc descCopy = desc;
        descCopy.entryName = entryName;

        return m_Device->createShader(descCopy, buffer.data(), buffer.size());
        //return m_Device->createShader(descCopy, permutationBytecode, permutationSize);
    }
}