// Fill out your copyright notice in the Description page of Project Settings.

#include "Graph/InputSequenceGraphSchema.h"
#include "Graph/InputSequenceGraph.h"
#include "Graph/InputSequenceGraphNode_Start.h"
#include "Graph/InputSequenceGraphNode_Finish.h"
#include "Graph/InputSequenceGraphNode_State.h"
#include "Graph/SInputSequenceGraphNode_State.h"
#include "Graph/InputSequenceGraphFactories.h"
#include "KismetPins/SGraphPinExec.h"
#include "Graph/SGraphPin_ActionAxis.h"
#include "Classes/EditorStyleSettings.h"
#include "GameFramework/InputSettings.h"

#define LOCTEXT_NAMESPACE "UInputSequenceGraphSchema"

template<class T>
TSharedPtr<T> AddNewActionAs(FGraphContextMenuBuilder& ContextMenuBuilder, const FText& Category, const FText& MenuDesc, const FText& Tooltip, const int32 Grouping = 0)
{
	TSharedPtr<T> newNode(new T(Category, MenuDesc, Tooltip, Grouping));
	ContextMenuBuilder.AddAction(newNode);
	return newNode;
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

TSharedPtr<SGraphNode> FInputSequenceGraphNodeFactory::CreateNode(UEdGraphNode* InNode) const
{
	if (UInputSequenceGraphNode_State* stateNode = Cast<UInputSequenceGraphNode_State>(InNode))
	{
		return SNew(SInputSequenceGraphNode_State);
	}

	return nullptr;
}

TSharedPtr<SGraphPin> FInputSequenceGraphPinFactory::CreatePin(UEdGraphPin* InPin) const
{
	if (InPin->GetSchema()->IsA<UInputSequenceGraphSchema>())
	{
		if (InPin->PinType.PinCategory == UInputSequenceGraphSchema::PC_Exec) return SNew(SGraphPinExec, InPin);

		if (InPin->PinType.PinCategory == UInputSequenceGraphSchema::PC_ActionAxis) return SNew(SGraphPin_ActionAxis, InPin);
	}

	return SNew(SGraphPin, InPin);
}

UInputSequenceGraph::UInputSequenceGraph(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	Schema = UInputSequenceGraphSchema::StaticClass();
}

const FName UInputSequenceGraphSchema::PC_Exec = FName("UInputSequenceGraphSchema_PC_Exec");

const FName UInputSequenceGraphSchema::PC_ActionAxis = FName("UInputSequenceGraphSchema_PC_ActionAxis");

void UInputSequenceGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	// Add state node
	TSharedPtr<FInputSequenceGraphSchemaAction_NewNode> Action = AddNewActionAs<FInputSequenceGraphSchemaAction_NewNode>(ContextMenuBuilder, FText::GetEmpty(), LOCTEXT("AddNode_State", "Add State Node..."), LOCTEXT("AddNode_State_Tooltip", "A new State Node"));
	Action->NodeTemplate = NewObject<UInputSequenceGraphNode_State>(ContextMenuBuilder.OwnerOfTemporaries);

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

void UInputSequenceGraphNode_State::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, UInputSequenceGraphSchema::PC_Exec, NAME_None);
	CreatePin(EGPD_Output, UInputSequenceGraphSchema::PC_Exec, NAME_None);

	const TArray<FInputActionKeyMapping>& actionMappings = UInputSettings::GetInputSettings()->GetActionMappings();
	for (const FInputActionKeyMapping& actionMapping : actionMappings)
	{
		UEdGraphPin* InputPin = CreatePin(EGPD_Input, UInputSequenceGraphSchema::PC_ActionAxis, actionMapping.ActionName);
		InputPin->bHidden = true;

		UEdGraphPin* OutputPin = CreatePin(EGPD_Output, UInputSequenceGraphSchema::PC_ActionAxis, actionMapping.ActionName);
	}
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

void UInputSequenceGraphNode_State::NodeConnectionListChanged()
{
	if (UEdGraphPin* flowInputPin = FindPin(NAME_None, EGPD_Input))
	{
		if (flowInputPin->LinkedTo.Num() > 0)
		{
			if (UInputSequenceGraphNode_State* stateNode = Cast<UInputSequenceGraphNode_State>(flowInputPin->LinkedTo[0]->GetOwningNode()))
			{


				return;
			}
		}

		for (UEdGraphPin* pin : Pins)
		{
			if (pin->PinName != NAME_None && pin->Direction == EGPD_Input)
			{
				pin->bHidden = true;
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE