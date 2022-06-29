// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EdGraph/EdGraph.h"
#include "InputSMGraph.generated.h"

UCLASS()
class UInputSMGraph : public UEdGraph
{
	GENERATED_UCLASS_BODY()

public:

	bool IsLocked() const { return bLockUpdates; }
	void LockUpdates() { bLockUpdates = 1; }
	void UnlockUpdates() { bLockUpdates = 0; }

	void Initialize();

	void UpdateAsset(int32 UpdateFlags = 0);

protected:

	uint32 bLockUpdates : 1;

	UPROPERTY()
		int32 GraphVersion;
};