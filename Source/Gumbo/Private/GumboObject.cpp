// Fill out your copyright notice in the Description page of Project Settings.


#include "GumboObject.h"
#include "GumboPure.h"

static GumboAttribute* GetAttributeInNode(GumboNode* node, GumboTag tag, const char* attributeName);

UGumboObject::UGumboObject()
{
	GumboPureObject = MakeShared<FGumboPure>();
}

UGumboObject* UGumboObject::Parse(FString HTML_Data)
{
	UGumboObject* obj = NewObject<UGumboObject>();
	obj->GumboPureObject->Parse(HTML_Data);
	return obj;
}

const FGumboAttribute UGumboObject::GetAttribute(E_GumboTag tag, FString name)
{
	if (GumboPureObject.IsValid()) 
	{
		if (GumboPureObject->GumboObject != nullptr) 
		{
			GumboNode* root = GumboPureObject->GumboObject->root;
			const char* name_utf8 = TCHAR_TO_UTF8(*name);

			GumboAttribute* attribute = GetAttributeInNode(root, (GumboTag)tag, name_utf8);
			const FGumboAttribute attributeStr(attribute, this);
			return attributeStr;
		}
	}
	return FGumboAttribute{};
}
						 
static GumboAttribute* GetAttributeInNode(GumboNode* node, GumboTag tag, const char* attributeName) {
	if (node->type != GUMBO_NODE_ELEMENT) {
		return nullptr;
	}
	GumboAttribute* attribute;
	attribute = gumbo_get_attribute(&node->v.element.attributes, attributeName);
	if (node->v.element.tag == tag && attribute != nullptr)
	{
		return attribute;
	}

	GumboVector* children = &node->v.element.children;
	for (unsigned int i = 0; i < children->length; ++i) {
		attribute = GetAttributeInNode(static_cast<GumboNode*>(children->data[i]), tag, attributeName);

		if (attribute != nullptr)
		{
			return attribute;
		}
	}
	return nullptr;
}

FGumboAttribute::FGumboAttribute(GumboAttribute* attribute, UGumboObject* gumboObject)
{
	if (attribute != nullptr && gumboObject != nullptr) 
	{
		Name = FString{ attribute->name };
		Value = FString{ attribute->value };
		GumboObject = gumboObject;
	}
}
