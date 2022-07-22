// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SGraphNode.h"

class SInputSequenceGraphNode_Start : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SInputSequenceGraphNode_Start) {}
	SLATE_END_ARGS();

	void Construct(const FArguments& InArgs, UEdGraphNode* InNode);

	virtual void UpdateGraphNode() override;
};