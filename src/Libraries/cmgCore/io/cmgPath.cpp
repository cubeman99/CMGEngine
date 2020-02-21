#include "cmgPath.h"
#include <cmgCore/io/cmgFile.h>
#include <cmgCore/cmgAssert.h>
#include <algorithm>
#include <filesystem>
#include <sys/types.h>
#include <sys/stat.h>
#include <Windows.h>


std::ostream& operator <<(std::ostream &out, const Path& path)
{
	out << path.ToString();
	return out;
}

std::ostream& operator <<(std::ostream &out, const PathU16& path)
{
	// Convert to UTF8
	std::u16string u16str = path.ToString();
	int sizeNeeded = WideCharToMultiByte(
		CP_UTF8, 0, (const wchar_t*) &u16str[0],
		u16str.size(), NULL, 0, NULL, NULL);
	std::string utf8string(sizeNeeded, 0);
	WideCharToMultiByte(CP_UTF8, 0, (const wchar_t*) u16str.c_str(),
		u16str.length(), &utf8string[0], sizeNeeded, NULL, NULL);

	out << utf8string;
	return out;
}


template<>
bool BasicPath<String, char, char>::Exists() const
{
	return std::experimental::filesystem::exists(m_path);
}

template<>
bool BasicPath<String, char, char>::FileExists() const
{
	return File().Open(*this, FileAccess::READ, FileType::BINARY).Passed();
}

template<>
bool BasicPath<String, char, char>::DirectoryExists() const
{
	struct stat info;
	if (stat(m_path.c_str(), &info) != 0)
		return false;
	return (info.st_mode & S_IFDIR);
}

template<>
bool BasicPath<StringU16, uint16, char16_t>::Exists() const
{
	return std::experimental::filesystem::exists(m_path);
}

template<>
bool BasicPath<StringU16, uint16, char16_t>::FileExists() const
{
	return File().Open(*this, FileAccess::READ, FileType::BINARY).Passed();
}

template<>
bool BasicPath<StringU16, uint16, char16_t>::DirectoryExists() const
{
	struct _stat64i32 info;
	if (_wstat((const wchar_t*) m_path.c_str(), &info) != 0)
		return false;
	return (info.st_mode & S_IFDIR);
}



/*
namespace
{
	static const char* g_pathSlash = "/";
	static const char* g_extension = ".";
};

//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

Path::Path()
	: m_path("")
{
}

Path::Path(const T_String& path)
	: m_path(path)
{
	// Replace backslashes with forward slashes
	std::replace(m_path.begin(), m_path.end(), '\\', '/');
}

Path::Path(const char* path)
	: m_path(path)
{
	// Replace backslashes with forward slashes
	std::replace(m_path.begin(), m_path.end(), '\\', '/');
}


//-----------------------------------------------------------------------------
// Getters
//-----------------------------------------------------------------------------

const char* Path::c_str() const
{
	return m_path.c_str();
}

const T_String& Path::ToT_String() const
{
	return m_path;
}

Path::operator T_String() const
{
	return m_path;
}

T_String Path::GetName() const
{
	unsigned int pos = m_path.find_last_of(g_pathSlash);
	if (pos != T_String::npos)
		return m_path.substr(pos + 1);
	return m_path;
}

T_String Path::GetNameWithoutExtension() const
{
	T_String result = GetName();
	size_t pos = result.find_last_of(g_extension);
	if (pos != T_String::npos)
		return result.substr(0, pos);
	return result;
}

T_String Path::GetExtension() const
{
	size_t pos = m_path.find_last_of(g_extension);
	if (pos != T_String::npos)
		return m_path.substr(pos + 1);
	return "";
}

Path Path::GetParent() const
{
	size_t pos = m_path.find_last_of(g_pathSlash);
	if (pos == T_String::npos)
		return Path("");
	return Path(m_path.substr(0, pos));
}

bool Path::Exists() const
{
	return std::experimental::filesystem::exists(m_path);
}

bool Path::FileExists() const
{
	File file(*this);
	return file.Open(FileAccess::READ, FileType::BINARY).Passed();
}

bool Path::DirectoryExists() const
{
	struct stat info;
	if (stat(m_path.c_str(), &info) != 0)
		return false;
	return (info.st_mode & S_IFDIR);
}


//-----------------------------------------------------------------------------
// Setters
//-----------------------------------------------------------------------------

void Path::Set(const T_String& path)
{
	m_path = path;
}

Path& Path::operator =(const T_String& path)
{
	m_path = path;
	return *this;
}



//-----------------------------------------------------------------------------
// Operators
//-----------------------------------------------------------------------------

bool Path::operator ==(const Path& other) const
{
	return (m_path == other.m_path);
}

bool Path::operator !=(const Path& other) const
{
	return (m_path != other.m_path);
}

bool Path::operator<(const Path & right) const
{
	return m_path < right.m_path;
}

bool Path::operator>(const Path & right) const
{
	return m_path > right.m_path;
}

bool Path::operator<=(const Path & right) const
{
	return m_path <= right.m_path;
}

bool Path::operator>=(const Path & right) const
{
	return m_path >= right.m_path;
}

Path Path::operator /(const Path& right) const
{
	return Path(m_path + g_pathSlash + right.m_path);
}

std::ostream& operator <<(std::ostream &out, const Path& path)
{
	out << path.ToT_String();
	return out;
}


//-----------------------------------------------------------------------------
// Static methods
//-----------------------------------------------------------------------------

Path Path::ResolvePath(const Path& path, const Array<Path>& paths)
{
	for (uint32 i = 0; i < paths.size(); i++)
	{
		Path relativePath = paths[i] / path;
		if (relativePath.Exists())
			return relativePath;
	}
	if (path.Exists())
		return path;
	return path;
}

*/