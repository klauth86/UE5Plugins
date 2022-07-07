// Fill out your copyright notice in the Description page of Project Settings.

#include "InputSM.h"

void FInputSM_Transition::ProcessInput(const FInputFrame& inputFrame)
{
	if (ActivationStack.Frames.IsValidIndex(ActiveFrameIndex + 1))
	{
		if (inputFrame.PackedBits == ActivationStack.Frames[ActiveFrameIndex + 1].PackedBits)
		{
			ActiveFrameIndex++;
		}
	}
}

bool UInputSM::ProcessInput(const FInputFrame& inputFrame)
{
	if (States.IsValidIndex(ActiveStateIndex))
	{
		FInputSM_State& activeState = States[ActiveStateIndex];
		
		if (activeState.Transitions.Num() > 0)
		{
			for (FInputSM_Transition& transition : activeState.Transitions)
			{
				transition.ProcessInput(inputFrame);

				if (transition.IsOpen())
				{
					ActiveStateIndex = transition.TargetIndex;
					transition.ResetActiveFrame();

					// Notify that state changed

					return true;
				}
			}
		}
	}

	return false;
}