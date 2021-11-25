// Copyright aSurgingRiver, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "Materials/MaterialInterface.h"
/**
 * WebCoreModule interface
 */
class ICefModule : public IModuleInterface
{
public:
	/**
	 * Get or load the Web Browser Module
	 * 
	 * @return The loaded module
	 */
	static inline ICefModule& Get() {
		return FModuleManager::LoadModuleChecked< ICefModule >("CefCore");
	}
	
	/**
	 * Check whether the module has already been loaded
	 * 
	 * @return True if the module is loaded
	 */
	static inline bool IsAvailable(){
		return FModuleManager::Get().IsModuleLoaded("CefCore");
	}

	virtual void OnBeginPIE() = 0;
	virtual void OnEndPIE() = 0;
	virtual void DefaultMaterial(UMaterialInterface* InDefaultMaterial) = 0;
	virtual void DefaultTranslucentMaterial(UMaterialInterface* InDefaultMaterial) = 0;

};
