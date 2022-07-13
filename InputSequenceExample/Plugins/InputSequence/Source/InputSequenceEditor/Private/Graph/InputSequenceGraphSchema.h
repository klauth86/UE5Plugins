// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EdGraph/EdGraphSchema.h"
#include "InputSequenceGraphSchema.generated.h"

UCLASS()
class UInputSequenceGraphSchema : public UEdGraphSchema
{
	GENERATED_BODY()

public:

	virtual void CreateDefaultNodesForGraph(UEdGraph& Graph) const override;
};