// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EdGraph/EdGraphSchema.h"
#include "InputSMGraphSchemaAction_NewStateNode.generated.h"

USTRUCT()
struct INPUTSMEDITOR_API FInputSMGraphSchemaAction_NewStateNode : public FEdGraphSchemaAction
{
	GENERATED_USTRUCT_BODY()

	UEdGraphNode* NodeTemplate;

	FInputSMGraphSchemaAction_NewStateNode()
		: FEdGraphSchemaAction()
		, NodeTemplate(nullptr)
	{}

	FInputSMGraphSchemaAction_NewStateNode(FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGrouping)
		: FEdGraphSchemaAction(MoveTemp(InNodeCategory), MoveTemp(InMenuDesc), MoveTemp(InToolTip), InGrouping)
		, NodeTemplate(nullptr)
	{}

	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

	template <typename NodeType>
	static NodeType* SpawnNodeFromTemplate(class UEdGraph* ParentGraph, NodeType* InTemplateNode, const FVector2D Location = FVector2D(0.0f, 0.0f), bool bSelectNewNode = true)
	{
		FInputSMGraphSchemaAction_NewStateNode Action;
		Action.NodeTemplate = InTemplateNode;

		return Cast<NodeType>(Action.PerformAction(ParentGraph, NULL, Location, bSelectNewNode));
	}
};
