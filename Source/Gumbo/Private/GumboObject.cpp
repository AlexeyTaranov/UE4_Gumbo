// Fill out your copyright notice in the Description page of Project Settings.


#include "GumboObject.h"
#include "GumboPure.h"

#pragma region GumboLibraryFunctions
static GumboNode* GetPureNodeByAttributeNameAndValue(GumboNode* node, GumboTag tag,
	const char* attributeName, const char* attributeValue, bool searchRecursive)
{
	if (node->type != GUMBO_NODE_ELEMENT) return nullptr;

	static int recursiveDepth = 2;
	//Search in this node and child node
	if (!searchRecursive) --recursiveDepth;

	GumboAttribute* attribute;
	attribute = gumbo_get_attribute(&node->v.element.attributes, attributeName);

	if (attribute != nullptr)
	{
		//Compare attribute values if setted otherwise compare only by name
		bool isEqualAttributeValue = true;
		if(strcmp("", attributeValue)) isEqualAttributeValue = !strcmp(attribute->value, attributeValue);

		bool IsEqualAttributes = (!strcmp(attribute->name, attributeName) && isEqualAttributeValue);

		if (IsEqualAttributes && node->v.element.tag == tag) return node;
	}

	GumboVector* childrens = &node->v.element.children;

	if (recursiveDepth != 0) 
	{
		//Recursive search in children nodes
		for (unsigned int i = 0; i < childrens->length; ++i) {
			node = GetPureNodeByAttributeNameAndValue(static_cast<GumboNode*>(childrens->data[i]),
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

	GumboVector* childrens = &node->v.element.children;

	if (recursiveDepth != 0)
	{
		//Recursive search in children nodes
		for (unsigned int i = 0; i < childrens->length; ++i) {
			node = GetPureNodeByTag(static_cast<GumboNode*>(childrens->data[i]), tag, searchRecursive);

			if (node != nullptr) return node;
		}
	}
	recursiveDepth = 2;
	return nullptr;
}

static GumboNode* GetNodeByTagPath(GumboNode* node,TArray<E_GumboTag> path, unsigned int count)
{
	if (node->type != GUMBO_NODE_ELEMENT) return nullptr;
	if (count >(unsigned int) path.Num()-1) return nullptr;
	if (!node->v.element.tag == (GumboTag)path[count]) return nullptr;
	GumboVector* childrens = &node->v.element.children;
	for (unsigned int i = 0; i < childrens->length; ++i) 
	{
		++count;
		node = GetNodeByTagPath(StaticCast<GumboNode*>(childrens->data[i]), path, count);
		if (node != nullptr) return node;
	}
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

FGumboNode UGumboObject::FindNodeByTag(E_GumboTag tag, const FGumboNode& startNode,
	bool searchRecursive)
{
	if(!startNode.IsValidForGumbo(this) || !IsValidGumboPure()) return FGumboNode{};
	GumboNode* firstNode = startNode.Node;
	GumboNode* target = GetPureNodeByTag(firstNode, static_cast<GumboTag>(tag), searchRecursive);
	return FGumboNode(this, target);
}

FGumboNode UGumboObject::FindNodeByAttributeValueAndName(E_GumboTag tag, const FString& name,
	const FGumboNode& startNode, const FString& value, bool searchRecursive)
{
	if (!startNode.IsValidForGumbo(this) || !IsValidGumboPure()) return FGumboNode();
	GumboNode* firstNode = startNode.Node;
	const char* name_utf8 = TCHAR_TO_UTF8(*name);
	const char* value_urf8 = TCHAR_TO_UTF8(*value);
	GumboNode* target = GetPureNodeByAttributeNameAndValue(firstNode, static_cast<GumboTag>(tag), name_utf8,
		value_urf8, searchRecursive);
	FGumboNode node(this, target);
	return node;
}

FGumboNode UGumboObject::FindNodeByTagPath(TArray<E_GumboTag> tagPath, const FGumboNode& startNode)
{
	if (!IsValidGumboPure() || !startNode.IsValidForGumbo(this)) return FGumboNode();
	GumboNode* node = startNode.Node;

	return FGumboNode();
}

FGumboNode UGumboObject::GetChildren(const FGumboNode& node, int number)
{
	if (!IsValidGumboPure() || !node.IsValidForGumbo(this)) return FGumboNode();
	if ((unsigned int)number > (node.Node->v.element.children.length)) return FGumboNode();
	GumboNode* targetNode = static_cast<GumboNode*>(node.Node->v.document.children.data[number]);
	return FGumboNode(this, targetNode);
}

TArray<FGumboNode> UGumboObject::GetChildrens(const FGumboNode& node)
{
	if (!IsValidGumboPure() || !node.IsValidForGumbo(this)) return TArray<FGumboNode>();
	TArray<FGumboNode> targetChilds;
	GumboVector* childrens = &node.Node->v.element.children;
	targetChilds.Reserve(childrens->length);
	for (unsigned int i = 0; i < childrens->length; ++i)
	{
		GumboNode* children = static_cast<GumboNode*>(childrens->data[i]);
		FGumboNode child(this, children);
		targetChilds.Emplace(child);
	}
	return targetChilds;
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

FString UGumboObject::GetTextFromNode(const FGumboNode& node)
{
	if (!IsValidGumboPure() || !node.IsValidForGumbo(this)) return FString();
	if (node.Node->type != GumboNodeType::GUMBO_NODE_ELEMENT) return FString();
	const char* text = node.Node->v.text.text;
	return FString(text);
}

FString UGumboObject::GetAttributeValue(const FGumboNode& node, const FString& attributeName)
{
	if (IsValidGumboPure() || !node.IsValidForGumbo(this)) return FString();
	const char* attrName_utf8 = TCHAR_TO_UTF8(*attributeName);
	GumboAttribute* target = gumbo_get_attribute(&node.Node->v.element.attributes, attrName_utf8);
	if (!target) return FString();
	return FString(target->value);
}

bool UGumboObject::IsValidGumboPure() const
{
	return (GumboPureObject.IsValid() || GumboPureObject->GumboObject != nullptr);
}
