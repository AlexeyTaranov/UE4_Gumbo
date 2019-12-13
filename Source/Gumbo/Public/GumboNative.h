// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

struct GumboInternalOutput;
typedef GumboInternalOutput GumboOutput;
/**
 * 
 */
//TODO rename to NativeGumbo
class GUMBO_API FGumboNative
{
public:
	FGumboNative();
	~FGumboNative();

	void Parse(const FString& data);

	GumboOutput* GumboObject;
};
