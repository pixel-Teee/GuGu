#include <pch.h>

#include "AssetViewWidgets.h"

#include <Core/UI/Overlay.h>
#include <Core/UI/ImageWidget.h>
#include <Core/UI/Box.h>
#include <Core/UI/BoxPanel.h>
#include <Core/UI/InlineEditableTextBlock.h>
#include <Editor/StyleSet/EditorStyleSet.h>

#include <Core/Model/StaticMesh.h> //GStaticMesh
#include <Core/GamePlay/Level.h> //Level
#include <Core/Reflection/Type.h>
#include <Core/Texture/GTexture.h>
#include <Core/GamePlay/GameUI/GFont.h> //GFont
#include <Core/Animation/GAnimation.h>
#include <Core/GamePlay/Prefab.h> //Prefab

namespace GuGu {
    void GAssetViewItem::init(const BuilderArguments& arguments)
    {
        m_assetItem = arguments.massetItem;
        m_isSelected = arguments.misSelected;
        m_onRenameCommit = arguments.monRenameCommit;

        m_childWidget = std::make_shared<SingleChildSlot>();
        m_childWidget->m_parentWidget = shared_from_this();
    }

	void GAssetViewItem::handleNameCommitted(const GuGuUtf8Str& newText, TextCommit::Type commitInfo)
	{
        if (m_onRenameCommit)
        {
            m_onRenameCommit(m_assetItem, newText, m_lastGeometry.getLayoutBoundingRect(), commitInfo);
        }
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
            .assetItem(arguments.massetItem)
            .isSelected(arguments.misSelected)
            .onRenameCommit(arguments.monRenameCommit));

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
                else if (meta::Type::getType(assetViewAsset->m_data.m_assetTypeGuid).GetID() == meta::TypeIDs<Prefab>::ID)
                {
					std::shared_ptr<Brush> assetImage = EditorStyleSet::getStyleSet()->getBrush("PrefabAssetIcon");
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
				//WIDGET_NEW(TextBlockWidget)
				//.text(getNameText())
				//.textColor(Attribute<math::float4>::Create([=]() {
				//    return EditorStyleSet::getStyleSet()->getColor("SecondaryColorLevel9");
				//}))
                WIDGET_NEW(InlineEditableTextBlock)
                .text(Attribute<GuGuUtf8Str>::CreateSP(this, &GAssetTileItem::getNameText))
				.textColor(Attribute<math::float4>::Create([=]() {
					return EditorStyleSet::getStyleSet()->getColor("SecondaryColorLevel9");
				}))
                .isSelected(m_isSelected)
                .onTextCommitted(this, &GAssetViewItem::handleNameCommitted)
            )
        );

        m_childWidget->m_childWidget = border;
        m_childWidget->m_childWidget->setParentWidget(shared_from_this());
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
				.textColor(Attribute<math::float4>::Create([=]() {
					return EditorStyleSet::getStyleSet()->getColor("SecondaryColorLevel9");
				}))
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