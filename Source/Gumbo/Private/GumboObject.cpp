// Fill out your copyright notice in the Description page of Project Settings.


#include "GumboObject.h"
#include "GumboPure.h"

#pragma region GumboLibraryFunctions
static GumboNode* GetPureNodeByAttributeNameAndValue(GumboNode* node, GumboTag tag,
	const char* attributeName, const char* attributeValue, bool searchRecursive)
{
	if (node->type != GUMBO_NODE_ELEMENT) return nullptr;

	//Need for search only in children, not children of childs
	static int recursiveDepth = 2;
	if (!searchRecursive) --recursiveDepth;

	GumboAttribute* attribute;
	attribute = gumbo_get_attribute(&node->v.element.attributes, attributeName);

	if (attribute != nullptr)
	{
		bool IsEqualAttributes = !strcmp(attribute->name, attributeName) &&
			!strcmp(attribute->value, attributeValue);

		if (IsEqualAttributes && node->v.element.tag == tag) return node;
	}

	GumboVector* children = &node->v.element.children;

	if (!searchRecursive && recursiveDepth != 0) 
	{
		//Recursive search in children nodes
		for (unsigned int i = 0; i < children->length; ++i) {
			node = GetPureNodeByAttributeNameAndValue(static_cast<GumboNode*>(children->data[i]),
				tag, attributeName, attributeValue, searchRecursive);

			if (node != nullptr) return node;
		}
	}
	recursiveDepth = 2;
	return nullptr;
}

static GumboNode* GetPureNodeByTag(GumboNode* node, GumboTag tag, bool searchRecursive)
{
	if (node->type != GUMBO_NODE_ELEMENT) return nullptr;

	//Need for search only in children, not children of childs
	static int recursiveDepth = 2;
	if (!searchRecursive) --recursiveDepth;

	if (node->v.element.tag == tag) return node;

	GumboVector* children = &node->v.element.children;

	if (!searchRecursive && recursiveDepth != 0)
	{
		//Recursive search in children nodes
		for (unsigned int i = 0; i < children->length; ++i) {
			node = GetPureNodeByTag(static_cast<GumboNode*>(children->data[i]), tag, searchRecursive);

			if (node != nullptr) return node;
		}
	}
	recursiveDepth = 2;
	return nullptr;
}

#pragma endregion

#pragma region Structs
FGumboNode::FGumboNode(UGumboObject* gumboObject, GumboNode* node):
	GumboObject(gumboObject),Node(node){}
FGumboNode::FGumboNode() {}

bool FGumboNode::IsValidForGumbo(UGumboObject* gumboObject) const
{
	if (GumboObject == gumboObject && Node != nullptr) return true;
	else 
	{
		if (gumboObject != nullptr) {
			UE_LOG(LogTemp, Warning, TEXT("%s don't have this node"), *(gumboObject->GetFullName()));
		}
		return false;
	}
}
#pragma endregion


UGumboObject::UGumboObject()
{
	GumboPureObject = MakeShared<FGumboPure>();
}

UGumboObject* UGumboObject::Parse(const FString& HTML_Data)
{
	UGumboObject* obj = NewObject<UGumboObject>();
	obj->GumboPureObject->Parse(HTML_Data);
	return obj;
}

FGumboNode UGumboObject::GetNodeByTag(E_GumboTag tag, const FGumboNode& startNode,
	bool searchRecursive)
{
	if(!startNode.IsValidForGumbo(this) || !IsValidGumboPure()) return FGumboNode{};
	GumboNode* firstNode = startNode.Node;
	GumboNode* target = GetPureNodeByTag(firstNode, (GumboTag)tag, searchRecursive);
	return FGumboNode(this, target);
}

FGumboNode UGumboObject::GetNodeByAttributeValueAndName(E_GumboTag tag, const FString& name,
	const FString& value, const FGumboNode& startNode, bool searchRecursive)
{
	if (!startNode.IsValidForGumbo(this) || !IsValidGumboPure()) return FGumboNode();
	GumboNode* firstNode = startNode.Node;
	const char* name_utf8 = TCHAR_TO_UTF8(*name);
	const char* value_urf8 = TCHAR_TO_UTF8(*value);
	GumboNode* target = GetPureNodeByAttributeNameAndValue(firstNode, (GumboTag)tag, name_utf8,
		value_urf8, searchRecursive);
	FGumboNode node(this, target);
	return node;
}

bool UGumboObject::IsGumboObjectFromThisNode(const FGumboNode& node)
{
	return (node.GumboObject == this);
}

FGumboNode UGumboObject::GetRootNode()
{
	if (!IsValidGumboPure()) return FGumboNode();

	return FGumboNode(this, GumboPureObject->GumboObject->root);
}

FString UGumboObject::GetTextFromNode(const FGumboNode &node)
{
	if (!IsValidGumboPure() || !node.IsValidForGumbo(this)) return FString();
	if (node.Node->type != GumboNodeType::GUMBO_NODE_TEXT) return FString();
	const char* text = node.Node->v.text.text;
	return FString(text);
}

bool UGumboObject::IsValidGumboPure() const
{
	return (GumboPureObject.IsValid() || GumboPureObject->GumboObject != nullptr);
}
