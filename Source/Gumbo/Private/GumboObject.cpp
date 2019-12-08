// Fill out your copyright notice in the Description page of Project Settings.


#include "GumboObject.h"
#include "GumboPure.h"
#include "gumbo-parser/include/gumbo.h"

static const char* GetAttributeValueInNode(GumboNode* node, GumboTag tag, const char* attributeName);

void UGumboObject::Parse(FString&& HTML_Data)
{
	if (!GumboPureObject.IsValid()) 
	{
		GumboPureObject = MakeShared<FGumboPure>();
	}
	GumboPureObject->Parse(MoveTemp(HTML_Data));
}

FString UGumboObject::GetAttributeValue(E_GumboTag tag, FString name)
{
	if (GumboPureObject.IsValid()) 
	{
		if (GumboPureObject->GumboObject) 
		{
			GumboNode* root = GumboPureObject->GumboObject->root;
			const char* name_utf8 = TCHAR_TO_UTF8(*name);

			const char* value = GetAttributeValueInNode(root, (GumboTag)tag, name_utf8);
			return FString(value);
		}
	}
	return FString("NONE");
}
#pragma optimize("", off)
static const char* GetAttributeValueInNode(GumboNode* node, GumboTag tag, const char* attributeName) {
	if (node->type != GUMBO_NODE_ELEMENT) {
		return nullptr;
	}
	GumboAttribute* attribute;
	attribute = gumbo_get_attribute(&node->v.element.attributes, attributeName);
	if (node->v.element.tag == tag && attribute != nullptr)
	{
		return attribute->value;
	}

	GumboVector* children = &node->v.element.children;
	for (unsigned int i = 0; i < children->length; ++i) {
		const char* value = GetAttributeValueInNode(static_cast<GumboNode*>(children->data[i]), tag, attributeName);

		if (value != nullptr) 
		{
			return value;
		}

	}
	return nullptr;
}

#pragma optimize("", on)