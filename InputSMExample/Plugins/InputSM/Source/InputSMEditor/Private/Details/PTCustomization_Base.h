// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "IPropertyTypeCustomization.h"
#include "DetailLayoutBuilder.h"

class FPTCustomization_Base : public IPropertyTypeCustomization
{
protected:

	template<typename T>
	T* GetPropertyAs() const
	{
		if (InternalPropertyHandle.IsValid())
		{
			TArray<void*> RawData;
			InternalPropertyHandle->AccessRawData(RawData);
			return reinterpret_cast<T*>(RawData[0]);
		}

		return nullptr;
	}

	TSharedPtr<IPropertyHandle> InternalPropertyHandle;
};