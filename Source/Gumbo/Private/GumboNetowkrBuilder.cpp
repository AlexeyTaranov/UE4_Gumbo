// Fill out your copyright notice in the Description page of Project Settings.


#include "GumboNetowkrBuilder.h"
#include "GumboObject.h"

UGumboNetowkrBuilder* UGumboNetowkrBuilder::CreateGumboParserByNetwork(const FString& URL, const FString& Request)
{
	UGumboNetowkrBuilder* _netObj = NewObject<UGumboNetowkrBuilder>();
	_netObj->_url = URL;
	_netObj->_request = Request;
	return _netObj;
}

void UGumboNetowkrBuilder::Activate()
{
	TSharedRef<IHttpRequest> http = FHttpModule::Get().CreateRequest();
	http->SetURL(_url);
	http->SetVerb(_request);
	FHttpRequestCompleteDelegate& requestDelegate = http->OnProcessRequestComplete();
	requestDelegate.BindUObject(this, &UGumboNetowkrBuilder::OnRequestComplete);
	http->ProcessRequest();
}

void UGumboNetowkrBuilder::OnRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	UGumboObject* obj = UGumboObject::CreateGumboParserFromString(Response->GetContentAsString());
	OnGumboByNetworkCreated.Broadcast(obj);
}
