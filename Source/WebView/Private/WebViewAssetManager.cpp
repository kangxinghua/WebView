// Copyright aSurgingRiver, Inc. All Rights Reserved.


#include "WebViewAssetManager.h"
//#if WITH_EDITOR || PLATFORM_ANDROID || PLATFORM_IOS
//#include "WebViewTexture.h"
//#endif

UWebViewAssetManager::UWebViewAssetManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) 
	//, DefaultMaterial(FString(TEXT("/WebView/WebTexture_M.WebTexture_M"))) 
	//, DefaultTranslucentMaterial(FString(TEXT("/WebView/WebTexture_TM.WebTexture_TM")))
{
#if WITH_EDITOR || PLATFORM_ANDROID || PLATFORM_IOS
	// Add a hard reference to UWebBrowserTexture, without this the WebBrowserTexture DLL never gets loaded on Windows.
	//UWebViewTexture::StaticClass();

#endif
};

void UWebViewAssetManager::LoadDefaultMaterials()
{
	DefaultMaterial.LoadSynchronous();
	DefaultTranslucentMaterial.LoadSynchronous();
}

UMaterial* UWebViewAssetManager::GetDefaultMaterial()
{
	return DefaultMaterial.Get();
}

UMaterial* UWebViewAssetManager::GetDefaultTranslucentMaterial()
{
	return DefaultTranslucentMaterial.Get();
}
