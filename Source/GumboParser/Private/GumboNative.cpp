// Fill out your copyright notice in the Description page of Project Settings.


#include "GumboNative.h"
#include "CoreMinimal.h"

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

static const GumboNode* GetNativeNodeByAttributeNameAndValue(const GumboNode* node, GumboTag tag,
    const char* attributeName, const char* attributeValue, bool searchRecursive)
{
	if (node->type != GUMBO_NODE_ELEMENT) return nullptr;

	static int recursiveDepth = 2;
	//Search in this node and child node
	if (!searchRecursive) --recursiveDepth;

	const GumboAttribute* attribute;
	attribute = gumbo_get_attribute(&node->v.element.attributes, attributeName);

	if (attribute != nullptr)
	{
		//Compare attribute values if setted otherwise compare only by name
		bool isEqualAttributeValue = true;
		if(strcmp("", attributeValue)) isEqualAttributeValue = !strcmp(attribute->value, attributeValue);

		bool IsEqualAttributes = (!strcmp(attribute->name, attributeName) && isEqualAttributeValue);

		if (IsEqualAttributes && node->v.element.tag == tag) return node;
	}

	const GumboVector* childrens = &node->v.element.children;

	if (recursiveDepth != 0) 
	{
		//Recursive search in children nodes
		for (unsigned int i = 0; i < childrens->length; ++i) {
			node = GetNativeNodeByAttributeNameAndValue(static_cast<GumboNode*>(childrens->data[i]),
                tag, attributeName, attributeValue, searchRecursive);

			if (node != nullptr) return node;
		}
	}
	recursiveDepth = 2;
	return nullptr;
}

static const GumboNode* GetNativeNodeByTag(const GumboNode* node, GumboTag tag, bool searchRecursive)
{
	if (node->type != GUMBO_NODE_ELEMENT) return nullptr;

	//Need for search only in children, not children of childs
	static int recursiveDepth = 2;
	if (!searchRecursive) --recursiveDepth;

	if (node->v.element.tag == tag) return node;

	const GumboVector* childrens = &node->v.element.children;

	if (recursiveDepth != 0)
	{
		//Recursive search in children nodes
		for (unsigned int i = 0; i < childrens->length; ++i) {
			node = GetNativeNodeByTag(static_cast<GumboNode*>(childrens->data[i]), tag, searchRecursive);
			if (node != nullptr) return node;
		}
	}
	recursiveDepth = 2;
	return nullptr;
}

