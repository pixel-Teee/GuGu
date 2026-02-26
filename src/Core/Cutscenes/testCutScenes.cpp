#include <pch.h>

#include "testCutScenes.h"
#include <Core/Cutscenes/Cutscenes.h>
#include <Core/Math/MyMath.h>
#include <Core/GamePlay/TransformComponent.h>
#include <Core/FileSystem/ByteOrder.h>
#include <Core/AssetManager/AssetManager.h>

namespace GuGu {
	static Array<uint8_t> convertDouble3ToBytes(math::double3 inValue)
	{
		Array<uint8_t> res;
		res.resize(3 * sizeof(double));
		inValue.x = HostToLittleDouble(inValue.x);
		inValue.y = HostToLittleDouble(inValue.y);
		inValue.z = HostToLittleDouble(inValue.z);
		std::memcpy(res.data(), &inValue, 3 * sizeof(double));
		return res;
	}

	void testCutscenes()
	{
		//create cutscenes
		std::shared_ptr<Cutscenes> cutscenes = std::make_shared<Cutscenes>();
		cutscenes->setCutscenesName("testCutscenes");
		cutscenes->setDuration(4.0);//duration
		cutscenes->setTicksPerSecond(30);

		Array<TrackData> trackDatas;
		TrackData trackData;
		trackData.m_isEvent = false;
		meta::Type currentType = typeof(math::double3);
		trackData.m_typeGuid = currentType.getGuid().getGuid();
		
		Array<Section> sections;
		Section section;
		section.m_mode = CutscenesInterpolationMode::Linear;
		section.m_startTime = 2.0f;
		section.m_endTime = 30.0f;
		section.m_levelPath = u8"content/testPlanetAdventure.json";
		section.m_levelName = u8"testPlanetAdventure";
		section.m_bindingObjectName = u8"TempCube";
		//component type guid
		meta::Type transformType = typeof(GuGu::TransformComponent);
		section.m_bindingObjectComponentTypeGuid = transformType.getGuid().getGuid();
		
		Array<CutscenesKeyFrame> keyFrames;
		
		CutscenesKeyFrame keyFrame_1;
		CutscenesKeyFrame keyFrame_2;
		keyFrame_1.m_fieldName = "m_Translation";
		keyFrame_1.m_timestamp = 2.0f;
		keyFrame_1.m_typeGuid = currentType.getGuid().getGuid();
		math::double3 keyFrame_1Value = math::double3(-0.677f, 2.0f, 17.32f);
		keyFrame_1.m_value = convertDouble3ToBytes(keyFrame_1Value);

		keyFrame_2.m_fieldName = "m_Translation";
		keyFrame_2.m_timestamp = 30.0f;
		keyFrame_2.m_typeGuid = currentType.getGuid().getGuid();
		math::double3 keyFrame_2Value = math::double3(-0.677f, 6.5f, 17.32f);
		keyFrame_2.m_value = convertDouble3ToBytes(keyFrame_2Value);

		keyFrames.push_back(keyFrame_1);
		keyFrames.push_back(keyFrame_2);
		section.m_keyFrames = keyFrames;
		
		sections.push_back(section);
		trackData.m_sections = sections;
		trackDatas.push_back(trackData);

		cutscenes->setTrackData(trackDatas);

		//serialize cutscenes
		nlohmann::json cutscenesJson = AssetManager::getAssetManager().serializeJson(cutscenes);
		GuGuUtf8Str guidStr = GGuid::generateGuid().getGuid();
		cutscenesJson["Version"] = std::to_string(GuGu_Version);
		GuGuUtf8Str fileContent = cutscenesJson.dump();

		//root path
		GuGuUtf8Str rootActualFilePath = AssetManager::getAssetManager().getActualPhysicalPath("");

		GuGuUtf8Str noFileExtensionsFileName = "testCutscene";

		GuGuUtf8Str outputFilePath = "content/" + noFileExtensionsFileName + ".json";

		guidStr = AssetManager::getAssetManager().registerAsset(guidStr, outputFilePath, noFileExtensionsFileName + ".json", meta::Type(meta::TypeIDs<Cutscenes>().ID));
		cutscenesJson["GUID"] = guidStr.getStr();
		//输出到目录
		AssetManager::getAssetManager().getRootFileSystem()->OpenFile(outputFilePath, GuGuFile::FileMode::OnlyWrite);
		AssetManager::getAssetManager().getRootFileSystem()->WriteFile((void*)fileContent.getStr(), fileContent.getTotalByteCount());
		AssetManager::getAssetManager().getRootFileSystem()->CloseFile();
	}

}