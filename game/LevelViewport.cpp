#include <pch.h>

#include "LevelViewport.h"
#include <Core/GamePlay/World.h>
#include "GameViewportClient.h"
#include <Core/AssetManager/AssetData.h>

namespace GuGu {
	void LevelViewport::init(const BuilderArguments& arguments)
	{
		ViewportWidget::init
		(
			ViewportWidget::BuilderArguments()
			.Content(arguments.mContent->getChildWidget())
			.visibility(Visibility::Visible)
		);
		std::shared_ptr<ViewportClient> viewportClient = std::make_shared<GameViewportClient>(std::static_pointer_cast<ViewportWidget>(shared_from_this()));
		World::getWorld()->setViewportClient(viewportClient);
		m_viewportClient = viewportClient;
	}
	void LevelViewport::Tick(const WidgetGeometry& allocatedGeometry, const double inCurrentTime, const float inDeltaTime)
	{
		//m_editorCamera.update(inDeltaTime);
		if (m_viewportClient.lock())
		{
			m_viewportClient.lock()->update(inDeltaTime);

			m_viewportClient.lock()->setLeftUpperCornerPosAndWidthAndHeight(allocatedGeometry.getAbsolutePosition(), allocatedGeometry.getLocalSize().x, allocatedGeometry.getLocalSize().y);
		}
	}

	bool LevelViewport::supportsKeyboardFocus() const
	{
		return true;
	}
}