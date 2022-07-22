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

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	virtual FLinearColor GetNodeTitleColor() const override;

	virtual FText GetTooltipText() const override { return FText::GetEmpty(); }

	virtual void AutowireNewNode(UEdGraphPin* FromPin) override;

	DECLARE_DELEGATE(FInvalidateWidgetEvent);
	
	FInvalidateWidgetEvent OnUpdateGraphNode;
};