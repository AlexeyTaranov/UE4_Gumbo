// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "gumbo-parser/include/gumbo.h"

/**
 * 
 */
class GUMBO_API PureGumbo
{
public:
	PureGumbo();
	~PureGumbo();

	void Parse(FString data);

	GumboOutput* GumboObject;
};
