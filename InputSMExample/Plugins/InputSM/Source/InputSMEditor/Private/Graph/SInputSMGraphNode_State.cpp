// Fill out your copyright notice in the Description page of Project Settings.

#include "Graph/SInputSMGraphNode_State.h"
#include "Graph/InputSMGraphNode_Base.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"
#include "SgraphPin.h"

class SInputSMOutputPin : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SInputSMOutputPin) {}
	SLATE_END_ARGS()

		void Construct(const FArguments& InArgs, UEdGraphPin* InPin);
protected:
	// Begin SGraphPin interface
	virtual TSharedRef<SWidget>	GetDefaultValueWidget() override;
	// End SGraphPin interface

	const FSlateBrush* GetPinBorder() const;
};

void SInputSMOutputPin::Construct(const FArguments& InArgs, UEdGraphPin* InPin)
{
	this->SetCursor(EMouseCursor::Default);

	bShowLabel = true;

	GraphPinObj = InPin;
	check(GraphPinObj != NULL);

	const UEdGraphSchema* Schema = GraphPinObj->GetSchema();
	check(Schema);

	// Set up a hover for pins that is tinted the color of the pin.
	SBorder::Construct(SBorder::FArguments()
		.BorderImage(this, &SInputSMOutputPin::GetPinBorder)
		.BorderBackgroundColor(this, &SInputSMOutputPin::GetPinColor)
		.OnMouseButtonDown(this, &SInputSMOutputPin::OnPinMouseDown)
		.Cursor(this, &SInputSMOutputPin::GetPinCursor)
	);
}

TSharedRef<SWidget>	SInputSMOutputPin::GetDefaultValueWidget()
{
	return SNew(STextBlock);
}

const FSlateBrush* SInputSMOutputPin::GetPinBorder() const
{
	return (IsHovered())
		? FEditorStyle::GetBrush(TEXT("Graph.StateNode.Pin.BackgroundHovered"))
		: FEditorStyle::GetBrush(TEXT("Graph.StateNode.Pin.Background"));
}

void SInputSMGraphNode_State::Construct(const FArguments& InArgs, UInputSMGraphNode_State* InNode)
{
	GraphNode = InNode;

	SetCursor(EMouseCursor::CardinalCross);

	UpdateGraphNode();
}

FSlateColor SInputSMGraphNode_State::GetBorderBackgroundColor() const
{
	FLinearColor InactiveStateColor(0.08f, 0.08f, 0.08f);
	FLinearColor ActiveStateColorDim(0.4f, 0.3f, 0.15f);
	FLinearColor ActiveStateColorBright(1.f, 0.6f, 0.35f);

	return InactiveStateColor;
}

void SInputSMGraphNode_State::UpdateGraphNode()
{
	InputPins.Empty();
	OutputPins.Empty();

	// Reset variables that are going to be exposed, in case we are refreshing an already setup node.
	RightNodeBox.Reset();
	LeftNodeBox.Reset();

	const FSlateBrush* NodeTypeIcon = GetNameIcon();

	FLinearColor TitleShadowColor(0.6f, 0.6f, 0.6f);
	TSharedPtr<SErrorText> ErrorText;
	TSharedPtr<SNodeTitle> NodeTitle = SNew(SNodeTitle, GraphNode);

	this->ContentScale.Bind(this, &SGraphNode::GetContentScale);
	this->GetOrAddSlot(ENodeZone::Center)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SBorder)
			.BorderImage(FEditorStyle::GetBrush("Graph.StateNode.Body"))
		.Padding(0)
		.BorderBackgroundColor(this, &SInputSMGraphNode_State::GetBorderBackgroundColor)
		[
			SNew(SOverlay)

			// PIN AREA
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SAssignNew(RightNodeBox, SVerticalBox)
		]

	// STATE NAME AREA
	+ SOverlay::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.Padding(10.0f)
		[
			SNew(SBorder)
			.BorderImage(FEditorStyle::GetBrush("Graph.StateNode.ColorSpill"))
		.BorderBackgroundColor(TitleShadowColor)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.Visibility(EVisibility::SelfHitTestInvisible)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			// POPUP ERROR MESSAGE
			SAssignNew(ErrorText, SErrorText)
			.BackgroundColor(this, &SInputSMGraphNode_State::GetErrorColor)
		.ToolTipText(this, &SInputSMGraphNode_State::GetErrorMsgToolTip)
		]
	+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
			SNew(SImage)
			.Image(NodeTypeIcon)
		]
	+ SHorizontalBox::Slot()
		.Padding(FMargin(4.0f, 0.0f, 4.0f, 0.0f))
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SAssignNew(InlineEditableText, SInlineEditableTextBlock)
			.Style(FEditorStyle::Get(), "Graph.StateNode.NodeTitleInlineEditableText")
		.Text(NodeTitle.Get(), &SNodeTitle::GetHeadTitle)
		.OnVerifyTextChanged(this, &SInputSMGraphNode_State::OnVerifyNameTextChanged)
		.OnTextCommitted(this, &SInputSMGraphNode_State::OnNameTextCommited)
		.IsReadOnly(this, &SInputSMGraphNode_State::IsNameReadOnly)
		.IsSelected(this, &SInputSMGraphNode_State::IsSelectedExclusively)
		]
	+ SVerticalBox::Slot()
		.AutoHeight()
		[
			NodeTitle.ToSharedRef()
		]
		]
		]
		]
		]
		];

	ErrorReporting = ErrorText;
	ErrorReporting->SetError(ErrorMsg);
	CreatePinWidgets();
}

void SInputSMGraphNode_State::CreatePinWidgets()
{
	UInputSMGraphNode_State* StateNode = CastChecked<UInputSMGraphNode_State>(GraphNode);

	UEdGraphPin* CurPin = StateNode->GetOutputPin();
	if (!CurPin->bHidden)
	{
		TSharedPtr<SGraphPin> NewPin = SNew(SInputSMOutputPin, CurPin);

		this->AddPin(NewPin.ToSharedRef());
	}
}

void SInputSMGraphNode_State::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
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

FText SInputSMGraphNode_State::GetPreviewCornerText() const
{
	UInputSMGraphNode_State* StateNode = CastChecked<UInputSMGraphNode_State>(GraphNode);

	return FText::Format(NSLOCTEXT("SInputSMGraphNode_State", "PreviewCornerStateText", "{0} state"), FText::FromString(StateNode->GetName()));
}

const FSlateBrush* SInputSMGraphNode_State::GetNameIcon() const
{
	return FEditorStyle::GetBrush(TEXT("Graph.StateNode.Icon"));
}