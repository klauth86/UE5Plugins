// Fill out your copyright notice in the Description page of Project Settings.

#include "Graph/InputSMGraphNode_Base.h"
#include "Graph/InputSMGraphSchema.h"

bool UInputSMGraphNode_Base::CanCreateUnderSpecifiedSchema(const UEdGraphSchema* DesiredSchema) const
{
	return DesiredSchema->GetClass()->IsChildOf(UInputSMGraphSchema::StaticClass());
}

void UInputSMGraphNode_Base::DestroyNode()
{
	TArray<TObjectPtr<UEdGraphNode>> graphNodes = GetGraph()->Nodes;

	for (UEdGraphNode* graphNode : graphNodes)
	{
		if (UInputSMGraphNode_Base* graphNodeBase = Cast<UInputSMGraphNode_Base>(graphNode))
		{
			graphNodeBase->RemoveTransition(graphNodes.IndexOfByKey(this), true);
		}
	}

	Super::DestroyNode();
}

void UInputSMGraphNode_Base::AddTransition(int32 indexB, const FInputFrameStack& activationStack)
{
	int32 emplacedIndex = Transitions.Emplace();
	
	Transitions[emplacedIndex].TargetIndex = indexB;
	Transitions[emplacedIndex].ActivationStack = activationStack;

	if (UInputSM* inputSM = GetTypedOuter<UInputSM>())
	{
		FInputSMNode& inputSMNode = inputSM->GetNodes()[GetGraph()->Nodes.IndexOfByKey(this)];
		
		emplacedIndex = inputSMNode.Transitions.Emplace();
		
		inputSMNode.Transitions[emplacedIndex].TargetIndex = indexB;
		inputSMNode.Transitions[emplacedIndex].ActivationStack = activationStack;
	}
}

void UInputSMGraphNode_Base::RemoveTransition(int32 indexB, bool decrementOthers)
{
	Transitions.RemoveAll([indexB](const FInputSMTransition& transition) { return transition.TargetIndex == indexB; });

	if (decrementOthers)
	{
		for (FInputSMTransition& transition : Transitions)
		{
			if (transition.TargetIndex >= indexB)
			{
				transition.TargetIndex--;
			}
		}
	}

	if (UInputSM* inputSM = GetTypedOuter<UInputSM>())
	{
		FInputSMNode& inputSMNode = inputSM->GetNodes()[GetGraph()->Nodes.IndexOfByKey(this)];
		
		inputSMNode.Transitions.RemoveAll([indexB](const FInputSMTransition& transition) { return transition.TargetIndex == indexB; });

		if (decrementOthers)
		{
			for (FInputSMTransition& transition : inputSMNode.Transitions)
			{
				if (transition.TargetIndex >= indexB)
				{
					transition.TargetIndex--;
				}
			}
		}
	}
}

void UInputSMGraphNode_Base::SetNodeAsset(UObject* nodeAsset)
{
	if (UInputSM* inputSM = GetTypedOuter<UInputSM>())
	{
		NodeAsset = nodeAsset;
		inputSM->GetNodes()[GetGraph()->Nodes.IndexOfByKey(this)].NodeAsset = nodeAsset;
	}
}

FText UInputSMGraphNode_Root::GetNodeTitle(ENodeTitleType::Type TitleType) const { return NSLOCTEXT("UInputSMGraphNode_Root", "NodeTitle", "ENTRY"); }

void UInputSMGraphNode_Root::AllocateDefaultPins()
{
	UEdGraphPin* Outputs = CreatePin(EGPD_Output, TEXT("Transition"), TEXT("Next"));
}

FText UInputSMGraphNode_State::GetNodeTitle(ENodeTitleType::Type TitleType) const { return NSLOCTEXT("UInputSMGraphNode_State", "NodeTitle", "State"); }

void UInputSMGraphNode_State::AllocateDefaultPins()
{
	UEdGraphPin* Inputs = CreatePin(EGPD_Input, TEXT("Transition"), TEXT("In"));
	UEdGraphPin* Outputs = CreatePin(EGPD_Output, TEXT("Transition"), TEXT("Next"));
}