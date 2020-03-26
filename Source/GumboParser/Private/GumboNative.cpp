// Fill out your copyright notice in the Description page of Project Settings.


#include "GumboNative.h"
#include "gumbo-parser/include/gumbo.h"

FGumboNative::FGumboNative(): GumboObject(nullptr)
{
}

FGumboNative::~FGumboNative()
{
	if (GumboObject != nullptr) {
		gumbo_destroy_output(&kGumboDefaultOptions, GumboObject);
	}
}

void FGumboNative::Parse(const FString& data)
{
	if (GumboObject != nullptr)
	{
		gumbo_destroy_output(&kGumboDefaultOptions, GumboObject);
	}
	const char* data_utf8 = TCHAR_TO_UTF8(*data);
	GumboObject = gumbo_parse(data_utf8);
}

