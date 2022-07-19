//////// Fill out your copyright notice in the Description page of Project Settings.
//////
//////#include "Graph/SInputSequenceGraphNode_State.h"
//////#include "SGraphPin.h"
//////
//////void SInputSequenceGraphNode_State::Construct(const FArguments& InArgs, UEdGraphNode* InNode)
//////{
//////	SetCursor(EMouseCursor::CardinalCross);
//////
//////	GraphNode = InNode;
//////	UpdateGraphNode();
//////}
//////
//////void SInputSequenceGraphNode_State::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
//////{
//////	PinToAdd->SetOwner(SharedThis(this));
//////
//////	const UEdGraphPin* PinObj = PinToAdd->GetPinObj();
//////	const bool bAdvancedParameter = (PinObj != nullptr) && PinObj->bAdvancedView;
//////	if (bAdvancedParameter)
//////	{
//////		PinToAdd->SetVisibility(TAttribute<EVisibility>(PinToAdd, &SGraphPin::IsPinVisibleAsAdvanced));
//////	}
//////
//////	if (PinToAdd->GetDirection() == EEdGraphPinDirection::EGPD_Input)
//////	{
//////		LeftNodeBox->AddSlot()
//////			.AutoHeight()
//////			.HAlign(HAlign_Left)
//////			.VAlign(VAlign_Center)
//////			[
//////				PinToAdd
//////			];
//////		InputPins.Add(PinToAdd);
//////	}
//////	else // Direction == EEdGraphPinDirection::EGPD_Output
//////	{
//////		RightNodeBox->AddSlot()
//////			.AutoHeight()
//////			.HAlign(HAlign_Right)
//////			.VAlign(VAlign_Center)
//////			[
//////				PinToAdd
//////			];
//////		OutputPins.Add(PinToAdd);
//////	}
//////}