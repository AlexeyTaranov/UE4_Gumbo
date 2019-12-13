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
	FGumboNode(class UGumboObject* gumboObject,const GumboNode* node,const FString& name);

	UPROPERTY(BlueprintReadWrite)
	FString Name;

	UPROPERTY(BlueprintReadOnly)
	class UGumboObject* GumboObject;

	//This node used in GumboObject
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

	UFUNCTION(BlueprintCallable)
	static UGumboObject* Parse(const FString& HTML_Data);

	UFUNCTION(BlueprintCallable)
	FGumboNode FindNodeByTag(E_GumboTag tag, const FGumboNode& startNode,
		bool searchRecursive = true,const FString& resultNodeName = "node");

	UFUNCTION(BlueprintCallable)
	FGumboNode FindNodeByAttributeValueAndName(E_GumboTag tag, const FString& name,
		const FGumboNode& startNode, const FString& value = "",
		bool searchRecursive = true,const FString& resultNodeName = "node");

	UFUNCTION(BlueprintCallable)
	FGumboNode FindNodeByTagPath(TArray<E_GumboTag> tagPath, const FGumboNode& startNode,
		const FString& resultNodeName = "node");
	
	UFUNCTION(BlueprintCallable)
	FGumboNode GetChildren(const FGumboNode& node, int number,
		const FString& resultNodeBaseName = "child_of_");

	UFUNCTION(BlueprintCallable)
	TArray<FGumboNode> GetChildrens(const FGumboNode& node,
		const FString& resultNodeBaseName = "child_of_");

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsGumboObjectFromThisNode(const FGumboNode& node);

	UFUNCTION(BlueprintCallable,BlueprintPure)
	FGumboNode GetRootNode();

	UFUNCTION(BlueprintCallable)
	FString GetTextFromNode(const FGumboNode& node);

	UFUNCTION(BlueprintCallable)
	FString GetAttributeValue(const FGumboNode& node, const FString& attributeName);

private:
	bool IsValidNativeGumbo() const;
};

