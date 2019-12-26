#include "cmgPath.h"
#include <cmgCore/io/cmgFile.h>
#include <cmgCore/cmgAssert.h>
#include <algorithm>
#include <filesystem>
#include <sys/types.h>
#include <sys/stat.h>

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

Path::Path(const String& path)
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

const String& Path::ToString() const
{
	return m_path;
}

Path::operator String() const
{
	return m_path;
}

String Path::GetName() const
{
	unsigned int pos = m_path.find_last_of(g_pathSlash);
	if (pos != String::npos)
		return m_path.substr(pos + 1);
	return m_path;
}

String Path::GetNameWithoutExtension() const
{
	String result = GetName();
	size_t pos = result.find_last_of(g_extension);
	if (pos != String::npos)
		return result.substr(0, pos);
	return result;
}

String Path::GetExtension() const
{
	size_t pos = m_path.find_last_of(g_extension);
	if (pos != String::npos)
		return m_path.substr(pos + 1);
	return "";
}

Path Path::GetParent() const
{
	size_t pos = m_path.find_last_of(g_pathSlash);
	if (pos == String::npos)
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
// Settets
//-----------------------------------------------------------------------------

void Path::Set(const String& path)
{
	m_path = path;
}

Path& Path::operator =(const String& path)
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
	return Path(m_path + "/" + right.m_path);
}

std::ostream& operator <<(std::ostream &out, const Path& path)
{
	out << path.ToString();
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

