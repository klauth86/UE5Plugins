// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SGraphNode.h"

class UInputSMGraphNode_Entry;

class INPUTSMEDITOR_API SInputSMGraphNode_Entry : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SInputSMGraphNode_Entry) {}
	SLATE_END_ARGS()

		void Construct(const FArguments& InArgs, UInputSMGraphNode_Entry* InNode);

	// SNodePanel::SNode interface
	virtual void GetNodeInfoPopups(FNodeInfoContext* Context, TArray<FGraphInformationPopupInfo>& Popups) const override {}
	// End of SNodePanel::SNode interface

	// SGraphNode interface
	virtual void UpdateGraphNode() override;
	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;

	// End of SGraphNode interface


protected:

	FSlateColor GetBorderBackgroundColor() const;

	FText GetPreviewCornerText() const;
};