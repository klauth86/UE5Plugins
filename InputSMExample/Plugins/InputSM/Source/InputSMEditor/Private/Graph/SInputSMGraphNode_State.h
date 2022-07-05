// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SGraphNode.h"

class UInputSMGraphNode_State;

class INPUTSMEDITOR_API SInputSMGraphNode_State : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SInputSMGraphNode_State) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UInputSMGraphNode_State* InNode);

	virtual ~SInputSMGraphNode_State();

	// SNodePanel::SNode interface
	virtual void GetNodeInfoPopups(FNodeInfoContext* Context, TArray<FGraphInformationPopupInfo>& Popups) const override {}
	// End of SNodePanel::SNode interface

	// SGraphNode interface
	virtual void UpdateGraphNode() override;
	virtual void CreatePinWidgets() override;
	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;
	// End of SGraphNode interface

protected:
	FSlateColor GetBorderBackgroundColor() const;

	virtual FText GetPreviewCornerText() const;
	virtual const FSlateBrush* GetNameIcon() const;
	void OnTitleChanged(UInputSMGraphNode_Base* node);

	TSharedPtr<SNodeTitle> NodeTitle;
};