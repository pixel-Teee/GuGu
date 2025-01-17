#include <pch.h>

#include "AssetDragDrop.h"
#include "Border.h"
#include "TextBlockWidget.h"

namespace GuGu {
    std::shared_ptr<Widget> AssetDragDrop::getDefaultDecorator() const
    {
        std::shared_ptr<Widget> thumbnailWidget;
        if (m_assetPaths.size() > 0)
        {
			thumbnailWidget = 
				WIDGET_NEW(TextBlockWidget)
				.text(m_assetPaths[0]);
        }
        else
        {
            thumbnailWidget = WIDGET_NEW(TextBlockWidget)
            .text(u8"null");
        }

		return WIDGET_NEW(Border)
			.BorderBackgroundColor(math::float4(0.65f, 0.56f, 0.45f, 1.0f)) //beige7
			.padding(Padding(2.0f))
			.Content
			(
				WIDGET_NEW(Border)
				.BorderBackgroundColor(math::float4(0.79f, 0.74f, 0.68f, 1.0f)) //beige4
				//.padding(Padding(11.0f))
				.Content
				(
                    thumbnailWidget
				)
			);
    }
    std::shared_ptr<AssetDragDrop> AssetDragDrop::New(const GuGuUtf8Str& inAssetPath)
    {
        std::vector<GuGuUtf8Str> assetPaths;
        assetPaths.push_back(inAssetPath);
        return New(std::vector<AssetData>(), assetPaths);
    }
    std::shared_ptr<AssetDragDrop> AssetDragDrop::New(std::vector<AssetData> inAssetData, std::vector<GuGuUtf8Str> inAssetPaths)
    {
        std::shared_ptr<AssetDragDrop> operation = std::make_shared<AssetDragDrop>();

        operation->m_thumbnailSize = 64;

        operation->m_assetData = inAssetData;
        operation->m_assetPaths = inAssetPaths;

        operation->construct();
        return operation;
    }
}