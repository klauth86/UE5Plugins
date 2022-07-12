// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AssetTypeActions_Base.h"

class FAssetTypeActions_InputSequenceAsset : public FAssetTypeActions_Base
{
public:
	virtual FText GetName() const override { return NSLOCTEXT("FAssetTypeActions_InputSequenceAsset", "Name", "Input Sequence Asset"); }
	virtual uint32 GetCategories() override;
	virtual FColor GetTypeColor() const override { return FColor(129, 50, 255); }
	virtual UClass* GetSupportedClass() const override;
};