// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EditorUndoClient.h"
#include "Toolkits/AssetEditorToolkit.h"

class UInputSequenceAsset;
class IDetailsView;

class FInputSequenceAssetEditor : public FEditorUndoClient, public FAssetEditorToolkit
{
public:

	static const FName AppIdentifier;
	static const FName DetailsTabId;
	static const FName GraphTabId;

	void InitAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, UInputSequenceAsset* inputSequenceAsset);

	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& TabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& TabManager) override;

	virtual FLinearColor GetWorldCentricTabColorScale() const override { return FLinearColor::White; }

	virtual FName GetToolkitFName() const override { return FName("InputSequenceAssetEditor"); }
	virtual FText GetBaseToolkitName() const override { return NSLOCTEXT("FInputSequenceAssetEditor", "BaseToolkitName", "Input Sequence Asset Editor"); }
	virtual FString GetWorldCentricTabPrefix() const override { return "InputSequenceAssetEditor"; }

protected:

	TSharedRef<SDockTab> SpawnTab_DetailsTab(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnTab_GraphTab(const FSpawnTabArgs& Args);

	void CreateCommandList();

	void OnSelectionChanged(const TSet<UObject*>& selectedNodes);

protected:

	UInputSequenceAsset* InputSequenceAsset;

	TSharedPtr<FUICommandList> GraphEditorCommands;

	TWeakPtr<SGraphEditor> GraphEditorPtr;

	TSharedPtr<IDetailsView> DetailsView;
};