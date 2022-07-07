#include "InputSMEditor.h"
#include "Framework/Application/SlateApplication.h"
#include "Editor/EditorEngine.h"
#include "EngineGlobals.h"
#include "Graph/InputSMGraph.h"
#include "Graph/InputSMGraphNode_Base.h"
#include "ScopedTransaction.h"
#include "EdGraphUtilities.h"
#include "Framework/Commands/GenericCommands.h"
#include "GraphEditorActions.h"
#include "HAL/PlatformApplicationMisc.h"
#include "EdGraph/EdGraphSchema.h"

#define LOCTEXT_NAMESPACE "FInputSMEditor"

const FName FInputSMEditor::InputSMAppIdentifier(TEXT("InputSMApp"));

const FName FInputSMEditor::InputSMPropertiesTabId(TEXT("InputSM_PropertiesTab"));

const FName FInputSMEditor::InputSMGraphTabId(TEXT("InputSM_GraphTab"));

void FInputSMEditor::RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu", "Input State Machine Editor"));
	TSharedRef<FWorkspaceItem> WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	InTabManager->RegisterTabSpawner(InputSMPropertiesTabId, FOnSpawnTab::CreateSP(this, &FInputSMEditor::SpawnTab_Properties))
		.SetDisplayName(LOCTEXT("PropertiesTab", "Details"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Details"));

	InTabManager->RegisterTabSpawner(InputSMGraphTabId, FOnSpawnTab::CreateSP(this, &FInputSMEditor::SpawnTab_Graph))
		.SetDisplayName(LOCTEXT("Graph", "Graph"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "GraphEditor.EventGraph_16x"));
}

void FInputSMEditor::UnregisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);

	InTabManager->UnregisterTabSpawner(InputSMPropertiesTabId);
	InTabManager->UnregisterTabSpawner(InputSMGraphTabId);
}

void FInputSMEditor::InitEditor(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, UInputSM* inputSM)
{
	InputSM = inputSM;
	
	check(InputSM != NULL);

	TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout("Standalone_InputSM_Layout")
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
				FTabManager::NewSplitter()->SetOrientation(Orient_Horizontal)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.35f)
					->AddTab(InputSMPropertiesTabId, ETabState::OpenedTab)
				)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.65f)
					->AddTab(InputSMGraphTabId, ETabState::OpenedTab)
				)
			)
		);

	FAssetEditorToolkit::InitAssetEditor(Mode, InitToolkitHost, InputSMAppIdentifier, StandaloneDefaultLayout, true, true, InputSM);
}

FName FInputSMEditor::GetToolkitFName() const { return FName("InputSMEditor"); }

FText FInputSMEditor::GetBaseToolkitName() const { return LOCTEXT("BaseToolkitName", "Input State Machine Editor"); }

FString FInputSMEditor::GetWorldCentricTabPrefix() const { return LOCTEXT("WorldCentricTabPrefix", "InputSMEditor").ToString(); }

FLinearColor FInputSMEditor::GetWorldCentricTabColorScale() const { return FLinearColor(0.0f, 0.0f, 0.2f, 0.5f); }

void FInputSMEditor::SaveAsset_Execute()
{
	TSharedPtr<SGraphEditor> graphEditor = GraphEditorPtr.Pin();
	if (graphEditor.IsValid())
	{

	}

	FAssetEditorToolkit::SaveAsset_Execute();
}

void FInputSMEditor::CreateInternalWidgets()
{
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs DetailsViewArgs = FDetailsViewArgs();
	DetailsViewArgs.bUpdatesFromSelection = false;
	DetailsViewArgs.bLockable = false;
	DetailsViewArgs.bAllowSearch = false;
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
	DetailsViewArgs.bHideSelectionTip = true;
	DetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	DetailsView->SetObject(NULL);
}

TSharedRef<SDockTab> FInputSMEditor::SpawnTab_Properties(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == InputSMPropertiesTabId);

	CreateInternalWidgets();

	TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab)
		.Label(LOCTEXT("PropertiesTab", "Details"))
		[
			DetailsView.ToSharedRef()
		];

	return SpawnedTab;
}

TSharedRef<SDockTab> FInputSMEditor::SpawnTab_Graph(const FSpawnTabArgs& Args)
{
	check(InputSM != NULL);

	check(Args.GetTabId().TabType == InputSMGraphTabId);

	if (InputSM->EdGraph == NULL)
	{
		InputSM->EdGraph = NewObject<UInputSMGraph>(InputSM, NAME_None, RF_Transactional);

		const UEdGraphSchema* Schema = InputSM->EdGraph->GetSchema();
		Schema->CreateDefaultNodesForGraph(*InputSM->EdGraph);

		SaveAsset_Execute();
	}

	check(InputSM->EdGraph != NULL);

	// Create the appearance info
	FGraphAppearanceInfo AppearanceInfo;
	AppearanceInfo.CornerText = LOCTEXT("AppearanceCornerText", "Input State Machine");

	SGraphEditor::FGraphEditorEvents InEvents;
	InEvents.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP(this, &FInputSMEditor::OnSelectedNodesChanged);

	CreateCommandList();

	// Make title bar
	TSharedRef<SWidget> TitleBarWidget =
		SNew(SBorder)
		.BorderImage(FEditorStyle::GetBrush(TEXT("Graph.TitleBackground")))
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
			.GraphToEdit(InputSM->EdGraph)
			.GraphEvents(InEvents)
		];
}

FInputSMEditor::FInputSMEditor()
{
	if (UEditorEngine* Editor = (UEditorEngine*)GEngine)
	{
		Editor->RegisterForUndo(this);
	}
}

FInputSMEditor::~FInputSMEditor()
{
	if (UEditorEngine* Editor = (UEditorEngine*)GEngine)
	{
		Editor->UnregisterForUndo(this);
	}
}

void FInputSMEditor::CreateCommandList()
{
	if (GraphEditorCommands.IsValid())
	{
		return;
	}

	GraphEditorCommands = MakeShareable(new FUICommandList);

	// Can't use CreateSP here because derived editor are already implementing TSharedFromThis<FAssetEditorToolkit>
	// however it should be safe, since commands are being used only within this editor
	// if it ever crashes, this function will have to go away and be reimplemented in each derived class

	GraphEditorCommands->MapAction(FGenericCommands::Get().SelectAll,
		FExecuteAction::CreateRaw(this, &FInputSMEditor::SelectAllNodes),
		FCanExecuteAction::CreateRaw(this, &FInputSMEditor::CanSelectAllNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Delete,
		FExecuteAction::CreateRaw(this, &FInputSMEditor::DeleteSelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FInputSMEditor::CanDeleteNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Copy,
		FExecuteAction::CreateRaw(this, &FInputSMEditor::CopySelectedNodesToClipboard),
		FCanExecuteAction::CreateRaw(this, &FInputSMEditor::CanCopyNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Cut,
		FExecuteAction::CreateRaw(this, &FInputSMEditor::CutSelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FInputSMEditor::CanCutNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Paste,
		FExecuteAction::CreateRaw(this, &FInputSMEditor::PasteNodes),
		FCanExecuteAction::CreateRaw(this, &FInputSMEditor::CanPasteNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Duplicate,
		FExecuteAction::CreateRaw(this, &FInputSMEditor::DuplicateNodes),
		FCanExecuteAction::CreateRaw(this, &FInputSMEditor::CanDuplicateNodes)
	);

	GraphEditorCommands->MapAction(FGraphEditorCommands::Get().CreateComment,
		FExecuteAction::CreateRaw(this, &FInputSMEditor::OnCreateComment),
		FCanExecuteAction::CreateRaw(this, &FInputSMEditor::CanCreateComment)
	);
}

FGraphPanelSelectionSet FInputSMEditor::GetSelectedNodes() const
{
	FGraphPanelSelectionSet CurrentSelection;
	
	if (TSharedPtr<SGraphEditor> graphEditor = GraphEditorPtr.Pin())
	{
		if (graphEditor.IsValid()) CurrentSelection = graphEditor->GetSelectedNodes();
	}

	return CurrentSelection;
}

void FInputSMEditor::OnSelectedNodesChanged(const TSet<class UObject*>& NewSelection)
{
	TArray<UObject*> Selection;

	if (NewSelection.Num())
	{
		for (TSet<class UObject*>::TConstIterator SetIt(NewSelection); SetIt; ++SetIt)
		{
			if (UInputSMGraphNode_Base* GraphNode = Cast<UInputSMGraphNode_Base>(*SetIt))
			{
				Selection.Add(GraphNode);
			}
		}
	}

	return Selection.Num() == 1 ? DetailsView->SetObjects(Selection) : DetailsView->SetObject(NULL);
}

void FInputSMEditor::PostUndo(bool bSuccess)
{
	if (bSuccess)
	{
		// Clear selection, to avoid holding refs to nodes that go away
		if (TSharedPtr<SGraphEditor> graphEditor = GraphEditorPtr.Pin())
		{
			if (graphEditor.IsValid())
			{
				graphEditor->ClearSelectionSet();
				graphEditor->NotifyGraphChanged();
			}
		}

		FSlateApplication::Get().DismissAllMenus();
	}
}

void FInputSMEditor::PostRedo(bool bSuccess)
{
	if (bSuccess)
	{
		// Clear selection, to avoid holding refs to nodes that go away
		if (TSharedPtr<SGraphEditor> graphEditor = GraphEditorPtr.Pin())
		{
			if (graphEditor.IsValid())
			{
				graphEditor->ClearSelectionSet();
				graphEditor->NotifyGraphChanged();
			}
		}

		FSlateApplication::Get().DismissAllMenus();
	}
}

void FInputSMEditor::SelectAllNodes()
{
	if (TSharedPtr<SGraphEditor> graphEditor = GraphEditorPtr.Pin())
	{
		if (graphEditor.IsValid()) graphEditor->SelectAllNodes();
	}
}

void FInputSMEditor::DeleteSelectedNodes()
{
	TSharedPtr<SGraphEditor> graphEditor = GraphEditorPtr.Pin();
	
	if (!graphEditor.IsValid()) return;

	const FScopedTransaction Transaction(FGenericCommands::Get().Delete->GetDescription());

	UEdGraph* currentGraph = graphEditor->GetCurrentGraph();

	currentGraph->Modify();

	const FGraphPanelSelectionSet SelectedNodes = graphEditor->GetSelectedNodes();

	graphEditor->ClearSelectionSet();

	const UEdGraphSchema* currentSchema = currentGraph->GetSchema();

	for (FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes); NodeIt; ++NodeIt)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*NodeIt);

		if (Node && Node->CanUserDeleteNode())
		{
			Node->Modify();
			currentSchema->BreakNodeLinks(*Node);
			Node->DestroyNode();
		}
	}
}

bool FInputSMEditor::CanDeleteNodes() const
{
	const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();
	
	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		
		if (Node && Node->CanUserDeleteNode()) return true;
	}

	return false;
}

void FInputSMEditor::CutSelectedNodes()
{
	TSharedPtr<SGraphEditor> graphEditor = GraphEditorPtr.Pin();
	if (!graphEditor.IsValid()) return;

	CopySelectedNodesToClipboard();

	const FGraphPanelSelectionSet OldSelectedNodes = graphEditor->GetSelectedNodes();

	graphEditor->ClearSelectionSet();

	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(OldSelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		if (Node && Node->CanDuplicateNode())
		{
			graphEditor->SetNodeSelection(Node, true);
		}
	}

	DeleteSelectedNodes();

	graphEditor->ClearSelectionSet();

	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(OldSelectedNodes); SelectedIter; ++SelectedIter)
	{
		if (UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter))
		{
			graphEditor->SetNodeSelection(Node, true);
		}
	}
}

void FInputSMEditor::CopySelectedNodesToClipboard()
{
	FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();

	for (FGraphPanelSelectionSet::TIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{		
		if (UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter)) 
			Node->PrepareForCopying();
		else
			SelectedIter.RemoveCurrent();
	}

	FString ExportedText;

	FEdGraphUtilities::ExportNodesToText(SelectedNodes, ExportedText);
	
	FPlatformApplicationMisc::ClipboardCopy(*ExportedText);
}

bool FInputSMEditor::CanCopyNodes() const
{
	const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();

	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);

		if (Node && Node->CanDuplicateNode()) return true;
	}

	return false;
}

void FInputSMEditor::PasteNodes()
{
	TSharedPtr<SGraphEditor> graphEditor = GraphEditorPtr.Pin();
	if (graphEditor.IsValid())
	{
		PasteNodesHere(graphEditor->GetPasteLocation());
	}
}

void FInputSMEditor::PasteNodesHere(const FVector2D& Location)
{
	TSharedPtr<SGraphEditor> graphEditor = GraphEditorPtr.Pin();
	
	if (!graphEditor.IsValid()) return;

	UEdGraph* graph = graphEditor->GetCurrentGraph();

	const FScopedTransaction Transaction(FGenericCommands::Get().Paste->GetDescription());

	graph->Modify();

	graphEditor->ClearSelectionSet();

	FString TextToImport;
	
	FPlatformApplicationMisc::ClipboardPaste(TextToImport);

	TSet<UEdGraphNode*> PastedNodes;
	
	FEdGraphUtilities::ImportNodesFromText(graph, TextToImport, /*out*/ PastedNodes);

	FVector2D AvgNodePosition(0.0f, 0.0f);

	int32 AvgCount = 0;

	for (TSet<UEdGraphNode*>::TIterator It(PastedNodes); It; ++It)
	{
		UEdGraphNode* EdNode = *It;
		UInputSMGraphNode_Base* graphNode = Cast<UInputSMGraphNode_Base>(EdNode);
		if (EdNode)
		{
			AvgNodePosition.X += EdNode->NodePosX;
			AvgNodePosition.Y += EdNode->NodePosY;
			++AvgCount;
		}
	}

	if (AvgCount > 0)
	{
		float InvNumNodes = 1.0f / float(AvgCount);
		AvgNodePosition.X *= InvNumNodes;
		AvgNodePosition.Y *= InvNumNodes;
	}

	for (TSet<UEdGraphNode*>::TIterator It(PastedNodes); It; ++It)
	{
		if (UEdGraphNode* PasteNode = *It)
		{
			graphEditor->SetNodeSelection(PasteNode, true);

			PasteNode->NodePosX = (PasteNode->NodePosX - AvgNodePosition.X) + Location.X;
			PasteNode->NodePosY = (PasteNode->NodePosY - AvgNodePosition.Y) + Location.Y;
			PasteNode->SnapToGrid(16);
			PasteNode->CreateNewGuid();
		}
	}

	graphEditor->NotifyGraphChanged();

	if (UObject* GraphOwner = graph->GetOuter())
	{
		GraphOwner->PostEditChange();
		GraphOwner->MarkPackageDirty();
	}
}

bool FInputSMEditor::CanPasteNodes() const
{
	TSharedPtr<SGraphEditor> graphEditor = GraphEditorPtr.Pin();
	
	if (!graphEditor.IsValid()) return false;

	FString ClipboardContent;
	
	FPlatformApplicationMisc::ClipboardPaste(ClipboardContent);

	return FEdGraphUtilities::CanImportNodesFromText(graphEditor->GetCurrentGraph(), ClipboardContent);
}

void FInputSMEditor::DuplicateNodes()
{
	CopySelectedNodesToClipboard();
	PasteNodes();
}

bool FInputSMEditor::CanCreateComment() const
{
	TSharedPtr<SGraphEditor> graphEditor = GraphEditorPtr.Pin();
	return graphEditor.IsValid() ? (graphEditor->GetNumberOfSelectedNodes() != 0) : false;
}

void FInputSMEditor::OnCreateComment()
{
	TSharedPtr<SGraphEditor> graphEditor = GraphEditorPtr.Pin();
	if (graphEditor.IsValid())
	{
		if (UEdGraph* graph = graphEditor->GetCurrentGraph())
		{
			TSharedPtr<FEdGraphSchemaAction> Action = graph->GetSchema()->GetCreateCommentAction();
			if (Action.IsValid())
			{
				Action->PerformAction(graph, nullptr, FVector2D());
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE