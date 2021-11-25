// Copyright aSurgingRiver, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "WebViewCallbackJS.h"
#include "Styling/SlateTypes.h"
#ifdef JSON_LIB
#include "JsonLibrary.h"
#endif

#include "WebViewWidget.generated.h"

class UWebViewObject;



/**
 * 
 */
UCLASS()
class WEBVIEW_API UWebViewWidget : public UWidget
{
	GENERATED_UCLASS_BODY()
	
public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUrlChanged, const FText&, Text);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoadState,int,state);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBeforePopup, FString, URL, FString, Frame);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnJsStrEvent, const FString&, Key, FString, JsonStr, const FString&, Callback);
	/**
	 * Load the specified URL
	 *
	 * @param NewURL New URL to load
	 */
	UFUNCTION(BlueprintCallable, Category="Web View")
	void LoadURL(FString NewURL);

	/**
	* Executes a JavaScript string in the context of the web page
	*
	* @param ScriptText JavaScript string to execute
	*/
	UFUNCTION(BlueprintCallable, Category = "Web View")
	void ExecuteJavascript(const FString& ScriptText);

	UFUNCTION(BlueprintCallable, Category = "Web View", meta = (AdvancedDisplay = "Data", AutoCreateRefTerm = "Data"))
		void CallJsonStr(const FString& Function, const FString& Data);

	/**
	 * Expose a UObject instance to the browser runtime.
	 * Properties and Functions will be accessible from JavaScript side.
	 * As all communication with the rendering procesis asynchronous, return values (both for properties and function results) are wrapped into JS Future objects.
	 *
	 * @param Name The name of the object. The object will show up as window.ue4.{Name} on the javascript side. If there is an existing object of the same name, this object will replace it. If bIsPermanent is false and there is an existing permanent binding, the permanent binding will be restored when the temporary one is removed.
	 * @param Object The object instance.
	 * @param bIsPermanent If true, the object will be visible to all pages loaded through this browser widget, otherwise, it will be deleted when navigating away from the current page. Non-permanent bindings should be registered from inside an OnLoadStarted event handler in order to be available before JS code starts loading.
	 */
	UFUNCTION(BlueprintCallable, Category = "Web View", meta = (AdvancedDisplay = "Name|Object"))
	void BindUObject(const FString& Name, UObject* Object, bool bIsPermanent = true);

	/**
	 * Remove an existing script binding registered by BindUObject.
	 *
	 * @param Name The name of the object to remove.
	 * @param Object The object will only be removed if it is the same object as the one passed in.
	 * @param bIsPermanent Must match the bIsPermanent argument passed to BindUObject.
	 */
	UFUNCTION(BlueprintCallable, Category = "Web View", meta = (AdvancedDisplay = "Name|Object"))
	void UnbindUObject(const FString& Name, UObject* Object, bool bIsPermanent = true);


	/**
	* Gets the currently loaded URL.
	*
	* @return The URL, or empty string if no document is loaded.
	*/
	UFUNCTION(BlueprintCallable, Category = "Web View")
	FString GetUrl() const;

	/** Called when the Url changes. */
	UPROPERTY(BlueprintAssignable, Category = "Web View|Event")
	FOnUrlChanged OnUrlChanged;

	/** Called when a popup is about to spawn. */
	UPROPERTY(BlueprintAssignable, Category = "Web View|Event")
	FOnBeforePopup OnBeforePopup;

	/** Called when a popup is about to spawn. */
	UPROPERTY(BlueprintAssignable, Category = "Web View|Event")
	FOnLoadState OnLoadState;


	UPROPERTY(EditAnywhere, meta = (DisplayName = "Enable Transparency"), Category = "Web View|Mouse")
	bool bEnableMouseTransparency=false;

	//UPROPERTY(EditAnywhere, meta = (DisplayName = "Transparency Threshold", UIMin = 0, UIMax = 1), Category = "Web View|Mouse")
	//float MouseTransparencyThreshold=0.333f;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "Text Style", UIMin = 0, UIMax = 1), Category = "Web View|Show Head")
	FTextBlockStyle  TextStyle;
	/** Whether to show standard controls like Back, Forward, Reload etc. */
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Show Controls", UIMin = 0, UIMax = 1), Category = "Web View|Show Head")
	bool  ShowControls=false;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "Show Popup Menu", UIMin = 0, UIMax = 1), Category = "Web View|Show Head")
	bool RightKeyPopup;

	/** Whether to show an address bar. */
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Show Address Bar", UIMin = 0, UIMax = 1), Category = "Web View|Show Head")
	bool  ShowAddressBar = false;

	/** Whether to show an address bar. */
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Switch Input Method", UIMin = 0, UIMax = 1), Category = "Web View|Show Head")
	bool  SwitchInputMethod = false;

	// Called with ue.interface.broadcast(name, data) in the browser context.
	UPROPERTY(BlueprintAssignable, Category = "Web View|Events")
	FOnJsStrEvent OnJsEventStr;

public:

	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif

	FString WindowJS();

protected:
	/** URL that the browser will initially navigate to. The URL should include the protocol, eg http:// */
	UPROPERTY(EditAnywhere, Category= "Web View")
	FString InitialURL;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "Background Color"), Category = "Web View")
	FColor  BackgroundColor;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "Frame Rate", ClampMin=30, ClampMax=60), Category = "Web View")
	int  FrameRate=30;


protected:
	TSharedPtr<class SCefCore> WebCoreWidget;

	FString windowJS;
protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	void HandleOnUrlChanged(const FText& Text);
	void HandleOnLoadState(const int state);
	bool HandleOnBeforePopup(FString URL, FString Frame);
	UPROPERTY()
	UWebViewObject*  JsData;// 保存UE4与Js的通信数据


public:

	UFUNCTION(BlueprintCallable, Category = "Web View")
	static void CallJSFunStr(UWebViewWidget* Interface, FString CallbackName, FString DataStr);

public:
	//DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnJsEvent, const FString&, Key, FJsonLibraryValue, Json, const FString&, Callback);
	//UFUNCTION(BlueprintCallable, Category = "Web View")
	//static void CallJSFun(UWebViewWidget* Interface, FString CallbackName, const FJsonLibraryValue& Data);
	//UPROPERTY(BlueprintAssignable, Category = "Web View | Events") 
	//FOnJsEvent OnJsEvent; 
	//UFUNCTION(BlueprintCallable, Category = "Web View", meta = (AdvancedDisplay = "Data", AutoCreateRefTerm = "Data"))
	//void Call(const FString& Function, const FJsonLibraryValue& Data); 
};
