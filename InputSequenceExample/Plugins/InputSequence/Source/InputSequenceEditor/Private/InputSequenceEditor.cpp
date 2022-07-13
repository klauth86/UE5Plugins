// Copyright Epic Games, Inc. All Rights Reserved.

#include "InputSequenceEditor.h"
#include "AssetToolsModule.h"
#include "AssetTypeActions/AssetTypeActions_InputSequenceAsset.h"
#include "Graph/InputSequenceGraphFactories.h"
#include "Graph/PTC_InputFrame.h"

#define LOCTEXT_NAMESPACE "FInputSequenceEditorModule"

const FName AssetToolsModuleName("AssetTools");
const FName PropertyEditorModuleName("PropertyEditor");

void FInputSequenceEditorModule::StartupModule()
{
	RegisterCustomPropertyTypeLayout("InputFrame", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FPTC_InputFrame::MakeInstance));

	RegisteredAssetTypeActions.Add(MakeShared<FAssetTypeActions_InputSequenceAsset>());

	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>(AssetToolsModuleName).Get();
	for (TSharedPtr<FAssetTypeActions_Base>& registeredAssetTypeAction : RegisteredAssetTypeActions)
	{
		if (registeredAssetTypeAction.IsValid()) AssetTools.RegisterAssetTypeActions(registeredAssetTypeAction.ToSharedRef());
	}

	InputSequenceGraphPinFactory = MakeShareable(new FInputSequenceGraphPinFactory());
	FEdGraphUtilities::RegisterVisualPinFactory(InputSequenceGraphPinFactory);
}

void FInputSequenceEditorModule::ShutdownModule()
{
	FEdGraphUtilities::UnregisterVisualPinFactory(InputSequenceGraphPinFactory);
	InputSequenceGraphPinFactory.Reset();

	if (FModuleManager::Get().IsModuleLoaded(AssetToolsModuleName))
	{
		IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>(AssetToolsModuleName).Get();
		for (TSharedPtr<FAssetTypeActions_Base>& registeredAssetTypeAction : RegisteredAssetTypeActions)
		{
			if (registeredAssetTypeAction.IsValid()) AssetTools.UnregisterAssetTypeActions(registeredAssetTypeAction.ToSharedRef());
		}
	}

	RegisteredAssetTypeActions.Empty();

	FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(PropertyEditorModuleName);
	for (FName& registeredPTName : RegisteredPTNames) PropertyModule.UnregisterCustomClassLayout(registeredPTName);
}

void FInputSequenceEditorModule::RegisterCustomPropertyTypeLayout(FName PTName, FOnGetPropertyTypeCustomizationInstance DetailLayoutDelegate)
{
	check(PTName != NAME_None);

	RegisteredPTNames.Add(PTName);

	FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(PropertyEditorModuleName);
	PropertyModule.RegisterCustomPropertyTypeLayout(PTName, DetailLayoutDelegate);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FInputSequenceEditorModule, InputSequenceEditor)