// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EditorUndoClient.h"
#include "GraphEditor.h"
#include "Toolkits/AssetEditorToolkit.h"

class UInputSM;

class FInputSMEditor : public FEditorUndoClient, public FAssetEditorToolkit
{
public:

	static const FName InputSMAppIdentifier;

	static const FName InputSMPropertiesTabId;
	static const FName InputSMGraphTabId;

	virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;

	void InitEditor(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, UInputSM* inputSM);

	//~ Begin IToolkit Interface
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	//~ End IToolkit Interface

private:

	TSharedRef<SDockTab> SpawnTab_Properties(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnTab_Graph(const FSpawnTabArgs& Args);

	UInputSM* InputSM;

	TSharedPtr<class IDetailsView> DetailsView;

	uint32 SelectedNodesCount;

public:

	FInputSMEditor();
	virtual ~FInputSMEditor();

	FGraphPanelSelectionSet GetSelectedNodes() const;
	void OnSelectionChanged(const TSet<UObject*>& selectedNodes);

	//~ Begin FEditorUndoClient Interface
	virtual void PostUndo(bool bSuccess) override;
	virtual void PostRedo(bool bSuccess) override;
	// End of FEditorUndoClient

	void CreateCommandList();

	// Delegates for graph editor commands
	void SelectAllNodes();
	bool CanSelectAllNodes() const { return true; }
	void DeleteSelectedNodes();
	bool CanDeleteSelectedNodes() const;
	void CutSelectedNodes();
	bool CanCutSelectedNodes() const { return CanCopySelectedNodesToClipboard() && CanDeleteSelectedNodes(); }
	void CopySelectedNodesToClipboard();
	bool CanCopySelectedNodesToClipboard() const;
	void PasteNodes();
	void PasteNodesHere(const FVector2D& Location);
	bool CanPasteNodes() const;
	void DuplicateNodes();
	bool CanDuplicateNodes() const { return CanCopySelectedNodesToClipboard(); }

	bool CanCreateComment() const;
	void OnCreateComment();

protected:

	/** Currently focused graph */
	TWeakPtr<SGraphEditor> GraphEditorPtr;

	/** The command list for this editor */
	TSharedPtr<FUICommandList> GraphEditorCommands;

	/** Handle to the registered OnClassListUpdated delegate */
	FDelegateHandle OnClassListUpdatedDelegateHandle;
};