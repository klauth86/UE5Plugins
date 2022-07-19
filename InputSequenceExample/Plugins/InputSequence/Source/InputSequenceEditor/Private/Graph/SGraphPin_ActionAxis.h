// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SGraphPin.h"

class UEdGraphPin;

class SGraphPin_ActionAxis : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SGraphPin_ActionAxis) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InPin);

protected:

	EVisibility Visibility_Raw() const;
};