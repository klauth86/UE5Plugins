// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EdGraphUtilities.h"

struct FInputSequenceGraphPinFactory : public FGraphPanelPinFactory
{
public:
	virtual TSharedPtr<class SGraphPin> CreatePin(class UEdGraphPin* Pin) const override;
};