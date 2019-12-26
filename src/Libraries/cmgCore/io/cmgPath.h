#ifndef _CMG_CORE_IO_PATH_H_
#define _CMG_CORE_IO_PATH_H_

#include <cmgCore/string/cmgString.h>
#include <cmgCore/containers/cmgArray.h>


// Class to represent a file system path, and provide easy path operations
// like concatenation, extensions, parents, and existence checks.
class Path
{
public:
	Path();
	Path(const String& path);
	Path(const char* path);

	const char* c_str() const;
	const String& ToString() const;
	operator String() const;
	String GetName() const;
	String GetNameWithoutExtension() const;
	String GetExtension() const;
	Path GetParent() const;
	bool Exists() const;
	bool FileExists() const;
	bool DirectoryExists() const;

	Path& operator =(const String& path);
	void Set(const String& path);

	bool operator ==(const Path& other) const;
	bool operator !=(const Path& other) const;
	bool operator <(const Path& other) const;
	bool operator >(const Path& other) const;
	bool operator <=(const Path& other) const;
	bool operator >=(const Path& other) const;
	Path operator /(const Path& other) const;

	static Path ResolvePath(const Path& path, const Array<Path>& paths);
	
private:
	mutable String m_path;
};


std::ostream& operator <<(std::ostream &out, const Path& path);


#endif // _CMG_CORE_IO_PATH_H_