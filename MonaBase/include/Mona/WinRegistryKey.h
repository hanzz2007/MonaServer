/*
Copyright 2013 Mona - mathieu.poux[a]gmail.com

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License received along this program for more
details (or else see http://www.gnu.org/licenses/).

This file is a part of Mona.
*/

#pragma once

#include "Mona/Mona.h"
#include "Poco/UnWindows.h"
#include <vector>

#if defined(POCO_WIN32_UTF8)
#define POCO_LPQUERY_SERVICE_CONFIG LPQUERY_SERVICE_CONFIGW
#else
#define POCO_LPQUERY_SERVICE_CONFIG LPQUERY_SERVICE_CONFIGA
#endif

namespace Mona {


class WinRegistryKey {
public:
	typedef std::vector<std::string> Keys;
	typedef std::vector<std::string> Values;
	
	enum Type {
		REGT_NONE = 0, 
		REGT_STRING = 1, 
		REGT_STRING_EXPAND = 2, 
		REGT_DWORD = 4
	};

	WinRegistryKey(const std::string& key, bool readOnly = false, REGSAM extraSam = 0);
		/// Creates the WinRegistryKey.
		///
		/// The key must start with one of the root key names
		/// like HKEY_CLASSES_ROOT, e.g. HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services.
		///
		/// If readOnly is true, then only read access to the registry
		/// is available and any attempt to write to the registry will
		/// result in an exception.
		///
		/// extraSam is used to pass extra flags (in addition to KEY_READ and KEY_WRITE)
		/// to the samDesired argument of RegOpenKeyEx() or RegCreateKeyEx().

	WinRegistryKey(HKEY hRootKey, const std::string& subKey, bool readOnly = false, REGSAM extraSam = 0);
	/// Creates the WinRegistryKey.
	///
	/// If readOnly is true, then only read access to the registry
	/// is available and any attempt to write to the registry will
	/// result in an exception.
	///
	/// extraSam is used to pass extra flags (in addition to KEY_READ and KEY_WRITE)
	/// to the samDesired argument of RegOpenKeyEx() or RegCreateKeyEx().

	virtual ~WinRegistryKey() { close(); }
		/// Destroys the WinRegistryKey.

	void setString(const std::string& name, const std::string& value);
		/// Sets the string value (REG_SZ) with the given name.
		/// An empty name denotes the default value.
		
	std::string getString(const std::string& name);
		/// Returns the string value (REG_SZ) with the given name.
		/// An empty name denotes the default value.
		///
		/// Throws a NotFoundException if the value does not exist.

	void setStringExpand(const std::string& name, const std::string& value);
		/// Sets the expandable string value (REG_EXPAND_SZ) with the given name.
		/// An empty name denotes the default value.
		
	std::string getStringExpand(const std::string& name);
		/// Returns the string value (REG_EXPAND_SZ) with the given name.
		/// An empty name denotes the default value.
		/// All references to environment variables (%VAR%) in the string
		/// are expanded.
		///
		/// Throws a NotFoundException if the value does not exist.

	void setInt(const std::string& name, int value);
		/// Sets the numeric (REG_DWORD) value with the given name.
		/// An empty name denotes the default value.
		
	int getInt(const std::string& name);
		/// Returns the numeric value (REG_DWORD) with the given name.
		/// An empty name denotes the default value.
		///
		/// Throws a NotFoundException if the value does not exist.

	void deleteValue(const std::string& name);
		/// Deletes the value with the given name.
		///
		/// Throws a NotFoundException if the value does not exist.

	void deleteKey();
		/// Recursively deletes the key and all subkeys.

	bool exists();
		/// Returns true iff the key exists.

	Type type(const std::string& name);
		/// Returns the type of the key value.
		
	bool exists(const std::string& name);
		/// Returns true iff the given value exists under that key.

	void subKeys(Keys& keys);
		/// Appends all subKey names to keys.

	void values(Values& vals);
		/// Appends all value names to vals;
		
	bool isReadOnly() const { return _readOnly; }
		/// Returns true iff the key has been opened for read-only access only.

protected:
	void open();
	void close();
	std::string key() const;
	std::string key(const std::string& valueName) const;
	static HKEY handleFor(const std::string& rootKey);

private:
	HKEY        _hRootKey;
	std::string _subKey;
	HKEY        _hKey;
	bool        _readOnly;
	REGSAM      _extraSam;
};

} // namespace Mona
