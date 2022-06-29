// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "InputSM.generated.h"

class UEdGraph;

USTRUCT()
struct INPUTSM_API FInputFrame
{
	GENERATED_USTRUCT_BODY()

public:

	FInputFrame() { Reset(); }

	void Reset() { LeftStickHor = LeftStickVer = RightStickHor = RightStickVer = 0; PackedBits = 0; }

	UPROPERTY()
	float LeftStickHor;
	UPROPERTY()
	float LeftStickVer;
	UPROPERTY()
	float RightStickHor;
	UPROPERTY()
	float RightStickVer;

	union
	{
		UPROPERTY()
		uint16 PackedBits;
		
		struct
		{
			uint16 LeftUpperTrigger : 2;
			uint16 LeftTrigger : 2;
			uint16 RightUpperTrigger : 2;
			uint16 RightTrigger : 2;

			uint16 X : 2;
			uint16 Y : 2;
			uint16 A : 2;
			uint16 B : 2;
		};
	};
};

USTRUCT()
struct INPUTSM_API FInputFrameStack
{
	GENERATED_USTRUCT_BODY()

public:

	FInputFrameStack() { Frames.Empty(); ActiveFrameIndex = INDEX_NONE; }

	UPROPERTY()
	TArray<FInputFrame> Frames;

	int32 ActiveFrameIndex;

	bool IsOpen() const { return Frames.Num() == ActiveFrameIndex; }
};

USTRUCT()
struct INPUTSM_API FInputSMTransition
{
	GENERATED_USTRUCT_BODY()

public:

	FInputSMTransition() { TargetIndex = INDEX_NONE; }

	UPROPERTY()
		int32 TargetIndex;

	UPROPERTY()
		FInputFrameStack ActivationStack;
};

USTRUCT()
struct INPUTSM_API FInputSMNode
{
	GENERATED_USTRUCT_BODY()

public:

	FInputSMNode() {}

	UPROPERTY()
		TArray<FInputSMTransition> Transitions;

	UPROPERTY()
		UObject* NodeAsset;
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

	TArray<FInputSMNode>& GetNodes() { return Nodes; }

protected:

	UPROPERTY()
		int32 ActiveNodeIndex = INDEX_NONE;

	UPROPERTY()
		TArray<FInputSMNode> Nodes;
};