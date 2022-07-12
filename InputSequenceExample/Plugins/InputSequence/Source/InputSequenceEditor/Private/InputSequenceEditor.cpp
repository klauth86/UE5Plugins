// Copyright Epic Games, Inc. All Rights Reserved.

#include "InputSequenceEditor.h"
#include "AssetToolsModule.h"
#include "AssetTypeActions/AssetTypeActions_InputSequenceAsset.h"

#define LOCTEXT_NAMESPACE "FInputSequenceEditorModule"

const FName AssetToolsModuleName("AssetTools");

void FInputSequenceEditorModule::StartupModule()
{
	RegisteredAssetTypeActions.Add(MakeShared<FAssetTypeActions_InputSequenceAsset>());

	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>(AssetToolsModuleName).Get();
	for (TSharedPtr<FAssetTypeActions_Base>& registeredAssetTypeAction : RegisteredAssetTypeActions)
	{
		if (registeredAssetTypeAction.IsValid())
		{
			AssetTools.RegisterAssetTypeActions(registeredAssetTypeAction.ToSharedRef());
		}
	}
}

void FInputSequenceEditorModule::ShutdownModule()
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
	
IMPLEMENT_MODULE(FInputSequenceEditorModule, InputSequenceEditor)