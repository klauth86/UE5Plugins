// Copyright Epic Games, Inc. All Rights Reserved.

#include "InputSMEditorModule.h"
#include "AssetToolsModule.h"
#include "ATActions/ATActions_InputSM.h"
#include "Graph/InputSMGraphNodeFactory.h"

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

	InputSMGraphNodeFactory = MakeShareable(new FInputSMGraphNodeFactory());
	FEdGraphUtilities::RegisterVisualNodeFactory(InputSMGraphNodeFactory);

	InputSMGraphPinFactory = MakeShareable(new FInputSMGraphPinFactory());
	FEdGraphUtilities::RegisterVisualPinFactory(InputSMGraphPinFactory);

	InputSMGraphPinConnectionFactory = MakeShareable(new FInputSMGraphPinConnectionFactory());
	FEdGraphUtilities::RegisterVisualPinConnectionFactory(InputSMGraphPinConnectionFactory);
}

void FInputSMEditorModule::ShutdownModule()
{
	FEdGraphUtilities::UnregisterVisualPinConnectionFactory(InputSMGraphPinConnectionFactory);

	FEdGraphUtilities::UnregisterVisualPinFactory(InputSMGraphPinFactory);

	FEdGraphUtilities::UnregisterVisualNodeFactory(InputSMGraphNodeFactory);

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