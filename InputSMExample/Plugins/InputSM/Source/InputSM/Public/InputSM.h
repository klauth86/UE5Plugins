// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "InputFrame.h"
#include "InputSM.generated.h"

class UEdGraph;

USTRUCT()
struct INPUTSM_API FInputSM_Transition
{
	GENERATED_USTRUCT_BODY()

public:

	FInputSM_Transition() { TargetIndex = INDEX_NONE; ActivationStack.Reset(); ResetActiveFrame(); }

	void ProcessInput(const FInputFrame& inputFrame);

	bool IsOpen() const { return ActivationStack.Frames.Num() == ActiveFrameIndex + 1; }

	void ResetActiveFrame() { ActiveFrameIndex = INDEX_NONE; }

	UPROPERTY()
		int32 TargetIndex;

	UPROPERTY()
		FInputFrameStack ActivationStack;

	int32 ActiveFrameIndex;
};

USTRUCT()
struct INPUTSM_API FInputSM_State
{
	GENERATED_USTRUCT_BODY()

public:

	FInputSM_State() { StateName = NAME_None; StateAsset = nullptr; Transitions.Empty(); }

	UPROPERTY()
		FName StateName;

	UPROPERTY()
		UObject* StateAsset;

	UPROPERTY()
		TArray<FInputSM_Transition> Transitions;
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

	TArray<FInputSM_State>& GetStates() { return States; }

	const FInputSM_State* GetActiveState() const { return States.IsValidIndex(ActiveStateIndex) ? &States[ActiveStateIndex] : nullptr; }

	void Reset(int32 newStatesNum = 0) { States.Empty(newStatesNum); }

	bool Start() { return (ActiveStateIndex == INDEX_NONE) ? SetActiveStateIndex(0) : false; }

	bool Stop() { return (ActiveStateIndex != INDEX_NONE) ? SetActiveStateIndex(INDEX_NONE) : false; }

	bool ProcessInput(const FInputFrame& inputFrame);

protected:

	bool SetActiveStateIndex(int32 newStateIndex);

protected:

	UPROPERTY()
		TArray<FInputSM_State> States;

	int32 ActiveStateIndex = INDEX_NONE;
};