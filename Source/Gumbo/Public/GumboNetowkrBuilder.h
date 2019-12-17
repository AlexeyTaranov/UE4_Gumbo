// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Http.h"
#include "GumboNetowkrBuilder.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGumboCreated, const UGumboObject*, CreatedObject);

UCLASS()
class GUMBO_API UGumboNetowkrBuilder : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category = "GumboParser/Create")
	static UGumboNetowkrBuilder* ConstructGumboParserByNetwork(const FString& URL,const FString& Request = TEXT("GET"));

	virtual void Activate() override;

	UPROPERTY(BlueprintAssignable)
	FOnGumboCreated OnGumboByNetworkCreated;
private:
	FString _url;
	FString _request;

	void OnRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);
};
