#include "cmgMesh.h"
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

struct ObjFace
{
	int position;
	int texCoord;
	int normal;

	ObjFace() :
		position(-1),
		texCoord(-1),
		normal(-1)
	{}
};

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

	return LoadOBJ(path, outMesh, options);
}

Error Mesh::LoadOBJ(const Path& path, Mesh*& outMesh, MeshLoadOptions::value_type options)
{
	String fileContents;
	Error openError = File::OpenAndGetContents(path, fileContents);
	if (openError.Failed())
	{
		openError.Uncheck();
		return openError;
	}

	bool flipTriangles = (options & MeshLoadOptions::k_flip_triangles) != 0;

	Array<Vector3f> positions;
	Array<Vector3f> normals;
	Array<Vector2f> texCoords;
	Array<ObjFace> faces;
	std::string line;
	std::stringstream fileStream(fileContents);

	while (std::getline(fileStream, line))
	{
		// Ignore empty lines or commented lines.
		if (line.length() == 0 || line[0] == '#')
			continue;

		std::stringstream lineStream(line);
		std::string lineType;
		lineStream >> lineType;
		
		Vector3f v3;
		Vector2f v2;

		if (lineType == "v")
		{
			if (lineStream >> v3.x >> v3.y >> v3.z)
				positions.push_back(v3);
		}
		else if (lineType == "vt")
		{
			if (lineStream >> v2.x >> v2.y)
				texCoords.push_back(v2);
		}
		else if (lineType == "vn")
		{
			if (lineStream >> v3.x >> v3.y >> v3.z)
				normals.push_back(v3);
		}
		else if (lineType == "f")
		{
			String token;
			unsigned int faceVertex = 0;
			unsigned int firstVertex = faces.size();
			while (lineStream >> token)
			{
				ObjFace face;

				// Parse indices.
				unsigned int index = token.find_first_of('/', 0);
				if (index > 0)
					face.position = atoi(token.substr(0, index).c_str()) - 1;
				unsigned int start = index + 1;
				index = token.find_first_of('/', start);
				if (index > start)
					face.texCoord = atoi(token.substr(start, index - start).c_str()) - 1;
				start = index + 1;
				if (start < token.length())
					face.normal = atoi(token.substr(start, token.length() - start).c_str()) - 1;
				
				// Triangulate faces with more than three vertices.
				if (faceVertex >= 3)
				{
					faces.push_back(faces[firstVertex]);
					faces.push_back(faces[faces.size() - 2]);
				}

				//cout << position << " / " << texCoord << " / " << normal << endl;

				if (flipTriangles)
					faces.insert(faces.begin(), face);
				else
					faces.push_back(face);
				faceVertex++;
			}
		}
		else if (lineType == "o")
		{
		}
		else if (lineType == "s")
		{
		}
	}

	Array<VertexPosNorm> vertices;
	Array<unsigned int> indices;

	for (unsigned int i = 0; i < faces.size(); i++)
	{
		VertexPosNorm vertex;
		vertex.position = positions[faces[i].position];
		vertex.normal = normals[faces[i].normal];
		vertex.normal.Normalize();
		vertices.push_back(vertex);
		indices.push_back(i);
	}


	Mesh* mesh = new Mesh();
	mesh->GetVertexData()->BufferVertices((int) vertices.size(), vertices.data());
	mesh->GetIndexData()->BufferIndices((int) indices.size(), indices.data());
	mesh->SetIndices(0, indices.size());
	outMesh = mesh;
	return CMG_ERROR_SUCCESS;
}

Error Mesh::LoadCMG(const Path& path, Mesh*& outMesh)
{
	File file(path);
	Error error = file.Open(FileAccess::READ, FileType::BINARY);
	if (error.Failed())
		return error.Uncheck();
	return DecodeCMG(file, outMesh);
}

Error Mesh::SaveCMG(const Path& path, const Mesh* mesh)
{
	File file(path);
	Error error = file.Open(FileAccess::WRITE, FileType::BINARY);
	if (error.Failed())
		return error.Uncheck();
	return EncodeCMG(file, mesh);
}

Error Mesh::EncodeCMG(File& file, const Mesh* mesh)
{
	CMGMeshFile contents;
	auto vertexData = mesh->GetVertexData();
	auto vertexBuffer = vertexData->GetVertexBuffer();
	auto indexData = mesh->GetIndexData();
	auto indexBuffer = indexData->GetIndexBuffer();

	// Header
	contents.header.version = 1;
	contents.header.primitiveType = (uint32) mesh->GetPrimitiveType();
	contents.header.vertexAttributeFlags = (uint32) vertexBuffer->GetAttributeFlags();
	contents.header.vertexCount = (uint32) vertexData->GetCount();
	contents.header.indexCount = (uint32) indexData->GetCount();
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
		(vertexData->GetStart() * contents.header.vertexSize),
		contents.header.vertexCount * contents.header.vertexSize);
	vertexBuffer->UnmapBufferData();
	contents.vertexData = nullptr;

	// Index data
	contents.indexData = (uint8*) indexBuffer->MapBufferDataRead<uint8>();
	file.Write(contents.indexData +
		(indexData->GetStart() * contents.header.indexSize),
		contents.header.indexCount * contents.header.indexSize);
	indexBuffer->UnmapBufferData();
	contents.indexData = nullptr;

	return CMG_ERROR_SUCCESS;
}

Error Mesh::DecodeCMG(File& file, Mesh*& outMesh)
{
	CMGMeshFile contents;

	// Read the header
	uint32 offset = file.Tell();
	file.Read(&contents.header, sizeof(contents.header));
	if (memcmp(contents.header.magic, CMGMeshFile::MAGIC, 8) != 0)
		return CMG_ERROR(CommonErrorTypes::k_file_corrupt);

	outMesh = new Mesh();

	// Read the vertex data
	file.SeekFromStart(offset + contents.header.vertexStartOffset);
	contents.vertexData = new uint8[
		contents.header.vertexSize * contents.header.vertexCount];
	file.Read(contents.vertexData, contents.header.vertexSize *
		contents.header.vertexCount);
	outMesh->GetVertexData()->BufferVertices(
		contents.header.vertexAttributeFlags, contents.header.vertexCount, 
		contents.vertexData);
	delete [] contents.vertexData;

	// Read the index data
	file.SeekFromStart(offset + contents.header.indexStartOffset);
	contents.indexData = new uint8[
		contents.header.indexSize * contents.header.indexCount];
	file.Read(contents.indexData, contents.header.indexSize *
		contents.header.indexCount);
	outMesh->GetIndexData()->BufferIndices(
		contents.header.indexCount, (uint32*) contents.indexData);
	delete [] contents.indexData;

	return CMG_ERROR_SUCCESS;
}
