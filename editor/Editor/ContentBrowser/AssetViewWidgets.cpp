#include <pch.h>

#include "AssetViewWidgets.h"

#include <Core/UI/Overlay.h>
#include <Core/UI/ImageWidget.h>
#include <Core/UI/Box.h>
#include <Core/UI/BoxPanel.h>
#include <Editor/StyleSet/EditorStyleSet.h>

#include <Core/Model/StaticMesh.h> //GStaticMesh
#include <Core/GamePlay/Level.h> //Level
#include <Core/Reflection/Type.h>
#include <Core/Texture/GTexture.h>
#include <Core/GamePlay/GameUI/GFont.h> //GFont
#include <Core/Animation/GAnimation.h>

namespace GuGu {
    void GAssetViewItem::init(const BuilderArguments& arguments)
    {
        m_assetItem = arguments.massetItem;

        m_childWidget = std::make_shared<SingleChildSlot>();
        m_childWidget->m_parentWidget = shared_from_this();
    }

    bool GAssetViewItem::isFolder() const
    {
        return m_assetItem && m_assetItem->getType() == AssetItemType::Folder;
    }

    GuGuUtf8Str GAssetViewItem::getNameText() const
    {
        if (m_assetItem)
        {
            if (m_assetItem->getType() == AssetItemType::Folder)
            {
                return std::static_pointer_cast<AssetViewFolder>(m_assetItem)->m_folderName;
            }
            else
            {
                GuGuUtf8Str noExtensionFileName = FilePath::getNonExtensionFileName(std::static_pointer_cast<AssetViewAsset>(m_assetItem)->m_data.m_fileName);
                return noExtensionFileName;
            }
        }

        return GuGuUtf8Str();
    }

	void GAssetTileItem::init(const BuilderArguments& arguments)
	{
        GAssetViewItem::init(GAssetViewItem::BuilderArguments()
            .assetItem(arguments.massetItem));

        m_itemWidth = arguments.mitemWidth;

        std::shared_ptr<Overlay> itemContentsOverlay =
        WIDGET_NEW(Overlay);

        if (isFolder())
        {
            //std::shared_ptr<AssetViewFolder> assetFolderItem = std::static_pointer_cast<AssetViewFolder>(m_assetItem);

            std::shared_ptr<Brush> folderTintImage = EditorStyleSet::getStyleSet()->getBrush("CloseFolder_128x128");
            itemContentsOverlay->addSlot()
            (
                WIDGET_NEW(ImageWidget)
                .brush(folderTintImage)
            );
        }
        else
		{
            std::shared_ptr<AssetViewAsset> assetViewAsset = std::static_pointer_cast<AssetViewAsset>(m_assetItem);

            if (assetViewAsset)
            {
                if (meta::Type::getType(assetViewAsset->m_data.m_assetTypeGuid).GetID() == meta::TypeIDs<GStaticMesh>::ID)
                {
					std::shared_ptr<Brush> assetImage = EditorStyleSet::getStyleSet()->getBrush("MeshAssetIcon");
					itemContentsOverlay->addSlot()
						(
							WIDGET_NEW(ImageWidget)
							.brush(assetImage)
						);
                }	
                else if (meta::Type::getType(assetViewAsset->m_data.m_assetTypeGuid).GetID() == meta::TypeIDs<Level>::ID)
                {
					std::shared_ptr<Brush> assetImage = EditorStyleSet::getStyleSet()->getBrush("LevelIcon");
					itemContentsOverlay->addSlot()
						(
							WIDGET_NEW(ImageWidget)
							.brush(assetImage)
						);
                }
                else if (meta::Type::getType(assetViewAsset->m_data.m_assetTypeGuid).GetID() == meta::TypeIDs<GTexture>::ID)
				{
					std::shared_ptr<Brush> assetImage = EditorStyleSet::getStyleSet()->getBrush("TextureAssetIcon");
					itemContentsOverlay->addSlot()
						(
							WIDGET_NEW(ImageWidget)
							.brush(assetImage)
						);
                }
                else if (meta::Type::getType(assetViewAsset->m_data.m_assetTypeGuid).GetID() == meta::TypeIDs<GFont>::ID)
                {
					std::shared_ptr<Brush> assetImage = EditorStyleSet::getStyleSet()->getBrush("FontAssetIcon");
					itemContentsOverlay->addSlot()
						(
							WIDGET_NEW(ImageWidget)
							.brush(assetImage)
						);
                }
				else if (meta::Type::getType(assetViewAsset->m_data.m_assetTypeGuid).GetID() == meta::TypeIDs<GAnimation>::ID)
				{
					std::shared_ptr<Brush> assetImage = EditorStyleSet::getStyleSet()->getBrush("AnimationAssetIcon");
					itemContentsOverlay->addSlot()
						(
							WIDGET_NEW(ImageWidget)
							.brush(assetImage)
						);
				}
            }	
        }

        std::shared_ptr<Border> border = WIDGET_NEW(Border)
        .BorderBackgroundColor(math::float4(1.0f, 1.0f, 1.0f, 0.0f))//transparent
        .Content
        (
            WIDGET_NEW(VerticalBox)
            + VerticalBox::Slot()
            .FixedHeight()
            .setHorizontalAlignment(HorizontalAlignment::Center)
            (
                WIDGET_NEW(BoxWidget)
                .WidthOverride(this, &GAssetTileItem::getThumbnailBoxSize) //64
                .HeightOverride(this, &GAssetTileItem::getThumbnailBoxSize) //64
                .Content
                (
                    WIDGET_NEW(Border)
                    .BorderBackgroundColor(math::float4(0.0f, 0.0f, 0.0f, 0.0f))//transparent
                    .Content
                    (
                        itemContentsOverlay
                    )
                )
            )
            +VerticalBox::Slot()
            .FixedHeight()
            .setHorizontalAlignment(HorizontalAlignment::Center)
            .setVerticalAlignment(VerticalAlignment::Center)
            (
                WIDGET_NEW(TextBlockWidget)
                .text(getNameText())
                .textColor(EditorStyleSet::getStyleSet()->getColor("beige9"))
            )
        );

        m_childWidget->m_childWidget = border;
        m_childWidget->m_childWidget->setParentWidget(border);
	}

    OptionalSize GAssetTileItem::getThumbnailBoxSize() const
    {
        return OptionalSize(m_itemWidth.Get());
    }

	void GAssetListItem::init(const BuilderArguments& arguments)
	{
        GAssetViewItem::init(GAssetViewItem::BuilderArguments()
            .assetItem(arguments.massetItem));

        m_itemHeight= arguments.mitemHeight;

        std::shared_ptr<Overlay> itemContentsOverlay =
        WIDGET_NEW(Overlay);

        if (isFolder())
        {
            //std::shared_ptr<AssetViewFolder> assetFolderItem = std::static_pointer_cast<AssetViewFolder>(m_assetItem);

            std::shared_ptr<Brush> folderTintImage = EditorStyleSet::getStyleSet()->getBrush("CloseFolder_128x128");
            itemContentsOverlay->addSlot()
            (
                WIDGET_NEW(ImageWidget)
                .brush(folderTintImage)
            );
        }
        else
		{
            std::shared_ptr<AssetViewAsset> assetViewAsset = std::static_pointer_cast<AssetViewAsset>(m_assetItem);

            if (assetViewAsset)
            {
                if (meta::Type::getType(assetViewAsset->m_data.m_assetTypeGuid).GetID() == meta::TypeIDs<GStaticMesh>::ID)
                {
					std::shared_ptr<Brush> assetImage = EditorStyleSet::getStyleSet()->getBrush("MeshAssetIcon");
					itemContentsOverlay->addSlot()
						(
							WIDGET_NEW(ImageWidget)
							.brush(assetImage)
						);
                }	
                else if (meta::Type::getType(assetViewAsset->m_data.m_assetTypeGuid).GetID() == meta::TypeIDs<Level>::ID)
                {
					std::shared_ptr<Brush> assetImage = EditorStyleSet::getStyleSet()->getBrush("LevelIcon");
					itemContentsOverlay->addSlot()
						(
							WIDGET_NEW(ImageWidget)
							.brush(assetImage)
						);
                }
				else if (meta::Type::getType(assetViewAsset->m_data.m_assetTypeGuid).GetID() == meta::TypeIDs<GTexture>::ID)
				{
					std::shared_ptr<Brush> assetImage = EditorStyleSet::getStyleSet()->getBrush("TextureAssetIcon");
					itemContentsOverlay->addSlot()
						(
							WIDGET_NEW(ImageWidget)
							.brush(assetImage)
						);
				}
				else if (meta::Type::getType(assetViewAsset->m_data.m_assetTypeGuid).GetID() == meta::TypeIDs<GFont>::ID)
				{
					std::shared_ptr<Brush> assetImage = EditorStyleSet::getStyleSet()->getBrush("FontAssetIcon");
					itemContentsOverlay->addSlot()
						(
							WIDGET_NEW(ImageWidget)
							.brush(assetImage)
						);
				}
            }	
        }

        std::shared_ptr<Border> border = WIDGET_NEW(Border)
        .BorderBackgroundColor(math::float4(1.0f, 1.0f, 1.0f, 0.0f))//transparent
        .Content
        (
            WIDGET_NEW(VerticalBox)
            + VerticalBox::Slot()
            .FixedHeight()
            .setHorizontalAlignment(HorizontalAlignment::Center)
            (
                WIDGET_NEW(BoxWidget)
                .WidthOverride(this, &GAssetListItem::getThumbnailBoxSize) //64
                .HeightOverride(this, &GAssetListItem::getThumbnailBoxSize) //64
                .Content
                (
                    WIDGET_NEW(Border)
                    .BorderBackgroundColor(math::float4(0.0f, 0.0f, 0.0f, 0.0f))//transparent
                    .Content
                    (
                        itemContentsOverlay
                    )
                )
            )
            +VerticalBox::Slot()
            .FixedHeight()
            .setHorizontalAlignment(HorizontalAlignment::Center)
            .setVerticalAlignment(VerticalAlignment::Center)
            (
                WIDGET_NEW(TextBlockWidget)
                .text(getNameText())
                .textColor(EditorStyleSet::getStyleSet()->getColor("beige9"))
            )
        );

        m_childWidget->m_childWidget = border;
        m_childWidget->m_childWidget->setParentWidget(border);
	}

	OptionalSize GAssetListItem::getThumbnailBoxSize() const
	{
        return OptionalSize(m_itemHeight.Get());
	}

}