// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "InputFrame.generated.h"

USTRUCT(BlueprintType)
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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Frame")
    float LeftStickHor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Frame")
    float LeftStickVer;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Frame")
    float RightStickHor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Frame")
    float RightStickVer;

#if CPP
    union
    {
        struct
        {
#endif
            UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Frame")
                uint16 LeftUpperTrigger : 1;

            uint16 LeftUpperTrigger_H : 1;

            UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Frame")
                uint16 LeftTrigger : 1;

            uint16 LeftTrigger_H : 1;

            UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Frame")
                uint16 RightUpperTrigger : 1;

            uint16 RightUpperTrigger_H : 1;

            UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Frame")
                uint16 RightTrigger : 1;

            uint16 RightTrigger_H : 1;

            UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Frame")
                uint16 X : 1;

            uint16 X_H : 1;

            UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Frame")
                uint16 A : 1;

            uint16 A_H : 1;

            UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Frame")
                uint16 Y : 1;

            uint16 Y_H : 1;

            UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Frame")
                uint16 B : 1;

            uint16 B_H : 1;
#if CPP
        };
        uint16 PackedBits;
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