#include <pch.h>

#include "ViewportClient.h"

namespace GuGu {
	ViewportClient::ViewportClient()
	{
	}
	ViewportClient::~ViewportClient()
	{
	}

	void ViewportClient::broadcastGameObjectSelectionChanged(const std::vector<GameObject*>& newSelection, bool bForceRefresh)
	{
		m_gameObjectSelectionChangedEvent(newSelection, bForceRefresh);
	}

}