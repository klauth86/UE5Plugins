// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Factories/Factory.h"
#include "Factory_InputSM.generated.h"

UCLASS()
class UFactory_InputSM : public UFactory
{
	GENERATED_UCLASS_BODY()

public:

	virtual FText GetDisplayName() const override { return NSLOCTEXT("UFactory_InputSM", "DisplayName", "Input State Machine"); }
	virtual uint32 GetMenuCategories() const override;
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext) override;
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override
	{
		return FactoryCreateNew(Class, InParent, Name, Flags, Context, Warn, NAME_None);
	}
};