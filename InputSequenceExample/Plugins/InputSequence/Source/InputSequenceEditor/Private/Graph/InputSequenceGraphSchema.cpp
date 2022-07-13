// Fill out your copyright notice in the Description page of Project Settings.

#include "Graph/InputSequenceGraphSchema.h"
#include "Graph/InputSequenceGraph.h"
#include "Graph/InputSequenceGraphNode_Start.h"
#include "Graph/InputSequenceGraphNode_Finish.h"
#include "Graph/InputSequenceGraphFactories.h"
#include "KismetPins/SGraphPinExec.h"

TSharedPtr<SGraphPin> FInputSequenceGraphPinFactory::CreatePin(UEdGraphPin* InPin) const
{
	if (InPin->GetSchema()->IsA<UInputSequenceGraphSchema>() && InPin->PinType.PinCategory == UInputSequenceGraphSchema::PC_Exec)
	{
		return SNew(SGraphPinExec, InPin);
	}

	return nullptr;
}

UInputSequenceGraph::UInputSequenceGraph(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	Schema = UInputSequenceGraphSchema::StaticClass();
}

const FName UInputSequenceGraphSchema::PC_Exec = FName("UInputSequenceGraphSchema_PC_Exec");

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