// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EdGraphUtilities.h"

class SGraphNode;
class UEdGraphNode;
class FConnectionDrawingPolicy;
class UEdGraphSchema;
class UEdGraph;

struct INPUTSMEDITOR_API FInputSMGraphNodeFactory : public FGraphPanelNodeFactory
{
	virtual TSharedPtr<SGraphNode> CreateNode(UEdGraphNode* InNode) const override;
};

struct INPUTSMEDITOR_API FInputSMGraphPinFactory : public FGraphPanelPinFactory
{
public:
	virtual TSharedPtr<class SGraphPin> CreatePin(class UEdGraphPin* Pin) const override;
};

struct INPUTSMEDITOR_API FInputSMGraphPinConnectionFactory : public FGraphPanelPinConnectionFactory
{
public:

	virtual FConnectionDrawingPolicy* CreateConnectionPolicy(const UEdGraphSchema* Schema, int32 InBackLayerID, int32 InFrontLayerID, float ZoomFactor, const class FSlateRect& InClippingRect, class FSlateWindowElementList& InDrawElements, UEdGraph* InGraphObj) const override;
};