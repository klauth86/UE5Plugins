// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SGraphPin.h"

class UEdGraphPin;

class SGraphPin_Action : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SGraphPin_Action) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InPin);

protected:

	EVisibility Visibility_Raw_SelfPin() const;

	EVisibility Visibility_Raw_ArrowUp() const;

	FReply OnClicked_Raw() const;
};