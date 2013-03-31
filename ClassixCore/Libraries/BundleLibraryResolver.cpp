//
// BundleLibraryResolver.cpp
// Classix
//
// Copyright (C) 2012 Félix Cloutier
//
// This file is part of Classix.
//
// Classix is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// Classix is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// Classix. If not, see http://www.gnu.org/licenses/.
//

#include <CoreFoundation/CoreFoundation.h>
#include <memory>

#include "BundleLibraryResolver.h"

// The contents of this file is specific to Mac OS X. Don't even try to port it.
// Only Mac OS has the concept of "bundles".

namespace
{
	template<typename TCFType>
	class CXCFOwningRef
	{
		TCFType cfObject;
		
	public:
		CXCFOwningRef(TCFType cfObject, bool retain = false)
		{
			this->cfObject = cfObject;
			if (retain)
				CFRetain(cfObject);
		}
		
		CXCFOwningRef(const CXCFOwningRef<TCFType>& that)
		{
			this->cfObject = CFRetain(that.cfObject);
		}
		
		CXCFOwningRef(CXCFOwningRef<TCFType>&& that)
		{
			this->cfObject = that.cfObject;
			that.cfObject = nullptr;
		}
		
		operator TCFType()
		{
			return cfObject;
		}
		
		~CXCFOwningRef()
		{
			CFRelease(cfObject);
		}
	};
	
	std::string CFStringToStdString(CFStringRef string, CFStringEncoding encoding = kCFStringEncodingMacRoman)
	{
		if (const char* ptr = CFStringGetCStringPtr(string, encoding))
			return ptr;
		
		size_t length = CFStringGetLength(string) + 1;
		char cString[length];
		if (CFStringGetCString(string, cString, length, encoding))
			return cString;
		
		throw std::logic_error("Could not convert CFStringRef to std::string");
	}
	
	std::string CFURLToStdString(CFURLRef url)
	{
		CXCFOwningRef<CFStringRef> path = CFURLCopyFileSystemPath(url, kCFURLPOSIXPathStyle);
		return CFStringToStdString(path);
	}
	
	const CFStringRef CXLibraryNameKey = CFSTR("com.felixcloutier.ClassixCore.BundleLibraryResolver.LibraryName");
}

namespace ClassixCore
{
	BundleLibrary::BundleLibrary(CFBundleRef bundle)
	{
		CXCFOwningRef<CFURLRef> bundleUrl = CFBundleCopyBundleURL(bundle);
		CFStringRef libraryName = (CFStringRef)CFBundleGetValueForInfoDictionaryKey(bundle, CXLibraryNameKey);
		void* onLoad = CFBundleGetFunctionPointerForName(bundle, CFSTR("LibraryLoad"));
		void* lookup = CFBundleGetFunctionPointerForName(bundle, CFSTR("LibraryLookup"));
		void* onUnload = CFBundleGetFunctionPointerForName(bundle, CFSTR("LibraryUnload"));
		void* symbols = CFBundleGetDataPointerForName(bundle, CFSTR("LibrarySymbolNames"));
		
		if (onLoad == nullptr || lookup == nullptr || onUnload == nullptr || symbols == nullptr)
			throw std::logic_error("Incomplete library");
		
		Path = CFURLToStdString(bundleUrl);
		Name = CFStringToStdString(libraryName);
		OnLoad = reinterpret_cast<OnLoadFunction>(onLoad);
		Lookup = reinterpret_cast<LookupFunction>(lookup);
		OnUnload = reinterpret_cast<OnUnloadFunction>(onUnload);
		Symbols = reinterpret_cast<const char**>(symbols);
		this->bundle = bundle;
		CFRetain(bundle);
	}
	
	BundleLibrary::BundleLibrary(BundleLibrary&& that)
	{
		Path = std::move(that.Path);
		Name = std::move(that.Name);
		OnLoad = std::move(that.OnLoad);
		Lookup = std::move(that.Lookup);
		OnUnload = std::move(that.OnUnload);
		Symbols = that.Symbols;
		bundle = that.bundle;
		that.bundle = nullptr;
	}
	
	BundleLibrary::~BundleLibrary()
	{
		CFRelease(bundle);
	}
	
	BundleLibraryResolver::BundleLibraryResolver(Common::IAllocator& allocator, const std::string& directoryPath)
	: allocator(allocator)
	{
		const UInt8* buffer = reinterpret_cast<const UInt8*>(directoryPath.c_str());
		CXCFOwningRef<CFURLRef> directoryUrl = CFURLCreateFromFileSystemRepresentation(kCFAllocatorDefault, buffer, directoryPath.length(), true);
		CXCFOwningRef<CFArrayRef> bundles = CFBundleCreateBundlesFromDirectory(kCFAllocatorDefault, directoryUrl, CFSTR("ixLibrary"));
		
		for (CFIndex i = 0; i < CFArrayGetCount(bundles); i++)
		{
			void* voidBundle = const_cast<void*>(CFArrayGetValueAtIndex(bundles, i));
			CFBundleRef bundle = reinterpret_cast<CFBundleRef>(voidBundle);
			CFStringRef libraryName = (CFStringRef)CFBundleGetValueForInfoDictionaryKey(bundle, CXLibraryNameKey);
			allBundles[CFStringToStdString(libraryName)] = bundle;
		}
	}
	
	BundleLibraryResolver::BundleLibraryResolver(const BundleLibraryResolver& that)
	: allocator(that.allocator), allowedBundles(that.allowedBundles)
	{
		allBundles.reserve(that.allBundles.size());
		for (auto& pair : that.allBundles)
		{
			CFRetain(pair.second);
			allBundles.insert(pair);
		}
	}
	
	BundleLibraryResolver::BundleLibraryResolver(BundleLibraryResolver&& that)
	: allocator(that.allocator)
	{
		allBundles.swap(that.allBundles);
		allowedBundles.swap(that.allowedBundles);
	}
	
	void BundleLibraryResolver::AllowLibrary(const std::string& cfmName)
	{
		allowedBundles.insert(cfmName);
	}
	
	CFM::SymbolResolver* BundleLibraryResolver::ResolveLibrary(const std::string& name)
	{
		if (allowedBundles.find(name) != allowedBundles.end())
		{
			auto iter = allBundles.find(name);
			if (iter != allBundles.end())
			{
				loadedLibraries.emplace_back(iter->second);
				resolvers.emplace_back(allocator, loadedLibraries.back());
				return &resolvers.back();
			}
		}
		
		return nullptr;
	}
	
	BundleLibraryResolver::~BundleLibraryResolver()
	{
		for (auto& pair : allBundles)
		{
			CFRelease(pair.second);
		}
	}
}
