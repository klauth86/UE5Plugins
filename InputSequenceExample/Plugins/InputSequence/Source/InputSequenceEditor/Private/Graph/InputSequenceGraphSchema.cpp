// Fill out your copyright notice in the Description page of Project Settings.

#include "Graph/InputSequenceGraphSchema.h"
#include "Graph/InputSequenceGraph.h"
#include "Graph/InputSequenceGraphNode_Start.h"
#include "Graph/InputSequenceGraphNode_Finish.h"
#include "Graph/InputSequenceGraphNode_Press.h"
#include "Graph/InputSequenceGraphNode_Release.h"
#include "Graph/InputSequenceGraphFactories.h"
#include "Graph/SInputSequenceGraphNode_Press.h"
#include "KismetPins/SGraphPinExec.h"
#include "Graph/SGraphPin_Action.h"
#include "Graph/SGraphPin_Add.h"
#include "SGraphActionMenu.h"
#include "Classes/EditorStyleSettings.h"
#include "GameFramework/InputSettings.h"
#include "ConnectionDrawingPolicy.h"
#include "SPinTypeSelector.h"
#include "SLevelOfDetailBranchNode.h"
#include "Widgets/Layout/SWrapBox.h"
#include "EdGraphUtilities.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Framework/Commands/GenericCommands.h"
#include "GraphEditorActions.h"
#include "InputSequenceAsset.h"
#include "InputSequenceAssetEditor.h"

#pragma region UInputSequenceGraphSchema
#define LOCTEXT_NAMESPACE "UInputSequenceGraphSchema"

class FInputSequenceConnectionDrawingPolicy : public FConnectionDrawingPolicy
{
public:	
	FInputSequenceConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float ZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements, UEdGraph* InGraphObj)
		: FConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, ZoomFactor, InClippingRect, InDrawElements)
		, GraphObj(InGraphObj)
	{}

	virtual void DetermineWiringStyle(UEdGraphPin* OutputPin, UEdGraphPin* InputPin, /*inout*/ FConnectionParams& Params) override
	{
		FConnectionDrawingPolicy::DetermineWiringStyle(OutputPin, InputPin, Params);

		if (OutputPin->PinType.PinCategory == UInputSequenceGraphSchema::PC_Exec)
		{
			Params.WireThickness = 4;
		}
		else
		{
			Params.bUserFlag1 = true;
		}

		const bool bDeemphasizeUnhoveredPins = HoveredPins.Num() > 0;
		if (bDeemphasizeUnhoveredPins)
		{
			ApplyHoverDeemphasis(OutputPin, InputPin, /*inout*/ Params.WireThickness, /*inout*/ Params.WireColor);
		}
	}

	virtual void DrawSplineWithArrow(const FVector2D& StartPoint, const FVector2D& EndPoint, const FConnectionParams& Params) override
	{
		DrawConnection(
			WireLayerID,
			StartPoint,
			EndPoint,
			Params);

		// Draw the arrow
		if (ArrowImage != nullptr && Params.bUserFlag1)
		{
			FVector2D ArrowPoint = EndPoint - ArrowRadius;

			FSlateDrawElement::MakeBox(
				DrawElementsList,
				ArrowLayerID,
				FPaintGeometry(ArrowPoint, ArrowImage->ImageSize * ZoomFactor, ZoomFactor),
				ArrowImage,
				ESlateDrawEffect::None,
				Params.WireColor
			);
		}
	}

protected:
	UEdGraph* GraphObj;
	TMap<UEdGraphNode*, int32> NodeWidgetMap;
};

template<class T>
TSharedPtr<T> AddNewActionAs(FGraphContextMenuBuilder& ContextMenuBuilder, const FText& Category, const FText& MenuDesc, const FText& Tooltip, const int32 Grouping = 0)
{
	TSharedPtr<T> Action(new T(Category, MenuDesc, Tooltip, Grouping));
	ContextMenuBuilder.AddAction(Action);
	return Action;
}

template<class T>
void AddNewActionIfHasNo(FGraphContextMenuBuilder& ContextMenuBuilder, const FText& Category, const FText& MenuDesc, const FText& Tooltip, const int32 Grouping = 0)
{
	for (auto NodeIt = ContextMenuBuilder.CurrentGraph->Nodes.CreateConstIterator(); NodeIt; ++NodeIt)
	{
		UEdGraphNode* Node = *NodeIt;
		if (const T* castedNode = Cast<T>(Node)) return;
	}

	TSharedPtr<FInputSequenceGraphSchemaAction_NewNode> Action = AddNewActionAs<FInputSequenceGraphSchemaAction_NewNode>(ContextMenuBuilder, FText::GetEmpty(), LOCTEXT("AddNode_Finish", "Add Finish Node..."), LOCTEXT("AddNode_Finish_Tooltip", "Define Finish Node"));
	Action->NodeTemplate = NewObject<T>(ContextMenuBuilder.OwnerOfTemporaries);
}

UEdGraphNode* FInputSequenceGraphSchemaAction_NewNode::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{
	UEdGraphNode* ResultNode = NULL;

	// If there is a template, we actually use it
	if (NodeTemplate != NULL)
	{
		const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "K2_AddNode", "Add Node"));
		ParentGraph->Modify();
		if (FromPin)
		{
			FromPin->Modify();
		}

		// set outer to be the graph so it doesn't go away
		NodeTemplate->Rename(NULL, ParentGraph);
		ParentGraph->AddNode(NodeTemplate, true, bSelectNewNode);

		NodeTemplate->CreateNewGuid();
		NodeTemplate->PostPlacedNewNode();
		NodeTemplate->AllocateDefaultPins();
		NodeTemplate->AutowireNewNode(FromPin);

		NodeTemplate->NodePosX = Location.X;
		NodeTemplate->NodePosY = Location.Y;
		NodeTemplate->SnapToGrid(GetDefault<UEditorStyleSettings>()->GridSnapSize);

		ResultNode = NodeTemplate;

		ResultNode->SetFlags(RF_Transactional);
	}

	return ResultNode;
}

void FInputSequenceGraphSchemaAction_NewNode::AddReferencedObjects(FReferenceCollector& Collector)
{
	FEdGraphSchemaAction::AddReferencedObjects(Collector);

	Collector.AddReferencedObject(NodeTemplate);
}

UEdGraphNode* FInputSequenceGraphSchemaAction_AddPin::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{
	UEdGraphNode* ResultNode = NULL;

	// If there is a template, we actually use it
	if (ActionName != NAME_None)
	{
		const int32 execPinCount = 2;

		const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "K2_AddPin", "Add Pin"));

		UEdGraphNode::FCreatePinParams params;
		params.Index = CorrectedActionIndex + execPinCount;
		FromPin->GetOwningNode()->CreatePin(EGPD_Output, UInputSequenceGraphSchema::PC_Action, ActionName, params);

		FromPin->GetOwningNode()->Modify();

		if (UInputSequenceGraphNode_Press* pressNode = Cast<UInputSequenceGraphNode_Press>(FromPin->GetOwningNode()))
		{
			pressNode->OnUpdateGraphNode.ExecuteIfBound();
		}
	}

	return ResultNode;
}

TSharedPtr<SGraphNode> FInputSequenceGraphNodeFactory::CreateNode(UEdGraphNode* InNode) const
{
	if (UInputSequenceGraphNode_Press* stateNode = Cast<UInputSequenceGraphNode_Press>(InNode))
	{
		return SNew(SInputSequenceGraphNode_Press, stateNode);
	}

	return nullptr;
}

TSharedPtr<SGraphPin> FInputSequenceGraphPinFactory::CreatePin(UEdGraphPin* InPin) const
{
	if (InPin->GetSchema()->IsA<UInputSequenceGraphSchema>())
	{
		if (InPin->PinType.PinCategory == UInputSequenceGraphSchema::PC_Exec) return SNew(SGraphPinExec, InPin);

		if (InPin->PinType.PinCategory == UInputSequenceGraphSchema::PC_Action) return SNew(SGraphPin_Action, InPin);

		if (InPin->PinType.PinCategory == UInputSequenceGraphSchema::PC_Add) return SNew(SGraphPin_Add, InPin);
	}

	return nullptr;
}

FConnectionDrawingPolicy* FInputSequenceGraphPinConnectionFactory::CreateConnectionPolicy(const UEdGraphSchema* Schema, int32 InBackLayerID, int32 InFrontLayerID, float ZoomFactor, const class FSlateRect& InClippingRect, class FSlateWindowElementList& InDrawElements, UEdGraph* InGraphObj) const
{
	if (Schema->IsA<UInputSequenceGraphSchema>())
	{
		return new FInputSequenceConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, ZoomFactor, InClippingRect, InDrawElements, InGraphObj);;
	}

	return nullptr;
}

UInputSequenceGraph::UInputSequenceGraph(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	Schema = UInputSequenceGraphSchema::StaticClass();
}

const FName UInputSequenceGraphSchema::PC_Exec = FName("UInputSequenceGraphSchema_PC_Exec");

const FName UInputSequenceGraphSchema::PC_Action = FName("UInputSequenceGraphSchema_PC_Action");

const FName UInputSequenceGraphSchema::PC_Add = FName("UInputSequenceGraphSchema_PC_Add");

void UInputSequenceGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	// Add Press state node
	TSharedPtr<FInputSequenceGraphSchemaAction_NewNode> Action = AddNewActionAs<FInputSequenceGraphSchemaAction_NewNode>(ContextMenuBuilder, FText::GetEmpty(), LOCTEXT("AddNode_Press", "Add Press Node..."), LOCTEXT("AddNode_Press_Tooltip", "A new Press Node"));
	Action->NodeTemplate = NewObject<UInputSequenceGraphNode_Press>(ContextMenuBuilder.OwnerOfTemporaries);

	// Add Start node if absent
	AddNewActionIfHasNo<UInputSequenceGraphNode_Start>(ContextMenuBuilder, FText::GetEmpty(), LOCTEXT("AddNode_Start", "Add Start Node..."), LOCTEXT("AddNode_Start_Tooltip", "Define Start Node"));

	// Add Finish node if absent
	AddNewActionIfHasNo<UInputSequenceGraphNode_Finish>(ContextMenuBuilder, FText::GetEmpty(), LOCTEXT("AddNode_Finish", "Add Finish Node..."), LOCTEXT("AddNode_Finish_Tooltip", "Define Finish Node"));
}

const FPinConnectionResponse UInputSequenceGraphSchema::CanCreateConnection(const UEdGraphPin* pinA, const UEdGraphPin* pinB) const
{
	if (pinA == nullptr || pinB == nullptr) return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("Pin(s)Null", "One or Both of the pins was null"));

	if (pinA->GetOwningNode() == pinB->GetOwningNode()) return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("PinsOfSameNode", "Both pins are on the same node"));

	if (pinA->Direction == pinB->Direction) return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("PinsOfSameDirection", "Both pins have same direction (both input or both output)"));

	if (pinA->PinName != pinB->PinName) return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("PinsMismatched", "The pin types are mismatched (Flow pins should be connected to Flow pins, Action pins - to Action pins)"));

	return FPinConnectionResponse(CONNECT_RESPONSE_BREAK_OTHERS_AB, TEXT(""));
}

void UInputSequenceGraphSchema::BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotifcation) const
{
	if (TargetPin.PinName == NAME_None) UEdGraphSchema::BreakPinLinks(TargetPin, bSendsNodeNotifcation);
}

void UInputSequenceGraphSchema::CreateDefaultNodesForGraph(UEdGraph& Graph) const
{
	FGraphNodeCreator<UInputSequenceGraphNode_Start> startNodeCreator(Graph);
	UInputSequenceGraphNode_Start* startNode = startNodeCreator.CreateNode();
	startNode->NodePosX = -300;
	startNodeCreator.Finalize();
	SetNodeMetaData(startNode, FNodeMetadata::DefaultGraphNode);

	FGraphNodeCreator<UInputSequenceGraphNode_Finish> finishNodeCreator(Graph);
	UInputSequenceGraphNode_Finish* finishNode = finishNodeCreator.CreateNode();
	finishNode->NodePosX = 300;
	finishNodeCreator.Finalize();
	SetNodeMetaData(finishNode, FNodeMetadata::DefaultGraphNode);
}

void UInputSequenceGraphNode_Finish::AllocateDefaultPins()
{
	UEdGraphPin* InputPin = CreatePin(EGPD_Input, UInputSequenceGraphSchema::PC_Exec, NAME_None);
}

FText UInputSequenceGraphNode_Finish::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("UInputSequenceGraphNode_Finish_Title", "Fin");
}

FLinearColor UInputSequenceGraphNode_Finish::GetNodeTitleColor() const { return FLinearColor::Green; }

FText UInputSequenceGraphNode_Finish::GetTooltipText() const
{
	return LOCTEXT("UInputSequenceGraphNode_Finish_ToolTip", "This is a Fin node of Input sequence...");
}

void UInputSequenceGraphNode_Press::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, UInputSequenceGraphSchema::PC_Exec, NAME_None);
	CreatePin(EGPD_Output, UInputSequenceGraphSchema::PC_Exec, NAME_None);

	CreatePin(EGPD_Output, UInputSequenceGraphSchema::PC_Add, "Add pin");
}

FText UInputSequenceGraphNode_Press::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("UInputSequenceGraphNode_Finish_Title", "Press node");
}

FLinearColor UInputSequenceGraphNode_Press::GetNodeTitleColor() const { return FLinearColor::Blue; }

void UInputSequenceGraphNode_Press::AutowireNewNode(UEdGraphPin* FromPin)
{
	if (FromPin && FromPin->PinName == NAME_None)
	{
		if (FromPin->Direction == EGPD_Output)
		{
			if (UEdGraphPin* OtherPin = FindPin(FromPin->PinName, EGPD_Input))
			{
				GetSchema()->TryCreateConnection(FromPin, OtherPin);
			}
		}
		else
		{
			if (UEdGraphPin* OtherPin = FindPin(FromPin->PinName, EGPD_Output))
			{
				GetSchema()->TryCreateConnection(FromPin, OtherPin);
			}
		}
	}
}

void UInputSequenceGraphNode_Release::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, UInputSequenceGraphSchema::PC_Exec, NAME_None);
	CreatePin(EGPD_Output, UInputSequenceGraphSchema::PC_Exec, NAME_None);
}

FText UInputSequenceGraphNode_Release::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("UInputSequenceGraphNode_Release_Title", "Release node");
}

FLinearColor UInputSequenceGraphNode_Release::GetNodeTitleColor() const { return FLinearColor::Blue; }

void UInputSequenceGraphNode_Release::AutowireNewNode(UEdGraphPin* FromPin)
{
	if (FromPin->Direction == EGPD_Output && FromPin && FromPin->PinName != NAME_None)
	{
		UEdGraphPin* OtherPin = CreatePin(EGPD_Input, UInputSequenceGraphSchema::PC_Action, FromPin->PinName);
		GetSchema()->TryCreateConnection(FromPin, OtherPin);
	}
}

void UInputSequenceGraphNode_Start::AllocateDefaultPins()
{
	UEdGraphPin* OutputPin = CreatePin(EGPD_Output, UInputSequenceGraphSchema::PC_Exec, NAME_None);
}

FText UInputSequenceGraphNode_Start::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("UInputSequenceGraphNode_Start_Title", "Start");
}

FLinearColor UInputSequenceGraphNode_Start::GetNodeTitleColor() const { return FLinearColor::Red; }

FText UInputSequenceGraphNode_Start::GetTooltipText() const
{
	return LOCTEXT("UInputSequenceGraphNode_Start_ToolTip", "This is a Start node of Input sequence...");
}

class SInputSequenceParameterMenu : public SCompoundWidget
{
public:
	DECLARE_DELEGATE_RetVal_OneParam(FText, FGetSectionTitle, int32);

	SLATE_BEGIN_ARGS(SInputSequenceParameterMenu) : _AutoExpandMenu(false) {}
	
	SLATE_ARGUMENT(bool, AutoExpandMenu)
	SLATE_EVENT(FGetSectionTitle, OnGetSectionTitle)
	SLATE_END_ARGS();

	void Construct(const FArguments& InArgs)
	{
		this->bAutoExpandMenu = InArgs._AutoExpandMenu;

		ChildSlot
		[
			SNew(SBorder).BorderImage(FEditorStyle::GetBrush("Menu.Background")).Padding(5)
			[
				SNew(SBox)
				.MinDesiredWidth(300)
				.MaxDesiredHeight(700) // Set max desired height to prevent flickering bug for menu larger than screen
				[
					SAssignNew(GraphMenu, SGraphActionMenu)
					.OnCollectAllActions(this, &SInputSequenceParameterMenu::CollectAllActions)
					.OnActionSelected(this, &SInputSequenceParameterMenu::OnActionSelected)
					.SortItemsRecursively(false)
					.AlphaSortItems(false)
					.AutoExpandActionMenu(bAutoExpandMenu)
					.ShowFilterTextBox(true)
					.OnGetSectionTitle(InArgs._OnGetSectionTitle)
					////// TODO.OnCreateCustomRowExpander_Static(&SNiagaraParameterMenu::CreateCustomActionExpander)
					////// TODO.OnCreateWidgetForAction_Lambda([](const FCreateWidgetForActionData* InData) { return SNew(SNiagaraGraphActionWidget, InData); })
				]
			]
		];
	}

	TSharedPtr<SEditableTextBox> GetSearchBox() { return GraphMenu->GetFilterTextBox(); }

protected:
	
	virtual void CollectAllActions(FGraphActionListBuilderBase& OutAllActions) = 0;
	
	virtual void OnActionSelected(const TArray<TSharedPtr<FEdGraphSchemaAction>>& SelectedActions, ESelectInfo::Type InSelectionType) = 0;

private:

	bool bAutoExpandMenu;

	TSharedPtr<SGraphActionMenu> GraphMenu;
};

class SInputSequenceParameterMenu_Pin : public SInputSequenceParameterMenu
{
public:
	SLATE_BEGIN_ARGS(SInputSequenceParameterMenu_Pin)
		: _AutoExpandMenu(false)
	{}
	//~ Begin Required Args
	SLATE_ARGUMENT(UEdGraphNode*, Node)
	//~ End Required Args
	SLATE_ARGUMENT(bool, AutoExpandMenu)
	SLATE_END_ARGS();

	void Construct(const FArguments& InArgs)
	{
		this->Node = InArgs._Node;

		SInputSequenceParameterMenu::FArguments SuperArgs;
		SuperArgs._AutoExpandMenu = InArgs._AutoExpandMenu;
		SInputSequenceParameterMenu::Construct(SuperArgs);
	}

protected:
	
	virtual void CollectAllActions(FGraphActionListBuilderBase& OutAllActions) override
	{
		const TArray<FInputActionKeyMapping>& actionMappings = UInputSettings::GetInputSettings()->GetActionMappings();

		int32 actionIndex = 0;

		TSet<int32> alreadyAdded;

		TArray<TSharedPtr<FEdGraphSchemaAction>> actions;

		for (const FInputActionKeyMapping& actionMapping : actionMappings)
		{
			if (Node->FindPin(actionMapping.ActionName))
			{
				alreadyAdded.Add(actionIndex);
			}
			else
			{
				TSharedPtr<FInputSequenceGraphSchemaAction_AddPin> action(new FInputSequenceGraphSchemaAction_AddPin(FText::GetEmpty(), FText::FromName(actionMapping.ActionName), FText::Format(LOCTEXT("AddPin_Action_Tooltip", "Add Action pin for {0}"), FText::FromName(actionMapping.ActionName)), 0));
				action->ActionName = actionMapping.ActionName;
				action->ActionIndex = actionIndex;
				action->CorrectedActionIndex = 0;
				actions.Add(action);				
			}

			actionIndex++;
		}

		for (TSharedPtr<FEdGraphSchemaAction> action : actions)
		{
			TSharedPtr<FInputSequenceGraphSchemaAction_AddPin> addPinAction = StaticCastSharedPtr<FInputSequenceGraphSchemaAction_AddPin>(action);
			for (int32 alreadyAddedIndex : alreadyAdded)
			{
				if (alreadyAddedIndex < addPinAction->ActionIndex) addPinAction->CorrectedActionIndex++;
			}

			OutAllActions.AddAction(action);
		}
	}

	virtual void OnActionSelected(const TArray<TSharedPtr<FEdGraphSchemaAction>>& SelectedActions, ESelectInfo::Type InSelectionType) override
	{
		if (InSelectionType == ESelectInfo::OnMouseClick || InSelectionType == ESelectInfo::OnKeyPress || SelectedActions.Num() == 0)
		{
			for (int32 ActionIndex = 0; ActionIndex < SelectedActions.Num(); ActionIndex++)
			{
				FSlateApplication::Get().DismissAllMenus();
				SelectedActions[ActionIndex]->PerformAction(Node->GetGraph(), Node->FindPin(NAME_None, EGPD_Output), FVector2D::ZeroVector);
			}
		}
	}

private:

	UEdGraphNode* Node;
};

void SInputSequenceGraphNode_Press::Construct(const FArguments& InArgs, UEdGraphNode* InNode)
{
	SetCursor(EMouseCursor::CardinalCross);

	GraphNode = InNode;

	if (UInputSequenceGraphNode_Press* pressNode = Cast<UInputSequenceGraphNode_Press>(InNode))
	{
		pressNode->OnUpdateGraphNode.BindLambda([&]() { UpdateGraphNode(); });
	}

	UpdateGraphNode();
}

SInputSequenceGraphNode_Press::~SInputSequenceGraphNode_Press()
{
	if (UInputSequenceGraphNode_Press* pressNode = Cast<UInputSequenceGraphNode_Press>(GraphNode))
	{
		pressNode->OnUpdateGraphNode.Unbind();
	}
}

#undef LOCTEXT_NAMESPACE
#pragma endregion

#pragma region SGraphPin_Add
#define LOCTEXT_NAMESPACE "SGraphPin_Add"

void SGraphPin_Add::Construct(const FArguments& Args, UEdGraphPin* InPin)
{
	SGraphPin::FArguments InArgs = SGraphPin::FArguments();

	bUsePinColorForText = InArgs._UsePinColorForText;
	this->SetCursor(EMouseCursor::Hand);
	this->SetToolTipText(LOCTEXT("AddPin_ToolTip", "Click to add new Action pin"));

	SetVisibility(MakeAttributeSP(this, &SGraphPin_Add::GetPinVisiblity));

	GraphPinObj = InPin;
	check(GraphPinObj != NULL);

	const UEdGraphSchema* Schema = GraphPinObj->GetSchema();
	checkf(
		Schema,
		TEXT("Missing schema for pin: %s with outer: %s of type %s"),
		*(GraphPinObj->GetName()),
		GraphPinObj->GetOuter() ? *(GraphPinObj->GetOuter()->GetName()) : TEXT("NULL OUTER"),
		GraphPinObj->GetOuter() ? *(GraphPinObj->GetOuter()->GetClass()->GetName()) : TEXT("NULL OUTER")
	);

	TSharedRef<SWidget> PinWidgetRef = SNew(SImage).Image(FEditorStyle::GetBrush(TEXT("Icons.PlusCircle")));

	PinImage = PinWidgetRef;

	// Create the pin indicator widget (used for watched values)
	static const FName NAME_NoBorder("NoBorder");
	TSharedRef<SWidget> PinStatusIndicator =
		SNew(SButton)
		.ButtonStyle(FEditorStyle::Get(), NAME_NoBorder)
		.Visibility(this, &SGraphPin_Add::GetPinStatusIconVisibility)
		.ContentPadding(0)
		.OnClicked(this, &SGraphPin_Add::ClickedOnPinStatusIcon)
		[
			SNew(SImage).Image(this, &SGraphPin_Add::GetPinStatusIcon)
		];

	TSharedRef<SWidget> LabelWidget = GetLabelWidget(InArgs._PinLabelStyle);

	// Create the widget used for the pin body (status indicator, label, and value)
	LabelAndValue =
		SNew(SWrapBox)
		.PreferredSize(150.f);

	LabelAndValue->AddSlot()
		.VAlign(VAlign_Center)
		[
			LabelWidget
		];

	LabelAndValue->AddSlot()
		.VAlign(VAlign_Center)
		[
			PinStatusIndicator
		];

	TSharedPtr<SHorizontalBox> PinContent;
	FullPinHorizontalRowWidget = PinContent = SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		.Padding(0, 0, InArgs._SideToSideMargin, 0)
		[
			LabelAndValue.ToSharedRef()
		]
	+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
			PinWidgetRef
		];

	// Set up a hover for pins that is tinted the color of the pin.
	SBorder::Construct(SBorder::FArguments()
		.BorderImage(FEditorStyle::GetBrush("NoBorder"))
		.BorderBackgroundColor(this, &SGraphPin_Add::GetPinColor)
		[
			SAssignNew(AddButton, SComboButton)
			.HasDownArrow(false)
		.ButtonStyle(FEditorStyle::Get(), "NoBorder")
		.ForegroundColor(FSlateColor::UseForeground())
		.OnGetMenuContent(this, &SGraphPin_Add::OnGetAddButtonMenuContent)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.ButtonContent()
		[
			PinContent.ToSharedRef()
		]
		]
	);
}

TSharedRef<SWidget> SGraphPin_Add::OnGetAddButtonMenuContent()
{
	TSharedRef<SInputSequenceParameterMenu_Pin> MenuWidget = SNew(SInputSequenceParameterMenu_Pin).Node(GetPinObj()->GetOwningNode());

	AddButton->SetMenuContentWidgetToFocus(MenuWidget->GetSearchBox());

	return MenuWidget;
}

#undef LOCTEXT_NAMESPACE
#pragma endregion

#pragma region SGraphPin_Action
#define LOCTEXT_NAMESPACE "SGraphPin_Action"

class SToolTip_Mock : public SLeafWidget, public IToolTip
{
public:

	SLATE_BEGIN_ARGS(SToolTip_Mock) {}
	SLATE_END_ARGS()

		void Construct(const FArguments& InArgs) {}

	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override { return LayerId; }
	virtual FVector2D ComputeDesiredSize(float) const override { return FVector2D::ZeroVector; }

	virtual TSharedRef<class SWidget> AsWidget() { return SNullWidget::NullWidget; }
	virtual TSharedRef<SWidget> GetContentWidget() { return SNullWidget::NullWidget; }
	virtual void SetContentWidget(const TSharedRef<SWidget>& InContentWidget) override {}
	virtual bool IsEmpty() const override { return false; }
	virtual bool IsInteractive() const { return false; }
	virtual void OnOpening() override {}
	virtual void OnClosed() override {}
};

void SGraphPin_Action::Construct(const FArguments& Args, UEdGraphPin* InPin)
{
	SGraphPin::FArguments InArgs = SGraphPin::FArguments();

	bUsePinColorForText = InArgs._UsePinColorForText;
	this->SetCursor(EMouseCursor::Default);
	this->SetToolTipText(LOCTEXT("ActionPin_ToolTip", "Mock ToolTip"));

	SetVisibility(MakeAttributeSP(this, &SGraphPin_Action::GetPinVisiblity));

	GraphPinObj = InPin;
	check(GraphPinObj != NULL);

	const UEdGraphSchema* Schema = GraphPinObj->GetSchema();
	checkf(
		Schema,
		TEXT("Missing schema for pin: %s with outer: %s of type %s"),
		*(GraphPinObj->GetName()),
		GraphPinObj->GetOuter() ? *(GraphPinObj->GetOuter()->GetName()) : TEXT("NULL OUTER"),
		GraphPinObj->GetOuter() ? *(GraphPinObj->GetOuter()->GetClass()->GetName()) : TEXT("NULL OUTER")
	);

	const bool bIsInput = (GetDirection() == EGPD_Input);

	// Create the pin icon widget
	TSharedRef<SWidget> SelfPinWidgetRef = SPinTypeSelector::ConstructPinTypeImage(
		MakeAttributeSP(this, &SGraphPin_Action::GetPinIcon),
		MakeAttributeSP(this, &SGraphPin_Action::GetPinColor),
		MakeAttributeSP(this, &SGraphPin_Action::GetSecondaryPinIcon),
		MakeAttributeSP(this, &SGraphPin_Action::GetSecondaryPinColor));

	SelfPinWidgetRef->SetVisibility(MakeAttributeRaw(this, &SGraphPin_Action::Visibility_Raw_SelfPin));

	TSharedRef<SWidget> PinWidgetRef =
		SNew(SOverlay)
		+ SOverlay::Slot()
		[
			SNew(SImage).Image(FEditorStyle::GetBrush(TEXT("Icons.CircleArrowUp")))
			.Visibility_Raw(this, &SGraphPin_Action::Visibility_Raw_ArrowUp)
		]
	+ SOverlay::Slot().VAlign(VAlign_Center).HAlign(HAlign_Center)
		[
			SelfPinWidgetRef
		];

	PinImage = PinWidgetRef;

	// Create the pin indicator widget (used for watched values)
	static const FName NAME_NoBorder("NoBorder");
	TSharedRef<SWidget> PinStatusIndicator =
		SNew(SButton)
		.ButtonStyle(FEditorStyle::Get(), NAME_NoBorder)
		.Visibility(this, &SGraphPin_Action::GetPinStatusIconVisibility)
		.ContentPadding(0)
		.OnClicked(this, &SGraphPin_Action::ClickedOnPinStatusIcon)
		[
			SNew(SImage)
			.Image(this, &SGraphPin_Action::GetPinStatusIcon)
		];

	TSharedRef<SWidget> LabelWidget = GetLabelWidget(InArgs._PinLabelStyle);
	LabelWidget->SetToolTipText(MakeAttributeRaw(this, &SGraphPin_Action::ToolTipText_Raw_Label));

	// Create the widget used for the pin body (status indicator, label, and value)
	LabelAndValue =
		SNew(SWrapBox)
		.PreferredSize(150.f);

	if (!bIsInput) // Input pin
	{
		LabelAndValue->AddSlot()
			.VAlign(VAlign_Center)
			[
				PinStatusIndicator
			];

		LabelAndValue->AddSlot()
			.VAlign(VAlign_Center)
			[
				LabelWidget
			];
	}
	else // Output pin
	{
		LabelAndValue->AddSlot()
			.VAlign(VAlign_Center)
			[
				LabelWidget
			];

		LabelAndValue->AddSlot()
			.VAlign(VAlign_Center)
			[
				PinStatusIndicator
			];
	}

	TSharedPtr<SHorizontalBox> PinContent;
	if (bIsInput) // Input pin
	{
		FullPinHorizontalRowWidget = PinContent =
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(0, 0, InArgs._SideToSideMargin, 0)
			[
				PinWidgetRef
			]
		+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(0, 0, InArgs._SideToSideMargin, 0)
			[
				SNew(SImage).Image(FEditorStyle::GetBrush(TEXT("Icons.CircleArrowUp")))
			]
		+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				LabelAndValue.ToSharedRef()
			];
	}
	else // Output pin
	{
		FullPinHorizontalRowWidget = PinContent = SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(0, 0, InArgs._SideToSideMargin, 0)
			[
				SNew(SButton).ToolTipText_Raw(this, &SGraphPin_Action::ToolTipText_Raw_RemovePin)
				.Cursor(EMouseCursor::Hand)
			.ButtonStyle(FEditorStyle::Get(), "NoBorder")
			.ForegroundColor(FSlateColor::UseForeground())
			.OnClicked_Raw(this, &SGraphPin_Action::OnClicked_Raw_RemovePin)
			[
				SNew(SImage).Image(FEditorStyle::GetBrush("Cross"))
			]
			]
		+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(0, 0, InArgs._SideToSideMargin, 0)
			[
				LabelAndValue.ToSharedRef()
			]
		+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(0, 0, InArgs._SideToSideMargin, 0)
			[
				SNew(SButton).ToolTipText_Raw(this, &SGraphPin_Action::ToolTipText_Raw_TogglePin)
				.Cursor(EMouseCursor::Hand)
			.ButtonStyle(FEditorStyle::Get(), "NoBorder")
			.ForegroundColor(FSlateColor::UseForeground())
			.OnClicked_Raw(this, &SGraphPin_Action::OnClicked_Raw_TogglePin)
			[
				SNew(SImage).Image(FEditorStyle::GetBrush(TEXT("Icons.CircleArrowDown")))
			]
			]
		+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				PinWidgetRef
			];
	}

	// Set up a hover for pins that is tinted the color of the pin.
	SBorder::Construct(SBorder::FArguments()
		.BorderImage(FEditorStyle::GetBrush("NoBorder"))
		.BorderBackgroundColor(this, &SGraphPin_Action::GetPinColor)
		[
			SNew(SLevelOfDetailBranchNode)
			.UseLowDetailSlot(this, &SGraphPin_Action::UseLowDetailPinNames)
		.LowDetail()
		[
			//@TODO: Try creating a pin-colored line replacement that doesn't measure text / call delegates but still renders
			PinWidgetRef
		]
	.HighDetail()
		[
			PinContent.ToSharedRef()
		]
		]
	);

	SetToolTip(SNew(SToolTip_Mock));
}

FSlateColor SGraphPin_Action::GetPinTextColor() const
{
	UEdGraphPin* GraphPin = GetPinObj();

	////// TODO At tthe moment implementation is not very eefective
	////// But this code is only for Editor, so is not a big deal

	bool isFoundInActionMappings = false;

	const TArray<FInputActionKeyMapping>& actionMappings = UInputSettings::GetInputSettings()->GetActionMappings();

	for (const FInputActionKeyMapping& actionMapping : actionMappings)
	{
		if (GraphPin->PinName == actionMapping.ActionName)
		{
			isFoundInActionMappings = true;
			break;
		}
	}

	if (!isFoundInActionMappings) return FLinearColor::Red;

	if (GraphPin)

		// If there is no schema there is no owning node (or basically this is a deleted node)
		if (UEdGraphNode* GraphNode = GraphPin ? GraphPin->GetOwningNodeUnchecked() : nullptr)
		{
			const bool bDisabled = (!GraphNode->IsNodeEnabled() || GraphNode->IsDisplayAsDisabledForced() || !IsEditingEnabled() || GraphNode->IsNodeUnrelated());
			if (GraphPin->bOrphanedPin)
			{
				FLinearColor PinColor = FLinearColor::Red;
				if (bDisabled)
				{
					PinColor.A = .25f;
				}
				return PinColor;
			}
			else if (bDisabled)
			{
				return FLinearColor(1.0f, 1.0f, 1.0f, 0.5f);
			}
			if (bUsePinColorForText)
			{
				return GetPinColor();
			}
		}

	return FLinearColor::White;
}

FText SGraphPin_Action::ToolTipText_Raw_Label() const
{
	UEdGraphPin* GraphPin = GetPinObj();

	////// TODO At tthe moment implementation is not very eefective
	////// But this code is only for Editor, so is not a big deal

	const TArray<FInputActionKeyMapping>& actionMappings = UInputSettings::GetInputSettings()->GetActionMappings();

	for (const FInputActionKeyMapping& actionMapping : actionMappings)
	{
		if (GraphPin->PinName == actionMapping.ActionName)
		{
			return FText::GetEmpty();
		}
	}

	return LOCTEXT("Label_TootTip_Error", "Cant find corresponding Action name in Input Settings!");
}

EVisibility SGraphPin_Action::Visibility_Raw_SelfPin() const
{
	if (UEdGraphPin* pin = GetPinObj())
	{
		return pin->HasAnyConnections() ? EVisibility::Visible : EVisibility::Hidden;
	}

	return EVisibility::Hidden;
}

EVisibility SGraphPin_Action::Visibility_Raw_ArrowUp() const
{
	if (UEdGraphPin* pin = GetPinObj())
	{
		return pin->HasAnyConnections() ? EVisibility::Hidden : EVisibility::Visible;
	}

	return EVisibility::Visible;
}

FText SGraphPin_Action::ToolTipText_Raw_RemovePin() const { return LOCTEXT("RemovePin_Tooltip", "Click to remove Action pin"); }

FReply SGraphPin_Action::OnClicked_Raw_RemovePin() const
{
	if (UEdGraphPin* FromPin = GetPinObj())
	{
		if (FromPin->HasAnyConnections())
		{
			UEdGraphNode* linkedNode = FromPin->LinkedTo[0]->GetOwningNode();
			linkedNode->Modify();
			linkedNode->DestroyNode();
		}

		UEdGraphNode* FromNode = FromPin->GetOwningNode();

		FromNode->RemovePin(FromPin);

		FromNode->Modify();

		if (UInputSequenceGraphNode_Press* pressNode = Cast<UInputSequenceGraphNode_Press>(FromNode))
		{
			pressNode->OnUpdateGraphNode.ExecuteIfBound();
		}
	}

	return FReply::Handled();
}

FText SGraphPin_Action::ToolTipText_Raw_TogglePin() const
{
	if (UEdGraphPin* FromPin = GetPinObj())
	{
		return FromPin->HasAnyConnections()
			? LOCTEXT("RemovePin_Tooltip_Click", "Click to set CLICK mode")
			: LOCTEXT("RemovePin_Tooltip_Press", "Click to set PRESS mode");
	}

	return LOCTEXT("RemovePin_Tooltip_Error", "Invalid pin object!");
}

FReply SGraphPin_Action::OnClicked_Raw_TogglePin() const
{
	if (UEdGraphPin* FromPin = GetPinObj())
	{
		if (FromPin->HasAnyConnections())
		{
			UEdGraphNode* linkedNode = FromPin->LinkedTo[0]->GetOwningNode();
			linkedNode->Modify();
			linkedNode->DestroyNode();
		}
		else
		{
			UEdGraphNode* FromNode = FromPin->GetOwningNode();

			UEdGraph* ParentGraph = FromNode->GetGraph();

			const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "K2_AddNode", "Add Node"));

			ParentGraph->Modify();
			
			if (FromPin)
			{
				FromPin->Modify();
			}

			// set outer to be the graph so it doesn't go away
			UEdGraphNode* ResultNode = NewObject<UInputSequenceGraphNode_Release>(ParentGraph);
			ParentGraph->AddNode(ResultNode, true, false);

			ResultNode->CreateNewGuid();
			ResultNode->PostPlacedNewNode();
			ResultNode->AllocateDefaultPins();
			ResultNode->AutowireNewNode(FromPin);

			ResultNode->NodePosX = FromNode->NodePosX + 300;
			ResultNode->NodePosY = FromNode->NodePosY;

			ResultNode->SnapToGrid(GetDefault<UEditorStyleSettings>()->GridSnapSize);;

			ResultNode->SetFlags(RF_Transactional);
		}
	}

	return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
#pragma endregion

#pragma region FInputSequenceAssetEditor
#define LOCTEXT_NAMESPACE "FInputSequenceAssetEditor"

const FName FInputSequenceAssetEditor::AppIdentifier(TEXT("FInputSequenceAssetEditor_AppIdentifier"));
const FName FInputSequenceAssetEditor::DetailsTabId(TEXT("FInputSequenceAssetEditor_DetailsTab_Id"));
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
				FTabManager::NewSplitter()->SetOrientation(Orient_Horizontal)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.3f)
					->AddTab(DetailsTabId, ETabState::OpenedTab)
					->SetHideTabWell(true)
				)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.7f)
					->AddTab(GraphTabId, ETabState::OpenedTab)
					->SetHideTabWell(true)
				)
			)
		);

	FAssetEditorToolkit::InitAssetEditor(Mode, InitToolkitHost, AppIdentifier, StandaloneDefaultLayout, true, true, InputSequenceAsset);
}

void FInputSequenceAssetEditor::RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenuCategory", "Input Sequence Asset Editor"));
	TSharedRef<FWorkspaceItem> WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	InTabManager->RegisterTabSpawner(DetailsTabId, FOnSpawnTab::CreateSP(this, &FInputSequenceAssetEditor::SpawnTab_DetailsTab))
		.SetDisplayName(LOCTEXT("DetailsTab_DisplayName", "Details"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Details"));

	InTabManager->RegisterTabSpawner(GraphTabId, FOnSpawnTab::CreateSP(this, &FInputSequenceAssetEditor::SpawnTab_GraphTab))
		.SetDisplayName(LOCTEXT("GraphTab_DisplayName", "Graph"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "GraphEditor.EventGraph_16x"));
}

void FInputSequenceAssetEditor::UnregisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);

	InTabManager->UnregisterTabSpawner(GraphTabId);
	InTabManager->UnregisterTabSpawner(DetailsTabId);
}

TSharedRef<SDockTab> FInputSequenceAssetEditor::SpawnTab_DetailsTab(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == DetailsTabId);

	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

	FDetailsViewArgs DetailsViewArgs = FDetailsViewArgs();
	DetailsViewArgs.bUpdatesFromSelection = false;
	DetailsViewArgs.bLockable = false;
	DetailsViewArgs.bAllowSearch = false;
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
	DetailsViewArgs.bHideSelectionTip = true;

	DetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	DetailsView->SetObject(NULL);

	return SNew(SDockTab).Label(LOCTEXT("DetailsTab_Label", "Details"))[DetailsView.ToSharedRef()];
}

TSharedRef<SDockTab> FInputSequenceAssetEditor::SpawnTab_GraphTab(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId().TabType == GraphTabId);

	check(InputSequenceAsset != NULL);

	if (InputSequenceAsset->EdGraph == NULL)
	{
		InputSequenceAsset->EdGraph = NewObject<UInputSequenceGraph>(InputSequenceAsset, NAME_None, RF_Transactional);
		InputSequenceAsset->EdGraph->GetSchema()->CreateDefaultNodesForGraph(*InputSequenceAsset->EdGraph);
	}

	check(InputSequenceAsset->EdGraph != NULL);

	FGraphAppearanceInfo AppearanceInfo;
	AppearanceInfo.CornerText = LOCTEXT("GraphTab_AppearanceInfo_CornerText", "Input Sequence Asset");

	SGraphEditor::FGraphEditorEvents InEvents;
	InEvents.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP(this, &FInputSequenceAssetEditor::OnSelectionChanged);

	CreateCommandList();

	return SNew(SDockTab)
		.Label(LOCTEXT("GraphTab_Label", "Graph"))
		.TabColorScale(GetTabColorScale())
		[
			SAssignNew(GraphEditorPtr, SGraphEditor)
			.AdditionalCommands(GraphEditorCommands)
		.Appearance(AppearanceInfo)
		.GraphEvents(InEvents)
		.TitleBar(SNew(STextBlock).Text(LOCTEXT("GraphTab_Title", "Input Sequence Asset")).TextStyle(FEditorStyle::Get(), TEXT("GraphBreadcrumbButtonText")))
		.GraphToEdit(InputSequenceAsset->EdGraph)
		];
}

void FInputSequenceAssetEditor::CreateCommandList()
{
	if (GraphEditorCommands.IsValid()) return;

	GraphEditorCommands = MakeShareable(new FUICommandList);

	GraphEditorCommands->MapAction(FGenericCommands::Get().SelectAll,
		FExecuteAction::CreateRaw(this, &FInputSequenceAssetEditor::SelectAllNodes),
		FCanExecuteAction::CreateRaw(this, &FInputSequenceAssetEditor::CanSelectAllNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Delete,
		FExecuteAction::CreateRaw(this, &FInputSequenceAssetEditor::DeleteSelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FInputSequenceAssetEditor::CanDeleteNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Copy,
		FExecuteAction::CreateRaw(this, &FInputSequenceAssetEditor::CopySelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FInputSequenceAssetEditor::CanCopyNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Cut,
		FExecuteAction::CreateRaw(this, &FInputSequenceAssetEditor::CutSelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FInputSequenceAssetEditor::CanCutNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Paste,
		FExecuteAction::CreateRaw(this, &FInputSequenceAssetEditor::PasteNodes),
		FCanExecuteAction::CreateRaw(this, &FInputSequenceAssetEditor::CanPasteNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Duplicate,
		FExecuteAction::CreateRaw(this, &FInputSequenceAssetEditor::DuplicateNodes),
		FCanExecuteAction::CreateRaw(this, &FInputSequenceAssetEditor::CanDuplicateNodes)
	);

	GraphEditorCommands->MapAction(
		FGraphEditorCommands::Get().CreateComment,
		FExecuteAction::CreateRaw(this, &FInputSequenceAssetEditor::OnCreateComment),
		FCanExecuteAction::CreateRaw(this, &FInputSequenceAssetEditor::CanCreateComment)
	);
}

void FInputSequenceAssetEditor::OnSelectionChanged(const TSet<UObject*>& selectedNodes)
{
	return selectedNodes.Num() == 1 ? DetailsView->SetObject(*selectedNodes.begin()) : DetailsView->SetObject(NULL);
}

FGraphPanelSelectionSet FInputSequenceAssetEditor::GetSelectedNodes() const
{
	FGraphPanelSelectionSet CurrentSelection;

	if (TSharedPtr<SGraphEditor> graphEditor = GraphEditorPtr.Pin())
	{
		if (graphEditor.IsValid())
		{
			CurrentSelection = graphEditor->GetSelectedNodes();
		}
	}

	return CurrentSelection;
}

void FInputSequenceAssetEditor::SelectAllNodes()
{
	if (TSharedPtr<SGraphEditor> graphEditor = GraphEditorPtr.Pin())
	{
		if (graphEditor.IsValid())
		{
			graphEditor->SelectAllNodes();
		}
	}
}

void FInputSequenceAssetEditor::DeleteSelectedNodes()
{
	if (TSharedPtr<SGraphEditor> graphEditor = GraphEditorPtr.Pin())
	{
		if (graphEditor.IsValid())
		{
			const FScopedTransaction Transaction(FGenericCommands::Get().Delete->GetDescription());

			graphEditor->GetCurrentGraph()->Modify();

			const FGraphPanelSelectionSet SelectedNodes = graphEditor->GetSelectedNodes();
			graphEditor->ClearSelectionSet();

			for (FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes); NodeIt; ++NodeIt)
			{
				if (UEdGraphNode* Node = Cast<UEdGraphNode>(*NodeIt))
				{
					if (Node->CanUserDeleteNode())
					{
						Node->Modify();
						Node->DestroyNode();
					}
				}
			}
		}
	}
}

bool FInputSequenceAssetEditor::CanDeleteNodes() const
{
	const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();
	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		if (Node && Node->CanUserDeleteNode()) return true;
	}

	return false;
}

void FInputSequenceAssetEditor::CopySelectedNodes()
{
	FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();

	FString ExportedText;

	int32 CopySubNodeIndex = 0;
	for (FGraphPanelSelectionSet::TIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);

		if (Node == nullptr)
		{
			SelectedIter.RemoveCurrent();
			continue;
		}

		Node->PrepareForCopying();
	}

	FEdGraphUtilities::ExportNodesToText(SelectedNodes, ExportedText);
	FPlatformApplicationMisc::ClipboardCopy(*ExportedText);

	for (FGraphPanelSelectionSet::TIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		////// TODO Check Outer for some reason they do reset outer here
		//////UAIGraphNode* Node = Cast<UAIGraphNode>(*SelectedIter);
		//////if (Node)
		//////{
		//////	Node->PostCopyNode();
		//////}
	}
}

bool FInputSequenceAssetEditor::CanCopyNodes() const
{
	const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();
	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		if (Node && Node->CanDuplicateNode()) return true;
	}

	return false;
}

void FInputSequenceAssetEditor::DeleteSelectedDuplicatableNodes()
{
	if (TSharedPtr<SGraphEditor> graphEditor = GraphEditorPtr.Pin())
	{
		if (graphEditor.IsValid())
		{
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
	}
}

void FInputSequenceAssetEditor::PasteNodes()
{
	if (TSharedPtr<SGraphEditor> graphEditor = GraphEditorPtr.Pin())
	{
		if (graphEditor.IsValid())
		{
			FVector2D Location = graphEditor->GetPasteLocation();

			UEdGraph* EdGraph = graphEditor->GetCurrentGraph();

			// Undo/Redo support
			const FScopedTransaction Transaction(FGenericCommands::Get().Paste->GetDescription());

			EdGraph->Modify();

			// Clear the selection set (newly pasted stuff will be selected)
			graphEditor->ClearSelectionSet();

			// Grab the text to paste from the clipboard.
			FString TextToImport;
			FPlatformApplicationMisc::ClipboardPaste(TextToImport);

			// Import the nodes
			TSet<UEdGraphNode*> PastedNodes;
			FEdGraphUtilities::ImportNodesFromText(EdGraph, TextToImport, /*out*/ PastedNodes);

			//Average position of nodes so we can move them while still maintaining relative distances to each other
			FVector2D AvgNodePosition(0.0f, 0.0f);

			for (TSet<UEdGraphNode*>::TIterator It(PastedNodes); It; ++It)
			{
				UEdGraphNode* Node = *It;
				AvgNodePosition.X += Node->NodePosX;
				AvgNodePosition.Y += Node->NodePosY;
			}

			if (PastedNodes.Num() > 0)
			{
				float InvNumNodes = 1.0f / float(PastedNodes.Num());
				AvgNodePosition.X *= InvNumNodes;
				AvgNodePosition.Y *= InvNumNodes;
			}

			for (TSet<UEdGraphNode*>::TIterator It(PastedNodes); It; ++It)
			{
				UEdGraphNode* Node = *It;

				// Select the newly pasted stuff
				graphEditor->SetNodeSelection(Node, true);

				Node->NodePosX = (Node->NodePosX - AvgNodePosition.X) + Location.X;
				Node->NodePosY = (Node->NodePosY - AvgNodePosition.Y) + Location.Y;

				Node->SnapToGrid(GetDefault<UEditorStyleSettings>()->GridSnapSize);

				// Give new node a different Guid from the old one
				Node->CreateNewGuid();
			}

			InputSequenceAsset->PostEditChange();
			InputSequenceAsset->MarkPackageDirty();
		}
	}
}

bool FInputSequenceAssetEditor::CanPasteNodes() const
{
	if (TSharedPtr<SGraphEditor> graphEditor = GraphEditorPtr.Pin())
	{
		if (graphEditor.IsValid())
		{
			FString ClipboardContent;
			FPlatformApplicationMisc::ClipboardPaste(ClipboardContent);

			return FEdGraphUtilities::CanImportNodesFromText(graphEditor->GetCurrentGraph(), ClipboardContent);
		}
	}

	return false;
}

void FInputSequenceAssetEditor::OnCreateComment()
{
	if (TSharedPtr<SGraphEditor> graphEditor = GraphEditorPtr.Pin())
	{
		if (graphEditor.IsValid())
		{
			TSharedPtr<FEdGraphSchemaAction> Action = graphEditor->GetCurrentGraph()->GetSchema()->GetCreateCommentAction();
			if (Action.IsValid())
			{
				Action->PerformAction(graphEditor->GetCurrentGraph(), nullptr, FVector2D());
			}
		}
	}
}

bool FInputSequenceAssetEditor::CanCreateComment() const
{
	const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();
	return SelectedNodes.Num() > 0;
}

#undef LOCTEXT_NAMESPACE
#pragma endregion