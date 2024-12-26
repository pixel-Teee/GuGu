#pragma once

#include "AssetViewTypes.h"
#include "SourcesViewWidgets.h"

#include <Core/UI/TileView.h>

namespace GuGu {
	class AssetTileView : public TileView<std::shared_ptr<AssetViewItem>>
	{
	public:
		virtual bool supportsKeyboardFocus() const { return true; }
	};
}