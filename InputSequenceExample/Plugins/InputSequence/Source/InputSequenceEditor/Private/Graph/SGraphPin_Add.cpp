// Fill out your copyright notice in the Description page of Project Settings.

#include "Graph/SGraphPin_Add.h"
#include "SPinTypeSelector.h"
#include "SLevelOfDetailBranchNode.h"
#include "Widgets/Layout/SWrapBox.h"

#define LOCTEXT_NAMESPACE "SGraphPin_Add"

void SGraphPin_Add::Construct(const FArguments& Args, UEdGraphPin* InPin)
{
	SGraphPin::FArguments InArgs = SGraphPin::FArguments();

	bUsePinColorForText = InArgs._UsePinColorForText;
	this->SetCursor(EMouseCursor::Hand);

	SetVisibility(MakeAttributeSP(this, &SGraphPin_Add::GetPinVisiblity));

	GraphPinObj = InPin;
	check(GraphPinObj != NULL);

	const UEdGraphSchema* Schema = GraphPinObj->GetSchema();
	checkf(
		Schema,
		TEXT("Missing schema for pin: %s with outer: %s of type %s"),
		*(GraphPinObj->GetName()),
		GraphPinObj->GetOuter() ? *(GraphPinObj->GetOuter()->GetName()) : TEXT("NULL OUTER"),
		GraphPinObj->GetOuter() ? *(GraphPinObj->GetOuter()->GetClass()->GetName()) : TEXT("NULL OUTER")
	);

	TSharedRef<SWidget> PinWidgetRef = SNew(SImage).Image(FEditorStyle::GetBrush(TEXT("Icons.PlusCircle")));

	PinImage = PinWidgetRef;

	// Create the pin indicator widget (used for watched values)
	static const FName NAME_NoBorder("NoBorder");
	TSharedRef<SWidget> PinStatusIndicator =
		SNew(SButton)
		.ButtonStyle(FEditorStyle::Get(), NAME_NoBorder)
		.Visibility(this, &SGraphPin_Add::GetPinStatusIconVisibility)
		.ContentPadding(0)
		.OnClicked(this, &SGraphPin_Add::ClickedOnPinStatusIcon)
		[
			SNew(SImage).Image(this, &SGraphPin_Add::GetPinStatusIcon)
		];

	TSharedRef<SWidget> LabelWidget = GetLabelWidget(InArgs._PinLabelStyle);

	// Create the widget used for the pin body (status indicator, label, and value)
	LabelAndValue =
		SNew(SWrapBox)
		.PreferredSize(150.f);

	LabelAndValue->AddSlot()
		.VAlign(VAlign_Center)
		[
			LabelWidget
		];

	LabelAndValue->AddSlot()
		.VAlign(VAlign_Center)
		[
			PinStatusIndicator
		];

	TSharedPtr<SHorizontalBox> PinContent;
	FullPinHorizontalRowWidget = PinContent = SNew(SHorizontalBox)
	+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		.Padding(0, 0, InArgs._SideToSideMargin, 0)
		[
			LabelAndValue.ToSharedRef()
		]
	+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
			PinWidgetRef
		];

	// Set up a hover for pins that is tinted the color of the pin.
	SBorder::Construct(SBorder::FArguments()
	.BorderImage(FEditorStyle::GetBrush("NoBorder"))
	.BorderBackgroundColor(this, &SGraphPin_Add::GetPinColor)
	[
		SAssignNew(AddButton, SComboButton)
		.HasDownArrow(false)
		.ButtonStyle(FEditorStyle::Get(), "NoBorder")
		.ForegroundColor(FSlateColor::UseForeground())
		.OnGetMenuContent(this, &SGraphPin_Add::OnGetAddButtonMenuContent)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.ToolTipText(LOCTEXT("AddPin_ToolTip", "Click to add new Action pin"))
		.ButtonContent()
		[
			PinContent.ToSharedRef()
		]
	]
	);
}

#undef LOCTEXT_NAMESPACE