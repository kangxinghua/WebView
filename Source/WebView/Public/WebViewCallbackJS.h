// Copyright aSurgingRiver, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#ifdef JSON_LIB
#include "JsonLibrary.h"
#endif
#include "WebViewCallbackJS.generated.h"

class UWebViewWidget;
class UWebViewObject;

USTRUCT(BlueprintType, Category = "Web UI")
struct WEBVIEW_API FWebViewCallbackJS
{
	friend class UWebViewWidget;
	friend class UWebViewObject;
	GENERATED_USTRUCT_BODY()
	FWebViewCallbackJS();

protected:
	FWebViewCallbackJS(TWeakObjectPtr<UWebViewWidget> Interface, const FString& Callback);

public:
	bool IsValid() const;
	
#ifdef JSON_LIB
	void Call(const FJsonLibraryValue& Data);
#endif

	void CallStr(const FString& Data);

private:
	FString CallbackName;
	TWeakObjectPtr<UWebViewWidget> widget;
};

