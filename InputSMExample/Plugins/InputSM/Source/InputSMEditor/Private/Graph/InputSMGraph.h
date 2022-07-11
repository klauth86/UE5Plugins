// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EdGraph/EdGraph.h"
#include "InputSMGraph.generated.h"

UCLASS()
class UInputSMGraph : public UEdGraph
{
	GENERATED_UCLASS_BODY()

public:

	virtual void PreSave(FObjectPreSaveContext SaveContext) override;
};