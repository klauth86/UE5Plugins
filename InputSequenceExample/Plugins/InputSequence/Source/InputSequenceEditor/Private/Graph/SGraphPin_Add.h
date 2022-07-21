// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SGraphPin.h"

class UEdGraphPin;
class SComboButton;

class SGraphPin_Add : public SGraphPin
{
	SLATE_BEGIN_ARGS(SGraphPin_Add) {}
	SLATE_END_ARGS();

	void Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj);

private:

	TSharedRef<SWidget>	ConstructAddButton();

	TSharedRef<SWidget> OnGetAddButtonMenuContent();

	TSharedPtr<SComboButton> AddButton;
};