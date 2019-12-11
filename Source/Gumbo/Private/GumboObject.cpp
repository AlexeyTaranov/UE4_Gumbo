// Fill out your copyright notice in the Description page of Project Settings.


#include "GumboObject.h"
#include "GumboPure.h"

#pragma region GumboLibraryFunctions
static GumboNode* GetPureNodeByAttributeNameAndValue(GumboNode* node, GumboTag tag,
	const char* attributeName, const char* attributeValue)
{
	if (node->type != GUMBO_NODE_ELEMENT) return nullptr;

	GumboAttribute* attribute;
	attribute = gumbo_get_attribute(&node->v.element.attributes, attributeName);

	if (attribute != nullptr)
	{
		bool IsEqualAttributes = !strcmp(attribute->name, attributeName) &&
			!strcmp(attribute->value, attributeValue);

		if (IsEqualAttributes && node->v.element.tag == tag) return node;
	}

	GumboVector* children = &node->v.element.children;

	//Recursive search in children nodes
	for (unsigned int i = 0; i < children->length; ++i) {
		node = GetPureNodeByAttributeNameAndValue(static_cast<GumboNode*>(children->data[i]),
			tag, attributeName, attributeValue);

		if (node != nullptr) return node;
	}
	return nullptr;
}

static GumboNode* GetPureNodeByTag(GumboNode* node, GumboTag tag)
{
	if (node->type != GUMBO_NODE_ELEMENT) return nullptr;

	if (node->v.element.tag == tag) return node;

	GumboVector* children = &node->v.element.children;

	//Recursive search in children nodes
	for (unsigned int i = 0; i < children->length; ++i) {
		node = GetPureNodeByTag(static_cast<GumboNode*>(children->data[i]), tag);

		if (node != nullptr) return node;
	}
	return nullptr;
}

#pragma endregion

#pragma region Structs
FGumboNode::FGumboNode(UGumboObject* gumboObject, GumboNode* node):
	GumboObject(gumboObject),Node(node){}
FGumboNode::FGumboNode() {}

bool FGumboNode::IsValid(UGumboObject* gumboObject) const
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

FGumboNode UGumboObject::GetNodeByTag(E_GumboTag tag, const FGumboNode& startNode)
{
	GumboNode* firstNode;
	//Check start node from this object
	if(!startNode.IsValid(this) || !IsValid()) return FGumboNode{};
	GumboNode* target = GetPureNodeByTag(firstNode, (GumboTag)tag);
	return FGumboNode(this, target);
}

FGumboNode UGumboObject::GetNodeByAttributeValueAndName(E_GumboTag tag, const FString& name,
	const FString& value, const FGumboNode& startNode)
{
	//Check start node from this object
	if (!startNode.IsValid(this) || !IsValid()) return FGumboNode();
	GumboNode* firstNode = startNode.Node;
	const char* name_utf8 = TCHAR_TO_UTF8(*name);
	const char* value_urf8 = TCHAR_TO_UTF8(*value);
	GumboNode* target = GetPureNodeByAttributeNameAndValue(firstNode, (GumboTag)tag, name_utf8, value_urf8);
	FGumboNode node(this, target);
	return node;
}

bool UGumboObject::IsGumboObjectFromThisNode(const FGumboNode& node)
{
	return (node.GumboObject == this);
}

FGumboNode UGumboObject::GetRootNode()
{
	if (!IsValid()) return FGumboNode();

	return FGumboNode(this, GumboPureObject->GumboObject->root);
}

bool UGumboObject::IsValid()
{
	return (GumboPureObject.IsValid() || GumboPureObject->GumboObject != nullptr);
}
