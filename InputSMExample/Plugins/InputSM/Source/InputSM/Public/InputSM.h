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

	void ProcessInput(uint16 packedBits);

	bool IsOpen() const { return ActivationStack.Frames.Num() == ActiveFrameIndex + 1; }

	void ResetActiveFrame() { ActiveFrameIndex = INDEX_NONE; }

	UPROPERTY()
		int32 TargetIndex;

	UPROPERTY()
		FInputFrameStack ActivationStack;

	int32 ActiveFrameIndex;
};

USTRUCT(BlueprintType)
struct INPUTSM_API FInputSM_State
{
	GENERATED_USTRUCT_BODY()

public:

	FInputSM_State() { StateName = NAME_None; StateAsset = nullptr; Transitions.Empty(); }

	UPROPERTY(BlueprintReadOnly, Category = "Input SM State")
		FName StateName;

	UPROPERTY(BlueprintReadOnly, Category = "Input SM State")
		UObject* StateAsset;

	UPROPERTY()
		TArray<FInputSM_Transition> Transitions;
};

UCLASS(BlueprintType)
class INPUTSM_API UInputSM : public UObject
{
	GENERATED_BODY()

public:

#if WITH_EDITORONLY_DATA
	UPROPERTY()
		UEdGraph* EdGraph;
#endif

	TArray<FInputSM_State>& GetStates() { return States; }

	int32 GetActiveStateIndex() const { return ActiveStateIndex; }

	UFUNCTION(BlueprintCallable, Category = "Input SM")
	bool Start() { return (ActiveStateIndex == INDEX_NONE) ? SetActiveStateIndex(0) : false; }

	UFUNCTION(BlueprintCallable, Category = "Input SM")
	bool Stop() { return (ActiveStateIndex != INDEX_NONE) ? SetActiveStateIndex(INDEX_NONE) : false; }

	UFUNCTION(BlueprintCallable, Category = "Input SM")
	bool ProcessInput(UPARAM(ref) FInputFrame& inputFrame);

protected:

	bool SetActiveStateIndex(int32 newStateIndex);

protected:

	UPROPERTY(BlueprintReadOnly, Category = "Input SM")
		TArray<FInputSM_State> States;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Input SM")
	int32 ActiveStateIndex = INDEX_NONE;
};