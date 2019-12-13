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

static const GumboNode* GetNativeNodeByTagPath(const GumboNode* node,TArray<E_GumboTag> path, unsigned int count)
{
	if (node->type != GUMBO_NODE_ELEMENT) return nullptr;
	if (count >(unsigned int) path.Num()-1) return nullptr;
	if (!node->v.element.tag == (GumboTag)path[count]) return nullptr;
	const GumboVector* childrens = &node->v.element.children;
	for (unsigned int i = 0; i < childrens->length; ++i) 
	{
		++count;
		node = GetNativeNodeByTagPath(StaticCast<GumboNode*>(childrens->data[i]), path, count);
		if (node != nullptr) return node;
	}
	return nullptr;
}

#pragma endregion

#pragma region Structs
FGumboNode::FGumboNode(UGumboObject* gumboObject,const GumboNode* node, const FString& name) :
	GumboObject(gumboObject), Node(node), Name(name) {}
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
	GumboNativeObject = MakeShared<FGumboNative>();
}

UGumboObject* UGumboObject::Parse(const FString& HTML_Data)
{
	UGumboObject* obj = NewObject<UGumboObject>();
	obj->GumboNativeObject->Parse(HTML_Data);
	return obj;
}

FGumboNode UGumboObject::FindNodeByTag(E_GumboTag tag, const FGumboNode& startNode,
	bool searchRecursive,const FString& resultNodeName)
{
	if(!startNode.IsValidForGumbo(this) || !IsValidNativeGumbo()) return FGumboNode{};
	const GumboNode* firstNode = startNode.Node;
	const GumboNode* target = GetNativeNodeByTag(firstNode, static_cast<GumboTag>(tag), searchRecursive);
	return FGumboNode{ this, target, resultNodeName };
}

FGumboNode UGumboObject::FindNodeByAttributeValueAndName(E_GumboTag tag, const FString& name,
	const FGumboNode& startNode, const FString& value, bool searchRecursive, const FString& resultNodeName)
{
	if (!startNode.IsValidForGumbo(this) || !IsValidNativeGumbo()) return FGumboNode{};
	const GumboNode* firstNode = startNode.Node;
	const char* name_utf8 = TCHAR_TO_UTF8(*name);
	const char* value_urf8 = TCHAR_TO_UTF8(*value);
	const GumboNode* target = GetNativeNodeByAttributeNameAndValue(firstNode, static_cast<GumboTag>(tag), name_utf8,
		value_urf8, searchRecursive);
	FGumboNode node{ this, target,resultNodeName };
	return node;
}

FGumboNode UGumboObject::FindNodeByTagPath(TArray<E_GumboTag> tagPath, const FGumboNode& startNode,
	const FString& resultNodeName)
{
	if (!IsValidNativeGumbo() || !startNode.IsValidForGumbo(this)) return FGumboNode{};
	const GumboNode* targetNode = GetNativeNodeByTagPath(startNode.Node, tagPath, 0);
	return FGumboNode{ this,targetNode,resultNodeName };
}

FGumboNode UGumboObject::GetChildren(const FGumboNode& node, int number, const FString& resultNodeBaseName)
{
	if (!IsValidNativeGumbo() || !node.IsValidForGumbo(this)) return FGumboNode{};
	if ((unsigned int)number > (node.Node->v.element.children.length))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s hasn't childs"), *(GetFullName()));
		return FGumboNode();
	}
	GumboNode* targetNode = static_cast<GumboNode*>(node.Node->v.document.children.data[number]);
	FString name{ resultNodeBaseName };
	name.Append(node.Name);
	return FGumboNode{ this, targetNode,name };
}

TArray<FGumboNode> UGumboObject::GetChildrens(const FGumboNode& node, const FString& resultNodeBaseName)
{
	if (!IsValidNativeGumbo() || !node.IsValidForGumbo(this)) return TArray<FGumboNode>();
	TArray<FGumboNode> targetChilds;
	const GumboVector* childrens = &node.Node->v.element.children;
	targetChilds.Reserve(childrens->length);
	for (unsigned int i = 0; i < childrens->length; ++i)
	{
		GumboNode* children = static_cast<GumboNode*>(childrens->data[i]);
		FString name{ resultNodeBaseName };
		name.Append(node.Name);
		name.AppendChar('_');
		name.AppendInt(i);
		FGumboNode child{ this, children,FString(name) };
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
	if (!IsValidNativeGumbo()) return FGumboNode();

	return FGumboNode{ this, GumboNativeObject->GumboObject->root,TEXT("Root") };
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
	const char* text = textNode->v.text.text;
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
