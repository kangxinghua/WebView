// Copyright aSurgingRiver, Inc. All Rights Reserved.

#include "WebView.h"
#include "Modules/ModuleManager.h"
#include "WebViewAssetManager.h"
#include "CefModule.h"
#include "Materials/Material.h"

#define LOCTEXT_NAMESPACE "FWebViewModule"

class FWebViewModule : public IWebViewModule
{
public:
	virtual void StartupModule() override;

	virtual void ShutdownModule() override;
	
private:
	//UWebViewAssetManager* WebViewAssetMgr;
};
void FWebViewModule::StartupModule()
{
#if WITH_EDITOR
	FEditorDelegates::BeginPIE.AddLambda([](bool) {ICefModule::Get().OnBeginPIE(); });
	FEditorDelegates::ResumePIE.AddLambda([](bool) {ICefModule::Get().OnBeginPIE(); });
	FEditorDelegates::EndPIE.AddLambda([](bool) {ICefModule::Get().OnEndPIE(); });
	FEditorDelegates::PausePIE.AddLambda([](bool) {ICefModule::Get().OnEndPIE(); });
#endif
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	//if (WebViewAssetMgr == nullptr)
	//{
	//	WebViewAssetMgr = NewObject<UWebViewAssetManager>((UObject*)GetTransientPackage(), NAME_None, RF_Transient | RF_Public);
	//	WebViewAssetMgr->LoadDefaultMaterials();

	//	ICefModule::Get().DefaultMaterial(WebViewAssetMgr->GetDefaultMaterial());
	//	ICefModule::Get().DefaultTranslucentMaterial(WebViewAssetMgr->GetDefaultTranslucentMaterial());
	//}
}

void FWebViewModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FWebViewModule, WebView)