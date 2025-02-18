﻿using System.Xml;

namespace InteropGen;

internal static class VcxprojParser
{
	// Note that these paths only work for the windows x64 platforms right now.
	// If we want other platforms and architectures, this will need changing..
	// InteropGen isn't architecture- or platform-specific in any way, so this
	// should not be a big deal.
	private const string ExternalIncludePath = "/rs:Project/rs:PropertyGroup[@Condition=\"'$(Configuration)|$(Platform)'=='Debug|x64'\"]/rs:ExternalIncludePath";
	private const string IncludePath = "/rs:Project/rs:PropertyGroup[@Condition=\"'$(Configuration)|$(Platform)'=='Debug|x64'\"]/rs:IncludePath";

	private static string GetNodeContents( XmlNode root, string xpath, XmlNamespaceManager namespaceManager )
	{
		var nodeList = root.SelectNodes( xpath, namespaceManager );
		if ( nodeList?.Count == 0 || nodeList?[0] == null )
			throw new Exception( "Couldn't find IncludePath!" );

#pragma warning disable CS8602 // Dereference of a possibly null reference.
		var includeStr = nodeList[0].InnerText;
#pragma warning restore CS8602 // Dereference of a possibly null reference.

		return includeStr;
	}

	/// <summary>
	/// Parse the include list from a vcxproj file.
	/// </summary>
	/// <remarks>
	/// This currently only supports x64-windows, so any different includes for other platforms
	/// will not be reflected here.
	/// </remarks>
	public static List<string> ParseIncludes( string path )
	{
		XmlDocument doc = new XmlDocument();
		doc.Load( path );

		XmlNamespaceManager namespaceManager = new XmlNamespaceManager( doc.NameTable );
		namespaceManager.AddNamespace( "rs", "http://schemas.microsoft.com/developer/msbuild/2003" );

		if ( doc.DocumentElement == null )
			throw new Exception( "Failed to parse root node!" );

		XmlNode root = doc.DocumentElement;

		List<string> includes = new();

		// Select Project -> PropertyGroup -> ExternalIncludePath
		{
			var includeStr = GetNodeContents( root, ExternalIncludePath, namespaceManager );
			includes.AddRange( includeStr.Split( ';', StringSplitOptions.TrimEntries ) );
		}

		// Select Project -> PropertyGroup -> IncludePath and merge it
		{
			var includeStr = GetNodeContents( root, IncludePath, namespaceManager );
			includes.AddRange( includeStr.Split( ';', StringSplitOptions.TrimEntries ) );
		}

		// Define environment variables
		var environmentVariables = new Dictionary<string, string>()
		{
			{ "VULKAN_SDK", Environment.GetEnvironmentVariable( "VULKAN_SDK" ) ?? @"C:\VulkanSDK\1.3.224.1\Include" },
			{ "ProjectDir", "..\\Host\\" },
			{ "SolutionDir", "..\\" },
			{ "Platform", "x64" },
			{ "VcpkgRoot", Environment.GetEnvironmentVariable( "VCPKG_ROOT" ) ?? $@"C:\Users\{Environment.UserName}\vcpkg" },
			{ "IncludePath", "..\\Host\\" },
			{ "ExternalIncludePath", "" }
		};

		List<string> parsedIncludes = new();

		// Simple find-and-replace for macros and environment variables
		foreach ( var include in includes )
		{
			var processedInclude = include;

			foreach ( var environmentVariable in environmentVariables )
			{
				processedInclude = processedInclude.Replace( $"$({environmentVariable.Key})", environmentVariable.Value );
			}

			parsedIncludes.Add( processedInclude );
		}

		return parsedIncludes;
	}
}
