// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EdGraph/EdGraphSchema.h"
#include "InputSequenceGraphSchema.generated.h"

UCLASS()
class UInputSequenceGraphSchema : public UEdGraphSchema
{
	GENERATED_BODY()

public:

	static const FName PC_Exec;

	virtual FLinearColor GetPinTypeColor(const FEdGraphPinType& PinType) const override { return FLinearColor::White; }

	virtual void CreateDefaultNodesForGraph(UEdGraph& Graph) const override;
};