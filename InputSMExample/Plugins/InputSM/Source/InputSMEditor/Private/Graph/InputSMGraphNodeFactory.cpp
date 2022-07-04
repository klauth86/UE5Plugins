// Fill out your copyright notice in the Description page of Project Settings.

#include "Graph/InputSMGraphNodeFactory.h"
#include "Graph/InputSMConnectionDrawingPolicy.h"
#include "Graph/InputSMGraphNode_Base.h"
#include "Graph/SInputSMGraphNode_State.h"
#include "Graph/SInputSMGraphNode_Entry.h"
#include "Graph/SInputSMGraphNode_Transition.h"
#include "Graph/InputSMGraphSchema.h"
#include "KismetPins/SGraphPinExec.h"

TSharedPtr<SGraphNode> FInputSMGraphNodeFactory::CreateNode(UEdGraphNode* InNode) const
{
	if (UInputSMGraphNode_Base* graphNodeBase = Cast<UInputSMGraphNode_Base>(InNode))
	{
		if (UInputSMGraphNode_Entry* graphNodeEntry = Cast<UInputSMGraphNode_Entry>(InNode))
		{
			return SNew(SInputSMGraphNode_Entry, graphNodeEntry);
		}
		else if (UInputSMGraphNode_State* graphNodeState = Cast<UInputSMGraphNode_State>(InNode))
		{
			return SNew(SInputSMGraphNode_State, graphNodeState);
		}
		else if (UInputSMGraphNode_Transition* graphNodeTransition = Cast<UInputSMGraphNode_Transition>(InNode))
		{
			return SNew(SInputSMGraphNode_Transition, graphNodeTransition);
		}
	}

	return nullptr;
}

TSharedPtr<SGraphPin> FInputSMGraphPinFactory::CreatePin(UEdGraphPin* InPin) const
{
	if (InPin->GetSchema()->IsA<UInputSMGraphSchema>() && InPin->PinType.PinCategory == UInputSMGraphNode_Entry::EntryOutputPinName)
	{
		return SNew(SGraphPinExec, InPin);
	}

	return nullptr;
}

FConnectionDrawingPolicy* FInputSMGraphPinConnectionFactory::CreateConnectionPolicy(const UEdGraphSchema* Schema, int32 InBackLayerID, int32 InFrontLayerID, float ZoomFactor, const class FSlateRect& InClippingRect, class FSlateWindowElementList& InDrawElements, UEdGraph* InGraphObj) const
{
	if (Schema->IsA<UInputSMGraphSchema>())
	{
		return new FInputSMConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, ZoomFactor, InClippingRect, InDrawElements, InGraphObj);;
	}

	return nullptr;
}