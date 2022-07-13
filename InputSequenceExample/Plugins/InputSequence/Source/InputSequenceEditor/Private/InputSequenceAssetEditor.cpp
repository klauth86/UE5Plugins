// Fill out your copyright notice in the Description page of Project Settings.

#include "InputSequenceAssetEditor.h"
#include "InputSequenceAsset.h"
#include "Graph/InputSequenceGraph.h"

#define LOCTEXT_NAMESPACE "FInputSequenceAssetEditor"

const FName FInputSequenceAssetEditor::AppIdentifier(TEXT("FInputSequenceAssetEditor_AppIdentifier"));
const FName FInputSequenceAssetEditor::GraphTabId(TEXT("FInputSequenceAssetEditor_GraphTab_Id"));

void FInputSequenceAssetEditor::InitAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, UInputSequenceAsset* inputSequenceAsset)
{
	check(inputSequenceAsset != NULL);

	InputSequenceAsset = inputSequenceAsset;

	InputSequenceAsset->SetFlags(RF_Transactional);

	TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout("FInputSequenceAssetEditor_StandaloneDefaultLayout")
		->AddArea
		(
			FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.1f)
				->SetHideTabWell(true)
			)
			->Split
			(
				FTabManager::NewStack()
				->AddTab(GraphTabId, ETabState::OpenedTab)
			)
		);

	FAssetEditorToolkit::InitAssetEditor(Mode, InitToolkitHost, AppIdentifier, StandaloneDefaultLayout, true, true, InputSequenceAsset);
}

void FInputSequenceAssetEditor::RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenuCategory", "Input Sequence Editor"));
	TSharedRef<FWorkspaceItem> WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	InTabManager->RegisterTabSpawner(GraphTabId, FOnSpawnTab::CreateSP(this, &FInputSequenceAssetEditor::SpawnTab_GraphTab))
		.SetDisplayName(LOCTEXT("FInputSequenceAssetEditor_GraphTab_DisplayName", "Graph"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "GraphEditor.EventGraph_16x"));
}

void FInputSequenceAssetEditor::UnregisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);

	InTabManager->UnregisterTabSpawner(GraphTabId);
}

TSharedRef<SDockTab> FInputSequenceAssetEditor::SpawnTab_GraphTab(const FSpawnTabArgs& Args)
{
	check(InputSequenceAsset != NULL);

	check(Args.GetTabId().TabType == GraphTabId);

	if (InputSequenceAsset->EdGraph == NULL)
	{
		InputSequenceAsset->EdGraph = NewObject<UInputSequenceGraph>(InputSequenceAsset, NAME_None, RF_Transactional);
	}

	check(InputSequenceAsset->EdGraph != NULL);

	FGraphAppearanceInfo AppearanceInfo;
	AppearanceInfo.CornerText = LOCTEXT("FInputSequenceAssetEditor_GraphTab_AppearanceInfo_CornerText", "Input Sequence Asset");

	CreateCommandList();

	TSharedRef<SWidget> TitleBarWidget =
		SNew(SBorder).BorderImage(FEditorStyle::GetBrush(TEXT("Graph.TitleBackground")))
		.HAlign(HAlign_Fill)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Center)
			.FillWidth(1.f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("TheQueryGraphLabel", "Input State Machine Graph"))
				.TextStyle(FEditorStyle::Get(), TEXT("GraphBreadcrumbButtonText"))
			]
		];

	return SNew(SDockTab)
		.Label(LOCTEXT("UpdateGraph", "Update Graph"))
		.TabColorScale(GetTabColorScale())
		[
			SAssignNew(GraphEditorPtr, SGraphEditor)
			.AdditionalCommands(GraphEditorCommands)
			.Appearance(AppearanceInfo)
			.TitleBar(TitleBarWidget)
			.GraphToEdit(InputSequenceAsset->EdGraph)
		];
}

void FInputSequenceAssetEditor::CreateCommandList()
{
	if (GraphEditorCommands.IsValid()) return;

	GraphEditorCommands = MakeShareable(new FUICommandList);
}

#undef LOCTEXT_NAMESPACE