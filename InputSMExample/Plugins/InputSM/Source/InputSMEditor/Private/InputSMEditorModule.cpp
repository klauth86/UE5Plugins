// Copyright Epic Games, Inc. All Rights Reserved.

#include "InputSMEditorModule.h"
#include "AssetToolsModule.h"
#include "ATActions/ATActions_InputSM.h"
#include "Graph/InputSMGraphNodeFactory.h"
#include "PropertyEditorModule.h"
#include "InputSM.h"
#include "Details/PTCustomization_IFS.h"

#define LOCTEXT_NAMESPACE "FInputSMEditorModule"

const FName AssetToolsModuleName("AssetTools");

const FName PropertyEditorModuleName("PropertyEditor");

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

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>(PropertyEditorModuleName);
	PropertyModule.RegisterCustomPropertyTypeLayout(FInputFrameStack::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FPTCustomization_IFS::MakeInstance));
}

void FInputSMEditorModule::ShutdownModule()
{
	if (FModuleManager::Get().IsModuleLoaded(PropertyEditorModuleName))
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>(PropertyEditorModuleName);
		PropertyModule.UnregisterCustomPropertyTypeLayout(FInputFrameStack::StaticStruct()->GetFName());
	}

	FEdGraphUtilities::UnregisterVisualPinConnectionFactory(InputSMGraphPinConnectionFactory);
	InputSMGraphPinConnectionFactory.Reset();

	FEdGraphUtilities::UnregisterVisualPinFactory(InputSMGraphPinFactory);
	InputSMGraphPinFactory.Reset();

	FEdGraphUtilities::UnregisterVisualNodeFactory(InputSMGraphNodeFactory);
	InputSMGraphNodeFactory.Reset();

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