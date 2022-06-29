// Fill out your copyright notice in the Description page of Project Settings.

#include "Graph/InputSMGraphSchema.h"
#include "Graph/InputSMGraph.h"
#include "Graph/InputSMGraphNode_Base.h"
#include "InputSM.h"

#define LOCTEXT_NAMESPACE "UInputSMGraphSchema"

void UInputSMGraphSchema::CreateDefaultNodesForGraph(UEdGraph& Graph) const
{
	FGraphNodeCreator<UInputSMGraphNode_Root> NodeCreator(Graph);
	UInputSMGraphNode_Root* MyNode = NodeCreator.CreateNode();
	NodeCreator.Finalize();
	
	SetNodeMetaData(MyNode, FNodeMetadata::DefaultGraphNode);

	if (UInputSM* inputSM = Graph.GetTypedOuter<UInputSM>())
	{
		inputSM->GetNodes().Emplace();
	}
}

const FPinConnectionResponse UInputSMGraphSchema::CanCreateConnection(const UEdGraphPin* PinA, const UEdGraphPin* PinB) const
{
	// Make sure the pins are not on the same node
	if (PinA->GetOwningNode() == PinB->GetOwningNode())
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Both are on the same node"));
	}

	// Compare the directions
	bool bDirectionsOK = false;

	if ((PinA->Direction == EGPD_Input) && (PinB->Direction == EGPD_Output))
	{
		bDirectionsOK = true;
	}
	else if ((PinB->Direction == EGPD_Input) && (PinA->Direction == EGPD_Output))
	{
		bDirectionsOK = true;
	}

	if (!bDirectionsOK)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT(""));
	}

	if (UInputSMGraphNode_Base* graphNodeBase = Cast<UInputSMGraphNode_Base>(PinA->GetOwningNode()))
	{
		TArray<TObjectPtr<UEdGraphNode>>& nodes = graphNodeBase->GetGraph()->Nodes;
		graphNodeBase->AddTransition(nodes.IndexOfByKey(PinB->GetOwningNode()), FInputFrameStack());
	}

	return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT(""));
}

void UInputSMGraphSchema::BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotifcation) const
{
	UEdGraphNode* targetNode = TargetPin.GetOwningNode();
	int32 targetNodeIndex = targetNode->GetGraph()->Nodes.IndexOfByKey(targetNode);

	for (UEdGraphPin* sourcePin : TargetPin.LinkedTo)
	{
		UInputSMGraphNode_Base* sourceNode = Cast<UInputSMGraphNode_Base>(sourcePin->GetOwningNode());
		if (sourceNode) sourceNode->RemoveTransition(targetNodeIndex, false);
	}

	Super::BreakPinLinks(TargetPin, bSendsNodeNotifcation);
}

void UInputSMGraphSchema::BreakSinglePinLink(UEdGraphPin* SourcePin, UEdGraphPin* TargetPin) const
{
	UEdGraphNode* targetNode = TargetPin->GetOwningNode();
	int32 targetNodeIndex = targetNode->GetGraph()->Nodes.IndexOfByKey(targetNode);

	UInputSMGraphNode_Base* sourceNode = Cast<UInputSMGraphNode_Base>(SourcePin->GetOwningNode());
	if (sourceNode) sourceNode->RemoveTransition(targetNodeIndex, false);

	Super::BreakSinglePinLink(SourcePin, TargetPin);
}

void UInputSMGraphSchema::DroppedAssetsOnGraph(const TArray<struct FAssetData>& Assets, const FVector2D& GraphPosition, UEdGraph* Graph) const
{
	UInputSMGraph* inputSMGraph = CastChecked<UInputSMGraph>(Graph);

	for (int32 AssetIdx = 0; AssetIdx < Assets.Num(); ++AssetIdx)
	{
		if (UObject* asset = Assets[AssetIdx].GetAsset())
		{
			const FScopedTransaction Transaction(LOCTEXT("InputSMEditorDropClasses", "Input State Machine Editor: Drag and Drop Assets"));

			inputSMGraph->Modify();

			float NodePosX = GraphPosition.X;
			float NodePosY = GraphPosition.Y;

			FGraphNodeCreator<UInputSMGraphNode_State> NodeCreator(*inputSMGraph);
			UInputSMGraphNode_State* MyNode = NodeCreator.CreateNode(true);
			MyNode->NodePosX = NodePosX;
			MyNode->NodePosY = NodePosY;
			NodeCreator.Finalize();

			if (UInputSM* inputSM = Graph->GetTypedOuter<UInputSM>())
			{
				inputSM->GetNodes().Emplace();
			}

			MyNode->SetNodeAsset(asset);

			NodePosY += 400;

			inputSMGraph->NotifyGraphChanged();
		}
	}
}

#undef LOCTEXT_NAMESPACE