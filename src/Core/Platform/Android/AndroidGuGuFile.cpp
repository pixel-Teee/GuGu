#include "pch.h"
#include "AndroidGuGuFile.h"

#include <Core/GuGuUtf8Str.h>

#include <android/asset_manager_jni.h>

namespace GuGu{
    GuGuUtf8Str AndroidGuGuFile::m_internalDataPath;
    GuGuUtf8Str AndroidGuGuFile::m_internalFilePath;
    AAssetManager* AndroidGuGuFile::m_assetManager;
    AndroidGuGuFile::AndroidGuGuFile() {
        m_usingAssetManager = false;
    }
    AndroidGuGuFile::~AndroidGuGuFile() {

    }
    void AndroidGuGuFile::OpenFile(const GuGuUtf8Str &path, FileMode fileMode) {
        m_usingAssetManager = false;
        GuGuUtf8Str actualPath(m_internalFilePath);
        actualPath.pushBack(u8"/");
        actualPath.append(path);
        switch(fileMode)
        {
            case FileMode::OnlyRead:
                m_fileHandle = fopen(actualPath.getStr(), "r");
                break;
            case FileMode::OnlyWrite:
                m_fileHandle = fopen(actualPath.getStr(), "w");
                break;
        }

        if(m_fileHandle == nullptr && fileMode == FileMode::OnlyRead)
        {
            GuGu_LOGD("open file error, try to read file from asset manager");
            m_asset = AAssetManager_open(m_assetManager, path.getStr(), AASSET_MODE_BUFFER);
            if(m_asset != nullptr)
            {
                m_usingAssetManager = true;
            }
            else
            {
                m_usingAssetManager = false;
            }
        }
        else if(m_fileHandle == nullptr)
        {
            GuGu_LOGD("open file error");
        }
    }
    void AndroidGuGuFile::CloseFile() {
        if(!m_usingAssetManager)
        {
            int32_t retCode = fclose(m_fileHandle);
            if(retCode == 0)
                GuGu_LOGD("close file success");
            else
                GuGu_LOGD("close file error");
            m_fileHandle = nullptr;
        }
        else
        {
            AAsset_close(m_asset);
            m_asset = nullptr;
        }
        m_usingAssetManager = false;
    }
    void AndroidGuGuFile::WriteFile(void *buffer, int32_t numberOfBytesToWrite) {
        //todo:implement this
        fwrite(buffer, 1, numberOfBytesToWrite, m_fileHandle);
    }
    int32_t AndroidGuGuFile::ReadFile(void *buffer, int32_t numberOfBytesToRead, int32_t& numberOfBytesHaveReaded) {
        if(!m_usingAssetManager)
        {
            numberOfBytesHaveReaded = fread(buffer, 1, numberOfBytesToRead, m_fileHandle);
            GuGu_LOGD("read file size: %d", numberOfBytesHaveReaded);

            if(!numberOfBytesHaveReaded) return false;
            return true;
        }
        else
        {
            numberOfBytesHaveReaded = AAsset_read(m_asset, buffer, numberOfBytesToRead);
            GuGu_LOGD("read file size: %d", numberOfBytesHaveReaded);
            if(numberOfBytesHaveReaded < 0) return false;
            return true;
        }
    }
    bool AndroidGuGuFile::Seek(uint64_t newPosition) {
        if(m_fileHandle != nullptr && !m_usingAssetManager)
        {
            fseek(m_fileHandle, newPosition, SEEK_SET);
            return true;
        }
        else
        {
            AAsset_seek(m_asset, newPosition, SEEK_SET);
        }
        return true;//todo:fix this
    }
    int32_t AndroidGuGuFile::getFileSize() {
        if(m_fileHandle != nullptr && !m_usingAssetManager)
        {
            fseek(m_fileHandle, 0, SEEK_END);
            int32_t length = ftell(m_fileHandle);
            fseek(m_fileHandle, 0, SEEK_SET);
            return length;
        }
        else
        {
            return AAsset_getLength(m_asset);
        }
    }
    void AndroidGuGuFile::setAssetManager(AAssetManager *assetManager) {
        m_assetManager = assetManager;
    }
    void AndroidGuGuFile::setInternalPath(const GuGuUtf8Str &internalDataPath,
                                          const GuGuUtf8Str &internalFilePath) {
        m_internalDataPath = internalDataPath;
        m_internalFilePath = internalFilePath;

        GuGu_LOGD("internal storage data path : %s", m_internalDataPath.getStr());
        GuGu_LOGD("internal storage file path : %s", m_internalFilePath.getStr());
    }
    std::shared_ptr<GuGuFile> CreateFileFactory()
    {
        return std::make_shared<AndroidGuGuFile>();
    }
}