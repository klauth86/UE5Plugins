//////// Fill out your copyright notice in the Description page of Project Settings.
//////
//////#pragma once
//////
//////#include "SGraphNode.h"
//////
//////class SInputSequenceGraphNode_State : public SGraphNode
//////{
//////public:
//////	SLATE_BEGIN_ARGS(SInputSequenceGraphNode_State) {}
//////	SLATE_END_ARGS();
//////
//////	void Construct(const FArguments& InArgs, UEdGraphNode* InNode);
//////
//////protected:
//////
//////	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd);
//////};