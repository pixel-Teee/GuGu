#include <pch.h>

#include "LevelViewport.h"
#include <Core/UI/AssetDragDrop.h>
#include <Core/GamePlay/World.h>
#include "EditorCamera.h"

namespace GuGu {
	void LevelViewport::init(const BuilderArguments& arguments)
	{
		ViewportWidget::init
		(
			ViewportWidget::BuilderArguments()
			.Content(arguments.mContent->getChildWidget())
			.visibility(Visibility::Visible)
		);
		std::shared_ptr<ViewportClient> viewportClient = std::make_shared<EditorCamera>(std::static_pointer_cast<ViewportWidget>(shared_from_this()));
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
			const std::vector<AssetData>& assetDatas = assetDragDrop->getAssets();
			for (int32_t i = 0; i < assetDatas.size(); ++i)
			{
				//GuGu_LOGD("%s\n", assetDatas[i].m_fileName.getStr());
				World::getWorld()->loadObject(assetDatas[i]);
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