// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GumboVariables.h"
#include "gumbo-parser/include/gumbo.h"
#include "GumboObject.generated.h"

USTRUCT(BlueprintType)
struct FGumboAttribute 
{
	GENERATED_USTRUCT_BODY()

public:
	FGumboAttribute() {};
	FGumboAttribute(GumboAttribute* attribute, class UGumboObject* gumboObject);

	UPROPERTY(BlueprintReadOnly)
	FString Name;

	UPROPERTY(BlueprintReadOnly)
	FString Value;

	UPROPERTY()
	UGumboObject* GumboObject;
};

/**
 * 
 */
UCLASS(Blueprintable,BlueprintType)
class GUMBO_API UGumboObject : public UObject
{
	GENERATED_BODY()

public:
	UGumboObject();
	TSharedPtr<class FGumboPure> GumboPureObject;

	UFUNCTION(BlueprintCallable)
	static UGumboObject* Parse(FString HTML_Data);

	UFUNCTION(BlueprintCallable)
	const FGumboAttribute GetAttribute(E_GumboTag tag, FString name);
};
