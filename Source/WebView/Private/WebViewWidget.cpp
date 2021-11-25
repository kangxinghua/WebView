// Copyright aSurgingRiver, Inc. All Rights Reserved.
#include "WebViewWidget.h"
#include "SCefCore.h"
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
	, TextStyle(FTextBlockStyle::GetDefault())
	, BackgroundColor(255,255,255,255)
{
	bIsVariable = true;
	TextStyle.Font.Size = 20;
	TextStyle.ColorAndOpacity = FSlateColor(FLinearColor(0.0f, 0.0f, 0.0f));
	windowJS = TEXT("ue");
	GConfig->GetString(TEXT("WebView"), TEXT("window"), windowJS, GGameIni);
}

void UWebViewWidget::LoadURL(FString NewURL)
{
	if ( WebCoreWidget.IsValid() )
	{
		return WebCoreWidget->LoadURL(NewURL);
	}
}

void UWebViewWidget::ExecuteJavascript(const FString& ScriptText)
{
	if (WebCoreWidget.IsValid())
	{
		return WebCoreWidget->ExecuteJavascript(ScriptText);
	}
}

void UWebViewWidget::CallJsonStr(const FString& Function, const FString& Data) 
{
	if (!WebCoreWidget.IsValid() || Function.IsEmpty() || Function == TEXT("dispatchue4event"))
		return;
	FString ScriptText;
	if (Data.Len()>=2) {
		ScriptText = FString::Printf(TEXT("%s.interface['%s'](%s)"),
			*windowJS,
			*Function,
			*Data);
	}
	else {
		ScriptText = FString::Printf(TEXT("%s.interface['%s']()"),
			*windowJS,
			*Function);
	}
	WebCoreWidget->ExecuteJavascript(ScriptText);

}

void UWebViewWidget::BindUObject(const FString& Name, UObject* Object, bool bIsPermanent) {
	if (WebCoreWidget.IsValid()){
		WebCoreWidget->BindUObject(Name, Object, bIsPermanent);
	}
}

void UWebViewWidget::UnbindUObject(const FString& Name, UObject* Object, bool bIsPermanent){
	if (WebCoreWidget.IsValid()) {
		WebCoreWidget->UnbindUObject(Name, Object, bIsPermanent);
	}
}

FString UWebViewWidget::GetUrl() const {
	if (WebCoreWidget.IsValid())	{
		return WebCoreWidget->GetUrl();
	}
	return FString();
}

void UWebViewWidget::ReleaseSlateResources(bool bReleaseChildren){
	WebCoreWidget.Reset();
	Super::ReleaseSlateResources(bReleaseChildren);
}

TSharedRef<SWidget> UWebViewWidget::RebuildWidget(){
	WebCoreWidget = SNew(SCefCore)
		.InitialURL(InitialURL)
		.ShowControls(ShowControls)
		.ShowAddressBar(ShowAddressBar)
		.BackgroundColor(BackgroundColor)
		.EnableMouseTransparency(bEnableMouseTransparency)
		.BrowserFrameRate(FrameRate)
		.RightKeyPopup(RightKeyPopup)
		.TextStyle(TextStyle)
		.ViewportSize(GetDesiredSize())
		.SwitchInputMethod(SwitchInputMethod)
		.OnLoadState_UObject(this, &UWebViewWidget::HandleOnLoadState)
		.OnUrlChanged_UObject(this, &UWebViewWidget::HandleOnUrlChanged)
		.OnBeforePopup_UObject(this, &UWebViewWidget::HandleOnBeforePopup);
	JsData = NewObject<UWebViewObject>();// 隔离JS和UE4之间的数据。
	if (JsData) {
		BindUObject("interface", JsData);
		JsData->SetWidget(this);
	}
	return WebCoreWidget.ToSharedRef();
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
	FFunctionGraphTask::CreateAndDispatchWhenReady([WeakThis, URL, Frame]() {
		if (WeakThis.IsValid()) {
			WeakThis->HandleOnBeforePopup(URL, Frame);
		}
	}, TStatId(), nullptr, ENamedThreads::GameThread);
	return true;

}

#if WITH_EDITOR
const FText UWebViewWidget::GetPaletteCategory(){
	return LOCTEXT("Experimental", "Experimental");
}
#endif

FString UWebViewWidget::WindowJS() {
	return windowJS;
}

void UWebViewWidget::CallJSFunStr(UWebViewWidget* Interface, FString CallbackName, FString jsonStr){
	if (Interface && CallbackName.Len() >= 4)	{
		Interface->CallJsonStr(CallbackName, jsonStr);
	}
}

/////////////////////////////////////////////////////



#ifdef JSON_LIB
//void UWebViewWidget::Call(const FString& Function, const FJsonLibraryValue& Data)
//{
//	if (!WebCoreWidget.IsValid() || Function.IsEmpty() || Function == TEXT("dispatchue4event"))
//		return;
//	FString ScriptText;
//	if (Data.GetType() != EJsonLibraryType::Invalid) {
//		ScriptText = FString::Printf(TEXT("%s.interface[%s](%s)"),
//			*windowJS,
//			*FJsonLibraryValue(Function).Stringify(),
//			*Data.Stringify());
//	}
//	else {
//		ScriptText = FString::Printf(TEXT("%s.interface[%s]()"),
//			*windowJS,
//			*FJsonLibraryValue(Function).Stringify());
//	}
//	WebCoreWidget->ExecuteJavascript(ScriptText);
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
