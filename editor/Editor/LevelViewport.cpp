#include <pch.h>

#include "LevelViewport.h"
#include <Core/UI/AssetDragDrop.h>
#include <Core/GamePlay/World.h>
#include "EditorViewportClient.h"

namespace GuGu {
	void LevelViewport::init(const BuilderArguments& arguments)
	{
		ViewportWidget::init
		(
			ViewportWidget::BuilderArguments()
			.Content(arguments.mContent->getChildWidget())
			.visibility(Visibility::Visible)
		);
		std::shared_ptr<ViewportClient> viewportClient = std::make_shared<EditorViewportClient>(std::static_pointer_cast<ViewportWidget>(shared_from_this()));
		World::getWorld()->setViewportClient(viewportClient);
		m_viewportClient = viewportClient;
	}
	void LevelViewport::Tick(const WidgetGeometry& allocatedGeometry, const double inCurrentTime, const float inDeltaTime)
	{
		//m_editorCamera.update(inDeltaTime);
		if (m_viewportClient.lock())
		{
			m_viewportClient.lock()->update(inDeltaTime);
		}	
	}
	Reply LevelViewport::OnDrop(const WidgetGeometry& myGeometry, const DragDropEvent& dragDropEvent)
	{
		std::shared_ptr<AssetDragDrop> assetDragDrop = dragDropEvent.getOperationAs<AssetDragDrop>();
		if (assetDragDrop)
		{
			const std::vector<GuGuUtf8Str>& assetPaths = assetDragDrop->getAssetPaths();
			std::vector<AssetData>& assetDatas = assetDragDrop->getAssets();
			for (int32_t i = 0; i < assetDatas.size(); ++i)
			{
				//GuGu_LOGD("type of gStaticMesh:%s", assetDatas[i].m_assetTypeGuid.getGuid().getStr());
				//meta::Type levelType = typeof(Level);
				//const char* str = levelType.getGuid().getGuid().getStr();
				//GuGu_LOGD("type of level:%s", str);
				meta::Type currentType = meta::Type::getType(assetDatas[i].m_assetTypeGuid);
				GuGu::GGuid guid = currentType.getGuid();
				GuGuUtf8Str typeStr = guid.getGuid();
				GuGu_LOGD("guid of current type:%s", typeStr.getStr());
				if (meta::Type::getType(assetDatas[i].m_assetTypeGuid) == typeof(GStaticMesh))
				{
					//GuGu_LOGD("%s\n", assetDatas[i].m_fileName.getStr());
					World::getWorld()->loadObject(assetDatas[i]);
				}
				else if(meta::Type::getType(assetDatas[i].m_assetTypeGuid) == typeof(Level))
				{
					World::getWorld()->loadLevel(assetDatas[i]);
				}			
			}
			return Reply::Handled();
		}
		return Reply::Unhandled();
	}
	bool LevelViewport::supportsKeyboardFocus() const
	{
		return true;
	}
}