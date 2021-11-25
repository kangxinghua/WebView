// Copyright aSurgingRiver, Inc. All Rights Reserved.


#include "WebViewCallbackJS.h"
// Copyright 2021 Tracer Interactive, LLC. All Rights Reserved.
#include "WebViewWidget.h"

FWebViewCallbackJS::FWebViewCallbackJS()
{
	//
}

FWebViewCallbackJS::FWebViewCallbackJS(TWeakObjectPtr<UWebViewWidget> Widget, const FString& Callback)
	: CallbackName(Callback)
	, widget(Widget)
{
}

bool FWebViewCallbackJS::IsValid() const
{
	if (widget.IsValid())
		return !CallbackName.IsEmpty();

	return false;
}
#ifdef JSON_LIB
void FWebViewCallbackJS::Call(const FJsonLibraryValue& Data)
{
	if (!widget.IsValid() || CallbackName.IsEmpty())
		return;

	if (Data.GetType() != EJsonLibraryType::Invalid)
		widget->ExecuteJavascript(FString::Printf(TEXT("%s.interface[%s](%s)"),
			*widget->WindowJS(),
			*FJsonLibraryValue(CallbackName).Stringify(),
			*Data.Stringify()));
	else
		widget->ExecuteJavascript(FString::Printf(TEXT("%s.interface[%s]()"),
			*widget->WindowJS(),
			*FJsonLibraryValue(CallbackName).Stringify()));
}
#endif
void FWebViewCallbackJS::CallStr(const FString& Data)
{

}
