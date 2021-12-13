// Copyright aSurgingRiver, Inc. All Rights Reserved.
#include "WebViewWidget.h"
#include "SCefBrowser.h"
#include "Async/Async.h"
#include "WebViewObject.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Async/TaskGraphInterfaces.h"
#include "UObject/ConstructorHelpers.h"
#include "Misc/ConfigCacheIni.h"

#if WITH_EDITOR
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialExpressionMaterialFunctionCall.h"
#include "Materials/MaterialExpressionTextureSample.h"
#include "Materials/MaterialExpressionTextureSampleParameter2D.h"
#include "Materials/MaterialFunction.h"
#include "Factories/MaterialFactoryNew.h"
#include "AssetRegistryModule.h"
#include "PackageHelperFunctions.h"
#endif

#define LOCTEXT_NAMESPACE "WebViewWidget"

/////////////////////////////////////////////////////
// UWebViewWidget

UWebViewWidget::UWebViewWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, styleText(FTextBlockStyle::GetDefault())
	, ColorBackground(255, 255, 255, 255)
	, jsWindow(TEXT("ue"))
{
	GConfig->GetString(TEXT("WebView"), TEXT("window"), jsWindow, GGameIni);
	styleText.ColorAndOpacity = FSlateColor(FLinearColor(0.0f, 0.0f, 0.0f));
	styleText.Font.Size = 20;
	bIsVariable = true;
}

void UWebViewWidget::LoadURL(FString NewURL)
{
	if (!CefCoreWidget.IsValid())return;
	return CefCoreWidget->LoadURL(NewURL);
}

void UWebViewWidget::ExecuteJavascript(const FString& ScriptText)
{
	if (!CefCoreWidget.IsValid())return;
	return CefCoreWidget->ExecuteJavascript(ScriptText);
}

void UWebViewWidget::CallJsonStr(const FString& Function, const FString& Data) 
{
	if (!CefCoreWidget.IsValid() || Function.IsEmpty() || Function == TEXT("synccallue"))
		return;
	FString TextScript;
	if (Data.Len()>=2) {
		TextScript = FString::Printf(TEXT("%s.interface['%s'](%s)"),
			*jsWindow, *Function, *Data);
	}
	else {
		TextScript = FString::Printf(TEXT("%s.interface['%s']()"),
			*jsWindow,*Function);
	}
	CefCoreWidget->ExecuteJavascript(TextScript);

}

FString UWebViewWidget::GetUrl() const {
	if (!CefCoreWidget.IsValid())return FString();
	return CefCoreWidget->GetUrl();
}

void UWebViewWidget::BindUObject(const FString& VarName, UObject* Object, bool bIsPermanent) {
	if (!CefCoreWidget.IsValid())return;
	CefCoreWidget->BindUObject(VarName, Object, bIsPermanent);
}

void UWebViewWidget::UnbindUObject(const FString& Name, UObject* Object, bool bIsPermanent){
	if (!CefCoreWidget.IsValid())return; 
	CefCoreWidget->UnbindUObject(Name, Object, bIsPermanent);
}

void UWebViewWidget::ReleaseSlateResources(bool bReleaseChildren){
	CefCoreWidget.Reset();
	Super::ReleaseSlateResources(bReleaseChildren);
}

TSharedRef<SWidget> UWebViewWidget::RebuildWidget(){
	CefCoreWidget = SNew(SCefBrowser)
		.ShowAddressBar(addressShow)
		.InitialURL(urlInitial)
		.BackgroundColor(ColorBackground)
		.ShowControls(controlShow)
		.RightKeyPopup(RightKeyPopup)
		.BrowserFrameRate(RateFrame)
		.TextStyle(styleText)
		.EnableMouseTransparency(bEnableTransparency)
		.SwitchInputMethod(SwitchInputMethod)
		.ViewportSize(GetDesiredSize())
		.OnUrlChanged_UObject(this, &UWebViewWidget::HandleOnUrlChanged)
		.OnBeforePopup_UObject(this, &UWebViewWidget::HandleOnBeforePopup)
		.OnLoadState_UObject(this, &UWebViewWidget::HandleOnLoadState);
	_ViewObject = NewObject<UWebViewObject>();// 隔离JS和UE4之间的数据。
	if (_ViewObject) {
		BindUObject("interface", _ViewObject);
		_ViewObject->SetUMG(this);
	}
	return CefCoreWidget.ToSharedRef();
}

inline void UWebViewWidget::CallBrowser(TFunction<void(TSharedPtr<class SCefBrowser>&)>& fun) {
	if (!CefCoreWidget.IsValid())return;
	fun(CefCoreWidget);
}

void UWebViewWidget::HandleOnUrlChanged(const FText& InText) {
	OnUrlChanged.Broadcast(InText);
}

void UWebViewWidget::HandleOnLoadState(const int state) {
	OnLoadState.Broadcast(state);
}

bool UWebViewWidget::HandleOnBeforePopup(FString URL, FString Frame) {
	if (!OnBeforePopup.IsBound()) return false;
	if (IsInGameThread()) {
		OnBeforePopup.Broadcast(URL, Frame);
		return true;
	}
	// Retry on the GameThread.
	TWeakObjectPtr<UWebViewWidget> WeakThis = this;
	AsyncTask(ENamedThreads::GameThread, [WeakThis, URL, Frame]() {
		if (!WeakThis.IsValid()) return;
		WeakThis->HandleOnBeforePopup(URL, Frame);
		});
	return true;

}

#if WITH_EDITOR
const FText UWebViewWidget::GetPaletteCategory(){
	return LOCTEXT("Experimental", "Experimental");
}
#endif

//void UWebViewWidget::CallJSFunStr(UWebViewWidget* Interface, FString CallbackName, FString jsonStr){
//	if (Interface && CallbackName.Len() >= 4)	{
//		Interface->CallJsonStr(CallbackName, jsonStr);
//	}
//}

FString UWebViewWidget::JSWindow() {
	return jsWindow;
}

/////////////////////////////////////////////////////



#ifdef JSON_LIB
//void UWebViewWidget::Call(const FString& Function, const FJsonLibraryValue& Data)
//{
//	if (!CefCoreWidget.IsValid() || Function.IsEmpty() || Function == TEXT("dispatchue4event"))
//		return;
//	FString ScriptText;
//	if (Data.GetType() != EJsonLibraryType::Invalid) {
//		ScriptText = FString::Printf(TEXT("%s.interface[%s](%s)"),
//			*jsWindow,
//			*FJsonLibraryValue(Function).Stringify(),
//			*Data.Stringify());
//	}
//	else {
//		ScriptText = FString::Printf(TEXT("%s.interface[%s]()"),
//			*jsWindow,
//			*FJsonLibraryValue(Function).Stringify());
//	}
//	CefCoreWidget->ExecuteJavascript(ScriptText);
//}
//void UWebViewWidget::CallJSFun(UWebViewWidget* Interface, FString CallbackName, const FJsonLibraryValue& Data)
//{
//	if (Interface && CallbackName.Len() >= 4)
//	{
//		Interface->Call(CallbackName, Data);
//	}
//}
#endif
#undef LOCTEXT_NAMESPACE
