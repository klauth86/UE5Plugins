// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "IPropertyTypeCustomization.h"

template<typename T>
T* GetPropertyAs(TSharedPtr<IPropertyHandle> propertyHandle)
{
	if (propertyHandle.IsValid())
	{
		TArray<void*> RawData;
		propertyHandle->AccessRawData(RawData);
		return reinterpret_cast<T*>(RawData[0]);
	}

	return nullptr;
}

void ModifyFirstOuterObject(TSharedPtr<IPropertyHandle> propertyHandle)
{
	if (propertyHandle.IsValid())
	{
		TArray<UObject*> outerObjects;
		propertyHandle->GetOuterObjects(outerObjects);
		if (outerObjects.Num() > 0 && outerObjects[0]) outerObjects[0]->Modify(); // Manually set as dirty	
	}
}

class FPTC_Base : public IPropertyTypeCustomization
{
protected:

	TSharedPtr<IPropertyHandle> InternalPropertyHandle;
};

class FPTC_InputFrame : public FPTC_Base
{
public:

	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) {}

	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) {}
};