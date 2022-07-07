// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "InputFrame.generated.h"

USTRUCT()
struct INPUTSM_API FInputFrame
{
    GENERATED_USTRUCT_BODY()

public:

    static const uint16 PRESS_MASK = 0b01;
    static const uint16 HOLD_MASK = 0b10;
    static const uint16 FULL_MASK = 0b11;

    FInputFrame() { Reset(); }

    void Reset() { LeftStickHor = LeftStickVer = RightStickHor = RightStickVer = 0; PackedBits = 0; }

    void Reset_LeftStick() { LeftStickHor = LeftStickVer = 0; }

    void Reset_RightStick() { RightStickHor = RightStickVer = 0; }

    UPROPERTY()
    float LeftStickHor;
    UPROPERTY()
    float LeftStickVer;
    UPROPERTY()
    float RightStickHor;
    UPROPERTY()
    float RightStickVer;

#if CPP
    union
    {
#endif
        UPROPERTY()
        uint16 PackedBits;

#if CPP
        struct
        {
            uint16 LeftUpperTrigger : 2;
            uint16 LeftTrigger : 2;
            uint16 RightUpperTrigger : 2;
            uint16 RightTrigger : 2;

            uint16 X : 2;
            uint16 A : 2;
            uint16 Y : 2;
            uint16 B : 2;
        };
    };
#endif
};

USTRUCT()
struct INPUTSM_API FInputFrameStack
{
    GENERATED_USTRUCT_BODY()

public:

    FInputFrameStack() { Reset(); }

    void Reset() { Frames.Empty(); }

    UPROPERTY()
        TArray<FInputFrame> Frames;
};