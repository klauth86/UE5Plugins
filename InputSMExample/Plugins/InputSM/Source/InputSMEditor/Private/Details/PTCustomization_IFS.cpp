// Fill out your copyright notice in the Description page of Project Settings.

#include "Details/PTCustomization_IFS.h"
#include "IDetailChildrenBuilder.h"
#include "DetailWidgetRow.h"
#include "Widgets/Layout/SGridPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SButton.h"
#include "InputSM.h"
#include "Styling/SlateTypes.h"

#define LOCTEXT_NAMESPACE "FPTCustomization_IFS"

class SInputFrameWidget : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SInputFrameWidget)
		: _Content()
		, _ContentScale(FVector2D(1, 1))
		, _HAlign(HAlign_Fill)
		, _VAlign(VAlign_Fill)
		, _Padding(FMargin(2.0f))
	{}
	SLATE_DEFAULT_SLOT(FArguments, Content)
		SLATE_ATTRIBUTE(FVector2D, ContentScale)
		SLATE_ARGUMENT(EHorizontalAlignment, HAlign)
		SLATE_ARGUMENT(EVerticalAlignment, VAlign)
		SLATE_ATTRIBUTE(FMargin, Padding)
		SLATE_END_ARGS()

	TSharedPtr<IPropertyHandle> ParentPropertyHandle;

	int32 FrameIndex;

	FInputFrame* GetInputFrame() const
	{
		if (FInputFrameStack* inputFrameStack = GetPropertyAs<FInputFrameStack>(ParentPropertyHandle))
		{
			if (inputFrameStack->Frames.IsValidIndex(FrameIndex)) return &inputFrameStack->Frames[FrameIndex];
		}

		return nullptr;
	}

	void Construct(FArguments InArgs, TSharedPtr<IPropertyHandle> parentPropertyHandle, int32 frameIndex)
	{
		SetContentScale(InArgs._ContentScale);

		ParentPropertyHandle = parentPropertyHandle;
		
		FrameIndex = frameIndex;

		ChildSlot
			.HAlign(InArgs._HAlign)
			.VAlign(InArgs._VAlign)
			.Padding(InArgs._Padding)
			[
				SNew(SGridPanel)
				.FillColumn(0, 0)
				.FillColumn(1, 1)
				.FillColumn(2, 0)
				.FillColumn(3, 1)
				.FillColumn(4, 0)
				.FillColumn(5, 1)
				.FillColumn(6, 0)
				.FillColumn(7, 1)
				.FillColumn(8, 0)

				+ SGridPanel::Slot(0, 0).VAlign(VAlign_Center).HAlign(HAlign_Center)
				[
					SNew(SGridPanel)

					+ SGridPanel::Slot(0, 0)[SNew(SButton).ContentPadding(0)
					.ButtonColorAndOpacity_Lambda([this]() { return GetColorAndOpacityForTriggers(0); })
					.OnClicked_Lambda([this]() { return ClickForTrigger(0); })
					.VAlign(VAlign_Center).HAlign(HAlign_Center)
						[
							SNew(SHorizontalBox)
							+ SHorizontalBox::Slot().VAlign(VAlign_Center).AutoWidth().Padding(FMargin(1))
							[
								SNew(SImage).Image(FEditorStyle::GetBrush(TEXT("ArrowDown"))).Visibility_Lambda([this]() { return GetPressedVisForPacked(0); })
							]
							+ SHorizontalBox::Slot().VAlign(VAlign_Center).AutoWidth().Padding(FMargin(2))
							[
								SNew(STextBlock)
								.Text(FText::FromString("LB"))
								.ColorAndOpacity_Lambda([this]() { return GetTextColorAndOpacityForPacked(0); })
								.Font(FEditorStyle::GetFontStyle("Persona.RetargetManager.SmallBoldFont"))
							]
						]
					]

					+ SGridPanel::Slot(0, 1)[SNew(SButton).ContentPadding(0)
					.ButtonColorAndOpacity_Lambda([this]() { return GetColorAndOpacityForTriggers(2); })
					.OnClicked_Lambda([this]() { return ClickForTrigger(2); })
					.VAlign(VAlign_Center).HAlign(HAlign_Center)
						[
							SNew(SHorizontalBox)
							+ SHorizontalBox::Slot().VAlign(VAlign_Center).AutoWidth().Padding(FMargin(1))
							[
								SNew(SImage).Image(FEditorStyle::GetBrush(TEXT("ArrowDown"))).Visibility_Lambda([this]() { return GetPressedVisForPacked(2); })
							]
							+ SHorizontalBox::Slot().VAlign(VAlign_Center).AutoWidth().Padding(FMargin(2))
							[
								SNew(STextBlock)
								.Text(FText::FromString("LT"))
								.ColorAndOpacity_Lambda([this]() { return GetTextColorAndOpacityForPacked(2); })
								.Font(FEditorStyle::GetFontStyle("Persona.RetargetManager.SmallBoldFont"))
							]
						]
					]
				]

				+ SGridPanel::Slot(2, 0).VAlign(VAlign_Center).HAlign(HAlign_Center)
				[
					SNew(SGridPanel)

					+ SGridPanel::Slot(0, 0)[SNew(SButton).ContentPadding(0)
					.OnClicked_Lambda([this]() { return ClickForStick(0, 7); })
					.VAlign(VAlign_Center).HAlign(HAlign_Center)
						[
							SNew(STextBlock)
							.Text(FText::FromString("315"))
							.ColorAndOpacity_Lambda([this]() { return GetTextColorAndOpacityForStick(0, 7); })
							.Font(FEditorStyle::GetFontStyle("Persona.RetargetManager.SmallBoldFont"))
						]
					]
					+ SGridPanel::Slot(1, 0)[SNew(SButton).ContentPadding(0)
					.OnClicked_Lambda([this]() { return ClickForStick(0, 0); })
					.VAlign(VAlign_Center).HAlign(HAlign_Center)
						[
							SNew(STextBlock)
							.Text(FText::FromString("0"))
							.ColorAndOpacity_Lambda([this]() { return GetTextColorAndOpacityForStick(0, 0); })
							.Font(FEditorStyle::GetFontStyle("Persona.RetargetManager.SmallBoldFont"))
						]
					]
					+ SGridPanel::Slot(2, 0)[SNew(SButton).ContentPadding(0)
					.OnClicked_Lambda([this]() { return ClickForStick(0, 1); })
					.VAlign(VAlign_Center).HAlign(HAlign_Center)
						[
							SNew(STextBlock)
							.Text(FText::FromString("45"))
							.ColorAndOpacity_Lambda([this]() { return GetTextColorAndOpacityForStick(0, 1); })
							.Font(FEditorStyle::GetFontStyle("Persona.RetargetManager.SmallBoldFont"))
						]
					]

					+ SGridPanel::Slot(0, 1)[SNew(SButton).ContentPadding(0)
					.OnClicked_Lambda([this]() { return ClickForStick(0, 6); })
					.VAlign(VAlign_Center).HAlign(HAlign_Center)
						[
							SNew(STextBlock)
							.Text(FText::FromString("270"))
							.ColorAndOpacity_Lambda([this]() { return GetTextColorAndOpacityForStick(0, 6); })
							.Font(FEditorStyle::GetFontStyle("Persona.RetargetManager.SmallBoldFont"))
						]
					]
					+ SGridPanel::Slot(2, 1)[SNew(SButton).ContentPadding(0)
					.OnClicked_Lambda([this]() { return ClickForStick(0, 2); })
					.VAlign(VAlign_Center).HAlign(HAlign_Center)
						[
							SNew(STextBlock)
							.Text(FText::FromString("90"))
							.ColorAndOpacity_Lambda([this]() { return GetTextColorAndOpacityForStick(0, 2); })
							.Font(FEditorStyle::GetFontStyle("Persona.RetargetManager.SmallBoldFont"))
						]
					]

					+ SGridPanel::Slot(0, 2)[SNew(SButton).ContentPadding(0)
					.OnClicked_Lambda([this]() { return ClickForStick(0, 5); })
					.VAlign(VAlign_Center).HAlign(HAlign_Center)
						[
							SNew(STextBlock)
							.Text(FText::FromString("225"))
							.ColorAndOpacity_Lambda([this]() { return GetTextColorAndOpacityForStick(0, 5); })
							.Font(FEditorStyle::GetFontStyle("Persona.RetargetManager.SmallBoldFont"))
						]
					]
					+ SGridPanel::Slot(1, 2)[SNew(SButton).ContentPadding(0)
					.OnClicked_Lambda([this]() { return ClickForStick(0, 4); })
					.VAlign(VAlign_Center).HAlign(HAlign_Center)
						[
							SNew(STextBlock)
							.Text(FText::FromString("180"))
							.ColorAndOpacity_Lambda([this]() { return GetTextColorAndOpacityForStick(0, 4); })
							.Font(FEditorStyle::GetFontStyle("Persona.RetargetManager.SmallBoldFont"))
						]
					]
					+ SGridPanel::Slot(2, 2)[SNew(SButton).ContentPadding(0)
					.OnClicked_Lambda([this]() { return ClickForStick(0, 3); })
					.VAlign(VAlign_Center).HAlign(HAlign_Center)
						[
							SNew(STextBlock)
							.Text(FText::FromString("135"))
							.ColorAndOpacity_Lambda([this]() { return GetTextColorAndOpacityForStick(0, 3); })
							.Font(FEditorStyle::GetFontStyle("Persona.RetargetManager.SmallBoldFont"))
						]
					]
				]

				+ SGridPanel::Slot(4, 0).VAlign(VAlign_Center).HAlign(HAlign_Center)
				[
					SNew(SGridPanel)

					+ SGridPanel::Slot(0, 0)[SNew(SButton).ContentPadding(0)
					.OnClicked_Lambda([this]() { return ClickForStick(1, 7); })
					.VAlign(VAlign_Center).HAlign(HAlign_Center)
						[
							SNew(STextBlock)
							.Text(FText::FromString("315"))
							.ColorAndOpacity_Lambda([this]() { return GetTextColorAndOpacityForStick(1, 7); })
							.Font(FEditorStyle::GetFontStyle("Persona.RetargetManager.SmallBoldFont"))
						]
					]
					+ SGridPanel::Slot(1, 0)[SNew(SButton).ContentPadding(0)
					.OnClicked_Lambda([this]() { return ClickForStick(1, 0); })
					.VAlign(VAlign_Center).HAlign(HAlign_Center)
						[
							SNew(STextBlock)
							.Text(FText::FromString("0"))
							.ColorAndOpacity_Lambda([this]() { return GetTextColorAndOpacityForStick(1, 0); })
							.Font(FEditorStyle::GetFontStyle("Persona.RetargetManager.SmallBoldFont"))
						]
					]
					+ SGridPanel::Slot(2, 0)[SNew(SButton).ContentPadding(0)
					.OnClicked_Lambda([this]() { return ClickForStick(1, 1); })
					.VAlign(VAlign_Center).HAlign(HAlign_Center)
						[
							SNew(STextBlock)
							.Text(FText::FromString("45"))
							.ColorAndOpacity_Lambda([this]() { return GetTextColorAndOpacityForStick(1, 1); })
							.Font(FEditorStyle::GetFontStyle("Persona.RetargetManager.SmallBoldFont"))
						]
					]

					+ SGridPanel::Slot(0, 1)[SNew(SButton).ContentPadding(0)
					.OnClicked_Lambda([this]() { return ClickForStick(1, 6); })
					.VAlign(VAlign_Center).HAlign(HAlign_Center)
						[
							SNew(STextBlock)
							.Text(FText::FromString("270"))
							.ColorAndOpacity_Lambda([this]() { return GetTextColorAndOpacityForStick(1, 6); })
							.Font(FEditorStyle::GetFontStyle("Persona.RetargetManager.SmallBoldFont"))
						]
					]
					+ SGridPanel::Slot(2, 1)[SNew(SButton).ContentPadding(0)
					.OnClicked_Lambda([this]() { return ClickForStick(1, 2); })
					.VAlign(VAlign_Center).HAlign(HAlign_Center)
						[
							SNew(STextBlock)
							.Text(FText::FromString("90"))
							.ColorAndOpacity_Lambda([this]() { return GetTextColorAndOpacityForStick(1, 2); })
							.Font(FEditorStyle::GetFontStyle("Persona.RetargetManager.SmallBoldFont"))
						]
					]

					+ SGridPanel::Slot(0, 2)[SNew(SButton).ContentPadding(0)
					.OnClicked_Lambda([this]() { return ClickForStick(1, 5); })
					.VAlign(VAlign_Center).HAlign(HAlign_Center)
						[
							SNew(STextBlock)
							.Text(FText::FromString("225"))
							.ColorAndOpacity_Lambda([this]() { return GetTextColorAndOpacityForStick(1, 5); })
							.Font(FEditorStyle::GetFontStyle("Persona.RetargetManager.SmallBoldFont"))
						]
					]
					+ SGridPanel::Slot(1, 2)[SNew(SButton).ContentPadding(0)
					.OnClicked_Lambda([this]() { return ClickForStick(1, 4); })
					.VAlign(VAlign_Center).HAlign(HAlign_Center)
						[
							SNew(STextBlock)
							.Text(FText::FromString("180"))
							.ColorAndOpacity_Lambda([this]() { return GetTextColorAndOpacityForStick(1, 4); })
							.Font(FEditorStyle::GetFontStyle("Persona.RetargetManager.SmallBoldFont"))
						]
					]
					+ SGridPanel::Slot(2, 2)[SNew(SButton).ContentPadding(0)
					.OnClicked_Lambda([this]() { return ClickForStick(1, 3); })
					.VAlign(VAlign_Center).HAlign(HAlign_Center)
						[
							SNew(STextBlock)
							.Text(FText::FromString("135"))
							.ColorAndOpacity_Lambda([this]() { return GetTextColorAndOpacityForStick(1, 3); })
							.Font(FEditorStyle::GetFontStyle("Persona.RetargetManager.SmallBoldFont"))
						]
					]
				]

				+ SGridPanel::Slot(6, 0).VAlign(VAlign_Center).HAlign(HAlign_Center)
				[
					SNew(SGridPanel)

					+ SGridPanel::Slot(0, 0)[SNew(SButton).ContentPadding(0)
					.ButtonColorAndOpacity_Lambda([this]() { return GetColorAndOpacityForTriggers(4); })
					.OnClicked_Lambda([this]() { return ClickForTrigger(4); })
					.VAlign(VAlign_Center).HAlign(HAlign_Center)
						[
							SNew(SHorizontalBox)
							+ SHorizontalBox::Slot().VAlign(VAlign_Center).AutoWidth().Padding(FMargin(1))
							[
								SNew(SImage).Image(FEditorStyle::GetBrush(TEXT("ArrowDown"))).Visibility_Lambda([this]() { return GetPressedVisForPacked(4); })
							]
							+ SHorizontalBox::Slot().VAlign(VAlign_Center).AutoWidth().Padding(FMargin(2))
							[
								SNew(STextBlock)
								.Text(FText::FromString("RB"))
								.ColorAndOpacity_Lambda([this]() { return GetTextColorAndOpacityForPacked(4); })
								.Font(FEditorStyle::GetFontStyle("Persona.RetargetManager.SmallBoldFont"))
							]
						]
					]

					+ SGridPanel::Slot(0, 1)[SNew(SButton).ContentPadding(0)
					.ButtonColorAndOpacity_Lambda([this]() { return GetColorAndOpacityForTriggers(6); })
					.OnClicked_Lambda([this]() { return ClickForTrigger(6); })
					.VAlign(VAlign_Center).HAlign(HAlign_Center)
						[
							SNew(SHorizontalBox)
							+ SHorizontalBox::Slot().VAlign(VAlign_Center).AutoWidth().Padding(FMargin(1))
							[
								SNew(SImage).Image(FEditorStyle::GetBrush(TEXT("ArrowDown"))).Visibility_Lambda([this]() { return GetPressedVisForPacked(6); })
							]
							+ SHorizontalBox::Slot().VAlign(VAlign_Center).AutoWidth().Padding(FMargin(2))
							[
								SNew(STextBlock)
								.Text(FText::FromString("RT"))
								.ColorAndOpacity_Lambda([this]() { return GetTextColorAndOpacityForPacked(6); })
								.Font(FEditorStyle::GetFontStyle("Persona.RetargetManager.SmallBoldFont"))
							]
						]
					]
				]

				+ SGridPanel::Slot(8, 0).VAlign(VAlign_Center).HAlign(HAlign_Center)
				[
					SNew(SGridPanel)

					+ SGridPanel::Slot(0, 0)[SNew(SButton).ContentPadding(0)
					.OnClicked_Lambda([this]() { return ClickForTrigger(8); })
					.VAlign(VAlign_Center).HAlign(HAlign_Center)
						[
							SNew(SHorizontalBox)
							+ SHorizontalBox::Slot().VAlign(VAlign_Center).AutoWidth().Padding(FMargin(1))
							[
								SNew(SImage).Image(FEditorStyle::GetBrush(TEXT("ArrowDown"))).Visibility_Lambda([this]() { return GetPressedVisForPacked(8); })
							]
							+ SHorizontalBox::Slot().VAlign(VAlign_Center).AutoWidth().Padding(FMargin(2))
							[
								SNew(STextBlock)
								.Text(FText::FromString("X"))
								.ColorAndOpacity_Lambda([this]() { return GetTextColorAndOpacityForPacked(8); })
								.Font(FEditorStyle::GetFontStyle("Persona.RetargetManager.SmallBoldFont"))
							]
						]
					]
					+ SGridPanel::Slot(0, 1)[SNew(SButton).ContentPadding(0)
					.OnClicked_Lambda([this]() { return ClickForTrigger(10); })
					.VAlign(VAlign_Center).HAlign(HAlign_Center)
						[
							SNew(SHorizontalBox)
							+ SHorizontalBox::Slot().VAlign(VAlign_Center).AutoWidth().Padding(FMargin(1))
							[
								SNew(SImage).Image(FEditorStyle::GetBrush(TEXT("ArrowDown"))).Visibility_Lambda([this]() { return GetPressedVisForPacked(10); })
							]
							+ SHorizontalBox::Slot().VAlign(VAlign_Center).AutoWidth().Padding(FMargin(2))
							[
								SNew(STextBlock)
								.Text(FText::FromString("A"))
								.ColorAndOpacity_Lambda([this]() { return GetTextColorAndOpacityForPacked(10); })
								.Font(FEditorStyle::GetFontStyle("Persona.RetargetManager.SmallBoldFont"))
							]
						]
					]
					+ SGridPanel::Slot(1, 0)[SNew(SButton).ContentPadding(0)
					.OnClicked_Lambda([this]() { return ClickForTrigger(12); })
					.VAlign(VAlign_Center).HAlign(HAlign_Center)
						[
							SNew(SHorizontalBox)
							+ SHorizontalBox::Slot().VAlign(VAlign_Center).AutoWidth().Padding(FMargin(1))
							[
								SNew(SImage).Image(FEditorStyle::GetBrush(TEXT("ArrowDown"))).Visibility_Lambda([this]() { return GetPressedVisForPacked(12); })
							]
							+ SHorizontalBox::Slot().VAlign(VAlign_Center).AutoWidth().Padding(FMargin(2))
							[
								SNew(STextBlock)
								.Text(FText::FromString("Y"))
								.ColorAndOpacity_Lambda([this]() { return GetTextColorAndOpacityForPacked(12); })
								.Font(FEditorStyle::GetFontStyle("Persona.RetargetManager.SmallBoldFont"))
							]
						]
					]
					+ SGridPanel::Slot(1, 1)[SNew(SButton).ContentPadding(0)
					.OnClicked_Lambda([this]() { return ClickForTrigger(14); })
					.VAlign(VAlign_Center).HAlign(HAlign_Center)
						[
							SNew(SHorizontalBox)
							+ SHorizontalBox::Slot().VAlign(VAlign_Center).AutoWidth().Padding(FMargin(1))
							[
								SNew(SImage).Image(FEditorStyle::GetBrush(TEXT("ArrowDown"))).Visibility_Lambda([this]() { return GetPressedVisForPacked(14); })
							]
							+ SHorizontalBox::Slot().VAlign(VAlign_Center).AutoWidth().Padding(FMargin(2))
							[
								SNew(STextBlock)
								.Text(FText::FromString("B"))
								.ColorAndOpacity_Lambda([this]() { return GetTextColorAndOpacityForPacked(14); })
								.Font(FEditorStyle::GetFontStyle("Persona.RetargetManager.SmallBoldFont"))
							]
						]
					]
				]
			];
	}

	FReply ClickForTrigger(uint16 bitOffset) const
	{
		if (FInputFrame* inputFrame = GetInputFrame())
		{
			uint16 offsettedMask = FInputFrame::FULL_MASK << bitOffset;
			uint16 value = offsettedMask & inputFrame->PackedBits;

			uint16 offsettedHoldMask = FInputFrame::HOLD_MASK << bitOffset;
			bool isHold = offsettedHoldMask & inputFrame->PackedBits;

			inputFrame->PackedBits -= value;

			if (!isHold) inputFrame->PackedBits += offsettedMask & (value + (FInputFrame::PRESS_MASK << bitOffset));
		}

		ModifyFirstOuterObject(ParentPropertyHandle);

		return FReply::Handled();
	}

	FSlateColor GetColorAndOpacityForTriggers(uint16 bitOffset) const
	{
		const FLinearColor ActiveColor(1, 1, 1, 1.0f);
		const FLinearColor HoverColor(1, 1, 1, 1.0f);
		const FLinearColor BaseColor(0.9f, 0.9f, 0.9f, 1.0f);

		if (FInputFrame* inputFrame = GetInputFrame())
		{
			uint16 offsettedHoldMask = FInputFrame::HOLD_MASK << bitOffset;
			bool isHold = offsettedHoldMask & inputFrame->PackedBits;

			uint16 offsettedPressMask = FInputFrame::PRESS_MASK << bitOffset;
			bool isPressed = offsettedPressMask & inputFrame->PackedBits;

			if (isHold) return HoverColor;

			if (isPressed) return ActiveColor;
		}

		return BaseColor;
	}

	EVisibility GetPressedVisForPacked(uint16 bitOffset) const
	{
		if (FInputFrame* inputFrame = GetInputFrame())
		{
			uint16 offsettedHoldMask = FInputFrame::HOLD_MASK << bitOffset;
			bool isHold = offsettedHoldMask & inputFrame->PackedBits;

			if (isHold) return EVisibility::Visible;
		}

		return EVisibility::Hidden;
	}

	FSlateColor GetTextColorAndOpacityForPacked(uint16 bitOffset) const
	{
		const FLinearColor HoverColor(1, 1, 1, 1);
		const FLinearColor BaseColor(1, 1, 1, 0.2);

		if (FInputFrame* inputFrame = GetInputFrame())
		{
			uint16 offsettedHoldMask = FInputFrame::HOLD_MASK << bitOffset;
			bool isHold = offsettedHoldMask & inputFrame->PackedBits;

			uint16 offsettedPressMask = FInputFrame::PRESS_MASK << bitOffset;
			bool isPressed = offsettedPressMask & inputFrame->PackedBits;

			if (isHold || isPressed) return HoverColor;
		}

		return BaseColor;
	}

	FReply ClickForStick(int32 stickIndex, int32 valueIndex)
	{
		if (FInputFrame* inputFrame = GetInputFrame())
		{
			if (stickIndex == 0)
			{
				switch (valueIndex)
				{
				case 0:
					if (inputFrame->LeftStickVer > 0 && inputFrame->LeftStickHor == 0)
					{
						inputFrame->Reset_LeftStick();
					}
					else
					{
						inputFrame->Reset_LeftStick();
						inputFrame->LeftStickVer = 1;
						inputFrame->LeftStickHor = 0;
					}
					break;
				case 1:
					if (inputFrame->LeftStickVer > 0 && inputFrame->LeftStickHor > 0)
					{
						inputFrame->Reset_LeftStick();
					}
					else
					{
						inputFrame->Reset_LeftStick();
						inputFrame->LeftStickVer = 1;
						inputFrame->LeftStickHor = 1;
					}
					break;
				case 2:
					if (inputFrame->LeftStickVer == 0 && inputFrame->LeftStickHor > 0)
					{
						inputFrame->Reset_LeftStick();
					}
					else
					{
						inputFrame->Reset_LeftStick();
						inputFrame->LeftStickVer = 0;
						inputFrame->LeftStickHor = 1;
					}
					break;
				case 3:
					if (inputFrame->LeftStickVer < 0 && inputFrame->LeftStickHor > 0)
					{
						inputFrame->Reset_LeftStick();
					}
					else
					{
						inputFrame->Reset_LeftStick();
						inputFrame->LeftStickVer = -1;
						inputFrame->LeftStickHor = 1;
					}
					break;
				case 4:
					if (inputFrame->LeftStickVer < 0 && inputFrame->LeftStickHor == 0)
					{
						inputFrame->Reset_LeftStick();
					}
					else
					{
						inputFrame->Reset_LeftStick();
						inputFrame->LeftStickVer = -1;
						inputFrame->LeftStickHor = 0;
					}
					break;
				case 5:
					if (inputFrame->LeftStickVer < 0 && inputFrame->LeftStickHor < 0)
					{
						inputFrame->Reset_LeftStick();
					}
					else
					{
						inputFrame->Reset_LeftStick();
						inputFrame->LeftStickVer = -1;
						inputFrame->LeftStickHor = -1;
					}
					break;
				case 6:
					if (inputFrame->LeftStickVer == 0 && inputFrame->LeftStickHor < 0)
					{
						inputFrame->Reset_LeftStick();
					}
					else
					{
						inputFrame->Reset_LeftStick();
						inputFrame->LeftStickVer = 0;
						inputFrame->LeftStickHor = -1;
					}
					break;
				case 7:
					if (inputFrame->LeftStickVer > 0 && inputFrame->LeftStickHor < 0)
					{
						inputFrame->Reset_LeftStick();
					}
					else
					{
						inputFrame->Reset_LeftStick();
						inputFrame->LeftStickVer = 1;
						inputFrame->LeftStickHor = -1;
					}
					break;
				}
			}
			else
			{
				switch (valueIndex)
				{
				case 0:
					if (inputFrame->RightStickVer > 0 && inputFrame->RightStickHor == 0)
					{
						inputFrame->Reset_RightStick();
					}
					else
					{
						inputFrame->Reset_RightStick();
						inputFrame->RightStickVer = 1;
						inputFrame->RightStickHor = 0;
					}
					break;
				case 1:
					if (inputFrame->RightStickVer > 0 && inputFrame->RightStickHor > 0)
					{
						inputFrame->Reset_RightStick();
					}
					else
					{
						inputFrame->Reset_RightStick();
						inputFrame->RightStickVer = 1;
						inputFrame->RightStickHor = 1;
					}
					break;
				case 2:
					if (inputFrame->RightStickVer == 0 && inputFrame->RightStickHor > 0)
					{
						inputFrame->Reset_RightStick();
					}
					else
					{
						inputFrame->Reset_RightStick();
						inputFrame->RightStickVer = 0;
						inputFrame->RightStickHor = 1;
					}
					break;
				case 3:
					if (inputFrame->RightStickVer < 0 && inputFrame->RightStickHor > 0)
					{
						inputFrame->Reset_RightStick();
					}
					else
					{
						inputFrame->Reset_RightStick();
						inputFrame->RightStickVer = -1;
						inputFrame->RightStickHor = 1;
					}
					break;
				case 4:
					if (inputFrame->RightStickVer < 0 && inputFrame->RightStickHor == 0)
					{
						inputFrame->Reset_RightStick();
					}
					else
					{
						inputFrame->Reset_RightStick();
						inputFrame->RightStickVer = -1;
						inputFrame->RightStickHor = 0;
					}
					break;
				case 5:
					if (inputFrame->RightStickVer < 0 && inputFrame->RightStickHor < 0)
					{
						inputFrame->Reset_RightStick();
					}
					else
					{
						inputFrame->Reset_RightStick();
						inputFrame->RightStickVer = -1;
						inputFrame->RightStickHor = -1;
					}
					break;
				case 6:
					if (inputFrame->RightStickVer == 0 && inputFrame->RightStickHor < 0)
					{
						inputFrame->Reset_RightStick();
					}
					else
					{
						inputFrame->Reset_RightStick();
						inputFrame->RightStickVer = 0;
						inputFrame->RightStickHor = -1;
					}
					break;
				case 7:
					if (inputFrame->RightStickVer > 0 && inputFrame->RightStickHor < 0)
					{
						inputFrame->Reset_RightStick();
					}
					else
					{
						inputFrame->Reset_RightStick();
						inputFrame->RightStickVer = 1;
						inputFrame->RightStickHor = -1;
					}
					break;
				}
			}
		}

		ModifyFirstOuterObject(ParentPropertyHandle);

		return FReply::Handled();
	}

	FSlateColor GetTextColorAndOpacityForStick(int32 stickIndex, int32 valueIndex) const
	{
		const FLinearColor HoverColor(1, 1, 1, 1);
		const FLinearColor BaseColor(1, 1, 1, 0.2);

		if (FInputFrame* inputFrame = GetInputFrame())
		{
			if (stickIndex == 0)
			{
				switch (valueIndex)
				{
				case 0:	return (inputFrame->LeftStickVer > 0 && inputFrame->LeftStickHor == 0) ? HoverColor : BaseColor;
				case 1:	return (inputFrame->LeftStickVer > 0 && inputFrame->LeftStickHor > 0) ? HoverColor : BaseColor;
				case 2:	return (inputFrame->LeftStickVer == 0 && inputFrame->LeftStickHor > 0) ? HoverColor : BaseColor;
				case 3:	return (inputFrame->LeftStickVer < 0 && inputFrame->LeftStickHor > 0) ? HoverColor : BaseColor;
				case 4:	return (inputFrame->LeftStickVer < 0 && inputFrame->LeftStickHor == 0) ? HoverColor : BaseColor;
				case 5:	return (inputFrame->LeftStickVer < 0 && inputFrame->LeftStickHor < 0) ? HoverColor : BaseColor;
				case 6:	return (inputFrame->LeftStickVer == 0 && inputFrame->LeftStickHor < 0) ? HoverColor : BaseColor;
				case 7:	return (inputFrame->LeftStickVer > 0 && inputFrame->LeftStickHor < 0) ? HoverColor : BaseColor;
				}
			}
			else
			{
				switch (valueIndex)
				{
				case 0:	return (inputFrame->RightStickVer > 0 && inputFrame->RightStickHor == 0) ? HoverColor : BaseColor;
				case 1:	return (inputFrame->RightStickVer > 0 && inputFrame->RightStickHor > 0) ? HoverColor : BaseColor;
				case 2:	return (inputFrame->RightStickVer == 0 && inputFrame->RightStickHor > 0) ? HoverColor : BaseColor;
				case 3:	return (inputFrame->RightStickVer < 0 && inputFrame->RightStickHor > 0) ? HoverColor : BaseColor;
				case 4:	return (inputFrame->RightStickVer < 0 && inputFrame->RightStickHor == 0) ? HoverColor : BaseColor;
				case 5:	return (inputFrame->RightStickVer < 0 && inputFrame->RightStickHor < 0) ? HoverColor : BaseColor;
				case 6:	return (inputFrame->RightStickVer == 0 && inputFrame->RightStickHor < 0) ? HoverColor : BaseColor;
				case 7:	return (inputFrame->RightStickVer > 0 && inputFrame->RightStickHor < 0) ? HoverColor : BaseColor;
				}
			}
		}

		return BaseColor;
	}
};

TSharedRef<IPropertyTypeCustomization> FPTCustomization_IFS::MakeInstance()
{
	return MakeShareable(new FPTCustomization_IFS);
}

void FPTCustomization_IFS::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	//FPTCustomization_Base::CustomizeHeader(PropertyHandle, HeaderRow, CustomizationUtils);
}

void FPTCustomization_IFS::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) {
	InternalPropertyHandle = PropertyHandle;

	ChildBuilder.AddCustomRow(LOCTEXT("SearchString", "Joystick Input"))[
		SNew(SVerticalBox)
			+ SVerticalBox::Slot().FillHeight(1)[SAssignNew(grid, SGridPanel).FillColumn(0, 1).FillColumn(1, 0)]
			+ SVerticalBox::Slot().AutoHeight().Padding(8)
			[
				SNew(SButton).OnClicked(this, &FPTCustomization_IFS::Add).HAlign(HAlign_Center)
				.ToolTipText(LOCTEXT("AddButtonToolTip", "Add"))
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot().AutoWidth().Padding(FMargin(2))
					[
						SNew(SImage).Image(FEditorStyle::GetBrush("Plus"))
					]
					+ SHorizontalBox::Slot()
					.VAlign(VAlign_Center).AutoWidth().Padding(FMargin(2))
					[
						SNew(STextBlock).Text(LOCTEXT("AddButtonText", "Add"))
					]
				]
			]
	];

	if (FInputFrameStack* InputFrameStack = GetPropertyAs<FInputFrameStack>(InternalPropertyHandle))
	{
		for (int32 i = 0; i < InputFrameStack->Frames.Num(); i++)
		{
			AddInputFrameWidget(i);
		}
	}
}

FReply FPTCustomization_IFS::Add()
{
	if (FInputFrameStack* inputFrameStack = GetPropertyAs<FInputFrameStack>(InternalPropertyHandle))
	{
		int32 emplacedIndex = inputFrameStack->Frames.Emplace();
		AddInputFrameWidget(emplacedIndex);


		ModifyFirstOuterObject(InternalPropertyHandle);
	}

	return FReply::Handled();
}

FReply FPTCustomization_IFS::Remove(TSharedPtr<class SInputFrameWidget> inputFrameWidget, TSharedPtr<SButton> button)
{
	if (FInputFrameStack* InputFrameStack = GetPropertyAs<FInputFrameStack>(InternalPropertyHandle))
	{
		grid->RemoveSlot(inputFrameWidget.ToSharedRef());
		grid->RemoveSlot(button.ToSharedRef());

		if (FChildren* children = grid->GetChildren())
		{
			size_t num = children->Num();
			for (size_t i = 0; i < num; i++)
			{
				TSharedRef<SInputFrameWidget> ref = StaticCastSharedRef<SInputFrameWidget>(children->GetChildAt(i));
				if (ref->FrameIndex > inputFrameWidget->FrameIndex) ref->FrameIndex--;
			}
		}

		InputFrameStack->Frames.RemoveAt(inputFrameWidget->FrameIndex);


		ModifyFirstOuterObject(InternalPropertyHandle);
	}

	return FReply::Handled();
}

void FPTCustomization_IFS::AddInputFrameWidget(int32 frameIndex)
{
	TSharedPtr<SInputFrameWidget> element = SNew(SInputFrameWidget, InternalPropertyHandle, frameIndex);

	TSharedPtr<SButton> button;

	grid->AddSlot(0, frameIndex).Padding(0, 8)[element.ToSharedRef()];
	grid->AddSlot(1, frameIndex).Padding(8, 8).VAlign(VAlign_Center).HAlign(HAlign_Center)
		[
			SAssignNew(button, SButton).ButtonStyle(FEditorStyle::Get(), "HoverHintOnly").ToolTipText(LOCTEXT("RemoveButtonTooltip", "Remove"))
				[
					SNew(SImage).Image(FEditorStyle::GetBrush("Cross"))
				]
		];

	button->SetOnClicked(FOnClicked::CreateRaw(this, &FPTCustomization_IFS::Remove, element, button));
}

#undef LOCTEXT_NAMESPACE