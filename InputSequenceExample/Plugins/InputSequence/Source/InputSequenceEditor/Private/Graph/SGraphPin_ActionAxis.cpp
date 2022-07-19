// Fill out your copyright notice in the Description page of Project Settings.

#include "Graph/SGraphPin_ActionAxis.h"

void SGraphPin_ActionAxis::Construct(const FArguments& InArgs, UEdGraphPin* InPin)
{
	SGraphPin::Construct(SGraphPin::FArguments().Visibility_Raw(this, &SGraphPin_ActionAxis::Visibility_Raw), InPin);
}

EVisibility SGraphPin_ActionAxis::Visibility_Raw() const { return GetPinObj()->bHidden ? EVisibility::Hidden : EVisibility::Visible; }