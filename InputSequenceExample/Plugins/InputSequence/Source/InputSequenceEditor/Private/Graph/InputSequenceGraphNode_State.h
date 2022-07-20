// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EdGraph/EdGraphNode.h"
#include "InputSequenceGraphNode_State.generated.h"

enum EInputEvent;

USTRUCT()
struct FInputFrame
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere)
		TMap<FName, TEnumAsByte<EInputEvent>> ActionsFrame;
};

UCLASS()
class UInputSequenceGraphNode_State : public UEdGraphNode
{
	GENERATED_BODY()

public:

	virtual void AllocateDefaultPins() override;

	virtual void ReconstructNode() override;

	virtual void AutowireNewNode(UEdGraphPin* FromPin) override;

	virtual void PinConnectionListChanged(UEdGraphPin* Pin) override;

protected:

	UPROPERTY(EditAnywhere)
		FInputFrame InputFrame;
};