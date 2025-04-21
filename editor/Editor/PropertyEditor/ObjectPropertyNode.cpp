#include <pch.h>
#include "ObjectPropertyNode.h"
#include "CategoryPropertyNode.h"
#include "ItemPropertyNode.h"

#include <Core/Reflection/Type.h>
#include <Core/Reflection/ReflectionDatabase.h>
#include <Core/GamePlay/GameObject.h>

namespace GuGu {
	static std::shared_ptr<PropertyNode> findChildCategory(std::shared_ptr<PropertyNode> parentNode, GuGuUtf8Str categoryName)
	{
		for (int32_t curNodeIndex = 0; curNodeIndex < parentNode->getNumChildNodes(); ++curNodeIndex)
		{
			std::shared_ptr<PropertyNode>& childNode = parentNode->getChildNode(curNodeIndex);

			//check this 
			CategoryPropertyNode* childCategoryNode = childNode->asCategoryNode();
			if (childCategoryNode != nullptr && childCategoryNode->getCategoryName() == categoryName)
			{
				return childNode;
			}
		}
		return nullptr;
	}

	ObjectPropertyNode::ObjectPropertyNode()
	{

	}

	ObjectPropertyNode::~ObjectPropertyNode()
	{

	}

	void ObjectPropertyNode::addObject(meta::Object* inObject)
	{
		m_objects.push_back(inObject);
	}

	void ObjectPropertyNode::addObjects(const std::vector<meta::Object*>& inObjects)
	{
		m_objects.insert(m_objects.end(), inObjects.begin(), inObjects.end());//append
	}

	meta::Object* ObjectPropertyNode::getObject(int32_t inIndex)
	{
		return m_objects[inIndex];
	}

	const meta::Object* ObjectPropertyNode::getObject(int32_t inIndex) const
	{
		return m_objects[inIndex];
	}

	void ObjectPropertyNode::finalize()
	{
		meta::Type oldBase = getObjectBaseClass();

		setBestBaseClass();
	}

	std::vector<meta::Type> ObjectPropertyNode::getAllStructures()
	{
		std::vector<meta::Type> retVal;
		retVal.push_back(m_baseClass);
		return retVal;//todo:修复这个
	}

	meta::Object* ObjectPropertyNode::getInstanceAsObject(int32_t index) const
	{
		return m_objects[index];
	}

	meta::Variant ObjectPropertyNode::getOwnerFieldVarint(const meta::Variant& startVarint)
	{
		for (int32_t i = 0; i < m_objects.size(); ++i)
		{
			meta::Object& object = startVarint.GetValue<meta::Object>();
			if (m_objects[i] == &object)
			{
				return startVarint;
			}
		}
		if (m_parentNodeWeakPtr.lock())
		{
			return m_parentNodeWeakPtr.lock()->getOwnerFieldVarint(startVarint);
		}
		return meta::Variant();
	}

	void ObjectPropertyNode::initChildNodes()
	{
		internalInitChildNodes();
	}

	void ObjectPropertyNode::initBeforeNodeFlags()
	{
		m_property = nullptr;
		finalize();
	}

	void ObjectPropertyNode::setBestBaseClass()
	{
		m_baseClass = meta::Type::Invalid();

		for (int32_t x = 0; x < m_objects.size(); ++x)
		{
			meta::Object* gameObject = m_objects[x];
			if (gameObject)
			{
				//get class
				meta::Type objClass = gameObject->GetType();

				m_baseClass = objClass;
			}
		}
	}

	void ObjectPropertyNode::internalInitChildNodes(GuGuUtf8Str singleChildName /*= ""*/)
	{
		
		//auto& fields = meta::ReflectionDatabase::Instance().types[type.GetID()].fields;

		std::set<meta::Type> classesToConsider;
		for (int32_t i = 0; i < getNumObjects(); ++i)
		{
			meta::Object* tempObject = getObject(i);
			classesToConsider.insert(tempObject->GetType());
		}

		std::set<GuGuUtf8Str> categoriesFromProperties;
		std::vector<GuGuUtf8Str> sortedCategories;
		std::vector<GuGuUtf8Str> prioritizeCategories;

		//auto& fields = meta::ReflectionDatabase::Instance().types[m_baseClass.GetID()].fields;
		//for (auto& field : fields)
		//{
		//	getCategoryProperties(classesToConsider, field, categoriesFromProperties, sortedCategories);
		//}

		//todo:暂时写死，获取组件列表
		std::set<meta::Type> componentTypes;
		for (int32_t i = 0; i < getNumObjects(); ++i)
		{
			meta::Object* tempObject = getObject(i);

			componentTypes.insert(tempObject->GetType());
		}
		for (auto& type : componentTypes)
		{
			getCategoryProperties(classesToConsider, type, categoriesFromProperties, sortedCategories);
		}

		meta::Type gameObjectType = typeof(GameObject);
		if (m_baseClass == gameObjectType)
		{
			//展示目录
			for (const GuGuUtf8Str& fullCategoryPath : sortedCategories)
			{
				std::vector<GuGuUtf8Str> fullCategoryPathStrings;
				fullCategoryPathStrings.push_back(fullCategoryPath);

				std::shared_ptr<PropertyNode> parentLevelNode = shared_from_this();
				GuGuUtf8Str curCategoryPathString;
				for (int32_t pathLevelIndex = 0; pathLevelIndex < fullCategoryPathStrings.size(); ++pathLevelIndex)
				{
					if (curCategoryPathString.len() != 0)
						curCategoryPathString += "|";
					curCategoryPathString += fullCategoryPathStrings[pathLevelIndex];
					const GuGuUtf8Str categoryName = curCategoryPathString;

					std::shared_ptr<PropertyNode> foundCategory = findChildCategory(parentLevelNode, categoryName);

					if (!foundCategory)
					{
						std::shared_ptr<CategoryPropertyNode> newCategoryNode = std::make_shared<CategoryPropertyNode>();
						{
							newCategoryNode->setCategoryName(categoryName);
							PropertyNodeInitParams initParams;
							initParams.m_parentNode = parentLevelNode;
							initParams.m_property = nullptr;

							newCategoryNode->initNode(initParams);

							parentLevelNode->addChildNode(newCategoryNode);
						}
						parentLevelNode = newCategoryNode;//每个目录是下一个目录的儿子
					}
					else
					{
						parentLevelNode = foundCategory;
					}
				}
			}
		}
		else
		{
			std::vector<meta::Field*> sortedProperties;

			meta::Type structProperty = m_baseClass;
			if (structProperty.IsClass())
			{
				auto& fields = meta::ReflectionDatabase::Instance().types[structProperty.GetID()].fields;
				for (auto& field : fields)
				{
					sortedProperties.push_back(&field);
				}
			}

			//todo:排序

			for (meta::Field* curProp : sortedProperties)
			{
				std::shared_ptr<ItemPropertyNode> newItemNode = std::make_shared<ItemPropertyNode>();

				PropertyNodeInitParams initParams;
				initParams.m_parentNode = shared_from_this();
				initParams.m_property = curProp;

				newItemNode->initNode(initParams);

				addChildNode(newItemNode);
			}
		}
	}

	void ObjectPropertyNode::getCategoryProperties(const std::set<meta::Type>& classesToConsider, 
		const meta::Field currentProperty, 
		std::set<GuGuUtf8Str>& categoriesFromProperties, 
		std::vector<GuGuUtf8Str>& sortedCategories)
	{
		//todo:implement get category name
		GuGuUtf8Str categoryName = currentProperty.GetName();//暂时用组件名字

		sortedCategories.push_back(categoryName);
		categoriesFromProperties.insert(categoryName);

		//递归进行
		meta::Type structProperty = currentProperty.GetType();
		if (structProperty.IsClass())
		{
			auto& fields = meta::ReflectionDatabase::Instance().types[structProperty.GetID()].fields;
			for (auto& field : fields)
			{
				getCategoryProperties(classesToConsider, field, categoriesFromProperties, sortedCategories);
			}
		}
	}

	void ObjectPropertyNode::getCategoryProperties(const std::set<meta::Type>& classesToConsider,
		const meta::Type currentProperty,
		std::set<GuGuUtf8Str>& categoriesFromProperties,
		std::vector<GuGuUtf8Str>& sortedCategories)
	{
		//todo:implement get category name
		GuGuUtf8Str categoryName = currentProperty.GetName();//暂时用组件名字

		sortedCategories.push_back(categoryName);
		categoriesFromProperties.insert(categoryName);

		//递归进行
		//meta::Type structProperty = currentProperty;
		//if (structProperty.IsClass())
		//{
		//	auto& fields = meta::ReflectionDatabase::Instance().types[structProperty.GetID()].fields;
		//	for (auto& field : fields)
		//	{
		//		getCategoryProperties(classesToConsider, field, categoriesFromProperties, sortedCategories);
		//	}
		//}
	}

}