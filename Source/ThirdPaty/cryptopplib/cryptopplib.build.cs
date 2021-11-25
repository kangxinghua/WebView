// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;
using System.Collections.Generic;

public class cryptopplib : ModuleRules
{
    public cryptopplib(ReadOnlyTargetRules Target) : base(Target)
    {
        if(Target.Platform != UnrealTargetPlatform.Win64)
            return ;
        Type = ModuleType.External;
        string CEFVersion = "cryptopp_8.6.0";
        string CEFRoot = Path.Combine(ModuleDirectory, CEFVersion);
        PublicAdditionalLibraries.Add(Path.Combine(CEFRoot, "lib", "win64","cryptlib.lib"));
		PublicIncludePaths.Add(CEFRoot);
        PublicDefinitions.Add("CRYPTOPP_ENABLE_NAMESPACE_WEAK=1"); //添加 自定义的宏 或者 引擎的宏
    }
}
