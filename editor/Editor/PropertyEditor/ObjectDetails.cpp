#include <pch.h>

#include "ObjectDetails.h"
#include "DetailsView.h"//details view
#include <Core/UI/WidgetPath.h>
#include <Core/UI/Button.h>
#include <Application/Application.h>
#include <Core/GamePlay/Component.h>
#include <Core/GamePlay/GameObject.h>
#include <Core/GamePlay/GameUI/UIComponent.h>
#include <Core/GamePlay/GameUI/UITransformComponent.h>
#include <Core/GamePlay/TransformComponent.h>
#include <Core/Reflection/Constructor.h>
#include <Editor/StyleSet/EditorStyleSet.h>
#include <Editor/Transaction/TransactionManager.h>

namespace GuGu {

	void ObjectDetails::init(const BuilderArguments& arguments)
	{
		m_childWidget = std::make_shared<SingleChildSlot>();
		m_childWidget->m_parentWidget = shared_from_this();

		std::shared_ptr<DetailsView> detailsView = WIDGET_NEW(DetailsView);

		m_detailsView = detailsView;
		m_childWidget->m_childWidget = detailsView;
		m_childWidget->m_childWidget->setParentWidget(shared_from_this());
	}

	ObjectDetails::~ObjectDetails()
	{

	}

	void ObjectDetails::setObjects(const std::vector<GameObject*>& inObjects, bool bForceRefresh /*= false*/)
	{
		m_objects = inObjects;
		m_detailsView->setObjects(inObjects, bForceRefresh);
	}

	Reply ObjectDetails::OnMouseButtonDown(const WidgetGeometry& geometry, const PointerEvent& inMouseEvent)
	{
		if (inMouseEvent.getEffectingButton() == Keys::RightMouseButton)
		{
			if (m_objects.size() == 1)
			{
				GameObject* object = m_objects[0];
				//right add game object

				std::shared_ptr<VerticalBox> box = WIDGET_NEW(VerticalBox);

				meta::Type::Set componentClasses;
				this->findAllDerivedClass(typeof(Component), componentClasses);

				for (const auto& item : componentClasses)
				{
					meta::Type derivedClass = item;
					GuGuUtf8Str addNameStr = "add " + derivedClass.GetName();
					std::shared_ptr<Widget> options = WIDGET_NEW(Button)
						.Clicked(this, &ObjectDetails::addComponent, derivedClass)
						.buttonSyle(EditorStyleSet::getStyleSet()->getStyle<ButtonStyle>(u8"normalBlueButton"))
						.Content
						(
							WIDGET_NEW(TextBlockWidget)
							.textColor(EditorStyleSet::getStyleSet()->getColor("beige9"))
							.text(addNameStr)
						);
					box->addSlot()
						.FixedHeight()
						(
							options
						);
				}
				WidgetPath widgetPath = WidgetPath();
				m_menu = Application::getApplication()->pushMenu(shared_from_this(), widgetPath, box, inMouseEvent.m_screenSpacePosition);
				return Reply::Handled();
			}
		}	
		return Reply::Unhandled();
	}

	Reply ObjectDetails::addComponent(meta::Type componentClass)
	{
		if (m_objects.size() == 1) //add component
		{
			TransactionManager& transactionManager = TransactionManager::getTransactionManager();
			transactionManager.beginTransaction();
			GameObject* object = m_objects[0];
			transactionManager.modifyObject(object->shared_from_this());
			meta::Constructor componentConstruct = componentClass.GetDynamicConstructor();
			meta::Variant componentInstance = componentConstruct.Invoke();
			Component* instancePointer = static_cast<Component*>(componentInstance.getBase()->GetPtr());
			object->addComponent(std::shared_ptr<Component>(instancePointer));
			
			//check is ui(then replace transform component to ui component)
			std::set<meta::Type> baseClasses = instancePointer->GetType().GetBaseClasses();
			if (baseClasses.find(typeof(UIComponent)) != baseClasses.end())
			{
				//replace
				if (object->getComponent<TransformComponent>() != nullptr)
				{
					object->deleteComponent(typeof(TransformComponent).GetName());
					object->addComponent(std::make_shared<UITransformComponent>());
				}
			}

			transactionManager.commit();
			m_menu->dismiss();
			setObjects(m_objects, true);//refresh
			return Reply::Handled();
		}
		return Reply::Unhandled();
	}

	void ObjectDetails::findAllDerivedClass(meta::Type componentClass, std::set<meta::Type>& allDerivedClasses)
	{
		meta::Type::Set componentClasses = componentClass.GetDerivedClasses();
		for (const auto& item : componentClasses)
		{
			allDerivedClasses.insert(item);
			findAllDerivedClass(item, allDerivedClasses);
		}
	}

}