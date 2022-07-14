// Fill out your copyright notice in the Description page of Project Settings.

#include "Graph/PTC_InputFrame.h"
#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "GameFramework/InputSettings.h"
#include "Widgets/Input/SComboBox.h"
#include "Graph/InputSequenceGraphNode_State.h"

#define LOCTEXT_NAMESPACE "FPTC_InputFrame"

TSharedRef<IPropertyTypeCustomization> FPTC_InputFrame::MakeInstance() { return MakeShareable(new FPTC_InputFrame); }

void FPTC_InputFrame::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{	
	InternalPropertyHandle = PropertyHandle;

	HeaderRow
		.NameContent()
		[
			PropertyHandle->CreatePropertyNameWidget()
		]
		.ValueContent()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().FillWidth(1)[SNullWidget::NullWidget]
			+ SHorizontalBox::Slot().FillWidth(1)[SNew(SButton).Text(FText::FromString("U"))]
			+ SHorizontalBox::Slot().FillWidth(1)[SNew(SButton).Text(FText::FromString("D"))]
		];
}

void FPTC_InputFrame::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	const TArray<FInputActionKeyMapping>& actionMappings = UInputSettings::GetInputSettings()->GetActionMappings();
	for (const FInputActionKeyMapping& actionMapping : actionMappings)
	{
		ChildBuilder.AddCustomRow(FText::FromName(actionMapping.ActionName))		
			.NameContent()
			[
				SNew(STextBlock).Text(FText::FromName(actionMapping.ActionName))
			]
		.ValueContent()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot().FillWidth(1)[
					SNew(SButton).Text_Raw(this, &FPTC_InputFrame::AddRemoveButtonText, actionMapping.ActionName)
						.OnClicked_Raw(this, &FPTC_InputFrame::AddRemoveButtonOnClicked, actionMapping.ActionName)
				]
				+ SHorizontalBox::Slot().FillWidth(1)[SNew(SButton).Text(FText::FromString("U"))]
				+ SHorizontalBox::Slot().FillWidth(1)[SNew(SButton).Text(FText::FromString("D"))]
			];
	}
}

FText FPTC_InputFrame::AddRemoveButtonText(FName actionName) const
{
	if (FInputFrame* inputFrame = GetPropertyAs<FInputFrame>(InternalPropertyHandle))
	{
		if (inputFrame->ActionsFrame.Contains(actionName)) return LOCTEXT("AddRemoveButtonText_Minus", "-");
	}
	return LOCTEXT("AddRemoveButtonText_Plus", "+");
}

FReply FPTC_InputFrame::AddRemoveButtonOnClicked(FName actionName) const
{
	if (FInputFrame* inputFrame = GetPropertyAs<FInputFrame>(InternalPropertyHandle))
	{
		if (inputFrame->ActionsFrame.Contains(actionName))
		{
			inputFrame->ActionsFrame.Remove(actionName);

			if (UInputSequenceGraphNode_State* stateNode = GetTypedFirstOuter<UInputSequenceGraphNode_State>(InternalPropertyHandle))
			{
				stateNode->Modify();
				stateNode->CascadeRemoveActionChange(actionName);
			}
		}
		else
		{
			inputFrame->ActionsFrame.Add(actionName);

			if (UInputSequenceGraphNode_State* stateNode = GetTypedFirstOuter<UInputSequenceGraphNode_State>(InternalPropertyHandle))
			{
				stateNode->Modify();
				stateNode->CascadeAddActionChange(actionName);
			}
		}
	}
	return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE