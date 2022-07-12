// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "InputSequenceAsset.generated.h"

class UEdGraph;

UCLASS()
class INPUTSEQUENCE_API UInputSequenceAsset : public UObject
{
	GENERATED_BODY()

public:

#if WITH_EDITORONLY_DATA
	UPROPERTY()
		UEdGraph* EdGraph;
#endif
};