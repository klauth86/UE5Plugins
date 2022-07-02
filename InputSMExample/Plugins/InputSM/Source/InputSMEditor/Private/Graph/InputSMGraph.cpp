// Fill out your copyright notice in the Description page of Project Settings.

#include "Graph/InputSMGraph.h"
#include "Graph/InputSMGraphSchema.h"
#include "Graph/InputSMGraphNode_Base.h"

UInputSMGraph::UInputSMGraph(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Schema = UInputSMGraphSchema::StaticClass();
}

void UInputSMGraph::Initialize()
{
	LockUpdates();

	////// TODO Spawn missed nodes if should

	UnlockUpdates();
}

void UInputSMGraph::UpdateAsset(int32 UpdateFlags)
{
	if (IsLocked()) return;


}