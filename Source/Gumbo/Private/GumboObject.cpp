// Fill out your copyright notice in the Description page of Project Settings.


#include "GumboObject.h"
#include "GumboNative.h"

#pragma region GumboLibraryFunctions
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

#pragma endregion

#pragma region Structs
FGumboNode::FGumboNode(UGumboObject* gumboObject,const GumboNode* node) :
	GumboObject(gumboObject), Node(node){}
FGumboNode::FGumboNode():GumboObject(nullptr) {}

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


UGumboObject::UGumboObject():GumboNativeObject(MakeShared<FGumboNative>())
{
}

UGumboObject* UGumboObject::ConstructGumboParserFromString(const FString& HTML_Data)
{
	UGumboObject* obj = NewObject<UGumboObject>();
	obj->GumboNativeObject->Parse(HTML_Data);
	return obj;
}

FGumboNode UGumboObject::GetNodeByTag(E_GumboTag tag, const FGumboNode& startNode,
                                       bool searchRecursive)
{
	if(!startNode.IsValidForGumbo(this) || !IsValidNativeGumbo()) return FGumboNode{};
	const GumboNode* firstNode = startNode.Node;
	const GumboNode* target = GetNativeNodeByTag(firstNode, static_cast<GumboTag>(tag), searchRecursive);
	return FGumboNode{ this, target };
}

FGumboNode UGumboObject::GetNodeByAttributeValueAndName(E_GumboTag tag, const FString& name,
                                                         const FGumboNode& startNode, const FString& value, bool searchRecursive)
{
	if (!startNode.IsValidForGumbo(this) || !IsValidNativeGumbo()) return FGumboNode{};
	const GumboNode* firstNode = startNode.Node;
	const char* name_utf8 = TCHAR_TO_UTF8(*name);
	const char* value_urf8 = TCHAR_TO_UTF8(*value);
	const GumboNode* target = GetNativeNodeByAttributeNameAndValue(firstNode, static_cast<GumboTag>(tag), name_utf8,
		value_urf8, searchRecursive);
	FGumboNode node{ this, target };
	return node;
}

FGumboNode UGumboObject::GetChildren(const FGumboNode& node, int number)
{
	if (!IsValidNativeGumbo() || !node.IsValidForGumbo(this)) return FGumboNode{};
	if ((unsigned int)number > (node.Node->v.element.children.length))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s hasn't childs"), *(GetFullName()));
		return FGumboNode();
	}
	GumboNode* targetNode = static_cast<GumboNode*>(node.Node->v.document.children.data[number]);
	return FGumboNode{ this, targetNode };
}

TArray<FGumboNode> UGumboObject::GetChildrens(const FGumboNode& node)
{
	if (!IsValidNativeGumbo() || !node.IsValidForGumbo(this)) return TArray<FGumboNode>();
	TArray<FGumboNode> targetChilds;
	const GumboVector* childrens = &node.Node->v.element.children;
	targetChilds.Reserve(childrens->length);
	for (unsigned int i = 0; i < childrens->length; ++i)
	{
		GumboNode* children = static_cast<GumboNode*>(childrens->data[i]);
		targetChilds.Emplace(this, children);
	}
	return targetChilds;
}

bool UGumboObject::IsHaveNode(const FGumboNode& node)
{
	return (node.GumboObject == this);
}

FGumboNode UGumboObject::GetRootNode()
{
	if (!IsValidNativeGumbo()) return FGumboNode();

	return FGumboNode{ this, GumboNativeObject->GumboObject->root};
}

FString UGumboObject::GetTextFromNode(const FGumboNode& node)
{
	if (!IsValidNativeGumbo() || !node.IsValidForGumbo(this)) return FString{};
	if (node.Node->type != GumboNodeType::GUMBO_NODE_ELEMENT) return FString{};
	const GumboVector* childs = &node.Node->v.element.children;
	if (childs->length <= 0)
	{
		UE_LOG(LogTemp,Warning,TEXT("%s on node hasn't childs"),*(GetFullName()))
		return FString{};
	}
	const GumboNode* textNode = static_cast<GumboNode*>(childs->data[0]);
	
	auto text = UTF8_TO_TCHAR(textNode->v.text.text);
	return FString{ text };
}

FString UGumboObject::GetAttributeValue(const FGumboNode& node, const FString& attributeName)
{
	if (!IsValidNativeGumbo() || !node.IsValidForGumbo(this)) return FString{};
	const char* attrName_utf8 = TCHAR_TO_UTF8(*attributeName);
	GumboAttribute* target = gumbo_get_attribute(&node.Node->v.element.attributes, attrName_utf8);
	if (!target) return FString{};
	return FString{ target->value };
}

bool UGumboObject::IsValidNativeGumbo() const
{
	return (GumboNativeObject.IsValid() || GumboNativeObject->GumboObject != nullptr);
}
