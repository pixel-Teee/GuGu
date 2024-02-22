#include <pch.h>

#include "ShaderFactory.h"

#include "ShaderBlob.h"

#include <Core/GuGuFile.h>
#include <Core/Archiver.h>

namespace GuGu{
    static std::vector<uint8_t> LoadBinaryFileToVector(const char* filePath, const char* entryName){
        if (!entryName)
            entryName = "main";

        GuGuUtf8Str adjustedName = filePath;
        {
            size_t pos = adjustedName.find(".hlsl");
            if (pos != -1)
                adjustedName.erase(pos, 5);
            if (entryName && strcmp(entryName, "main"))
            {
                adjustedName += GuGuUtf8Str("_") + GuGuUtf8Str(entryName);
            }
        }

        GuGuUtf8Str shaderFilePath = adjustedName + GuGuUtf8Str(".bin");
#if 0
        std::vector<uint8_t> fileContent;
        std::shared_ptr<GuGuFile> file = CreateFileFactory();
        file->OpenFile(shaderFilePath, GuGuFile::FileMode::OnlyRead);
        int32_t fileLength = file->getFileSize();
        fileContent.resize(fileLength);
        int32_t haveReadedLength = 0;
        file->ReadFile(fileContent.data(), fileLength, haveReadedLength);
        file->CloseFile();       

#else
		std::vector<uint8_t> fileContent;
		ReadArchive(shaderFilePath, fileContent);
#endif
        return fileContent;
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
        std::vector<uint8_t> buffer = LoadBinaryFileToVector(fileName, entryName);

        if (buffer.empty())
            return nullptr;

        std::vector<ShaderMake::ShaderConstant> constants;
		if (pDefines)
		{
			for (const ShaderMacro& define : *pDefines)
				constants.push_back(ShaderMake::ShaderConstant{ define.name.getStr(), define.definition.getStr() });
		}

		nvrhi::ShaderDesc descCopy = desc;
		descCopy.entryName = entryName;
            
		const void* permutationBytecode = nullptr;
		size_t permutationSize = 0;
		if (!ShaderMake::FindPermutationInBlob(buffer.data(), buffer.size(), constants.data(), uint32_t(constants.size()), &permutationBytecode, &permutationSize))
		{
			const std::string message = ShaderMake::FormatShaderNotFoundMessage(buffer.data(), buffer.size(), constants.data(), uint32_t(constants.size()));
			//log::error("%s", message.c_str());
            GuGu_LOGD("%s", message.c_str());

			return nullptr;
		}

		return m_Device->createShader(descCopy, permutationBytecode, permutationSize);
    }
}