// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SGraphNode.h"

class SInputSequenceGraphNode_Press : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SInputSequenceGraphNode_Press) {}
	SLATE_END_ARGS();

	void Construct(const FArguments& InArgs, UEdGraphNode* InNode);

	virtual ~SInputSequenceGraphNode_Press();

	TSharedRef<SWidget> AddPinButtonContent_Custom(FText PinText, FText PinTooltipText, TSharedPtr<SToolTip> CustomTooltip = nullptr);

	virtual void CreateOutputSideAddButton(TSharedPtr<SVerticalBox> OutputBox) override;

	virtual FReply OnAddPin() override;

protected:

	TSharedRef<SWidget> OnGetAddButtonMenuContent();

protected:

	TSharedPtr<SComboButton> AddButton;
};