#include <pch.h>

#include "LevelViewport.h"
#include <Core/GamePlay/World.h>
#include "GameViewportClient.h"

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
		}
	}

	bool LevelViewport::supportsKeyboardFocus() const
	{
		return true;
	}
}