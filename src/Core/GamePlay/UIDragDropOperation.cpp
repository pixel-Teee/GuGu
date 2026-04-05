#include <pch.h>

#include "UIDragDropOperation.h"
#include <Core/Reflection/TypeInfo.h>
#include <Core/GamePlay/GamePlayerReflectionRegister.h>

namespace GuGu {
	static bool registerGuGuUIDragDropOperation()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::UIDragDropOperation");
		auto& type = db.types[id];
		meta::TypeInfo<UIDragDropOperation>::Register(id, type, true, "2195C5B8-9CD7-4E34-B995-92BF6C5958EB");

		auto typeID = typeidof(UIDragDropOperation);
		if (typeID != meta::InvalidTypeID && !meta::TypeInfo<UIDragDropOperation>::Defined)
		{
			auto& type = db.types[typeID];

			//array constructor
			type.SetArrayConstructor<UIDragDropOperation>();

			type.AddConstructor<UIDragDropOperation, false, false>({});

			type.AddConstructor<UIDragDropOperation, true, true>({});

			type.LoadBaseClasses(db, typeID, { typeof(meta::Object) });

			meta::TypeInfo<UIDragDropOperation>::Defined = true;
		}

		{
			auto id = db.AllocateType("std::shared_ptr<GuGu::UIDragDropOperation>");
			auto& type = db.types[id];
			meta::TypeInfo<std::shared_ptr<UIDragDropOperation>>::Register(id, type, false, "6822FC64-AB02-445A-8643-139793B8A112");
		}

		{
			auto id = db.AllocateType("std::weak_ptr<GuGu::UIDragDropOperation>");
			auto& type = db.types[id];
			meta::TypeInfo<std::weak_ptr<UIDragDropOperation>>::Register(id, type, false, "639955DD-E696-47A3-BCC2-605790FD46A8");
		}
		return true;
	}

	static bool registerGuGuUIDragDropOperationFields()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto& type = db.types[typeof(UIDragDropOperation).GetID()];

		return true;
	}
	IMPLEMENT_INITIAL_BEGIN(UIDragDropOperation)
		ADD_PRIORITY(meta::Object)
		ADD_INITIAL_FUNCTION_WITH_PRIORITY(registerGuGuUIDragDropOperation)
	IMPLEMENT_INITIAL_END

	IMPLEMENT_INITIAL_FIELDS_BEGIN(UIDragDropOperation)
		ADD_INITIAL_FIELDS_FUNCTION_WITH_PRIORITY(registerGuGuUIDragDropOperationFields)
	IMPLEMENT_INITIAL_FIELDS_END
	UIDragDropOperation::UIDragDropOperation()
	{
	}
	UIDragDropOperation::~UIDragDropOperation()
	{
	}
	meta::Object* UIDragDropOperation::Clone(void) const
	{
		UIDragDropOperation* dragDropOperation = new UIDragDropOperation();
		return dragDropOperation;
	}
	void UIDragDropOperation::Update(float fElapsedTimeSeconds)
	{
		/*
            nothing
        */
	}
	meta::Type UIDragDropOperation::GetType() const
	{
		return meta::Type(typeof(UIDragDropOperation));
	}
    
    void UIDragDropOperation::PostLoad()
    {
    }
}