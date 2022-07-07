// Fill out your copyright notice in the Description page of Project Settings.

#include "Graph/InputSMGraphSchema.h"
#include "Graph/InputSMGraph.h"
#include "Graph/InputSMGraphNode_Base.h"
#include "Graph/InputSMGraphSchemaAction_NewStateNode.h"
#include "Classes/EditorStyleSettings.h"
#include "EdGraphUtilities.h"

#define LOCTEXT_NAMESPACE "UInputSMGraphSchema"

UInputSMGraph::UInputSMGraph(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Schema = UInputSMGraphSchema::StaticClass();
}

template<class T>
TSharedPtr<T> AddNewActionAs(FGraphContextMenuBuilder& ContextMenuBuilder, const FText& Category, const FText& MenuDesc, const FText& Tooltip, const int32 Grouping = 0)
{
	TSharedPtr<T> NewStateNode(new T(Category, MenuDesc, Tooltip, Grouping));
	ContextMenuBuilder.AddAction(NewStateNode);
	return NewStateNode;
}

UEdGraphNode* FInputSMGraphSchemaAction_NewStateNode::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
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

void FInputSMGraphSchemaAction_NewStateNode::AddReferencedObjects(FReferenceCollector& Collector)
{
	FEdGraphSchemaAction::AddReferencedObjects(Collector);

	// These don't get saved to disk, but we want to make sure the objects don't get GC'd while the action array is around
	Collector.AddReferencedObject(NodeTemplate);
}

FLinearColor UInputSMGraphSchema::GetPinTypeColor(const FEdGraphPinType& PinType) const
{
	return FLinearColor::White;
}

void UInputSMGraphSchema::CreateDefaultNodesForGraph(UEdGraph& Graph) const
{
	FGraphNodeCreator<UInputSMGraphNode_Entry> NodeCreator(Graph);
	UInputSMGraphNode_Entry* MyNode = NodeCreator.CreateNode();
	NodeCreator.Finalize();
	
	SetNodeMetaData(MyNode, FNodeMetadata::DefaultGraphNode);
}

const FPinConnectionResponse UInputSMGraphSchema::CanCreateConnection(const UEdGraphPin* PinA, const UEdGraphPin* PinB) const
{
	// Make sure the pins are not on the same node
	if (PinA->GetOwningNode() == PinB->GetOwningNode())
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Both are on the same node"));
	}

	// Connect entry node to a state is OK
	const bool bPinAIsEntry = PinA->GetOwningNode()->IsA(UInputSMGraphNode_Entry::StaticClass());
	const bool bPinBIsEntry = PinB->GetOwningNode()->IsA(UInputSMGraphNode_Entry::StaticClass());
	const bool bPinAIsStateNode = PinA->GetOwningNode()->IsA(UInputSMGraphNode_State::StaticClass());
	const bool bPinBIsStateNode = PinB->GetOwningNode()->IsA(UInputSMGraphNode_State::StaticClass());

	if (bPinAIsEntry || bPinBIsEntry)
	{
		if (bPinAIsEntry && bPinBIsStateNode)
		{
			return FPinConnectionResponse(CONNECT_RESPONSE_BREAK_OTHERS_A, TEXT(""));
		}

		if (bPinBIsEntry && bPinAIsStateNode)
		{
			return FPinConnectionResponse(CONNECT_RESPONSE_BREAK_OTHERS_B, TEXT(""));
		}

		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Entry must connect to a state node"));
	}

	const bool bPinAIsTransition = PinA->GetOwningNode()->IsA(UInputSMGraphNode_Transition::StaticClass());
	const bool bPinBIsTransition = PinB->GetOwningNode()->IsA(UInputSMGraphNode_Transition::StaticClass());

	if (bPinAIsTransition || bPinBIsTransition)
	{
		if (bPinAIsTransition && bPinBIsStateNode)
		{
			return FPinConnectionResponse(CONNECT_RESPONSE_BREAK_OTHERS_A, TEXT(""));
		}

		if (bPinBIsTransition && bPinAIsStateNode)
		{
			return FPinConnectionResponse(CONNECT_RESPONSE_BREAK_OTHERS_B, TEXT(""));
		}

		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Transition must connect to a state node"));
	}

	return FPinConnectionResponse(CONNECT_RESPONSE_MAKE_WITH_CONVERSION_NODE, TEXT("Create a transition"));
}

bool UInputSMGraphSchema::TryCreateConnection(UEdGraphPin* PinA, UEdGraphPin* PinB) const
{
	const bool bPinAIsEntry = PinA->GetOwningNode()->IsA(UInputSMGraphNode_Entry::StaticClass());
	const bool bPinBIsEntry = PinB->GetOwningNode()->IsA(UInputSMGraphNode_Entry::StaticClass());

	if (bPinAIsEntry)
	{
		UInputSMGraphNode_State* NodeB = Cast<UInputSMGraphNode_State>(PinB->GetOwningNode());
		if (NodeB) PinB = NodeB->GetInputPin();
	}
	else if (bPinBIsEntry)
	{
		UInputSMGraphNode_State* NodeA = Cast<UInputSMGraphNode_State>(PinA->GetOwningNode());
		if (NodeA) PinA = NodeA->GetInputPin();
	}
	else
	{
		UInputSMGraphNode_State* NodeA = Cast<UInputSMGraphNode_State>(PinA->GetOwningNode());
		if (NodeA) PinA = NodeA->GetOutputPin();

		UInputSMGraphNode_State* NodeB = Cast<UInputSMGraphNode_State>(PinB->GetOwningNode());
		if (NodeB) PinB = NodeB->GetInputPin();

		for (UEdGraphPin* inputPin : PinA->LinkedTo)
		{
			for (UEdGraphPin* outputPin : PinB->LinkedTo)
			{
				if (inputPin->GetOwningNode() == outputPin->GetOwningNode()) return false;
			}
		}
	}

	return UEdGraphSchema::TryCreateConnection(PinA, PinB);
}

bool UInputSMGraphSchema::CreateAutomaticConversionNodeAndConnections(UEdGraphPin* PinA, UEdGraphPin* PinB) const
{
	UInputSMGraphNode_State* NodeA = Cast<UInputSMGraphNode_State>(PinA->GetOwningNode());
	UInputSMGraphNode_State* NodeB = Cast<UInputSMGraphNode_State>(PinB->GetOwningNode());

	if ((NodeA != NULL) && (NodeB != NULL)
		&& (NodeA->GetInputPin() != NULL) && (NodeA->GetOutputPin() != NULL)
		&& (NodeB->GetInputPin() != NULL) && (NodeB->GetOutputPin() != NULL))
	{
		UInputSMGraphNode_Transition* TransitionNode = FInputSMGraphSchemaAction_NewStateNode::SpawnNodeFromTemplate<UInputSMGraphNode_Transition>(NodeA->GetGraph(), NewObject<UInputSMGraphNode_Transition>(), FVector2D(0.0f, 0.0f), false);

		if (PinA->Direction == EGPD_Output)
		{
			TransitionNode->CreateConnections(NodeA, NodeB);
		}
		else
		{
			TransitionNode->CreateConnections(NodeB, NodeA);
		}

		return true;
	}

	return false;
}

void UInputSMGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	// Add state node
	{
		TSharedPtr<FInputSMGraphSchemaAction_NewStateNode> Action = AddNewActionAs<FInputSMGraphSchemaAction_NewStateNode>(ContextMenuBuilder, FText::GetEmpty(), LOCTEXT("AddState", "Add State..."), LOCTEXT("AddStateTooltip", "A new state"));
		Action->NodeTemplate = NewObject<UInputSMGraphNode_State>(ContextMenuBuilder.OwnerOfTemporaries);
	}

	// Entry point (only if doesn't already exist)
	{
		bool bHasEntry = false;
		for (auto NodeIt = ContextMenuBuilder.CurrentGraph->Nodes.CreateConstIterator(); NodeIt; ++NodeIt)
		{
			UEdGraphNode* Node = *NodeIt;
			if (const UInputSMGraphNode_Entry* StateNode = Cast<UInputSMGraphNode_Entry>(Node))
			{
				bHasEntry = true;
				break;
			}
		}

		if (!bHasEntry)
		{
			TSharedPtr<FInputSMGraphSchemaAction_NewStateNode> Action = AddNewActionAs<FInputSMGraphSchemaAction_NewStateNode>(ContextMenuBuilder, FText::GetEmpty(), LOCTEXT("AddEntry", "Add Entry..."), LOCTEXT("AddRootTooltip", "Define State Machine's Root"));
			Action->NodeTemplate = NewObject<UInputSMGraphNode_Entry>(ContextMenuBuilder.OwnerOfTemporaries);
		}
	}
}

void UInputSMGraphSchema::DroppedAssetsOnGraph(const TArray<struct FAssetData>& Assets, const FVector2D& GraphPosition, UEdGraph* Graph) const
{
	UInputSMGraph* inputSMGraph = CastChecked<UInputSMGraph>(Graph);

	float NodePosX = GraphPosition.X;
	float NodePosY = GraphPosition.Y;

	for (int32 AssetIdx = 0; AssetIdx < Assets.Num(); ++AssetIdx)
	{
		if (UObject* asset = Assets[AssetIdx].GetAsset())
		{
			const FScopedTransaction Transaction(LOCTEXT("InputSMEditorDropClasses", "Input State Machine Editor: Drag and Drop Assets"));

			inputSMGraph->Modify();

			FGraphNodeCreator<UInputSMGraphNode_State> NodeCreator(*inputSMGraph);
			UInputSMGraphNode_State* MyNode = NodeCreator.CreateNode(true);
			MyNode->NodePosX = NodePosX;
			MyNode->NodePosY = NodePosY;
			NodeCreator.Finalize();

			MyNode->SetStateAsset(asset);

			NodePosY += 300;

			inputSMGraph->NotifyGraphChanged();
		}
	}
}

#undef LOCTEXT_NAMESPACE