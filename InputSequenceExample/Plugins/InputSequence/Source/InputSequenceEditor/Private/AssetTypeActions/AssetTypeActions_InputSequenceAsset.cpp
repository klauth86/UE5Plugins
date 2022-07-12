// Fill out your copyright notice in the Description page of Project Settings.

#include "AssetTypeActions/AssetTypeActions_InputSequenceAsset.h"
#include "AssetTypeCategories.h"
#include "InputSequenceAsset.h"

uint32 FAssetTypeActions_InputSequenceAsset::GetCategories() { return EAssetTypeCategories::Misc; }

UClass* FAssetTypeActions_InputSequenceAsset::GetSupportedClass() const { return UInputSequenceAsset::StaticClass(); }