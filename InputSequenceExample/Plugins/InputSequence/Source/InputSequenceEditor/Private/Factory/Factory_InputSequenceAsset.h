// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Factories/Factory.h"
#include "Factory_InputSequenceAsset.generated.h"

UCLASS()
class UFactory_InputSequenceAsset : public UFactory
{
	GENERATED_UCLASS_BODY()

public:

	virtual FText GetDisplayName() const override { return NSLOCTEXT("UFactory_InputSequenceAsset", "DisplayName", "Input Sequence Asset"); }
	virtual uint32 GetMenuCategories() const override;
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext) override;
};