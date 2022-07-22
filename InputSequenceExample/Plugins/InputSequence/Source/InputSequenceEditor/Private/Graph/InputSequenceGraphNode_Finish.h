// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EdGraph/EdGraphNode.h"
#include "InputSequenceGraphNode_Finish.generated.h"

UCLASS()
class UInputSequenceGraphNode_Finish : public UEdGraphNode
{
	GENERATED_BODY()

public:

	virtual void AllocateDefaultPins() override;

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	virtual FLinearColor GetNodeTitleColor() const override;

	virtual FText GetTooltipText() const override;

	virtual bool CanDuplicateNode() const override { return false; }

	virtual bool CanUserDeleteNode() const override { return false; }
};