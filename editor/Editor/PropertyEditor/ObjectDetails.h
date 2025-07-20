#pragma once

#include <Core/UI/CompoundWidget.h>
#include <Core/UI/UIMacros.h>
#include <Core/Reflection/Type.h>

namespace GuGu {
	class IDetailsView;
	class GameObject;
	class IMenu;
	class ObjectDetails : public CompoundWidget
	{
	public:
		struct BuilderArguments : public Arguments<ObjectDetails>
		{
			BuilderArguments() {}

			~BuilderArguments() = default;
		};

		void init(const BuilderArguments& arguments);

		~ObjectDetails();

		void setObjects(const std::vector<GameObject*>& inObjects, bool bForceRefresh = false);

		Reply OnMouseButtonDown(const WidgetGeometry& geometry, const PointerEvent& inMouseEvent);

		Reply addComponent(meta::Type componentClass);

		void findAllDerivedClass(meta::Type componentClass, std::set<meta::Type>& allDerivedClasses);
	private:
		std::shared_ptr<IDetailsView> m_detailsView;

		std::vector<GameObject*> m_objects;

		std::shared_ptr<IMenu> m_menu;
	};
}