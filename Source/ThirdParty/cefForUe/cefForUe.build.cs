// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System;
using System.IO;
using System.Text;
using System.Collections.Generic;
using System.IO.Compression;

public class cefForUe : ModuleRules
{
    public cefForUe(ReadOnlyTargetRules Target) : base(Target)
    {
        Type = ModuleType.External;
        //Console.WriteLine(Target.bBuildEditor ? "editor":"runtime");
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            InitCEF3("cef_95.4638", "win64", "cefhelper.exe", ".dll", !Target.bBuildEditor);
        }
        else if (Target.Platform == UnrealTargetPlatform.Linux)
        {
            InitCEF3("cef_95.4638", "linux", "cefhelper", ".so", !Target.bBuildEditor);
        }
        else
        {
            return;
        }
        if (!Target.bBuildEditor) return;
        CheckLicense(""+Target.ProjectFile.ToFileInfo().Directory);
        // Target.ProjectFile.ToFileInfo().Directory
        // Console.WriteLine("============="+  Target.ProjectFile.ToFileInfo().Directory);
        // 检查license授权目录 
        // Process.GetCurrentProcess().Kill(); 
    }
    void InitCEF3(string CEFVersion, string platform, string renderName, string subfixDLL,bool isRuntime)
    {
        string CEFRoot = Path.Combine(ModuleDirectory, CEFVersion);
        string LibraryPath = Path.Combine(CEFRoot, platform, "lib");
        Type = ModuleType.External;
        string split = ".split";
        // merge file
        Dictionary<string, Dictionary<int, string>> mapFile = new Dictionary<string, Dictionary<int, string>>();
        // And the entire Resources folder. Enumerate the entire directory instead of mentioning each file manually here.
        foreach (string FileName in Directory.EnumerateFiles(CEFRoot, "*"+ split, SearchOption.AllDirectories))
        {// 获取合并文件
            if(isRuntime)break;
            string file = Path.GetFileName(FileName);
            string filePath = Path.GetDirectoryName(FileName);
            //Console.WriteLine(FileName);
            if (!filePath.EndsWith(".dir")) continue;
            //Console.WriteLine(filePath);
            string splitName = Path.GetFileName(filePath).Replace(".dir", "");
            string splitPath = Path.GetDirectoryName(filePath);
            string splitPN = Path.Combine(splitPath, splitName);
            if (File.Exists(splitPN)) continue;
            if (!mapFile.ContainsKey(splitPN))
                mapFile.Add(splitPN, new Dictionary<int, string>());
            int idx = int.Parse(file.Replace(split, ""));
            mapFile[splitPN].Add(idx, FileName);
        }
        const int maxBuff = 1024 * 1024 * 100;
        byte[] readBuff = new byte[maxBuff];// 单个文件最大100M
        foreach (KeyValuePair<string, Dictionary<int, string>> kvp in mapFile){
            if (kvp.Value.Count == 0) continue;
            FileStream fileDst = new FileStream(kvp.Key, FileMode.OpenOrCreate);
            foreach (KeyValuePair<int, string> oneKvp in kvp.Value) {
                //Console.WriteLine(oneKvp.Value + " write");
                FileStream fileSrc = new FileStream(oneKvp.Value, FileMode.Open);
                long fileSize = fileSrc.Length;
                while (0 < fileSize){
                    int readLen = fileSrc.Read(readBuff, 0, maxBuff);
                    fileDst.Write(readBuff, 0, readLen);
                    fileSize -= readLen;
                }
            }
        }


        PublicSystemIncludePaths.Add(Path.Combine(CEFRoot, platform));
        PublicDefinitions.Add("CEF3_RENDER=\"" + renderName + "\""); //
        PublicDefinitions.Add("CEF3_VERSION=\"" + CEFVersion + "\""); //

        // And the entire Resources folder. Enumerate the entire directory instead of mentioning each file manually here.
        if (Target.Platform == UnrealTargetPlatform.Win64)
            foreach (string FileName in Directory.EnumerateFiles(LibraryPath, "*.lib", SearchOption.TopDirectoryOnly)){
                PublicAdditionalLibraries.Add(FileName);
            }
        foreach (string FileName in Directory.EnumerateFiles(LibraryPath, "*"+ subfixDLL, SearchOption.TopDirectoryOnly)){
            //System.Console.WriteLine("cef3lib: " + LibraryPath+" "+ System.IO.Path.GetFileName(FileName));
            PublicDelayLoadDLLs.Add(System.IO.Path.GetFileName(FileName));
            RuntimeDependencies.Add(FileName);
        }
        List<string> Dlls = new List<string>();
        Dlls.Add("icudtl.dat");
        Dlls.Add("snapshot_blob.bin");
        Dlls.Add("v8_context_snapshot.bin");
        if (Target.Platform == UnrealTargetPlatform.Linux)
        {
            Dlls.Add("vk_swiftshader_icd.json");
            PublicDelayLoadDLLs.Add("libvulkan.so.1");
            RuntimeDependencies.Add(Path.Combine(LibraryPath, "libvulkan.so.1"));
        }
        Dlls.Add(Path.Combine("swiftshader", "libEGL"+ subfixDLL));
        Dlls.Add(Path.Combine("swiftshader", "libGLESv2"+ subfixDLL));
        foreach (string Dll in Dlls) {
            RuntimeDependencies.Add(Path.Combine(LibraryPath, Dll));
        }
        foreach (string FileName in Directory.EnumerateFiles(LibraryPath, "*.pak", SearchOption.AllDirectories)) {
            string DependencyName = FileName.Substring(Target.UEThirdPartyBinariesDirectory.Length).Replace('\\', '/');
            RuntimeDependencies.Add(FileName);
        }
        RuntimeDependencies.Add(Path.Combine(LibraryPath, renderName));
        //string webviewLic = Path.Combine(ModuleDirectory, "license", "webview.dat");
        //if (File.Exists(webviewLic)) {// 如果存在则放入license
        //    RuntimeDependencies.Add(webviewLic);
        //}
        // Restore backup
        string modulePath = ModuleDirectory;
        string pluginPath = Path.Combine(modulePath, "..", "..", "..");
// Source\ThirdPaty\cef3lib\cef_94.4638\include
// Source\ThirdPaty\cef3lib\cef_94.4638\lib\win64\language

        if (isRuntime) return ;
        CopyDir(".lng", Path.Combine(modulePath, "language", "Intermediate"), Path.Combine(pluginPath, "Intermediate"));
        CopyDir(".lng", Path.Combine(modulePath, "language", "Binaries"), Path.Combine(pluginPath, "Binaries"));
    }
    void CopyDir(string subfix, string outPath, string DstRoot) {
        if (!Directory.Exists(outPath)) return;
        foreach (string FileName in Directory.EnumerateFiles(outPath, "*"+subfix, SearchOption.AllDirectories))
        {
            string newFile = FileName.Replace(outPath, DstRoot);
            string file = Path.GetFileName(newFile).Replace(subfix,"");
            string pathDst = Path.GetDirectoryName(newFile);
            newFile = Path.Combine(pathDst, file);
            if (File.Exists(newFile)) continue;
            if (!Directory.Exists(pathDst)){
                Directory.CreateDirectory(pathDst);
            }
            System.IO.File.Copy(FileName, newFile, true);
        }
    }

    void CheckLicense(string Target)
    {
        string licensePath = Path.Combine(Target, "Content", "license");
        if (!Directory.Exists(licensePath)){
            Directory.CreateDirectory(licensePath);
        }
        string license = Path.Combine(licensePath, "webview.dat");
        if (!File.Exists(license)) {
            string webviewLic = Path.Combine(ModuleDirectory, "license", "webview.dat");
            if (File.Exists(webviewLic)) {
                System.IO.File.Copy(webviewLic, license);
            }
        }
        string GamePath = Path.Combine(Target, "Config");
        string GameCfg = Path.Combine(GamePath, "DefaultGame.ini");
        if (!Directory.Exists(GamePath)) {
            Directory.CreateDirectory(GamePath);
        }
        if (!File.Exists(GameCfg)) {
            File.Create(GameCfg);
        }
        //if( File.OpenWrite(GameCfg)) return ;
        string content;
        try { content = File.ReadAllText(GameCfg/*, Encoding.UTF8*/); }
        catch
        {// 文件在使用中
            return;
        }
            
        string licensePak = "+DirectoriesToAlwaysStageAsUFS=(Path=\"license\")";
        string licenseNode = "[/Script/UnrealEd.ProjectPackagingSettings]";
        if (content.Contains(licenseNode))
        {
            if (content.Contains(licensePak)) {
                Console.WriteLine(GameCfg+" has configure!");
                return;// 已经有配置
            }
            content = content.Replace(licenseNode, licenseNode + "\n" + licensePak);
        }
        else {
            content += "\n\n" + licenseNode + "\n" + licensePak;
        }
        File.WriteAllText(GameCfg, content, Encoding.UTF8);
        Console.WriteLine(GameCfg + " auto configure!");
    }
}
