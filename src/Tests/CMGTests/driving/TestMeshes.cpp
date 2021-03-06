#include "TestMeshes.h"
#include <sstream>


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


Mesh* TestMeshes::LoadMesh(const Path& path)
{
	String fileContents;
	File::OpenAndGetContents(path, fileContents);

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
	return mesh;
}



