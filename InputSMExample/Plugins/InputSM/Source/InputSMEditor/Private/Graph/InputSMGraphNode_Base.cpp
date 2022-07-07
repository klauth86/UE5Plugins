// Fill out your copyright notice in the Description page of Project Settings.

#include "Graph/InputSMGraphNode_Base.h"
#include "Graph/InputSMGraphSchema.h"

UInputSMGraphNode_Base::FTitleChanged UInputSMGraphNode_Base::OnTitleChanged;

bool UInputSMGraphNode_Base::CanCreateUnderSpecifiedSchema(const UEdGraphSchema* DesiredSchema) const
{
	return DesiredSchema->GetClass()->IsChildOf(UInputSMGraphSchema::StaticClass());
}

void UInputSMGraphNode_Base::GetTransitionList(TArray<UInputSMGraphNode_Transition*>& OutTransitions, bool bWantSortedList /*= false*/) const
{
    // Normal transitions
    for (int32 LinkIndex = 0; LinkIndex < Pins[1]->LinkedTo.Num(); ++LinkIndex)
    {
        UEdGraphNode* TargetNode = Pins[1]->LinkedTo[LinkIndex]->GetOwningNode();
        if (UInputSMGraphNode_Transition* Transition = Cast<UInputSMGraphNode_Transition>(TargetNode))
        {
            OutTransitions.Add(Transition);
        }
    }

    // Sort the transitions by priority order, lower numbers are higher priority
    if (bWantSortedList)
    {
        struct FCompareTransitionsByPriority
        {
            FORCEINLINE bool operator()(const UInputSMGraphNode_Transition& A, const UInputSMGraphNode_Transition& B) const
            {
                return A.Priority < B.Priority;
            }
        };

        OutTransitions.Sort(FCompareTransitionsByPriority());
    }
}

void UInputSMGraphNode_Entry::AllocateDefaultPins()
{
    UEdGraphPin* Outputs = CreatePin(EGPD_Output, EntryOutputPinName, TEXT("Entry"));
}

UEdGraphNode* UInputSMGraphNode_Entry::GetOutputNode() const
{
	if (Pins.Num() > 0 && Pins[0] != NULL)
	{
		check(Pins[0]->LinkedTo.Num() <= 1);
		if (Pins[0]->LinkedTo.Num() > 0 && Pins[0]->LinkedTo[0]->GetOwningNode() != NULL)
		{
			return Pins[0]->LinkedTo[0]->GetOwningNode();
		}
	}
	return NULL;
}

FText UInputSMGraphNode_Entry::GetNodeTitle(ENodeTitleType::Type TitleType) const { return FText::FromString(GetGraph()->GetName()); }

const FName UInputSMGraphNode_Entry::EntryOutputPinName("RootOutputPin");

void UInputSMGraphNode_State::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(UInputSMGraphNode_State, StateName) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(UInputSMGraphNode_State, StateAsset))
	{
		OnTitleChanged.Broadcast(this);
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UInputSMGraphNode_State::AllocateDefaultPins()
{
    UEdGraphPin* Inputs = CreatePin(EGPD_Input, TEXT("Transition"), TEXT("In"));
    UEdGraphPin* Outputs = CreatePin(EGPD_Output, TEXT("Transition"), TEXT("Out"));
}

FText UInputSMGraphNode_State::GetNodeTitle(ENodeTitleType::Type TitleType) const 
{ 
	FFormatNamedArguments Args;
	Args.Add(TEXT("StateName"), FText::FromName(StateName));
	Args.Add(TEXT("StateAsset"), FText::FromString(StateAsset ? StateAsset->GetName() : FString("NULL")));

	return FText::Format(NSLOCTEXT("UInputSMGraphNode_State", "NodeTitle", "{StateName} [{StateAsset}]"), Args);
}

void UInputSMGraphNode_Transition::AllocateDefaultPins()
{
	UEdGraphPin* Inputs = CreatePin(EGPD_Input, TEXT("Transition"), TEXT("In"));
	Inputs->bHidden = true;
	UEdGraphPin* Outputs = CreatePin(EGPD_Output, TEXT("Transition"), TEXT("Out"));
	Outputs->bHidden = true;
}

FText UInputSMGraphNode_Transition::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	UInputSMGraphNode_State* PrevState = GetPreviousState();
	UInputSMGraphNode_State* NextState = GetNextState();

	FFormatNamedArguments Args;
	Args.Add(TEXT("PrevState"), FText::FromString(PrevState ? PrevState->GetName() : "???"));
	Args.Add(TEXT("NextState"), FText::FromString(NextState ? NextState->GetName() : "???"));

	return FText::Format(NSLOCTEXT("UInputSMGraphNode_Transition", "NodeTitle", "{PrevState} => {NextState}"), Args);
}

void UInputSMGraphNode_Transition::PinConnectionListChanged(UEdGraphPin* Pin)
{
	if (Pin->LinkedTo.Num() == 0)
	{
		// Commit suicide; transitions must always have an input and output connection
		Modify();

		// Our parent graph will have our graph in SubGraphs so needs to be modified to record that.
		if (UEdGraph* ParentGraph = GetGraph())
		{
			ParentGraph->Modify();
		}

		DestroyNode();
	}
}

UInputSMGraphNode_State* UInputSMGraphNode_Transition::GetPreviousState() const
{
	if (Pins[0]->LinkedTo.Num() > 0)
	{
		return Cast<UInputSMGraphNode_State>(Pins[0]->LinkedTo[0]->GetOwningNode());
	}
	else
	{
		return NULL;
	}
}

UInputSMGraphNode_State* UInputSMGraphNode_Transition::GetNextState() const
{
	if (Pins[1]->LinkedTo.Num() > 0)
	{
		return Cast<UInputSMGraphNode_State>(Pins[1]->LinkedTo[0]->GetOwningNode());
	}
	else
	{
		return NULL;
	}
}

void UInputSMGraphNode_Transition::CreateConnections(UInputSMGraphNode_State* PreviousState, UInputSMGraphNode_State* NextState)
{
	// Previous to this
	Pins[0]->Modify();
	Pins[0]->LinkedTo.Empty();

	PreviousState->GetOutputPin()->Modify();
	Pins[0]->MakeLinkTo(PreviousState->GetOutputPin());

	// This to next
	Pins[1]->Modify();
	Pins[1]->LinkedTo.Empty();

	NextState->GetInputPin()->Modify();
	Pins[1]->MakeLinkTo(NextState->GetInputPin());
}