// Fill out your copyright notice in the Description page of Project Settings.

#include "Graph/InputSequenceGraphSchema.h"
#include "Graph/InputSequenceGraph.h"

UInputSequenceGraph::UInputSequenceGraph(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	Schema = UInputSequenceGraphSchema::StaticClass();
}