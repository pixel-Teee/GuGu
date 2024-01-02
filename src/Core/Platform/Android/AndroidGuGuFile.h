#pragma once

#include <stdio.h>
#include <Core/GuGuFile.h>

struct AAssetManager;
struct AAsset;
namespace GuGu{
    class GuGuUtf8Str;

    class AndroidGuGuFile : public GuGuFile
    {
    public:
        AndroidGuGuFile();

        virtual ~AndroidGuGuFile();

        virtual void OpenFile(const GuGuUtf8Str& path, FileMode fileMode) override;

        virtual void CloseFile() override;

        virtual void WriteFile(void* buffer, int32_t numberOfBytesToWrite) override;

        virtual void ReadFile(void* buffer, int32_t numberOfBytesToRead) override;

        static void setInternalPath(const GuGuUtf8Str& internalDataPath, const GuGuUtf8Str& internalFilePath);

        static void setAssetManager(AAssetManager* assetManager);

        virtual int32_t getFileSize() override;
    private:
        static GuGuUtf8Str m_internalDataPath;
        static GuGuUtf8Str m_internalFilePath;
        static AAssetManager* m_assetManager;
        AAsset* m_asset;
        bool m_usingAssetManager;
        FILE* m_fileHandle;
    };
}