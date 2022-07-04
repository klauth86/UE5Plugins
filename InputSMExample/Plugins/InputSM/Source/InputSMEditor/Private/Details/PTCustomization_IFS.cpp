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

	FInputFrameStack* InputFrameStack;

	int32 FrameIndex;

	FInputFrame* GetInputFrame() const { return InputFrameStack->Frames.IsValidIndex(FrameIndex) ? &InputFrameStack->Frames[FrameIndex] : nullptr; }

	void Construct(FArguments InArgs, FInputFrameStack* inputFrameStack, int32 frameIndex)
	{
		SetContentScale(InArgs._ContentScale);

		InputFrameStack = inputFrameStack;
		
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
				+ SGridPanel::Slot(0, 0).VAlign(VAlign_Center)
				[
					SNew(SGridPanel)

					+ SGridPanel::Slot(0, 0)[SNew(SButton)
					.ButtonColorAndOpacity_Lambda([this]() { return GetColorAndOpacityForTriggers(0); })
					.Text_Lambda([this]() { return GetTextForTriggers("LB", 0); })
					.OnClicked_Lambda([this]() { return ClickForTrigger(0); })
					]

					+ SGridPanel::Slot(0, 1)[SNew(SButton)
					.ButtonColorAndOpacity_Lambda([this]() { return GetColorAndOpacityForTriggers(2); })
					.Text_Lambda([this]() { return GetTextForTriggers("LT", 2); })
					.OnClicked_Lambda([this]() { return ClickForTrigger(2); })
					]
				]

				+ SGridPanel::Slot(2, 0).VAlign(VAlign_Center)
				[
					SNew(SGridPanel)

					+ SGridPanel::Slot(0, 0)[SNew(SButton)
					//.ButtonColorAndOpacity_Raw(this, &SInputFrameWidget::ColorAndOpacity_LeftStick_315)
					.Text(FText::FromString("315"))
					.OnClicked_Lambda([this]() { return ClickForStick(0, 7); })
					]
					+ SGridPanel::Slot(1, 0)[SNew(SButton)
					//.ButtonColorAndOpacity_Raw(this, &SInputFrameWidget::ColorAndOpacity_LeftStick_0)
					.Text(FText::FromString("0"))
					.OnClicked_Lambda([this]() { return ClickForStick(0, 0); })
					]
					+ SGridPanel::Slot(2, 0)[SNew(SButton)
					//.ButtonColorAndOpacity_Raw(this, &SInputFrameWidget::ColorAndOpacity_LeftStick_45)
					.Text(FText::FromString("45"))
					.OnClicked_Lambda([this]() { return ClickForStick(0, 1); })
					]

					+ SGridPanel::Slot(0, 1)[SNew(SButton)
					//.ButtonColorAndOpacity_Raw(this, &SInputFrameWidget::ColorAndOpacity_LeftStick_270)
					.Text(FText::FromString("270"))
					.OnClicked_Lambda([this]() { return ClickForStick(0, 6); })
					]
					+ SGridPanel::Slot(2, 1)[SNew(SButton)
					//.ButtonColorAndOpacity_Raw(this, &SInputFrameWidget::ColorAndOpacity_LeftStick_90)
					.Text(FText::FromString("90"))
					.OnClicked_Lambda([this]() { return ClickForStick(0, 2); })
					]

					+ SGridPanel::Slot(0, 2)[SNew(SButton)
					//.ButtonColorAndOpacity_Raw(this, &SInputFrameWidget::ColorAndOpacity_LeftStick_225)
					.Text(FText::FromString("225"))
					.OnClicked_Lambda([this]() { return ClickForStick(0, 5); })
					]
					+ SGridPanel::Slot(1, 2)[SNew(SButton)
					//.ButtonColorAndOpacity_Raw(this, &SInputFrameWidget::ColorAndOpacity_LeftStick_180)
					.Text(FText::FromString("180"))
					.OnClicked_Lambda([this]() { return ClickForStick(0, 4); })
					]
					+ SGridPanel::Slot(2, 2)[SNew(SButton)
					//.ButtonColorAndOpacity_Raw(this, &SInputFrameWidget::ColorAndOpacity_LeftStick_135)
					.Text(FText::FromString("135"))
					.OnClicked_Lambda([this]() { return ClickForStick(0, 3); })
					]
				]

				+ SGridPanel::Slot(4, 0).VAlign(VAlign_Center)
				[
					SNew(SGridPanel)

					+ SGridPanel::Slot(0, 0)[SNew(SButton)
					//.ButtonColorAndOpacity_Raw(this, &SInputFrameWidget::ColorAndOpacity_LeftStick_315)
					.Text(FText::FromString("315"))
					.OnClicked_Lambda([this]() { return ClickForStick(1, 7); })
					]
					+ SGridPanel::Slot(1, 0)[SNew(SButton)
					//.ButtonColorAndOpacity_Raw(this, &SInputFrameWidget::ColorAndOpacity_LeftStick_0)
					.Text(FText::FromString("0"))
					.OnClicked_Lambda([this]() { return ClickForStick(1, 0); })
					]
					+ SGridPanel::Slot(2, 0)[SNew(SButton)
					//.ButtonColorAndOpacity_Raw(this, &SInputFrameWidget::ColorAndOpacity_LeftStick_45)
					.Text(FText::FromString("45"))
					.OnClicked_Lambda([this]() { return ClickForStick(1, 1); })
					]

					+ SGridPanel::Slot(0, 1)[SNew(SButton)
					//.ButtonColorAndOpacity_Raw(this, &SInputFrameWidget::ColorAndOpacity_LeftStick_270)
					.Text(FText::FromString("270"))
					.OnClicked_Lambda([this]() { return ClickForStick(1, 6); })
					]
					+ SGridPanel::Slot(2, 1)[SNew(SButton)
					//.ButtonColorAndOpacity_Raw(this, &SInputFrameWidget::ColorAndOpacity_LeftStick_90)
					.Text(FText::FromString("90"))
					.OnClicked_Lambda([this]() { return ClickForStick(1, 2); })
					]

					+ SGridPanel::Slot(0, 2)[SNew(SButton)
					//.ButtonColorAndOpacity_Raw(this, &SInputFrameWidget::ColorAndOpacity_LeftStick_225)
					.Text(FText::FromString("225"))
					.OnClicked_Lambda([this]() { return ClickForStick(1, 5); })
					]
					+ SGridPanel::Slot(1, 2)[SNew(SButton)
					//.ButtonColorAndOpacity_Raw(this, &SInputFrameWidget::ColorAndOpacity_LeftStick_180)
					.Text(FText::FromString("180"))
					.OnClicked_Lambda([this]() { return ClickForStick(1, 4); })
					]
					+ SGridPanel::Slot(2, 2)[SNew(SButton)
					//.ButtonColorAndOpacity_Raw(this, &SInputFrameWidget::ColorAndOpacity_LeftStick_135)
					.Text(FText::FromString("135"))
					.OnClicked_Lambda([this]() { return ClickForStick(1, 3); })
					]
				]

				+ SGridPanel::Slot(6, 0).VAlign(VAlign_Center)
				[
					SNew(SGridPanel)

					+ SGridPanel::Slot(0, 0)[SNew(SButton)
					.ButtonColorAndOpacity_Lambda([this]() { return GetColorAndOpacityForTriggers(4); })
					.Text_Lambda([this]() { return GetTextForTriggers("RB", 4); })
					.OnClicked_Lambda([this]() { return ClickForTrigger(4); })
					]

					+ SGridPanel::Slot(0, 1)[SNew(SButton)
					.ButtonColorAndOpacity_Lambda([this]() { return GetColorAndOpacityForTriggers(6); })
					.Text_Lambda([this]() { return GetTextForTriggers("RT", 6); })
					.OnClicked_Lambda([this]() { return ClickForTrigger(6); })
					]
				]

				+ SGridPanel::Slot(8, 0).VAlign(VAlign_Center)
				[
					SNew(SGridPanel)

					+ SGridPanel::Slot(0, 0)[SNew(SButton)
					//.ButtonColorAndOpacity_Raw(this, &SInputFrameWidget::ColorAndOpacity_X)
					.Text_Lambda([this]() { return GetTextForTriggers("X", 8); })
					.OnClicked_Lambda([this]() { return ClickForTrigger(8); })
					]
					+ SGridPanel::Slot(0, 1)[SNew(SButton)
					//.ButtonColorAndOpacity_Raw(this, &SInputFrameWidget::ColorAndOpacity_A)
					.Text_Lambda([this]() { return GetTextForTriggers("A", 10); })
					.OnClicked_Lambda([this]() { return ClickForTrigger(10); })
					]
					+ SGridPanel::Slot(1, 0)[SNew(SButton)
					//.ButtonColorAndOpacity_Raw(this, &SInputFrameWidget::ColorAndOpacity_Y)
					.Text_Lambda([this]() { return GetTextForTriggers("Y", 12); })
					.OnClicked_Lambda([this]() { return ClickForTrigger(12); })
					]
					+ SGridPanel::Slot(1, 1)[SNew(SButton)
					//.ButtonColorAndOpacity_Raw(this, &SInputFrameWidget::ColorAndOpacity_B)
					.Text_Lambda([this]() { return GetTextForTriggers("B", 14); })
					.OnClicked_Lambda([this]() { return ClickForTrigger(14); })
					]
				]
			];
	}

	FText GetTextForTriggers(FString base, uint16 bitOffset) const
	{
		if (FInputFrame* inputFrame = GetInputFrame())
		{
			uint16 offsettedHoldMask = FInputFrame::HOLD_MASK << bitOffset;
			bool isHold = offsettedHoldMask & inputFrame->PackedBits;

			return FText::FromString(isHold ? "Holded " + base : base);
		}

		return FText::GetEmpty();
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

	FReply ClickForStick(int32 stickIndex, int32 valueIndex)
	{
		if (FInputFrame* inputFrame = GetInputFrame())
		{
			if (stickIndex)
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

		return FReply::Handled();
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
			+ SVerticalBox::Slot().AutoHeight().Padding(8)[SNew(SButton).OnClicked(this, &FPTCustomization_IFS::Add).HAlign(HAlign_Center)[
				SNew(STextBlock).Text(LOCTEXT("Add", "ADD"))
			]]
	];

	if (FInputFrameStack* InputFrameStack = GetPropertyAs<FInputFrameStack>())
	{
		for (int32 i = 0; i < InputFrameStack->Frames.Num(); i++)
		{
			AddWidget(InputFrameStack, i);
		}
	}
}

FReply FPTCustomization_IFS::Add()
{
	if (FInputFrameStack* inputFrameStack = GetPropertyAs<FInputFrameStack>())
	{
		int32 emplacedIndex = inputFrameStack->Frames.Emplace();

		if (emplacedIndex > 0)
		{
			FInputFrame& parentElement = inputFrameStack->Frames[emplacedIndex - 1];
			FInputFrame& emplacedElement = inputFrameStack->Frames[emplacedIndex];
		}
		AddWidget(inputFrameStack, emplacedIndex);
	}

	return FReply::Handled();
}

FReply FPTCustomization_IFS::Remove(TSharedPtr<class SInputFrameWidget> inputFrameWidget, TSharedPtr<SButton> button)
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

	if (FInputFrameStack* InputFrameStack = GetPropertyAs<FInputFrameStack>())
	{
		InputFrameStack->Frames.RemoveAt(inputFrameWidget->FrameIndex);
	}

	return FReply::Handled();
}

void FPTCustomization_IFS::AddWidget(FInputFrameStack* inputFrameStack, int32 frameIndex)
{
	TSharedPtr<SInputFrameWidget> element = SNew(SInputFrameWidget, inputFrameStack, frameIndex);

	TSharedPtr<SButton> button;

	grid->AddSlot(0, frameIndex).Padding(0, 8)[element.ToSharedRef()];
	grid->AddSlot(1, frameIndex).Padding(8, 8).VAlign(VAlign_Center)[SAssignNew(button, SButton).VAlign(VAlign_Center)[SNew(STextBlock).Text(LOCTEXT("Remove", "REMOVE"))]];

	button->SetOnClicked(FOnClicked::CreateRaw(this, &FPTCustomization_IFS::Remove, element, button));
}

#undef LOCTEXT_NAMESPACE