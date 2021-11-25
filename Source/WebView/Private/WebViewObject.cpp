// Copyright aSurgingRiver, Inc. All Rights Reserved.

#include "WebViewObject.h"
#include "WebViewWidget.h"
#include "WebViewCallbackJS.h"


void UWebViewObject::dispatchUE4Event(const FString& Name, const FString& Data, const FString& Callback)
{
	if (!Widget.IsValid())
		return;
#ifdef JSON_LIB
	if (Widget->OnJsEvent.IsBound()){
		Widget->OnJsEvent.Broadcast(Name, FJsonLibraryValue::Parse(Data), Callback);
	}
#endif
	if (Widget->OnJsEventStr.IsBound())  {
		Widget->OnJsEventStr.Broadcast(Name, Data, Callback);
	}

}


void UWebViewObject::SetWidget(UWebViewWidget* InWidget){
	Widget = InWidget;// StaticCast<UWebViewWidget*>(InWidget);
}