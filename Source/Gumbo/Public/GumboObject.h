// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GumboVariables.h"
#include "GumboObject.generated.h"

/**
 * 
 */
UCLASS(Blueprintable,BlueprintType)
class GUMBO_API UGumboObject : public UObject
{
	GENERATED_BODY()

public:
	TSharedPtr<class FGumboPure> GumboPureObject;

	void Parse(FString&& HTML_Data);

	UFUNCTION(BlueprintCallable)
	FString GetAttributeValue(E_GumboTag tag, FString name);
};
