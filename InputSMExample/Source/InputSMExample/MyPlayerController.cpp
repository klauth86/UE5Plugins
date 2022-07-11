// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayerController.h"
#include "InputSM.h"

void AMyPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (InputSM) InputSM->Stop();

	Super::EndPlay(EndPlayReason);
}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (InputSM) InputSM->Start();
}

void AMyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("Q", IE_Pressed, this, &AMyPlayerController::OnQ);
	InputComponent->BindAction("W", IE_Released, this, &AMyPlayerController::OnW);
	InputComponent->BindAction("E", IE_Pressed, this, &AMyPlayerController::OnE);
	InputComponent->BindAction("A", IE_Pressed, this, &AMyPlayerController::OnA);
	InputComponent->BindAction("D", IE_Pressed, this, &AMyPlayerController::OnD);
	InputComponent->BindAction("Z", IE_Pressed, this, &AMyPlayerController::OnZ);
	InputComponent->BindAction("X", IE_Pressed, this, &AMyPlayerController::OnX);
	InputComponent->BindAction("C", IE_Pressed, this, &AMyPlayerController::OnC);
}

void AMyPlayerController::OnQ() { InputFrame.LeftUpperTrigger = 1; }
void AMyPlayerController::OnW() { InputFrame.B = 1; }
void AMyPlayerController::OnE() { InputFrame.RightUpperTrigger; }
void AMyPlayerController::OnA() { InputFrame.A = 1; }
void AMyPlayerController::OnD() { InputFrame.Y = 1; }
void AMyPlayerController::OnZ() { InputFrame.LeftTrigger = 1; }
void AMyPlayerController::OnX() { InputFrame.X = 1; }
void AMyPlayerController::OnC() { InputFrame.RightTrigger = 1; }

void AMyPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	Super::PostProcessInput(DeltaTime, bGamePaused);

	if (InputSM && InputSM->ProcessInput(InputFrame))
	{
		int32 activeStateIndex = InputSM->GetActiveStateIndex();

		if (InputSM->GetStates().IsValidIndex(activeStateIndex))
		{
			FInputSM_State& activeState = InputSM->GetStates()[activeStateIndex];
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, *(activeState.StateName.ToString()));
		}
	}
}