// Fill out your copyright notice in the Description page of Project Settings.


#include "GumboObject.h"
#include "GumboPure.h"

#pragma region GumboLibraryFunctions
static GumboNode* GetNodeByAttributeNameAndValue(GumboNode* node, GumboTag tag,
	const char* attributeName, const char* attributeValue)
{
	if (node->type != GUMBO_NODE_ELEMENT) {
		return nullptr;
	}
	GumboAttribute* attribute;
	attribute = gumbo_get_attribute(&node->v.element.attributes, attributeName);
	if (attribute != nullptr)
	{
		bool IsEqualAttributes = !strcmp(attribute->name, attributeName) &&
			!strcmp(attribute->value, attributeValue);
		if (IsEqualAttributes && node->v.element.tag == tag)
		{
			return node;
		}
	}

	GumboVector* children = &node->v.element.children;

	//Recursive search in children nodes
	for (unsigned int i = 0; i < children->length; ++i) {
		node = GetNodeByAttributeNameAndValue(static_cast<GumboNode*>(children->data[i]), tag, attributeName, attributeValue);

		if (node != nullptr)
		{
			return node;
		}
	}
	return nullptr;
}
#pragma endregion

#pragma region Structs
FGumboNode::FGumboNode(UGumboObject* gumboObject, GumboNode* node):
	GumboObject(gumboObject),Node(node){}
FGumboNode::FGumboNode() {}
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

FGumboNode UGumboObject::GetNodeInNodeByAttributeValue(E_GumboTag tag, const FString& name,
	const FString& value, const FGumboNode& startNode)
{
	GumboNode* firstNode;
	//Check start node from this object
	if (startNode.GumboObject == this && startNode.Node != nullptr)
	{
		firstNode = startNode.Node;
	}	
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s don't have this node"), *(GetFullName()));
		return FGumboNode{};
	}
	if (!GumboPureObject.IsValid() && GumboPureObject->GumboObject == nullptr) return FGumboNode{};

	const char* name_utf8 = TCHAR_TO_UTF8(*name);
	const char* value_urf8 = TCHAR_TO_UTF8(*value);
	GumboNode* target = GetNodeByAttributeNameAndValue(firstNode, (GumboTag)tag, name_utf8, value_urf8);
	FGumboNode node(this, target);
	return node;
}

UFUNCTION(BlueprintCallable)
FGumboNode UGumboObject::GetNodeInRootByAttributeValue(E_GumboTag tag, const FString& name,
	const FString& value) 
{
	if (!GumboPureObject.IsValid() && GumboPureObject->GumboObject == nullptr) return FGumboNode{};

	GumboNode* root = GumboPureObject->GumboObject->root;
	const char* name_utf8 = TCHAR_TO_UTF8(*name);
	const char* value_urf8 = TCHAR_TO_UTF8(*value);
	GumboNode* target = GetNodeByAttributeNameAndValue(root, (GumboTag)tag, name_utf8, value_urf8);
	FGumboNode node(this, target);
	return node;
}

bool UGumboObject::IsGumboObjectFromThisNode(const FGumboNode& node)
{
	return (node.GumboObject == this);
}
