#ifndef _CMG_CORE_IO_PATH_H_
#define _CMG_CORE_IO_PATH_H_

#include <cmgCore/string/cmgString.h>
#include <cmgCore/containers/cmgArray.h>


class Path
{
public:
	Path();
	Path(const String& path);
	Path(const char* path);

	String GetFileName() const;
	String GetFileNameWithoutExtension() const;
	String GetExtension() const;
	String GetDirectory() const;

	bool Exists() const;
	bool FileExists() const;
	bool DirectoryExists() const;
	bool HasFilename() const;

	const char* c_str() const;
	const String& GetPath() const;
	void SetPath(const String& path);

	bool operator ==(const Path& other) const;
	bool operator !=(const Path& other) const;

	Path operator +(const Path& right) const;
	static Path ResolvePath(const Path& path, const Array<Path>& paths);

private:
	mutable String m_path;
};


std::ostream& operator <<(std::ostream &out, const Path& path);


#endif // _CMG_CORE_IO_PATH_H_