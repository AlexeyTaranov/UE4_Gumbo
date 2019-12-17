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
	FGumboNode(class UGumboObject* gumboObject,const GumboNode* node);

	UPROPERTY(BlueprintReadOnly, Category = "GumboParser")
	class UGumboObject* GumboObject;

	const GumboNode* Node;

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
	TSharedPtr<class FGumboNative> GumboNativeObject;

	UFUNCTION(BlueprintCallable, Category = "GumboParser/Create")
	static UGumboObject* ConstructGumboParserFromString(const FString& HTML_Data);

	UFUNCTION(BlueprintCallable, Category = "GumboParser/Get")
	FGumboNode GetNodeByTag(E_GumboTag tag, const FGumboNode& startNode,
		bool searchRecursive = true);

	UFUNCTION(BlueprintCallable, Category = "GumboParser/Get")
	FGumboNode GetNodeByAttributeValueAndName(E_GumboTag tag, const FString& name,
		const FGumboNode& startNode, const FString& value = "", bool searchRecursive = true);
	
	UFUNCTION(BlueprintCallable, Category = "GumboParser/Get")
	FGumboNode GetChildren(const FGumboNode& node, int number);

	UFUNCTION(BlueprintCallable, Category = "GumboParser/Get")
	TArray<FGumboNode> GetChildrens(const FGumboNode& node);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GumboParser/Check")
	bool IsHaveNode(const FGumboNode& node);

	UFUNCTION(BlueprintCallable,BlueprintPure, Category = "GumboParser/Get")
	FGumboNode GetRootNode();

	UFUNCTION(BlueprintCallable, Category = "GumboParser/Get")
	FString GetTextFromNode(const FGumboNode& node);

	UFUNCTION(BlueprintCallable, Category = "GumboParser/Get")
	FString GetAttributeValue(const FGumboNode& node, const FString& attributeName);

private:
	bool IsValidNativeGumbo() const;
};

