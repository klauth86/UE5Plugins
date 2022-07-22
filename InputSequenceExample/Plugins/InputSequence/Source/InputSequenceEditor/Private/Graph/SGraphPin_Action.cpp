// Fill out your copyright notice in the Description page of Project Settings.

#include "Graph/SGraphPin_Action.h"
#include "SPinTypeSelector.h"
#include "SLevelOfDetailBranchNode.h"
#include "Widgets/Layout/SWrapBox.h"

#define LOCTEXT_NAMESPACE "SGraphPin_Action"

class SToolTip_Mock : public SLeafWidget, public IToolTip
{
public:

	SLATE_BEGIN_ARGS(SToolTip_Mock)	{}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs) {}

	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override { return LayerId; }
	virtual FVector2D ComputeDesiredSize(float) const override { return FVector2D::ZeroVector; }

	virtual TSharedRef<class SWidget> AsWidget() { return SNullWidget::NullWidget; }
	virtual TSharedRef<SWidget> GetContentWidget() { return SNullWidget::NullWidget; }
	virtual void SetContentWidget(const TSharedRef<SWidget>& InContentWidget) override {}
	virtual bool IsEmpty() const override { return false; }
	virtual bool IsInteractive() const { return false; }
	virtual void OnOpening() override {}
	virtual void OnClosed() override {}
};

void SGraphPin_Action::Construct(const FArguments& Args, UEdGraphPin* InPin)
{
	SGraphPin::FArguments InArgs = SGraphPin::FArguments();

	bUsePinColorForText = InArgs._UsePinColorForText;
	this->SetCursor(EMouseCursor::Default);
	this->SetToolTipText(LOCTEXT("ActionPin_ToolTip", "Mock ToolTip"));

	SetVisibility(MakeAttributeSP(this, &SGraphPin_Action::GetPinVisiblity));

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

	const bool bIsInput = (GetDirection() == EGPD_Input);

	// Create the pin icon widget
	TSharedRef<SWidget> SelfPinWidgetRef = SPinTypeSelector::ConstructPinTypeImage(
		MakeAttributeSP(this, &SGraphPin_Action::GetPinIcon),
		MakeAttributeSP(this, &SGraphPin_Action::GetPinColor),
		MakeAttributeSP(this, &SGraphPin_Action::GetSecondaryPinIcon),
		MakeAttributeSP(this, &SGraphPin_Action::GetSecondaryPinColor));

	SelfPinWidgetRef->SetVisibility(MakeAttributeRaw(this, &SGraphPin_Action::Visibility_Raw_SelfPin));

	TSharedRef<SWidget> PinWidgetRef = 
		SNew(SOverlay)
		+ SOverlay::Slot()
		[
			SNew(SImage).Image(FEditorStyle::GetBrush(TEXT("Icons.CircleArrowUp")))
			.Visibility_Raw(this, &SGraphPin_Action::Visibility_Raw_ArrowUp)
		]
	+ SOverlay::Slot().VAlign(VAlign_Center).HAlign(HAlign_Center)
		[
			SelfPinWidgetRef
		];

	PinImage = PinWidgetRef;

	// Create the pin indicator widget (used for watched values)
	static const FName NAME_NoBorder("NoBorder");
	TSharedRef<SWidget> PinStatusIndicator =
		SNew(SButton)
		.ButtonStyle(FEditorStyle::Get(), NAME_NoBorder)
		.Visibility(this, &SGraphPin_Action::GetPinStatusIconVisibility)
		.ContentPadding(0)
		.OnClicked(this, &SGraphPin_Action::ClickedOnPinStatusIcon)
		[
			SNew(SImage)
			.Image(this, &SGraphPin_Action::GetPinStatusIcon)
		];

	TSharedRef<SWidget> LabelWidget = GetLabelWidget(InArgs._PinLabelStyle);

	// Create the widget used for the pin body (status indicator, label, and value)
	LabelAndValue =
		SNew(SWrapBox)
		.PreferredSize(150.f);

	if (!bIsInput) // Input pin
	{
		LabelAndValue->AddSlot()
			.VAlign(VAlign_Center)
			[
				PinStatusIndicator
			];

		LabelAndValue->AddSlot()
			.VAlign(VAlign_Center)
			[
				LabelWidget
			];
	}
	else // Output pin
	{
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
	}

	TSharedPtr<SHorizontalBox> PinContent;
	if (bIsInput) // Input pin
	{
		FullPinHorizontalRowWidget = PinContent =
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(0, 0, InArgs._SideToSideMargin, 0)
			[
				PinWidgetRef
			]
		+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(0, 0, InArgs._SideToSideMargin, 0)
			[
				SNew(SImage).Image(FEditorStyle::GetBrush(TEXT("Icons.CircleArrowUp")))
			]
		+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				LabelAndValue.ToSharedRef()
			];
	}
	else // Output pin
	{
		FullPinHorizontalRowWidget = PinContent = SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(0, 0, InArgs._SideToSideMargin, 0)
			[
				SNew(SButton).ToolTipText_Raw(this, &SGraphPin_Action::ToolTipText_Raw_RemovePin)
				.Cursor(EMouseCursor::Hand)
				.ButtonStyle(FEditorStyle::Get(), "NoBorder")
				.ForegroundColor(FSlateColor::UseForeground())
				.OnClicked_Raw(this, &SGraphPin_Action::OnClicked_Raw_RemovePin)
				[
					SNew(SImage).Image(FEditorStyle::GetBrush("Cross"))
				]
			]
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
			.Padding(0, 0, InArgs._SideToSideMargin, 0)
			[
				SNew(SButton).ToolTipText_Raw(this, &SGraphPin_Action::ToolTipText_Raw_TogglePin)
				.Cursor(EMouseCursor::Hand)
				.ButtonStyle(FEditorStyle::Get(), "NoBorder")
				.ForegroundColor(FSlateColor::UseForeground())
				.OnClicked_Raw(this, &SGraphPin_Action::OnClicked_Raw_TogglePin)
				[
					SNew(SImage).Image(FEditorStyle::GetBrush(TEXT("Icons.CircleArrowDown")))
				]
			]
		+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				PinWidgetRef
			];
	}

	// Set up a hover for pins that is tinted the color of the pin.
	SBorder::Construct(SBorder::FArguments()
		.BorderImage(FEditorStyle::GetBrush("NoBorder"))
		.BorderBackgroundColor(this, &SGraphPin_Action::GetPinColor)
		[
			SNew(SLevelOfDetailBranchNode)
			.UseLowDetailSlot(this, &SGraphPin_Action::UseLowDetailPinNames)
			.LowDetail()
			[
				//@TODO: Try creating a pin-colored line replacement that doesn't measure text / call delegates but still renders
				PinWidgetRef
			]
			.HighDetail()
			[
				PinContent.ToSharedRef()
			]
		]
	);

	SetToolTip(SNew(SToolTip_Mock));
}

#undef LOCTEXT_NAMESPACE