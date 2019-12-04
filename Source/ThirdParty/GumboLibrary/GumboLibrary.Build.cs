// Fill out your copyright notice in the Description page of Project Settings.

using System;
using System.IO;
using UnrealBuildTool;

public class GumboLibrary : ModuleRules
{
    public GumboLibrary(ReadOnlyTargetRules Target) : base(Target)
    {
        Type = ModuleType.External;

        LoadGumbo(Target);
    }

    private string ModulePath
    {
        get { return ModuleDirectory; }
    }

    private string BinariesPath
    {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "../Binaries/")); }
    }


    public bool LoadGumbo(ReadOnlyTargetRules target)
    {
        bool isSupportedLibrary = false;

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            isSupportedLibrary = true;

            PublicAdditionalLibraries.Add(Path.Combine(ModulePath, "gumbo-parser", "lib", "x64" , "Release" , "gumbo.lib"));
        }
        if (isSupportedLibrary)
        {
            PublicIncludePaths.Add(Path.Combine(ModulePath, "gumbo-parser"));
        }

        return isSupportedLibrary;
    }
}
