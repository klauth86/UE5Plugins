// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EditorUndoClient.h"
#include "Toolkits/AssetEditorToolkit.h"

class UInputSequenceAsset;

class FInputSequenceAssetEditor : public FEditorUndoClient, public FAssetEditorToolkit
{
public:

	static const FName AppIdentifier;

	void InitEditor(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, UInputSequenceAsset* inputSequenceAsset);

	virtual FLinearColor GetWorldCentricTabColorScale() const override { return FLinearColor::White; }

	virtual FName GetToolkitFName() const override { return FName("InputSequenceAssetEditor"); }
	virtual FText GetBaseToolkitName() const override { return NSLOCTEXT("FInputSequenceAssetEditor", "BaseToolkitName", "Input Sequence Asset Editor"); }
	virtual FString GetWorldCentricTabPrefix() const override { return "InputSequenceAssetEditor"; }

protected:

	UInputSequenceAsset* InputSequenceAsset;
};