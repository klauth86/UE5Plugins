// Fill out your copyright notice in the Description page of Project Settings.

#include "Graph/SGraphPin_Add.h"

#define LOCTEXT_NAMESPACE "NiagaraGraphPinAdd"

void SGraphPin_Add::Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj)
{
	SetShowLabel(false);

	SGraphPin::Construct(SGraphPin::FArguments().Visibility_Raw(this, &SGraphPin_Add::Visibility_Raw), InGraphPinObj);

	TSharedPtr<SHorizontalBox> PinBox = GetFullPinHorizontalRowWidget().Pin();
	if (PinBox.IsValid())
	{
		PinBox->InsertSlot(0)[ConstructAddButton()];
	}
}

TSharedRef<SWidget>	SGraphPin_Add::ConstructAddButton()
{
	AddButton = SNew(SComboButton)
		.HasDownArrow(false)
		.ButtonStyle(FEditorStyle::Get(), "HoverHintOnly")
		.ForegroundColor(FSlateColor::UseForeground())
		.OnGetMenuContent(this, &SGraphPin_Add::OnGetAddButtonMenuContent)
		.ContentPadding(FMargin(2))
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.ToolTipText(LOCTEXT("AddPinButtonToolTip", "Connect this pin to add a new typed pin, or choose from the drop-down."))
		.ButtonContent()
		[
			SNew(SImage)
			.ColorAndOpacity(FSlateColor::UseForeground())
		.Image(FEditorStyle::GetBrush("Plus"))
		];

	return AddButton.ToSharedRef();
}

#undef LOCTEXT_NAMESPACE