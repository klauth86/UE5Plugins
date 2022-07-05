// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SGraphNode.h"

class UInputSMGraphNode_Transition;

class INPUTSMEDITOR_API SInputSMGraphNode_Transition : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SInputSMGraphNode_Transition) {}
	SLATE_END_ARGS()

		void Construct(const FArguments& InArgs, UInputSMGraphNode_Transition* InNode);

	// SNodePanel::SNode interface
	virtual void GetNodeInfoPopups(FNodeInfoContext* Context, TArray<FGraphInformationPopupInfo>& Popups) const override {}
	virtual void MoveTo(const FVector2D& NewPosition, FNodeSet& NodeFilter, bool bMarkDirty = true) override {}
	virtual bool RequiresSecondPassLayout() const override { return true; }
	virtual void PerformSecondPassLayout(const TMap< UObject*, TSharedRef<SNode> >& NodeToWidgetLookup) const override;
	// End of SNodePanel::SNode interface

	// SGraphNode interface
	virtual void UpdateGraphNode() override;
	// End of SGraphNode interface

	void PositionBetweenTwoNodesWithOffset(const FGeometry& StartGeom, const FGeometry& EndGeom, int32 NodeIndex, int32 MaxNodes) const;

	static FLinearColor StaticGetTransitionColor(UInputSMGraphNode_Transition* TransNode, bool bIsHovered);

	void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);

	void OnMouseLeave(const FPointerEvent& MouseEvent);

private:

	FSlateColor GetTransitionColor() const;

	const FSlateBrush* GetTransitionIconImage() const;

	mutable TWeakPtr<SNode> PrevStateNodeWidgetPtr;
};