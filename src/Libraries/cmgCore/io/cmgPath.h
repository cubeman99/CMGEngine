#ifndef _CMG_CORE_IO_PATH_H_
#define _CMG_CORE_IO_PATH_H_

#include <cmgCore/string/cmgString.h>
#include <cmgCore/containers/cmgArray.h>


// Class to represent a file system path, and provide easy path operations
// like concatenation, extensions, parents, and existence checks.
template
<
	class T_String,
	typename T_Char,
	typename T_InternalChar = T_Char
>
class BasicPath
{
public:
	BasicPath();
	BasicPath(const T_String& path);
	BasicPath(const T_Char* path) :
		m_path(T_String(path))
	{
		// Replace backslashes with forward slashes
		std::replace(m_path.begin(), m_path.end(), s_charBadSlash, s_charSlash);
	}

	const T_Char* c_str() const;
	const T_String& ToString() const;
	operator T_String() const;
	T_String GetName() const;
	T_String GetNameWithoutExtension() const;
	T_String GetExtension() const;
	BasicPath GetParent() const;
	bool Exists() const;
	bool FileExists() const;
	bool DirectoryExists() const;

	BasicPath& operator =(const T_String& path);
	void Set(const T_String& path);

	bool operator ==(const BasicPath& other) const;
	bool operator !=(const BasicPath& other) const;
	bool operator <(const BasicPath& other) const;
	bool operator >(const BasicPath& other) const;
	bool operator <=(const BasicPath& other) const;
	bool operator >=(const BasicPath& other) const;
	BasicPath operator /(const BasicPath& other) const;

	static BasicPath ResolvePath(const BasicPath& path, const Array<BasicPath>& paths);
	
private:
	static constexpr T_InternalChar s_charBadSlash = (T_InternalChar) '\\';
	static constexpr T_InternalChar s_charSlash = (T_InternalChar) '/';
	static constexpr T_InternalChar s_charExtension = (T_InternalChar) '.';
	static constexpr T_InternalChar s_pathSlash[2] = { s_charSlash, (T_InternalChar) 0 };
	static constexpr T_InternalChar s_extension[2] = { s_charExtension, (T_InternalChar) 0 };

	mutable T_String m_path;
};


using Path = BasicPath<String, char, char>;
using PathU16 = BasicPath<StringU16, uint16, char16_t>;



std::ostream& operator <<(std::ostream &out, const Path& path);

std::ostream& operator <<(std::ostream &out, const PathU16& path);


template<> bool BasicPath<String, char, char>::Exists() const;
template<> bool BasicPath<String, char, char>::FileExists() const;
template<> bool BasicPath<String, char, char>::DirectoryExists() const;
template<> bool BasicPath<StringU16, uint16, char16_t>::Exists() const;
template<> bool BasicPath<StringU16, uint16, char16_t>::FileExists() const;
template<> bool BasicPath<StringU16, uint16, char16_t>::DirectoryExists() const;



template<class T_String, typename T_Char, typename T_InternalChar>
inline BasicPath<T_String, T_Char, T_InternalChar>::BasicPath()
{
}

template<class T_String, typename T_Char, typename T_InternalChar>
inline BasicPath<T_String, T_Char, T_InternalChar>::BasicPath(const T_String& path) :
	m_path(path)
{
	// Replace backslashes with forward slashes
	std::replace(m_path.begin(), m_path.end(), s_charBadSlash, s_charSlash);
}
/*
template<class T_String, typename T_Char, typename T_InternalChar>
inline BasicPath<T_String, T_Char, T_InternalChar>::BasicPath(const T_Char* path) :
	m_path(T_String(path))
{
	// Replace backslashes with forward slashes
	std::replace(m_path.begin(), m_path.end(), s_charBadSlash, s_charSlash);
}*/

template<class T_String, typename T_Char, typename T_InternalChar>
inline const T_Char* BasicPath<T_String, T_Char, T_InternalChar>::c_str() const
{
	return (const T_Char*) m_path.c_str();
}

template<class T_String, typename T_Char, typename T_InternalChar>
inline const T_String& BasicPath<T_String, T_Char, T_InternalChar>::ToString() const
{
	return m_path;
}

template<class T_String, typename T_Char, typename T_InternalChar>
inline BasicPath<T_String, T_Char, T_InternalChar>::operator T_String() const
{
	return m_path;
}

template<class T_String, typename T_Char, typename T_InternalChar>
inline T_String BasicPath<T_String, T_Char, T_InternalChar>::GetName() const
{
	size_t pos = m_path.find_last_of(s_pathSlash);
	if (pos != T_String::npos)
		return m_path.substr(pos + 1);
	return m_path;
}

template<class T_String, typename T_Char, typename T_InternalChar>
inline T_String BasicPath<T_String, T_Char, T_InternalChar>::GetNameWithoutExtension() const
{
	T_String result = GetName();
	size_t pos = result.find_last_of(s_extension);
	if (pos != T_String::npos)
		return result.substr(0, pos);
	return result;
}

template<class T_String, typename T_Char, typename T_InternalChar>
inline T_String BasicPath<T_String, T_Char, T_InternalChar>::GetExtension() const
{
	size_t pos = m_path.find_last_of(s_extension);
	if (pos != T_String::npos)
		return m_path.substr(pos + 1);
	return "";
}

template<class T_String, typename T_Char, typename T_InternalChar>
inline BasicPath<T_String, T_Char, T_InternalChar> BasicPath<T_String, T_Char, T_InternalChar>::GetParent() const
{
	size_t pos = m_path.find_last_of(s_pathSlash);
	if (pos == T_String::npos)
		return BasicPath<T_String, T_Char, T_InternalChar>();
	return BasicPath<T_String, T_Char, T_InternalChar>(m_path.substr(0, pos));
}

template<class T_String, typename T_Char, typename T_InternalChar>
inline BasicPath<T_String, T_Char, T_InternalChar>& BasicPath<T_String, T_Char, T_InternalChar>::operator=(const T_String& path)
{
	m_path = path;
	return *this;
}

template<class T_String, typename T_Char, typename T_InternalChar>
inline void BasicPath<T_String, T_Char, T_InternalChar>::Set(const T_String& path)
{
	m_path = path;
}

template<class T_String, typename T_Char, typename T_InternalChar>
inline bool BasicPath<T_String, T_Char, T_InternalChar>::operator==(const BasicPath& other) const
{
	return (m_path == other.m_path);
}

template<class T_String, typename T_Char, typename T_InternalChar>
inline bool BasicPath<T_String, T_Char, T_InternalChar>::operator!=(const BasicPath& other) const
{
	return (m_path != other.m_path);
}

template<class T_String, typename T_Char, typename T_InternalChar>
inline bool BasicPath<T_String, T_Char, T_InternalChar>::operator<(const BasicPath& other) const
{
	return (m_path < other.m_path);
}

template<class T_String, typename T_Char, typename T_InternalChar>
inline bool BasicPath<T_String, T_Char, T_InternalChar>::operator>(const BasicPath& other) const
{
	return (m_path > other.m_path);
}

template<class T_String, typename T_Char, typename T_InternalChar>
inline bool BasicPath<T_String, T_Char, T_InternalChar>::operator<=(const BasicPath& other) const
{
	return (m_path <= other.m_path);
}

template<class T_String, typename T_Char, typename T_InternalChar>
inline bool BasicPath<T_String, T_Char, T_InternalChar>::operator>=(const BasicPath& other) const
{
	return (m_path >= other.m_path);
}

template<class T_String, typename T_Char, typename T_InternalChar>
inline BasicPath<T_String, T_Char, T_InternalChar>
BasicPath<T_String, T_Char, T_InternalChar>::operator/(const BasicPath& right) const
{
	return BasicPath<T_String, T_Char, T_InternalChar>(m_path + s_pathSlash + right.m_path);
}

template<class T_String, typename T_Char, typename T_InternalChar>
inline BasicPath<T_String, T_Char, T_InternalChar>
BasicPath<T_String, T_Char, T_InternalChar>::ResolvePath(
	const BasicPath& path, const Array<BasicPath>& paths)
{
	for (uint32 i = 0; i < paths.size(); i++)
	{
		BasicPath<T_String, T_Char, T_InternalChar> relativePath = paths[i] / path;
		if (relativePath.Exists())
			return relativePath;
	}
	return path;
}

#endif // _CMG_CORE_IO_PATH_H_