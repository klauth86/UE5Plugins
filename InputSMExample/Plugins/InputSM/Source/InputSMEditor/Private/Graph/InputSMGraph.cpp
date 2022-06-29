// Fill out your copyright notice in the Description page of Project Settings.

#include "Graph/InputSMGraph.h"
#include "Graph/InputSMGraphSchema.h"
#include "Graph/InputSMGraphNode_Base.h"

UInputSMGraph::UInputSMGraph(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Schema = UInputSMGraphSchema::StaticClass();
}

void UInputSMGraph::Initialize() {
	LockUpdates();

	////// TODO Spawn missed nodes if should
	
	UnlockUpdates();
}

void UInputSMGraph::UpdateAsset(int32 UpdateFlags)
{
	if (IsLocked()) return;

	if (UInputSM* inputSM = GetTypedOuter<UInputSM>())
	{
		TArray<FInputSMNode>& inputSMNodes = inputSM->GetNodes();

		inputSMNodes.Reset(Nodes.Num());

		for (size_t nodeIndex = 0; nodeIndex < Nodes.Num(); nodeIndex++)
		{
			if (UInputSMGraphNode_Base* graphNodeBase = Cast<UInputSMGraphNode_Base>(Nodes[nodeIndex]))
			{
				inputSMNodes.EmplaceAt(nodeIndex);
				FInputSMNode& inputSMNode = inputSMNodes[nodeIndex];
				inputSMNode.NodeAsset = graphNodeBase->GetNodeAsset();
				inputSMNode.Transitions = graphNodeBase->GetTransitions();
			}
		}
	}
}