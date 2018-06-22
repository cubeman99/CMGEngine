#include "cmgPath.h"
#include <cmgCore/io/cmgFile.h>
#include <cmgCore/cmgAssert.h>
#include <algorithm>

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
// Accessors
//-----------------------------------------------------------------------------

String Path::GetFileName() const
{
	unsigned int pos = m_path.find_last_of(g_pathSlash);
	if (pos != String::npos)
		return m_path.substr(pos + 1);
	return "";
}

String Path::GetFileNameWithoutExtension() const
{
	String result = GetFileName();
	unsigned int pos = result.find_last_of(g_extension);
	if (pos != String::npos)
		return result.substr(0, pos);
	return result;
}

String Path::GetExtension() const
{
	unsigned int pos = m_path.find_last_of(g_extension);
	if (pos != String::npos)
		return m_path.substr(pos + 1);
	return "";
}

String Path::GetPathWithoutFileName() const
{
	unsigned int pos = m_path.find_last_of(g_pathSlash);
	return m_path.substr(0, pos);
}


//-----------------------------------------------------------------------------
// Path Checks
//-----------------------------------------------------------------------------

bool Path::FileExists()
{
	File file(*this);
	return file.Open(FileAccess::READ, FileType::BINARY).Succeeded();
}

bool Path::DirectoryExists() const
{
	//CMG_ASSERT_UNIMPLEMENTED_FUNCTION();
	return true;
}

bool Path::HasFilename() const
{
	//CMG_ASSERT_UNIMPLEMENTED_FUNCTION();
	return true;
}


//-----------------------------------------------------------------------------
// Path Getter & Setter
//-----------------------------------------------------------------------------

const char* Path::c_str() const
{
	return m_path.c_str();
}

const String& Path::GetPath() const
{
	return m_path;
}

void Path::SetPath(const String& path)
{
	m_path = path;
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
