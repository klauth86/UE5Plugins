// Copyright Epic Games, Inc. All Rights Reserved.

#include "InputSMEditorModule.h"
#include "AssetToolsModule.h"
#include "ATActions/ATActions_InputSM.h"

#define LOCTEXT_NAMESPACE "FInputSMEditorModule"

const FName AssetToolsModuleName("AssetTools");

void FInputSMEditorModule::StartupModule()
{
	RegisteredAssetTypeActions.Add(MakeShared<FATActions_InputSM>());

	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>(AssetToolsModuleName).Get();
	for (TSharedPtr<FAssetTypeActions_Base>& registeredAssetTypeAction : RegisteredAssetTypeActions)
	{
		if (registeredAssetTypeAction.IsValid())
		{
			AssetTools.RegisterAssetTypeActions(registeredAssetTypeAction.ToSharedRef());
		}
	}
}

void FInputSMEditorModule::ShutdownModule()
{
	if (FModuleManager::Get().IsModuleLoaded(AssetToolsModuleName))
	{
		IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>(AssetToolsModuleName).Get();
		for (TSharedPtr<FAssetTypeActions_Base>& registeredAssetTypeAction : RegisteredAssetTypeActions)
		{
			if (registeredAssetTypeAction.IsValid())
			{
				AssetTools.UnregisterAssetTypeActions(registeredAssetTypeAction.ToSharedRef());
				registeredAssetTypeAction.Reset();
			}
		}
	}

	RegisteredAssetTypeActions.Empty();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FInputSMEditorModule, InputSMEditor)