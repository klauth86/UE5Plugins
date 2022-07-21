// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EdGraph/EdGraphNode.h"
#include "InputSequenceGraphNode_Press.generated.h"

UCLASS()
class UInputSequenceGraphNode_Press : public UEdGraphNode
{
	GENERATED_BODY()

public:

	virtual void AllocateDefaultPins() override;

	virtual void AutowireNewNode(UEdGraphPin* FromPin) override;

	DECLARE_DELEGATE(FInvalidateWidgetEvent);
	
	FInvalidateWidgetEvent OnUpdateGraphNode;
};