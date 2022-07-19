// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FAssetTypeActions_Base;

class FInputSequenceEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

protected:

	void RegisterCustomPropertyTypeLayout(FName ClassName, FOnGetPropertyTypeCustomizationInstance DetailLayoutDelegate);

private:
	TArray<FName> RegisteredPTNames;
	TArray<TSharedPtr<FAssetTypeActions_Base>> RegisteredAssetTypeActions;
	TSharedPtr<FInputSequenceGraphNodeFactory> InputSequenceGraphNodeFactory;
	TSharedPtr<FInputSequenceGraphPinFactory> InputSequenceGraphPinFactory;
};