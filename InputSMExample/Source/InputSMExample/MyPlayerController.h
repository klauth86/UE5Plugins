// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "InputFrame.h"
#include "MyPlayerController.generated.h"

class UInputSM;

UCLASS()
class INPUTSMEXAMPLE_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;

protected:

	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

	void OnQ();
	void OnW();
	void OnE();
	void OnA();
	void OnS();
	void OnD();
	void OnZ();
	void OnX();
	void OnC();

protected:

	UPROPERTY(EditDefaultsOnly)
		UInputSM* InputSM;

	FInputFrame InputFrame;
};