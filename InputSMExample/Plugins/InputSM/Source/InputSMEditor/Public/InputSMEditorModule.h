// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

struct FInputSMGraphNodeFactory;
struct FInputSMGraphPinFactory;
struct FInputSMGraphPinConnectionFactory;

class FInputSMEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	TArray<TSharedPtr<FAssetTypeActions_Base>> RegisteredAssetTypeActions;

	TSharedPtr<FInputSMGraphNodeFactory> InputSMGraphNodeFactory;
	TSharedPtr<FInputSMGraphPinFactory> InputSMGraphPinFactory;
	TSharedPtr<FInputSMGraphPinConnectionFactory> InputSMGraphPinConnectionFactory;
};