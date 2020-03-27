#include "cmgMesh.h"
#include <cmgGraphics/importers/cmgSourceModelImporter.h>
#include <cmgGraphics/importers/cmgObjModelImporter.h>
#include <sstream>


struct CMGMeshFile
{
	static constexpr char MAGIC[] = "CMGMesh_";

	// Header (64 bytes)
	struct Header
	{
		char magic[8] = {'C', 'M', 'G', 'M', 'e', 's', 'h', '_'};
		uint32 version = 1;
		uint32 primitiveType;
		uint32 vertexAttributeFlags;
		uint32 vertexSize = 4;
		uint32 vertexCount = 0;
		uint32 indexSize = 4;
		uint32 indexCount = 0;
		uint32 vertexStartOffset = 64;
		uint32 indexStartOffset = 64;
		uint32 reserved[5] = { 0, 0, 0, 0, 0 };
	} header;

	// Data
	uint8* vertexData;
	uint8* indexData;
};


//-----------------------------------------------------------------------------
// Constructors & Destructor
//-----------------------------------------------------------------------------

Mesh::Mesh() :
	m_firstIndex(0),
	m_numIndices(0),
	m_primitiveType(VertexPrimitiveType::k_triangles)
{
}

Mesh::~Mesh()
{
}


//-----------------------------------------------------------------------------
// Static Methods
//-----------------------------------------------------------------------------

Error Mesh::Load(const Path& path, Mesh*& outMesh, MeshLoadOptions::value_type options)
{
	// Open the file to read the magic header ID
	File file(path);
	Error error = file.Open(FileAccess::READ, FileType::BINARY);
	if (error.Failed())
		return error.Uncheck();
	char magic[9] = { 0 };
	file.Read(magic, 8);
	if (strncmp(magic, CMGMeshFile::MAGIC, 8) == 0)
		return LoadCMG(path, outMesh);
	file.Close();
	return LoadOBJ(path, outMesh, options);
}

Error Mesh::LoadOBJ(const Path& path, Mesh*& outMesh, MeshLoadOptions::value_type options)
{
	return CMG_ERROR_NOT_IMPLEMENTED;
}

Error Mesh::LoadCMG(const Path& path, Mesh*& outMesh)
{
	File file(path);
	Error error = file.Open(FileAccess::READ, FileType::BINARY);
	if (error.Failed())
		return error.Uncheck();
	return outMesh->DecodeCMG(file);
}

Error Mesh::SaveCMG(const Path& path, const Mesh* mesh)
{
	File file(path);
	Error error = file.Open(FileAccess::WRITE, FileType::BINARY);
	if (error.Failed())
		return error.Uncheck();
	return mesh->EncodeCMG(file);
}

Error Mesh::EncodeCMG(File& file) const
{
	CMGMeshFile contents;
	auto vertexBuffer = m_vertexData.GetVertexBuffer();
	auto indexBuffer = m_indexData.GetIndexBuffer();

	// Header
	contents.header.version = 1;
	contents.header.primitiveType = (uint32) m_primitiveType;
	contents.header.vertexAttributeFlags = (uint32) vertexBuffer->GetAttributeFlags();
	contents.header.vertexCount = (uint32) m_vertexData.GetCount();
	contents.header.indexCount = (uint32) m_indexData.GetCount();
	contents.header.vertexSize = VertexBuffer::CalcVertexSize(vertexBuffer->GetAttributeFlags());
	contents.header.indexSize = sizeof(uint32);
	memset(contents.header.reserved, 0, sizeof(contents.header.reserved));
	contents.header.vertexStartOffset = sizeof(contents.header);
	contents.header.indexStartOffset = contents.header.vertexStartOffset +
		(contents.header.vertexCount * contents.header.vertexSize);
	file.Write(&contents.header, sizeof(contents.header));

	// Vertex data
	contents.vertexData = (uint8*) vertexBuffer->MapBufferDataRead<uint8>();
	file.Write(contents.vertexData +
		(m_vertexData.GetStart() * contents.header.vertexSize),
		contents.header.vertexCount * contents.header.vertexSize);
	vertexBuffer->UnmapBufferData();
	contents.vertexData = nullptr;

	// Index data
	contents.indexData = (uint8*) indexBuffer->MapBufferDataRead<uint8>();
	file.Write(contents.indexData +
		(m_indexData.GetStart() * contents.header.indexSize),
		contents.header.indexCount * contents.header.indexSize);
	indexBuffer->UnmapBufferData();
	contents.indexData = nullptr;

	return CMG_ERROR_SUCCESS;
}

Error Mesh::DecodeCMG(File& file)
{
	CMGMeshFile contents;

	// Read the header
	uint32 offset = file.Tell();
	file.Read(&contents.header, sizeof(contents.header));
	if (memcmp(contents.header.magic, CMGMeshFile::MAGIC, 8) != 0)
		return CMG_ERROR(Error::k_file_corrupt);

	// Read the vertex data
	file.SeekFromStart(offset + contents.header.vertexStartOffset);
	contents.vertexData = new uint8[
		contents.header.vertexSize * contents.header.vertexCount];
	file.Read(contents.vertexData, contents.header.vertexSize *
		contents.header.vertexCount);
	m_vertexData.BufferVertices(
		contents.header.vertexAttributeFlags, contents.header.vertexCount, 
		contents.vertexData);
	delete [] contents.vertexData;

	// Read the index data
	file.SeekFromStart(offset + contents.header.indexStartOffset);
	contents.indexData = new uint8[
		contents.header.indexSize * contents.header.indexCount];
	file.Read(contents.indexData, contents.header.indexSize *
		contents.header.indexCount);
	m_indexData.BufferIndices(
		contents.header.indexCount, (uint32*) contents.indexData);
	delete [] contents.indexData;

	return CMG_ERROR_SUCCESS;
}


Error Mesh::UnloadImpl()
{
	m_jointNames.clear();
	m_vertexData.Clear();
	m_indexData.Clear();
	return CMG_ERROR_SUCCESS;
}

Error Mesh::LoadImpl()
{
	m_jointNames.clear();
	m_vertexData.Clear();
	m_indexData.Clear();

	Path path = GetResourceLoader()->GetResourcePath(GetResourceName());
	if (!path.FileExists())
		return CMG_ERROR_FILE_NOT_FOUND;

	String extension = cmg::string::ToLower(path.GetExtension());

	if (extension == "smd")
	{
		return CMG_ERROR_NOT_IMPLEMENTED;
	}
	else if (extension == "obj")
	{
		return CMG_ERROR_NOT_IMPLEMENTED;
	}
	else
	{
		// Load a CMG model file
		File file(path);
		Error error = file.Open(FileAccess::READ, FileType::BINARY);
		if (error.Failed())
			return error.Uncheck();
		return DecodeCMG(file);
	}

	return CMG_ERROR_SUCCESS;
}

