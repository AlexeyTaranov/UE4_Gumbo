// Fill out your copyright notice in the Description page of Project Settings.

using System;
using System.IO;
using UnrealBuildTool;

public class GumboLibrary : ModuleRules
{
    public GumboLibrary(ReadOnlyTargetRules Target) : base(Target)
    {
        Type = ModuleType.External;

        PublicIncludePaths.Add(Path.Combine(ModuleDirectory));
        PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "gumbo-parser", "lib", "x64", "Release", "gumbo.lib"));
    }
}
