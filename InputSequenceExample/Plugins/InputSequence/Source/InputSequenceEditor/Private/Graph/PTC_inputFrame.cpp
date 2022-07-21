// Fill out your copyright notice in the Description page of Project Settings.

#include "Graph/PTC_InputFrame.h"
#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "GameFramework/InputSettings.h"
#include "Widgets/Input/SComboBox.h"

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
			SNullWidget::NullWidget
		];
}

void FPTC_InputFrame::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{

}

#undef LOCTEXT_NAMESPACE