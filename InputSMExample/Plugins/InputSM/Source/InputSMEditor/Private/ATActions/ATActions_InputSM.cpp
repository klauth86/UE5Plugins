// Fill out your copyright notice in the Description page of Project Settings.

#include "ATActions/ATActions_InputSM.h"
#include "InputSM.h"
#include "InputSMEditor.h"

UClass* FATActions_InputSM::GetSupportedClass() const { return UInputSM::StaticClass(); }

void FATActions_InputSM::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (TArray<UObject*>::TConstIterator ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		if (UInputSM* inputSM = Cast<UInputSM>(*ObjIt))
		{
			TSharedRef<FInputSMEditor> NewEditor(new FInputSMEditor());
			NewEditor->InitEditor(Mode, EditWithinLevelEditor, inputSM);
		}
	}
}