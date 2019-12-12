// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GumboVariables.h"
#include "gumbo-parser/include/gumbo.h"
#include "GumboObject.generated.h"

USTRUCT(BlueprintType)
struct FGumboNode
{
	GENERATED_USTRUCT_BODY()

public:
	FGumboNode();
	FGumboNode(class UGumboObject* gumboObject, GumboNode* node);

	UPROPERTY(BlueprintReadOnly)
	class UGumboObject* GumboObject;

	//This node used in GumboObject
	GumboNode* Node;

	bool IsValidForGumbo(UGumboObject* gummboObject) const;
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
	static UGumboObject* Parse(const FString& HTML_Data);

	UFUNCTION(BlueprintCallable)
	FGumboNode FindNodeByTag(E_GumboTag tag, const FGumboNode& startNode,
		bool searchRecursive = true);

	UFUNCTION(BlueprintCallable)
	FGumboNode FindNodeByAttributeValueAndName(E_GumboTag tag, const FString& name,
		const FGumboNode& startNode, const FString& value = "",bool searchRecursive = true);

	UFUNCTION(BlueprintCallable)
	FGumboNode FindNodeByTagPath(TArray<E_GumboTag> tagPath, const FGumboNode& startNode);
	
	UFUNCTION(BlueprintCallable)
	FGumboNode GetChildren(const FGumboNode& node, int number);

	UFUNCTION(BlueprintCallable)
	TArray<FGumboNode> GetChildrens(const FGumboNode& node);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsGumboObjectFromThisNode(const FGumboNode& node);

	UFUNCTION(BlueprintCallable,BlueprintPure)
	FGumboNode GetRootNode();

	UFUNCTION(BlueprintCallable)
	FString GetTextFromNode(const FGumboNode& node);

	UFUNCTION(BlueprintCallable)
	FString GetAttributeValue(const FGumboNode& node, const FString& attributeName);

private:
	bool IsValidGumboPure() const;
};

