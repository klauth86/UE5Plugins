// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "InputFrameStack.h"
#include "InputSM.generated.h"

class UEdGraph;

USTRUCT()
struct INPUTSM_API FInputSMTransition
{
	GENERATED_USTRUCT_BODY()

public:

	FInputSMTransition() { TargetIndex = INDEX_NONE; }

	UPROPERTY(EditAnywhere)
		int32 TargetIndex;

	FInputFrameStack ActivationStack;
};

USTRUCT()
struct INPUTSM_API FInputSMNode
{
	GENERATED_USTRUCT_BODY()

public:

	FInputSMNode() {}

	UPROPERTY(VisibleAnywhere)
		TArray<FInputSMTransition> Transitions;
};

UCLASS()
class INPUTSM_API UInputSM : public UObject
{
	GENERATED_BODY()

public:

#if WITH_EDITORONLY_DATA
	UPROPERTY()
		UEdGraph* EdGraph;
#endif

	TMap<int32, FInputSMNode>& GetInputGraph() { return InputGraph; }

protected:

	UPROPERTY()
		TMap<int32, FInputSMNode> InputGraph;
};