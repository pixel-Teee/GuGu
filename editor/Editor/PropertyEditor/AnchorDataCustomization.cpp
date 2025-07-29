#include <pch.h>

#include "AnchorDataCustomization.h"
#include "PropertyHandleImp.h"
#include "DetailPropertyRow.h"
#include "PropertyNode.h"
#include "CustomChildBuilder.h"//IDetailChildrenBuilder
#include <Editor/StyleSet/EditorStyleSet.h>

#include <Core/UI/TextBlockWidget.h>
#include <Core/UI/ComboButton.h>
#include <Core/UI/Border.h>
#include <Core/UI/BoxPanel.h>
#include <Core/UI/ImageWidget.h>
#include <Core/UI/UniformGridPanel.h>
#include <Core/UI/CompoundWidget.h>
#include <Core/UI/ConstraintCanvas.h>
#include <Core/UI/Button.h>
#include <Core/UI/Border.h>
#include <Core/UI/Box.h>

#include <Core/GamePlay/GameUI/UIAnchorData.h>
#include <Core/GamePlay/GameUI/UIAnchors.h>
#include <Core/GamePlay/GameObject.h>

namespace GuGu {

	class AnchorPreviewWidget : public CompoundWidget
	{
	public:
		AnchorPreviewWidget() {}
		virtual ~AnchorPreviewWidget() {}

		struct BuilderArguments : public Arguments<AnchorPreviewWidget>
		{
			BuilderArguments() {}

			~BuilderArguments() = default;
		};

		void init(const BuilderArguments& arguments, 
		std::shared_ptr<IPropertyHandle> anchorsHandle,
		std::shared_ptr<IPropertyHandle> alignmentHandle,
		std::shared_ptr<IPropertyHandle> offsetsHandle,
		const GuGuUtf8Str& labelText,
		UIAnchors anchors)
		{
			std::shared_ptr<Button> button;
			WIDGET_ASSIGN_NEW(Button, button)
			.buttonSyle(EditorStyleSet::getStyleSet()->getStyle<ButtonStyle>(u8"normalBlueButton"))
			.Clicked(this, &AnchorPreviewWidget::onAnchorClicked, anchorsHandle, alignmentHandle, offsetsHandle, anchors)
			.Content
			(
				WIDGET_NEW(VerticalBox)
				+ VerticalBox::Slot()
				.FixedHeight()
				(
					WIDGET_NEW(Border)
					.brush(EditorStyleSet::getStyleSet()->getBrush("anchorGrid"))
					.padding(0)
					.Content
					(
						WIDGET_NEW(BoxWidget)
						.WidthOverride(64)
						.HeightOverride(64)
						.VAlign(VerticalAlignment::Center)
						.HAlign(HorizontalAlignment::Center)
						.Content
						(
							WIDGET_NEW(BoxWidget)
							.WidthOverride(this, &AnchorPreviewWidget::getCurrentWidth)
							.HeightOverride(this, &AnchorPreviewWidget::getCurrentHeight)
							.VAlign(VerticalAlignment::Stretch)
							.HAlign(HorizontalAlignment::Stretch)
							.Content
							(
								WIDGET_NEW(Border)
								.brush(CoreStyle::getStyleSet()->getBrush("border"))
								.padding(1)
								.Content
								(
									WIDGET_NEW(ConstraintCanvas)
									+ ConstraintCanvas::Slot()
									.anchors(Anchors(anchors.m_minimum.x, anchors.m_minimum.y, anchors.m_maximum.x, anchors.m_maximum.y))
									.offset(Padding(0, 0, anchors.isStretchedHorizontal() ? 0 : 15, anchors.isStretchedVertical() ? 0 : 15))
									.alignment(math::float2(anchors.isStretchedHorizontal() ? 0 : anchors.m_minimum.x, anchors.isStretchedVertical() ? 0 : anchors.m_minimum.y))
									(
										WIDGET_NEW(ImageWidget)
										.brush(EditorStyleSet::getStyleSet()->getBrush("anchorWidget"))
									)
								)
							)
						)
					)
				)
			);
			m_childWidget = std::make_shared<SingleChildSlot>();
			m_childWidget->m_childWidget = button;
			m_childWidget->m_parentWidget = shared_from_this();
			m_childWidget->m_childWidget->setParentWidget(shared_from_this());
		}

		Reply onAnchorClicked(
			std::shared_ptr<IPropertyHandle> anchorsHandle,
			std::shared_ptr<IPropertyHandle> alignmentHandle,
			std::shared_ptr<IPropertyHandle> offsetsHandle,
			UIAnchors anchors
		)
		{
			//todo:add undo/redo
			{
				//set anchors
				char buf[128];
				std::sprintf(buf, "(m_minimum = (x = %f, y = %f), m_maximum = (x = %f, y = %f))", anchors.m_minimum.x, anchors.m_minimum.y, anchors.m_maximum.x, anchors.m_maximum.y);

				const GuGuUtf8Str& value = buf;
				anchorsHandle->setValueFromFormattedString(value);
			}

			return Reply::Handled();
		}

		OptionalSize getCurrentWidth() const
		{
			return 48;
		}

		OptionalSize getCurrentHeight() const
		{
			return 48;
		}
	};

	std::shared_ptr<AnchorDataCustomization> AnchorDataCustomization::create()
	{
		return std::make_shared<AnchorDataCustomization>();
	}

	void AnchorDataCustomization::customizeHeader(std::shared_ptr<IPropertyHandle> propertyHandle, DetailWidgetRow& headerRow)
	{
		//headerRow
		//.nameContent()
		//(
		//	WIDGET_NEW(TextBlockWidget)
		//	.text("114514")
		//)
		//.valueContent()
		//(
		//	WIDGET_NEW(TextBlockWidget)
		//	.text("114514")
		//);
	}

	void AnchorDataCustomization::cutomizeChildren(std::shared_ptr<IPropertyHandle> propertyHandle, IDetailChildrenBuilder& childBuilder)
	{
		customizeLayoutData(propertyHandle, childBuilder);
	}

	void AnchorDataCustomization::createEditorWithDynamicLabel(IDetailPropertyRow& propertyRow, Attribute<GuGuUtf8Str> textAttribute)
	{
		std::shared_ptr<Widget> nameWidget;
		std::shared_ptr<Widget> valueWidget;
		DetailWidgetRow row;
		propertyRow.getDefaultWidgets(nameWidget, valueWidget, row);

		propertyRow.customWidget(/*bShowChildren*/true)
		.nameContent()
		(
			WIDGET_NEW(TextBlockWidget)
			.text(textAttribute) //dynamic text
			.textColor(EditorStyleSet::getStyleSet()->getColor("beige9"))
		)
		.valueContent()
		(
			valueWidget
		);
	}

	void AnchorDataCustomization::customizeLayoutData(std::shared_ptr<IPropertyHandle> propertyHandle, IDetailChildrenBuilder& childBuilder)
	{
		customizeAnchors(propertyHandle, childBuilder);
		customizeOffsets(propertyHandle, childBuilder);

		std::shared_ptr<IPropertyHandle> alignmentHandle = propertyHandle->getChildHandle("m_alignment");
		childBuilder.addProperty(alignmentHandle);
	}

	void AnchorDataCustomization::customizeAnchors(std::shared_ptr<IPropertyHandle> propertyHandle, IDetailChildrenBuilder& childBuilder)
	{
		std::shared_ptr<IPropertyHandle> anchorsHandle = propertyHandle->getChildHandle("m_anchors");
		std::shared_ptr<IPropertyHandle> alignmentHandle = propertyHandle->getChildHandle("m_alignment");
		std::shared_ptr<IPropertyHandle> offsetsHandle = propertyHandle->getChildHandle("m_offset");

		IDetailPropertyRow& anchorsPropertyRow = childBuilder.addProperty(anchorsHandle);

		const float fillDividePadding = 2;

		anchorsPropertyRow.customWidget(/*bShowChildren*/true)
		.nameContent()
		(
			WIDGET_NEW(TextBlockWidget)
			.text("Anchors")
			.textColor(EditorStyleSet::getStyleSet()->getColor("beige9"))
		)
		.valueContent()
		(
			WIDGET_NEW(ComboButton)
			.method(std::optional<PopupMethod>(PopupMethod::CreateNewWindow))
			.buttonContent
			(
				WIDGET_NEW(TextBlockWidget)
				.text("Anchors")
				.textColor(EditorStyleSet::getStyleSet()->getColor("beige9"))
			)
			.menuContent
			(
				WIDGET_NEW(Border)
				.padding(5)
				.BorderBackgroundColor(EditorStyleSet::getStyleSet()->getColor("blueLevel4"))
				.Content
				(
					WIDGET_NEW(Border)
					.BorderBackgroundColor(EditorStyleSet::getStyleSet()->getColor("blueLevel4"))
					.verticalAlignment(VerticalAlignment::Center)
					.horizontalAlignment(HorizontalAlignment::Center)
					.padding(0)
					.Content
					(
						WIDGET_NEW(VerticalBox)
						+ VerticalBox::Slot()
						.FixedHeight()
						(
							WIDGET_NEW(HorizontalBox)
							+ HorizontalBox::Slot()
							.FixedWidth()
							(
								//top row
								WIDGET_NEW(UniformGridPanel)
								+ UniformGridPanel::Slot(0, 0)
								(
									WIDGET_NEW(AnchorPreviewWidget, anchorsHandle, alignmentHandle, offsetsHandle, "TopLeft", UIAnchors(0, 0, 0, 0))
								)
								+ UniformGridPanel::Slot(1, 0)
								(
									WIDGET_NEW(AnchorPreviewWidget, anchorsHandle, alignmentHandle, offsetsHandle, "TopCenter", UIAnchors(0.5, 0, 0.5, 0))
								)
								+ UniformGridPanel::Slot(2, 0)
								(
									WIDGET_NEW(AnchorPreviewWidget, anchorsHandle, alignmentHandle, offsetsHandle, "TopCenter", UIAnchors(1, 0, 1, 0))
								)
								//center row
								+ UniformGridPanel::Slot(0, 1)
								(
									WIDGET_NEW(AnchorPreviewWidget, anchorsHandle, alignmentHandle, offsetsHandle, "TopLeft", UIAnchors(0, 0.5, 0, 0.5))
								)
								+ UniformGridPanel::Slot(1, 1)
								(
									WIDGET_NEW(AnchorPreviewWidget, anchorsHandle, alignmentHandle, offsetsHandle, "CenterCenter", UIAnchors(0.5, 0.5, 0.5, 0.5))
								)
								+ UniformGridPanel::Slot(2, 1)
								(
									WIDGET_NEW(AnchorPreviewWidget, anchorsHandle, alignmentHandle, offsetsHandle, "CenterRight", UIAnchors(1, 0.5, 1, 0.5))
								)
								//bottom row
								+ UniformGridPanel::Slot(0, 2)
								(
									WIDGET_NEW(AnchorPreviewWidget, anchorsHandle, alignmentHandle, offsetsHandle, "BottomLeft", UIAnchors(0, 1, 0, 1))
								)
								+ UniformGridPanel::Slot(1, 2)
								(
									WIDGET_NEW(AnchorPreviewWidget, anchorsHandle, alignmentHandle, offsetsHandle, "BottomCenter", UIAnchors(0.5, 1, 0.5, 1))
								)
								+ UniformGridPanel::Slot(2, 2)
								(
									WIDGET_NEW(AnchorPreviewWidget, anchorsHandle, alignmentHandle, offsetsHandle, "BottomRight", UIAnchors(1, 1, 1, 1))
								)
							)
							+ HorizontalBox::Slot()
							.FixedWidth()
							.setPadding(Padding(fillDividePadding, 0, 0, 0))
							(
								WIDGET_NEW(UniformGridPanel)
								+ UniformGridPanel::Slot(0, 0)
								(
									WIDGET_NEW(AnchorPreviewWidget, anchorsHandle, alignmentHandle, offsetsHandle, "TopFill", UIAnchors(0, 0, 1, 0))
								)
								+ UniformGridPanel::Slot(0, 1)
								(
									WIDGET_NEW(AnchorPreviewWidget, anchorsHandle, alignmentHandle, offsetsHandle, "CenterFill", UIAnchors(0, 0.5, 1, 0.5))
								)
								+ UniformGridPanel::Slot(0, 2)
								(
									WIDGET_NEW(AnchorPreviewWidget, anchorsHandle, alignmentHandle, offsetsHandle, "BottomFill", UIAnchors(0, 1, 1, 1))
								)
							)
						)
						+ VerticalBox::Slot()
						.FixedHeight()
						.setPadding(Padding(0, fillDividePadding, 0, 0))
						(
							WIDGET_NEW(HorizontalBox)
							//fill row
							+ HorizontalBox::Slot()
							.FixedWidth()
							(
								WIDGET_NEW(AnchorPreviewWidget, anchorsHandle, alignmentHandle, offsetsHandle, "FillLeft", UIAnchors(0, 0, 0, 1))
							)
							+ HorizontalBox::Slot()
							.FixedWidth()
							(
								WIDGET_NEW(AnchorPreviewWidget, anchorsHandle, alignmentHandle, offsetsHandle, "FillCenter", UIAnchors(0.5, 0, 0.5, 1))
							)
							+ HorizontalBox::Slot()
							.FixedWidth()
							(
								WIDGET_NEW(AnchorPreviewWidget, anchorsHandle, alignmentHandle, offsetsHandle, "FillRight", UIAnchors(1, 0, 1, 1))
							)
							+ HorizontalBox::Slot()
							.setPadding(Padding(fillDividePadding, 0, 0, 0))
							.FixedWidth()
							(
								WIDGET_NEW(AnchorPreviewWidget, anchorsHandle, alignmentHandle, offsetsHandle, "FillFill", UIAnchors(0, 0, 1, 1))
							)
						)
					)
				)
			)
		);
	}

	void AnchorDataCustomization::customizeOffsets(std::shared_ptr<IPropertyHandle> propertyHandle, IDetailChildrenBuilder& childBuilder)
	{
		std::shared_ptr<IPropertyHandle> anchorsHandle = propertyHandle->getChildHandle("m_anchors");
		std::shared_ptr<IPropertyHandle> offsetsHandle = propertyHandle->getChildHandle("m_offset");

		std::shared_ptr<IPropertyHandle> leftHandle = offsetsHandle->getChildHandle("left");
		std::shared_ptr<IPropertyHandle> topHandle = offsetsHandle->getChildHandle("top");
		std::shared_ptr<IPropertyHandle> rightHandle = offsetsHandle->getChildHandle("right");
		std::shared_ptr<IPropertyHandle> bottomHandle = offsetsHandle->getChildHandle("bottom");

		IDetailPropertyRow& leftRow = childBuilder.addProperty(leftHandle);
		IDetailPropertyRow& topRow = childBuilder.addProperty(topHandle);
		IDetailPropertyRow& rightRow = childBuilder.addProperty(rightHandle);
		IDetailPropertyRow& bottomRow = childBuilder.addProperty(bottomHandle);

		Attribute<GuGuUtf8Str> leftLabel = Attribute<GuGuUtf8Str>::Create(std::bind(&AnchorDataCustomization::getOffsetLabel, propertyHandle, Orientation::Horizontal, "Position X", "Offset Left"));
		Attribute<GuGuUtf8Str> topLabel = Attribute<GuGuUtf8Str>::Create(std::bind(&AnchorDataCustomization::getOffsetLabel, propertyHandle, Orientation::Vertical, "Position Y", "Offset Top"));
		Attribute<GuGuUtf8Str> rightLabel = Attribute<GuGuUtf8Str>::Create(std::bind(&AnchorDataCustomization::getOffsetLabel, propertyHandle, Orientation::Horizontal, "SizeX", "Offset Right"));
		Attribute<GuGuUtf8Str> bottomLabel = Attribute<GuGuUtf8Str>::Create(std::bind(&AnchorDataCustomization::getOffsetLabel, propertyHandle, Orientation::Vertical, "SizeY", "Offset Bottom"));

		createEditorWithDynamicLabel(leftRow, leftLabel);
		createEditorWithDynamicLabel(topRow, topLabel);
		createEditorWithDynamicLabel(rightRow, rightLabel);
		createEditorWithDynamicLabel(bottomRow, bottomLabel);
	}

	GuGuUtf8Str AnchorDataCustomization::getOffsetLabel(std::shared_ptr<IPropertyHandle> propertyHandle, Orientation orientation, const GuGuUtf8Str& nonStretchingLabel, const GuGuUtf8Str& stretchingLabel)
	{
		//get outer objects
		std::vector<meta::Object*> objects;
		propertyHandle->getOuterObjects(objects);

		std::shared_ptr<PropertyNode> propertyNode = std::static_pointer_cast<PropertyHandleBase>(propertyHandle)->getPropertyNode();

		meta::Object* owner;
		if (propertyNode)
		{
			meta::Field* field = propertyNode->getField();
			if (field)
			{
				for (size_t i = 0; i < objects.size(); ++i)
				{
					//get fields
					meta::Field curField = meta::ReflectionDatabase::Instance().types[objects[i]->GetType().GetID()].GetField(field->GetName().getStr());//have this field?
					if (curField.GetType() == field->GetType())
					{
						owner = objects[i];
						break;
					}
				}
			}
		}
		
		if (owner)
		{
			//获取当前字段所在结构体的variant
			meta::Variant startVarint = ObjectVariant(owner);
			
			std::vector<meta::Variant> owners;
			if (propertyNode != nullptr)
			{
				meta::Variant owner = propertyNode->getParentNode()->getOwnerFieldVarint(startVarint);
				if (owner != meta::Variant())
					owners.push_back(std::move(owner));
			}

			meta::Variant fieldValue;

			//read value
			meta::Field* field = propertyNode->getField();
			for (int32_t i = 0; i < owners.size(); ++i)
			{
				meta::Field curField = meta::ReflectionDatabase::Instance().types[owners[i].GetType().GetID()].GetField(field->GetName().getStr());//have this field?
				if (curField.GetType() == field->GetType())
				{
					meta::Variant& instance = owners[i];
					fieldValue = propertyNode->getField()->GetValue(instance);
				}
			}

			//convert to anchor data
			UIAnchorData anchorData = fieldValue.GetValue<UIAnchorData>();

			const bool bStretching = (orientation == Orientation::Horizontal && anchorData.m_anchors.isStretchedHorizontal())
			|| (orientation == Orientation::Vertical && anchorData.m_anchors.isStretchedVertical());
			return bStretching ? stretchingLabel : nonStretchingLabel;
		}
		else
		{
			return stretchingLabel;
		}
	}

}