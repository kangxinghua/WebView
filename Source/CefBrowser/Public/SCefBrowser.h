// Copyright aSurgingRiver, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Layout/Visibility.h"
#include "Input/Reply.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "Framework/SlateDelegates.h"



class UWebCoreData;

class CEFBROWSER_API SCefBrowser
	: public SCompoundWidget
{
public:
	DECLARE_DELEGATE_RetVal_OneParam(bool, FOnKeyUp, const FKeyEvent& );
	DECLARE_DELEGATE_RetVal_OneParam(bool, FOnKeyDown, const FKeyEvent& );
	DECLARE_DELEGATE_RetVal_OneParam(bool, FOnKeyChar, const FCharacterEvent& );
	DECLARE_DELEGATE_OneParam(FOnLoadState, const int);
	DECLARE_DELEGATE_RetVal_TwoParams(bool, FOnBeforePopup, FString, FString);
	DECLARE_DELEGATE_TwoParams(FOnDownloadComplete, FString, FString);

	SLATE_BEGIN_ARGS(SCefBrowser)
		: _ViewportSize(FVector2D::ZeroVector)
		, _SwitchInputMethod(false)
		, _EnableMouseTransparency(false)
		, _InitialURL(TEXT("https://www.baidu.com"))
		, _BackgroundColor(255, 255, 255, 255)
		, _ShowControls(true)
		, _ShowAddressBar(false)
		, _BrowserFrameRate(30)
	{
		_Visibility = EVisibility::SelfHitTestInvisible;
	}

		/* this party for event */
		/** Called before a popup window happens */
		SLATE_EVENT(FOnBeforePopup, OnBeforePopup)
		/** Called when document loading change. */
		SLATE_EVENT(FOnLoadState, OnLoadState)
		/** Called when the Url changes. */
		SLATE_EVENT(FOnTextChanged, OnUrlChanged)
		/** Called when file download finish. */
		SLATE_EVENT(FOnDownloadComplete, OnDownloadComplete)

		/* this party for params */
		/** Control and Editor show text style  */
		SLATE_ARGUMENT(FTextBlockStyle, TextStyle)
		/** Desired size of the web browser viewport. */
		SLATE_ARGUMENT(FVector2D, ViewportSize)
		/** Desired size of the web browser viewport. */
		SLATE_ARGUMENT(bool, SwitchInputMethod)
		/** allow mouse transcparency webpage */
		SLATE_ARGUMENT(bool, EnableMouseTransparency)
		/** URL that the browser will initially navigate to. The URL should include the protocol, eg http:// */
		SLATE_ARGUMENT(FString, InitialURL)
		/** Opaque background color used before a document is loaded and when no document color is specified. */
		SLATE_ARGUMENT(FColor, BackgroundColor)
		/** Whether to show standard controls like Back, Forward, Reload etc. */
		SLATE_ARGUMENT(bool, ShowControls)
		/** Whether to show an address bar. */
		SLATE_ARGUMENT(bool, RightKeyPopup)
		/** Whether to show an address bar. */
		SLATE_ARGUMENT(bool, ShowAddressBar)
		/** The frames per second rate that the browser will attempt to use. */
		SLATE_ARGUMENT(int, BrowserFrameRate)
		/** fixed pixel. */
		SLATE_ARGUMENT(FIntPoint, Pixel)
		/** zoom level*/
		SLATE_ARGUMENT(float, zoom)
		/** download show tip */
		SLATE_ARGUMENT(bool, downloadTip)
		//SLATE_ARGUMENT(float, zoomlevel)
	SLATE_END_ARGS()

	/**
		* Load the specified URL.
		* @param NewURL New URL to load.
		*/
	void LoadURL(FString NewURL);

	/** Get the current title of the web page. */
	FText GetTitleText() const;

	/** Stop loading the page. */
	void StopLoad();
	/** Reload the current page. */
	void Reload();
	/** Whether the document is currently being loaded. */
	bool IsLoading() const;
	/** Execute javascript on the current window */
	void ExecuteJavascript(const FString& ScriptText);
	/** Returns true if the browser can navigate backwards. */
	bool CanGoBack() const;
	/** Returns true if the browser can navigate forwards. */
	bool CanGoForward() const;
	/** Navigate backwards. */
	void GoBack();
	/**
	 * Gets the currently loaded URL.
	 * @return The URL, or empty string if no document is loaded.
	 */
	FString GetUrl() const;
	/** Navigate forwards. */
	void GoForward();
	/** Set Page Zoom level */
	void ZoomLevel(float zoomlevel);
	/** Set Page Zoom level */
	void WebPixel(FIntPoint pixel);
	/**
	 * Expose a UObject instance to the browser runtime.
	 * Properties and Functions will be accessible from JavaScript side.
	 * As all communication with the rendering procesis asynchronous, return values (both for properties and function results) are wrapped into JS Future objects.
	 * @param Name The name of the object. The object will show up as window.ue4.{Name} on the javascript side. If there is an existing object of the same name, this object will replace it. If bIsPermanent is false and there is an existing permanent binding, the permanent binding will be restored when the temporary one is removed.
	 * @param Object The object instance.
	 * @param bIsPermanent If true, the object will be visible to all pages loaded through this browser widget, otherwise, it will be deleted when navigating away from the current page. Non-permanent bindings should be registered from inside an OnLoadStarted event handler in order to be available before JS code starts loading.
	 */
	void BindUObject(const FString& Name, UObject* Object, bool bIsPermanent = true);

	/**
	 * Remove an existing script binding registered by BindUObject.
	 * @param Name The name of the object to remove.
	 * @param Object The object will only be removed if it is the same object as the one passed in.
	 * @param bIsPermanent Must match the bIsPermanent argument passed to BindUObject.
	 */
	void UnbindUObject(const FString& Name, UObject* Object, bool bIsPermanent = true);

	//virtual bool SupportsKeyboardFocus() const override { return true; }
public:
	/** Default constructor. */
	SCefBrowser();
	~SCefBrowser();
	/**
	 * Construct the widget.
	 * @param InArgs  Declaration from which to construct the widget.
	 */
	void Construct(const FArguments& InArgs);
private:
	//
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime);
	virtual void OnDragEnter(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;
	virtual void OnDragLeave(const FDragDropEvent& DragDropEvent) override;
	virtual FReply OnDragOver(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;
	virtual FReply OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)override;
private:
	TSharedPtr<UWebCoreData> _WebCoreData;
};
