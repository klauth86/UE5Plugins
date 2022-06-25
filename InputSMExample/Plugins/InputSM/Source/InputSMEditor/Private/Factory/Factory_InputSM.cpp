// Fill out your copyright notice in the Description page of Project Settings.

#include "Factory/Factory_InputSM.h"
#include "InputSM.h"
#include "AssetTypeCategories.h"

UFactory_InputSM::UFactory_InputSM(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UInputSM::StaticClass();
}

uint32 UFactory_InputSM::GetMenuCategories() const { return EAssetTypeCategories::Misc; }

UObject* UFactory_InputSM::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	return NewObject<UInputSM>(InParent, InClass, InName, Flags);
}