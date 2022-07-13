// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EdGraph/EdGraphNode.h"
#include "InputSequenceGraphNode_Start.generated.h"

UCLASS()
class UInputSequenceGraphNode_Start : public UEdGraphNode
{
	GENERATED_BODY()

public:

	virtual bool CanDuplicateNode() const override { return false; }

	virtual bool CanUserDeleteNode() const override { return false; }
};