// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EdGraph/EdGraphNode.h"
#include "InputSM.h"
#include "InputSMGraphNode_Base.generated.h"

UCLASS()
class UInputSMGraphNode_Base : public UEdGraphNode
{
	GENERATED_BODY()

public:

	bool CanCreateUnderSpecifiedSchema(const UEdGraphSchema* DesiredSchema) const override;

	void DestroyNode() override;

	void AddTransition(int32 indexB, const FInputFrameStack& activationStack);

	void RemoveTransition(int32 indexB, bool decrementOthers);

	UObject* GetNodeAsset() const { return NodeAsset; }

	void SetNodeAsset(UObject* nodeAsset);

	TArray<FInputSMTransition>& GetTransitions() { return Transitions; }

protected:

	UPROPERTY()
		UObject* NodeAsset;

	UPROPERTY()
		TArray<FInputSMTransition> Transitions;
};

UCLASS()
class UInputSMGraphNode_Root : public UInputSMGraphNode_Base
{
	GENERATED_BODY()

public:

	virtual bool CanDuplicateNode() const override { return false; }

	virtual bool CanUserDeleteNode() const override { return false; }

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	virtual void AllocateDefaultPins() override;
};

UCLASS()
class UInputSMGraphNode_State : public UInputSMGraphNode_Base
{
	GENERATED_BODY()

public:

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	virtual void AllocateDefaultPins() override;
};