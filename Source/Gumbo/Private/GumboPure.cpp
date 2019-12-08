// Fill out your copyright notice in the Description page of Project Settings.


#include "GumboPure.h"
#include "gumbo-parser/include/gumbo.h"

FGumboPure::FGumboPure()
{
}

FGumboPure::~FGumboPure()
{
	if (GumboObject != nullptr) {
		gumbo_destroy_output(&kGumboDefaultOptions, GumboObject);
	}
}

void FGumboPure::Parse(FString&& data)
{
	const char* data_utf8 = TCHAR_TO_UTF8(*data);
	GumboObject = gumbo_parse(data_utf8);
}