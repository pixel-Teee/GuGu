#include <pch.h>

#include "ViewportClient.h"

namespace GuGu {
	ViewportClient::ViewportClient()
	{
	}
	ViewportClient::~ViewportClient()
	{
	}

	void ViewportClient::setGameObjectSelectionChangedEvent(std::function<void(const std::vector<GameObject*>&, bool)> inGameObjectSelectionChangedEvent)
	{
		m_gameObjectSelectionChangedEvent = inGameObjectSelectionChangedEvent;
	}

	void ViewportClient::broadcastGameObjectSelectionChanged(const std::vector<GameObject*>& newSelection, bool bForceRefresh)
	{
		m_gameObjectSelectionChangedEvent(newSelection, bForceRefresh);
	}

}