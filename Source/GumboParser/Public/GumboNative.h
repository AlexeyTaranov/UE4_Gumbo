// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "gumbo-parser/include/gumbo.h"

struct GumboInternalOutput;
typedef GumboInternalOutput GumboOutput;
/**
 * 
 */
class GUMBOPARSER_API FGumboNative
{
public:
	FGumboNative();
	~FGumboNative();

	void Parse(const class FString& data);

	GumboOutput* GumboObject;
};

static const GumboNode* GetNativeNodeByAttributeNameAndValue(const GumboNode* node, GumboTag tag,
    const char* attributeName, const char* attributeValue, bool searchRecursive);

static const GumboNode* GetNativeNodeByTag(const GumboNode* node, GumboTag tag, bool searchRecursive);

