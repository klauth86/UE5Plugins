// Fill out your copyright notice in the Description page of Project Settings.

#include "Graph/SInputSMGraphNode_Entry.h"

void SInputSMGraphNode_Entry::Construct(const FArguments& InArgs, UInputSMGraphNode_Entry* InNode)
{
	GraphNode = InNode;
	SetCursor(EMouseCursor::CardinalCross);
	UpdateGraphNode();
}

FSlateColor SInputSMGraphNode_Entry::GetBorderBackgroundColor() const
{
	FLinearColor InactiveStateColor(0.08f, 0.08f, 0.08f);
	FLinearColor ActiveStateColorDim(0.4f, 0.3f, 0.15f);
	FLinearColor ActiveStateColorBright(1.f, 0.6f, 0.35f);

	return InactiveStateColor;
}

void SInputSMGraphNode_Entry::UpdateGraphNode()
{
	InputPins.Empty();
	OutputPins.Empty();

	// Reset variables that are going to be exposed, in case we are refreshing an already setup node.
	RightNodeBox.Reset();
	LeftNodeBox.Reset();


	FLinearColor TitleShadowColor(0.6f, 0.6f, 0.6f);

	this->ContentScale.Bind(this, &SGraphNode::GetContentScale);
	this->GetOrAddSlot(ENodeZone::Center)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SBorder)
			.BorderImage(FEditorStyle::GetBrush("Graph.StateNode.Body"))
		.Padding(10)
		.BorderBackgroundColor(this, &SInputSMGraphNode_Entry::GetBorderBackgroundColor)
		[
			SAssignNew(RightNodeBox, SVerticalBox)
		]
		];

	CreatePinWidgets();
}

void SInputSMGraphNode_Entry::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
{
	PinToAdd->SetOwner(SharedThis(this));
	RightNodeBox->AddSlot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		.FillHeight(1.0f)
		[
			PinToAdd
		];
	OutputPins.Add(PinToAdd);
}

FText SInputSMGraphNode_Entry::GetPreviewCornerText() const
{
	return NSLOCTEXT("SInputSMGraphNode_Entry", "CornerTextDescription", "Entry point for state machine");
}