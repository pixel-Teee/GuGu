#include <pch.h>

#include "DetailCategoryTableRow.h"
#include "DetailExpanderArrow.h"
#include "DetailLayoutBuilderImpl.h"
#include "DetailCategoryBuilderImpl.h"
#include "ObjectPropertyNode.h"
#include "DetailsViewBase.h"
#include <Core/UI/WidgetPath.h>
#include <Core/UI/MenuAnchor.h>
#include <Core/UI/Button.h>
#include <Core/GamePlay/Component.h>
#include <Core/GamePlay/GameObject.h>
#include <Application/Application.h>
#include <Editor/StyleSet/EditorStyleSet.h>

namespace GuGu {

	void DetailCategoryTableRow::init(const BuilderArguments& inArgs, std::shared_ptr<DetailTreeNode> inOwnerTreeNode, const std::shared_ptr<TableViewBase>& inOwnerTableView, 
		std::shared_ptr<DetailLayoutBuilderImpl>  inDetailLayoutBuilderImpl)
	{
		m_ownerTreeNode = inOwnerTreeNode;
		m_detailLayoutBuilderImpl = inDetailLayoutBuilderImpl;
		//m_onContextMenuOpening = inArgs.monContextMenuOpening;

		std::shared_ptr<HorizontalBox> headerBox = WIDGET_NEW(HorizontalBox)
			+ HorizontalBox::Slot()
			.setHorizontalAlignment(HorizontalAlignment::Left)
			.setVerticalAlignment(VerticalAlignment::Center)
			.setPadding(Padding(2, 0, 0, 0))
			.FixedWidth()
			(
				WIDGET_NEW(DetailExpanderArrow, std::static_pointer_cast<DetailCategoryTableRow>(shared_from_this()))
			)
			+ HorizontalBox::Slot()
			.setVerticalAlignment(VerticalAlignment::Center)
			.setPadding(Padding(4, 0, 0, 0))
			.FixedWidth()
			(
				WIDGET_NEW(TextBlockWidget)
				.text(inArgs.mdisplayName)
				.textColor(Attribute<math::float4>::Create([=]() {
					return EditorStyleSet::getStyleSet()->getColor("SecondaryColorLevel9");
				}))
			);

		if (inArgs.mheaderContent)
		{
			headerBox->addSlot()
			.setVerticalAlignment(VerticalAlignment::Center)
			.StretchWidth(1.0f)
			(
				inArgs.mheaderContent
			);
		}

		m_childWidget = std::make_shared<SingleChildSlot>();
		m_childWidget->m_parentWidget = shared_from_this();

		m_childWidget->m_childWidget = headerBox;

		TableRow<std::shared_ptr<DetailTreeNode>>::initInternal(
			TableRow::BuilderArguments()
			.Style(EditorStyleSet::getStyleSet()->getStyle<TableRowStyle>(u8"tablerow.editor"))
			.Content
			(
				NullWidget::getNullWidget()
			),
			inOwnerTableView
		);
	}

	Reply DetailCategoryTableRow::OnMouseButtonDown(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent)
	{
		if (inMouseEvent.getEffectingButton() == Keys::LeftMouseButton)
		{
			toggleExpansion();
			return Reply::Handled();
		}
		else
		{
			

			std::shared_ptr<Widget> menuContent;
			menuContent = WIDGET_NEW(Button)
						  .Clicked(this, &DetailCategoryTableRow::rightClick)
						  .buttonSyle(EditorStyleSet::getStyleSet()->getStyle<ButtonStyle>(u8"normalBlueButton"))
						  .Content
						  (
							  WIDGET_NEW(TextBlockWidget)
							  .textColor(Attribute<math::float4>::Create([=]() {
								  return EditorStyleSet::getStyleSet()->getColor("SecondaryColorLevel9");
							   }))
							  .text("delete component")
						  );

			WidgetPath widgetPath = WidgetPath();
			m_menu = Application::getApplication()->pushMenu(shared_from_this(), widgetPath, menuContent, inMouseEvent.m_screenSpacePosition);
			return Reply::Handled();
		}
	}

	GuGu::Reply DetailCategoryTableRow::rightClick()
	{
		std::shared_ptr<DetailCategoryImpl> detailCategoryImpl = std::static_pointer_cast<DetailCategoryImpl>(m_ownerTreeNode.lock());
		GuGuUtf8Str categoryName = detailCategoryImpl->getCategoryName();

		if (m_detailLayoutBuilderImpl.lock())
		{
			std::shared_ptr<ComplexPropertyNode> propertyNode = m_detailLayoutBuilderImpl.lock()->getRootNode();
			if (propertyNode)
			{
				ObjectPropertyNode* objectPropertyNode = propertyNode->asObjectNode();
				if (objectPropertyNode)
				{
					for (int32_t i = 0; i < objectPropertyNode->getNumChildNodes(); ++i)
					{
						meta::Object* object = objectPropertyNode->getInstanceAsObject(i);
						if (object->GetType() == meta::Type::GetFromName(categoryName))
						{
							Component* comp = static_cast<Component*>(object);
							if (comp != nullptr)
							{
								std::shared_ptr<GameObject> parentGameObject = comp->getParentGameObject().lock();
								//meta::Variant owner = objectPropertyNode->getParentNode()->getOwnerFieldVarint(startVarint);
								if (parentGameObject)
								{
									m_menu->dismiss();
									//delete component
									parentGameObject->deleteComponent(comp->GetType().GetName());
									//refresh details view
									std::vector<GameObject*> gameObjects;
									gameObjects.push_back(parentGameObject.get());
									m_detailLayoutBuilderImpl.lock()->getDetailsView()->setObjects(gameObjects, true);
									//World::getWorld()->m_onLevelChanged = std::bind(&ObjectModeInteractive::onLevelChanged, this);
								}
							}
						}
					}
				}
			}
		}
		return Reply::Handled();
	}

}