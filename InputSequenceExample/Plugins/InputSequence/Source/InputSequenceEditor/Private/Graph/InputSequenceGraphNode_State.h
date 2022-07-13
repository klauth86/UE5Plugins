// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EdGraph/EdGraphNode.h"
#include "InputSequenceGraphNode_State.generated.h"

UCLASS()
class UInputSequenceGraphNode_State : public UEdGraphNode
{
	GENERATED_BODY()

public:

	UEdGraphPin* GetInputPin() const;

	UEdGraphPin* GetOutputPin() const;

	virtual void AllocateDefaultPins() override;

	virtual void AutowireNewNode(UEdGraphPin* FromPin) override;
};