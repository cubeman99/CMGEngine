#ifndef _CMG_CORE_IO_PATH_H_
#define _CMG_CORE_IO_PATH_H_

#include <cmgCore/string/cmgString.h>


class Path
{
public:
	Path();
	Path(const String& path);
	Path(const char* path);

	String GetFileName() const;
	String GetFileNameWithoutExtension() const;
	String GetExtension() const;
	String GetPathWithoutFileName() const;

	bool FileExists();

	bool DirectoryExists() const;
	bool HasFilename() const;

	const char* c_str() const;
	const String& GetPath() const;
	void SetPath(const String& path);

	bool operator ==(const Path& other) const;
	bool operator !=(const Path& other) const;

private:
	mutable String m_path;
};


#endif // _CMG_CORE_IO_PATH_H_