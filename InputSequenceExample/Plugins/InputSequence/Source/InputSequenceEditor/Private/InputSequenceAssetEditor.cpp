// Fill out your copyright notice in the Description page of Project Settings.

#include "InputSequenceAssetEditor.h"

const FName FInputSequenceAssetEditor::AppIdentifier(TEXT("FInputSequenceAssetEditor"));

void FInputSequenceAssetEditor::InitEditor(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, UInputSequenceAsset* inputSequenceAsset)
{
	check(inputSequenceAsset != NULL);

	InputSequenceAsset = inputSequenceAsset;

	InputSequenceAsset->SetFlags(RF_Transactional);

	TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout("FInputSequenceAssetEditor_StandaloneDefaultLayout")
		->AddArea
		(
			FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
		);

	FAssetEditorToolkit::InitAssetEditor(Mode, InitToolkitHost, AppIdentifier, StandaloneDefaultLayout, true, true, InputSequenceAsset);
}