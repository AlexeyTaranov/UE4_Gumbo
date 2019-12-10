// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

struct GumboInternalOutput;
typedef GumboInternalOutput GumboOutput;
/**
 * 
 */
class GUMBO_API FGumboPure
{
public:
	FGumboPure();
	~FGumboPure();

	void Parse(const FString& data);

	GumboOutput* GumboObject;
};
