// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Containers/Array.h"
#include "HAL/Platform.h"

struct INPUTSM_API FInputFrame
{
public:

	FInputFrame() { Reset(); }

	void Reset() { LeftStickHor = LeftStickVer = RightStickHor = RightStickVer = 0; PackedBits = 0; }

	float LeftStickHor;
	float LeftStickVer;

	float RightStickHor;
	float RightStickVer;

	union
	{
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

struct INPUTSM_API FInputFrameStack
{
public:

	FInputFrameStack() { Frames.Empty(); ActiveFrameIndex = INDEX_NONE; }

	TArray<FInputFrame> Frames;

	int32 ActiveFrameIndex;

	bool IsOpen() const { return Frames.Num() == ActiveFrameIndex; }
};