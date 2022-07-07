// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EdGraph/EdGraphNode.h"
#include "InputSM.h"
#include "InputSMGraphNode_Base.generated.h"

UCLASS()
class UInputSMGraphNode_Base :public UEdGraphNode
{
	GENERATED_BODY()

public:

	DECLARE_MULTICAST_DELEGATE_OneParam(FTitleChanged, UInputSMGraphNode_Base*);

	static FTitleChanged OnTitleChanged;

	virtual bool CanCreateUnderSpecifiedSchema(const UEdGraphSchema* DesiredSchema) const override;

	void GetTransitionList(TArray<UInputSMGraphNode_Transition*>& OutTransitions, bool bWantSortedList = false) const;
};

UCLASS()
class UInputSMGraphNode_Entry : public UInputSMGraphNode_Base
{
	GENERATED_BODY()

public:

	virtual void AllocateDefaultPins() override;

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	virtual bool CanDuplicateNode() const override { return false; }

	virtual bool CanUserDeleteNode() const override { return false; }

	UEdGraphNode* GetOutputNode() const;

	static const FName EntryOutputPinName;
};

UCLASS()
class UInputSMGraphNode_State : public UInputSMGraphNode_Base
{
	GENERATED_BODY()

public:

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	virtual void AllocateDefaultPins() override;

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	UEdGraphPin* GetInputPin() const { return Pins[0]; }
	
	UEdGraphPin* GetOutputPin() const { return Pins[1]; }

	void SetStateAsset(UObject* stateAsset) { StateAsset = stateAsset; }

protected:

	UPROPERTY(EditAnywhere, Category = State)
		FName StateName;

	UPROPERTY(EditAnywhere, Category = State)
		UObject* StateAsset;
};

UCLASS()
class UInputSMGraphNode_Transition : public UInputSMGraphNode_Base
{
	GENERATED_BODY()

public:

	virtual void AllocateDefaultPins() override;

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	virtual void PinConnectionListChanged(UEdGraphPin* Pin) override;

	UInputSMGraphNode_State* GetPreviousState() const;

	UInputSMGraphNode_State* GetNextState() const;

	UEdGraphPin* GetInputPin() const { return Pins[0]; }

	UEdGraphPin* GetOutputPin() const { return Pins[1]; }

	void CreateConnections(UInputSMGraphNode_State* PreviousState, UInputSMGraphNode_State* NextState);

public:

	UPROPERTY(EditAnywhere, Category = Transition)
		int32 Priority;

	UPROPERTY(EditAnywhere, Category = Transition)
		FInputFrameStack ActivationStack;
};