// Fill out your copyright notice in the Description page of Project Settings.

#include "Graph/InputSequenceGraphSchema.h"
#include "Graph/InputSequenceGraph.h"
#include "Graph/InputSequenceGraphNode_Start.h"
#include "Graph/InputSequenceGraphNode_Finish.h"
#include "Graph/InputSequenceGraphNode_Press.h"
#include "Graph/InputSequenceGraphNode_Release.h"
#include "Graph/InputSequenceGraphNode_State.h"
#include "Graph/SInputSequenceGraphNode_State.h"
#include "Graph/InputSequenceGraphFactories.h"
#include "KismetPins/SGraphPinExec.h"
#include "Graph/SGraphPin_ActionAxis.h"
#include "Graph/SGraphPin_Add.h"
#include "SGraphActionMenu.h"
#include "Classes/EditorStyleSettings.h"
#include "GameFramework/InputSettings.h"
#include "ConnectionDrawingPolicy.h"

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
			Params.bUserFlag1 ? ArrowLayerID : WireLayerID,
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
		const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "K2_AddPin", "Add Pin"));
		
		//////ParentGraph->Modify();
		//////
		//////if (FromPin)
		//////{
		//////	FromPin->Modify();
		//////}
	}

	return ResultNode;
}

UInputSequenceGraphNode_State* GetNextStateNode(const UEdGraphNode* node)
{
	if (UEdGraphPin* flowOutputPin = node->FindPin(NAME_None, EGPD_Output))
	{
		if (flowOutputPin->LinkedTo.IsValidIndex(0))
		{
			return Cast<UInputSequenceGraphNode_State>(flowOutputPin->LinkedTo[0]->GetOwningNode());
		}
	}

	return nullptr;
}

void ShowReleasePins(FName draggedPinName, const UEdGraphNode* draggedPinNode)
{
	const UEdGraphNode* currentNode = draggedPinNode ? GetNextStateNode(draggedPinNode) : nullptr;
	while (currentNode)
	{
		UEdGraphPin* releasePin = currentNode->FindPin(draggedPinName, EGPD_Input);
		releasePin->PinType.PinSubCategory = NAME_None;

		UEdGraphPin* pressPin = currentNode->FindPin(draggedPinName, EGPD_Output);

		currentNode = pressPin->HasAnyConnections() ? nullptr : GetNextStateNode(currentNode);
	}
}

void HideReleasePins(FName draggedPinName, const UEdGraphNode* draggedPinNode)
{
	const UEdGraphNode* currentNode = draggedPinNode ? GetNextStateNode(draggedPinNode) : nullptr;
	while (currentNode)
	{
		UEdGraphPin* releasePin = currentNode->FindPin(draggedPinName, EGPD_Input);
		releasePin->PinType.PinSubCategory = UInputSequenceGraphSchema::PSC_Hidden;

		currentNode = GetNextStateNode(currentNode);
	}
}

void OnReleaseAction(FName draggedPinName, const UEdGraphNode* pressedPinNode)
{
	const UEdGraphNode* currentNode = pressedPinNode ? GetNextStateNode(pressedPinNode) : nullptr;
	while (currentNode)
	{
		UEdGraphPin* releasePin = currentNode->FindPin(draggedPinName, EGPD_Input);
		releasePin->bNotConnectable = false;

		UEdGraphPin* pressPin = currentNode->FindPin(draggedPinName, EGPD_Output);
		pressPin->bNotConnectable = false;

		currentNode = pressPin->HasAnyConnections() ? nullptr : GetNextStateNode(currentNode);
	}
}

void OnPressAction(FName draggedPinName, const UEdGraphNode* pressedPinNode, const UEdGraphNode* releasedPinNode)
{
	const UEdGraphNode* currentNode = pressedPinNode ? GetNextStateNode(pressedPinNode) : nullptr;
	while (currentNode && currentNode != releasedPinNode)
	{
		UEdGraphPin* releasePin = currentNode->FindPin(draggedPinName, EGPD_Input);
		releasePin->bNotConnectable = true;

		UEdGraphPin* pressPin = currentNode->FindPin(draggedPinName, EGPD_Output);
		pressPin->bNotConnectable = true;

		currentNode = GetNextStateNode(currentNode);
	}
}

//////TSharedPtr<SGraphNode> FInputSequenceGraphNodeFactory::CreateNode(UEdGraphNode* InNode) const
//////{
//////	if (UInputSequenceGraphNode_State* stateNode = Cast<UInputSequenceGraphNode_State>(InNode))
//////	{
//////		return SNew(SInputSequenceGraphNode_State, stateNode);
//////	}
//////
//////	return nullptr;
//////}

TSharedPtr<SGraphPin> FInputSequenceGraphPinFactory::CreatePin(UEdGraphPin* InPin) const
{
	if (InPin->GetSchema()->IsA<UInputSequenceGraphSchema>())
	{
		if (InPin->PinType.PinCategory == UInputSequenceGraphSchema::PC_Exec) return SNew(SGraphPinExec, InPin);

		if (InPin->PinType.PinCategory == UInputSequenceGraphSchema::PC_Add) return SNew(SGraphPin_Add, InPin);

		if (InPin->PinType.PinCategory == UInputSequenceGraphSchema::PC_ActionAxis) return SNew(SGraphPin_ActionAxis, InPin);
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

const FName UInputSequenceGraphSchema::PC_Add = FName("UInputSequenceGraphSchema_PC_Add");

const FName UInputSequenceGraphSchema::PC_ActionAxis = FName("UInputSequenceGraphSchema_PC_ActionAxis");

const FName UInputSequenceGraphSchema::PSC_Hidden = FName("UInputSequenceGraphSchema_PSC_Hidden");

void UInputSequenceGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	if (ContextMenuBuilder.FromPin)
	{
		HideReleasePins(ContextMenuBuilder.FromPin->PinName, ContextMenuBuilder.FromPin->GetOwningNode());
	}

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

	if (pinA->PinName != pinB->PinName) return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("PinsMismatched", "The pin types are mismatched (Flow pins should be connected to Flow pins, Key pins - to Key pins)"));

	return FPinConnectionResponse(CONNECT_RESPONSE_BREAK_OTHERS_AB, TEXT(""));
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

void UInputSequenceGraphNode_Start::AllocateDefaultPins()
{
	UEdGraphPin* OutputPin = CreatePin(EGPD_Output, UInputSequenceGraphSchema::PC_Exec, NAME_None);
}

void UInputSequenceGraphNode_Finish::AllocateDefaultPins()
{
	UEdGraphPin* InputPin = CreatePin(EGPD_Input, UInputSequenceGraphSchema::PC_Exec, NAME_None);
}

void UInputSequenceGraphNode_Press::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, UInputSequenceGraphSchema::PC_Exec, NAME_None);
	CreatePin(EGPD_Output, UInputSequenceGraphSchema::PC_Exec, NAME_None);

	CreatePin(EGPD_Output, UInputSequenceGraphSchema::PC_Add, NAME_None);
}

void UInputSequenceGraphNode_Release::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, UInputSequenceGraphSchema::PC_Exec, NAME_None);
	CreatePin(EGPD_Output, UInputSequenceGraphSchema::PC_Exec, NAME_None);
}

void UInputSequenceGraphNode_State::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, UInputSequenceGraphSchema::PC_Exec, NAME_None);
	CreatePin(EGPD_Output, UInputSequenceGraphSchema::PC_Exec, NAME_None);

	CreatePin(EGPD_Output, UInputSequenceGraphSchema::PC_Add, NAME_None);
}

void UInputSequenceGraphNode_State::ReconstructNode()
{
	const TArray<FInputActionKeyMapping>& actionMappings = UInputSettings::GetInputSettings()->GetActionMappings();

	TArray<UEdGraphPin*> pinsToRemove;
	for (UEdGraphPin* pin : Pins)
	{
		FName pinName = pin->PinName;
		if (pinName != NAME_None &&
			!actionMappings.ContainsByPredicate([pinName](const FInputActionKeyMapping& actionMapping) { return actionMapping.ActionName == pinName; }))
		{
			pinsToRemove.Add(pin);
		}
	}

	for (UEdGraphPin* pin : pinsToRemove) RemovePin(pin);

	FCreatePinParams params = FCreatePinParams();
	params.Index = 2; // Flow pins are first two pins in any case

	int32 newPins = 0;

	for (size_t i = 0; i < actionMappings.Num(); i++)
	{
		const FInputActionKeyMapping& actionMapping = actionMappings[i];
		
		if (!FindPin(actionMapping.ActionName))
		{
			params.Index++;
			CreatePin(EGPD_Input, UInputSequenceGraphSchema::PC_ActionAxis, actionMapping.ActionName, params);
			newPins++;

			params.Index++;
			CreatePin(EGPD_Output, UInputSequenceGraphSchema::PC_ActionAxis, actionMapping.ActionName, params);
			newPins++;
		}
		else
		{
			params.Index += 2;
		}
	}

	if (pinsToRemove.Num() * newPins > 0) Modify();
}

void UInputSequenceGraphNode_State::AutowireNewNode(UEdGraphPin* FromPin)
{
	if (FromPin)
	{
		if (FromPin->Direction == EGPD_Output)
		{
			if (UEdGraphPin* OtherPin = FindPin(FromPin->PinName, EGPD_Input))
			{
				bool connectionCreated = GetSchema()->TryCreateConnection(FromPin, OtherPin);

				if (FromPin->PinName != NAME_None) // Connect flow pins if before we connected input action pins
				{
					UEdGraphPin* FromFlowPin = FromPin->GetOwningNode()->FindPin(NAME_None, EGPD_Output);
					UEdGraphPin* OtherFlowPin = FindPin(NAME_None, EGPD_Input);
					connectionCreated |= GetSchema()->TryCreateConnection(FromFlowPin, OtherFlowPin);
				}

				if (connectionCreated) FromPin->GetOwningNode()->NodeConnectionListChanged();
			}
		}
		else if (FromPin->Direction == EGPD_Input)
		{
			if (UEdGraphPin* OtherPin = FindPin(FromPin->PinName, EGPD_Output))
			{
				bool connectionCreated = GetSchema()->TryCreateConnection(FromPin, OtherPin);

				if (FromPin->PinName != NAME_None) // Connect flow pins if before we connected input action pins
				{
					UEdGraphPin* FromFlowPin = FromPin->GetOwningNode()->FindPin(NAME_None, EGPD_Input);
					UEdGraphPin* OtherFlowPin = FindPin(NAME_None, EGPD_Output);
					connectionCreated |= GetSchema()->TryCreateConnection(FromFlowPin, OtherFlowPin);
				}

				if (connectionCreated) FromPin->GetOwningNode()->NodeConnectionListChanged();
			}
		}
	}
}

void UInputSequenceGraphNode_State::PinConnectionListChanged(UEdGraphPin* Pin)
{
	if (Pin->PinName != NAME_None)
	{
		if (Pin->Direction == EGPD_Output)
		{
			if (!Pin->HasAnyConnections())
			{
				OnReleaseAction(Pin->PinName, Pin->GetOwningNode());
			}
			else
			{
				HideReleasePins(Pin->PinName, Pin->GetOwningNode());
				OnPressAction(Pin->PinName, Pin->GetOwningNode(), Pin->LinkedTo[0]->GetOwningNode());
			}
		}
	}
	else if (!Pin->HasAnyConnections()) // If broke flow link, break all action links
	{
		for (UEdGraphPin* otherPin : Pins)
		{
			if (otherPin->PinName != NAME_None && otherPin->Direction == Pin->Direction)
			{
				otherPin->BreakAllPinLinks(true);
			}
		}
	}
}

TSharedRef<FDragDropOperation> SGraphPin_ActionAxis::SpawnPinDragEvent(const TSharedRef<class SGraphPanel>& InGraphPanel, const TArray< TSharedRef<SGraphPin> >& InStartingPins)
{
	if (UInputSequenceGraphNode_State* stateNode = Cast<UInputSequenceGraphNode_State>(GetPinObj()->GetOwningNode()))
	{
		ShowReleasePins(GetPinObj()->PinName, GetPinObj()->GetOwningNode());
	}

	return SGraphPin::SpawnPinDragEvent(InGraphPanel, InStartingPins);
}

EVisibility SGraphPin_ActionAxis::Visibility_Raw() const
{
	if (GetPinObj()->bNotConnectable) return EVisibility::Hidden;

	if (UInputSequenceGraphSchema::PSC_Hidden == GetPinObj()->PinType.PinSubCategory && !GetPinObj()->HasAnyConnections()) return EVisibility::Hidden;
	
	if (UseLowDetailPinNames()) return EVisibility::HitTestInvisible;
	
	return EVisibility::Visible;
}

EVisibility SGraphPin_Add::Visibility_Raw() const
{
	if (UInputSequenceGraphSchema::PSC_Hidden == GetPinObj()->PinType.PinSubCategory) return EVisibility::Collapsed;

	if (UseLowDetailPinNames()) return EVisibility::HitTestInvisible;

	return EVisibility::Visible;
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
	SLATE_ARGUMENT(UEdGraphPin*, Pin)
	//~ End Required Args
	SLATE_ARGUMENT(bool, AutoExpandMenu)
	SLATE_END_ARGS();

	void Construct(const FArguments& InArgs)
	{
		this->Node = InArgs._Node;
		this->Pin = InArgs._Pin;

		SInputSequenceParameterMenu::FArguments SuperArgs;
		SuperArgs._AutoExpandMenu = InArgs._AutoExpandMenu;
		SInputSequenceParameterMenu::Construct(SuperArgs);
	}

protected:
	
	virtual void CollectAllActions(FGraphActionListBuilderBase& OutAllActions) override
	{
		const TArray<FInputActionKeyMapping>& actionMappings = UInputSettings::GetInputSettings()->GetActionMappings();

		for (const FInputActionKeyMapping& actionMapping : actionMappings)
		{
			if (!Node->FindPin(actionMapping.ActionName))
			{
				TSharedPtr<FInputSequenceGraphSchemaAction_AddPin> Action(new FInputSequenceGraphSchemaAction_AddPin(FText::GetEmpty(), FText::FromName(actionMapping.ActionName), FText::Format(LOCTEXT("AddPin_Action_Tooltip", "Add Action pin for {0}"), FText::FromName(actionMapping.ActionName)), 0));
				Action->ActionName = actionMapping.ActionName;
				OutAllActions.AddAction(Action);
			}
		}
	}

	virtual void OnActionSelected(const TArray<TSharedPtr<FEdGraphSchemaAction>>& SelectedActions, ESelectInfo::Type InSelectionType) override
	{
		if (InSelectionType == ESelectInfo::OnMouseClick || InSelectionType == ESelectInfo::OnKeyPress || SelectedActions.Num() == 0)
		{
			for (int32 ActionIndex = 0; ActionIndex < SelectedActions.Num(); ActionIndex++)
			{
				FSlateApplication::Get().DismissAllMenus();
				SelectedActions[ActionIndex]->PerformAction(Node->GetGraph(), Pin, FVector2D::ZeroVector);
			}
		}
	}

private:

	UEdGraphNode* Node;
	UEdGraphPin* Pin;
};

TSharedRef<SWidget> SGraphPin_Add::OnGetAddButtonMenuContent()
{
	UEdGraphPin* pin = GetPinObj();

	UEdGraphNode* node = pin->GetOwningNode();

	TSharedRef<SInputSequenceParameterMenu_Pin> MenuWidget = SNew(SInputSequenceParameterMenu_Pin).Node(node).Pin(pin);

	AddButton->SetMenuContentWidgetToFocus(MenuWidget->GetSearchBox());
	
	return MenuWidget;
}

#undef LOCTEXT_NAMESPACE