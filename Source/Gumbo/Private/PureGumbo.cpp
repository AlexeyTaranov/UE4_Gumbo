// Fill out your copyright notice in the Description page of Project Settings.


#include "PureGumbo.h"

PureGumbo::PureGumbo()
{
}

PureGumbo::~PureGumbo()
{
	gumbo_destroy_output(&kGumboDefaultOptions, GumboObject);
}

void PureGumbo::Parse(FString data)
{
	const char* data_utf8 = TCHAR_TO_UTF8(*data);
	GumboObject = gumbo_parse(data_utf8);
}
