// Copyright aSurgingRiver, Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
    public class WebView : ModuleRules
    {
        public WebView(ReadOnlyTargetRules Target) : base(Target)
        {
            bool isUsingJson = false;
            if (isUsingJson)  {// use for JsonLibaray
                PublicDefinitions.Add("JSON_LIB"); //添加 自定义的宏 或者 引擎的宏
                PublicDependencyModuleNames.Add("JsonLibrary");
            }

            PublicDependencyModuleNames.AddRange(
                new string[]{
                    "Core",
                    "CoreUObject",
                    "Slate",
                    "SlateCore",
                    "UMG",
                    "Engine",
                    "CefBrowser"
                }
            );

            if (Target.bBuildEditor == true) {
                PrivateIncludePathModuleNames.AddRange(
                    new string[] {
                        "UnrealEd",
                    }
                );
                PrivateDependencyModuleNames.AddRange(
                    new string[] {
                        "UnrealEd",
                    }
                );
            }
        }
    }
}
