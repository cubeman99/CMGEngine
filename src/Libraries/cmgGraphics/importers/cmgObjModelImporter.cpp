#include "cmgObjModelImporter.h"
#include <sstream>
#include <fstream>

namespace cmg {

ObjModelImporter::ObjModelImporter(ResourceLoader<Texture>& textureLoader) :
	m_textureLoader(textureLoader)
{

}

ObjFaceVertex ObjModelImporter::ParseFaceVertex(const String& str)
{
	std::stringstream ss(str);
	String token;
	ObjFaceVertex faceVertex;
	faceVertex.normal = 0;
	faceVertex.texCoord = 0;
	int index = 0;

	while (std::getline(ss, token, '/'))
	{
		if (token == "")
		{
			faceVertex.indices[index] = 0;
		}
		else
		{
			int number = atoi(token.c_str());
			faceVertex.indices[index] = number;
		}
		index++;
	}

	return faceVertex;
}

void ObjModelImporter::AddVertex(ObjMesh* mesh, const ObjFaceVertex& faceVertex)
{
	// Append a new vertex
	VertexPosTexNorm vertex;
	if (faceVertex.position > 0)
		vertex.position = m_positions[faceVertex.position - 1];
	else if (faceVertex.position < 0)
		vertex.position = m_positions[m_positions.size() - faceVertex.position];
	if (faceVertex.texCoord > 0)
		vertex.texCoord = m_texCoords[faceVertex.texCoord - 1];
	else if (faceVertex.texCoord < 0)
		vertex.texCoord = m_texCoords[m_texCoords.size() - faceVertex.texCoord];
	if (faceVertex.normal > 0)
		vertex.normal = m_normals[faceVertex.normal - 1];
	else if (faceVertex.normal < 0)
		vertex.normal = m_normals[m_normals.size() - faceVertex.normal];
	mesh->indices.push_back((uint32) mesh->vertices.size());
	mesh->vertices.push_back(vertex);
}

void ObjModelImporter::CalcNormals(ObjMesh* objMesh)
{
	Vector3f normal;
	for (uint32 i = 0; i < objMesh->vertices.size() - 2; i += 3)
	{
		VertexPosTexNorm& a = objMesh->vertices[i];
		VertexPosTexNorm& b = objMesh->vertices[i + 1];
		VertexPosTexNorm& c = objMesh->vertices[i + 2];
		normal = Vector3f::Cross(c.position - b.position,
			a.position - b.position);
		normal.Normalize();
		a.normal = normal;
		b.normal = normal;
		c.normal = normal;
	}
}

ObjMesh* ObjModelImporter::BeginNewMesh(const String& name)
{
	ObjMesh* mesh = new ObjMesh();
	mesh->material = name;
	m_objModel.meshes[name] = mesh;
	return mesh;
}

void ObjModelImporter::CompleteMesh(ObjMesh* objMesh)
{
	if (objMesh == nullptr || objMesh->vertices.empty())
		return;

	CalcNormals(objMesh);

	Mesh* mesh = new Mesh();
	mesh->GetVertexData()->BufferVertices(objMesh->vertices);
	mesh->GetIndexData()->BufferIndices(objMesh->indices);

	Material* material = nullptr;
	auto it = m_objModel.materials.find(objMesh->material);
	if (it != m_objModel.materials.end())
	{
		ObjMaterial* objMaterial = it->second;
		material = new Material();
		material->SetUniform("u_color",
			Vector4f(objMaterial->diffuseColor, 1.0f));
		material->SetUniform("s_diffuse", objMaterial->diffuseMap);
	}

	uint32 meshIndex = m_model->GetMeshCount();
	m_model->SetMeshCount(meshIndex + 1);
	m_model->SetMesh(meshIndex, mesh);
	m_model->SetMaterial(meshIndex, material);
}


Error ObjModelImporter::ImportModel(const Path& path, Model*& outModel)
{
	// Open the file
	std::ifstream file;
	file.open(path.ToString());
	if (!file.is_open())
		return CMG_ERROR_FAILURE;

	if (outModel == nullptr)
		outModel = new Model();
	m_model = outModel;

	m_objModel.materials.clear();
	m_objModel.meshes.clear();
	//BeginNewObject();

	Vector3f v;
	ObjFace face;
	String line;
	String token;
	ObjMaterial* currentMaterial = nullptr;
	ObjMesh* currentMesh = nullptr;
	String value;

	while (std::getline(file, line))
	{
		// Ignore comments
		if (line[0] == '#')
			continue;

		std::stringstream ss(line);
		token = "";
		ss >> token;

		if (token == "mtllib")
		{
			String mtllibPath;
			ss >> mtllibPath;
			LoadMaterials(path.GetParent() / mtllibPath);
		}
		else if (token == "v")
		{
			ss >> v.x; ss >> v.y; ss >> v.z;
			m_positions.push_back(v);
		}
		else if (token == "vn")
		{
			ss >> v.x; ss >> v.y; ss >> v.z;
			m_normals.push_back(v);
		}
		else if (token == "vt")
		{
			ss >> v.x; ss >> v.y;
			if (v.x < 0.0f)
				v.x = -v.x;
			if (v.y < 0.0f)
				v.y = -v.y;
			m_texCoords.push_back(v.xy);
		}
		else if (token == "g")
		{
		}
		else if (token == "usemtl")
		{
			ss >> value;
			CompleteMesh(currentMesh);
			currentMesh = BeginNewMesh(value);
			currentMaterial = m_objModel.materials.at(value);
		}
		else if (token == "f")
		{
			uint32 index = 0;
			while (ss >> token)
			{
				if (index >= 3)
				{
					AddVertex(currentMesh, face.vertices[0]);
					AddVertex(currentMesh, face.vertices[2]);
				}

				ObjFaceVertex faceVertex = ParseFaceVertex(token);
				face.vertices[Math::Min(index, 2u)] = faceVertex;
				AddVertex(currentMesh, faceVertex);

				index++;
			}
		}
	}

	CompleteMesh(currentMesh);
	return CMG_ERROR_SUCCESS;
}

// Load an OBJ model's list of materials from an MTL file.
Error ObjModelImporter::LoadMaterials(const Path& mtlFilePath)
{
	// Open the file
	std::ifstream file;
	file.open(mtlFilePath.ToString());
	if (!file.is_open())
		return CMG_ERROR_FAILURE;

	Vector3f v;
	String line;
	String token;
	String value;
	ObjMaterial* material;

	while (std::getline(file, line))
	{
		// Ignore comments
		if (line[0] == '#')
			continue;

		std::stringstream ss(line);
		token = "";
		ss >> token;

		// Parse the tokens
		if (token == "newmtl")
		{
			String name;
			ss >> name;
			material = GetOrCreateMaterial(name);
		}
		else if (token == "Kd")
		{
			ss >> v.x; ss >> v.y; ss >> v.z;
			material->diffuseColor = v;
		}
		else if (token == "Ka")
		{
			ss >> v.x; ss >> v.y; ss >> v.z;
			material->ambientColor = v;
		}
		else if (token == "Ks")
		{
			ss >> v.x; ss >> v.y; ss >> v.z;
			material->specularColor = v;
		}
		else if (token == "illum")
		{
			ss >> material->illuminationModel;
		}
		else if (token == "Ns")
		{
			ss >> material->specularExponent;
		}
		else if (token == "d")
		{
			ss >> material->opacity;
		}
		else if (token == "Tr")
		{
			ss >> material->opacity;
			material->opacity = 1.0f - material->opacity;
		}
		else if (token == "map_Kd")
		{
			ss >> value;
			Path texturePath = mtlFilePath.GetParent() / value;
			material->diffuseMap = m_textureLoader.Load(texturePath);
		}
	}
	
	return CMG_ERROR_SUCCESS;
}

ObjMaterial* ObjModelImporter::GetOrCreateMaterial(const String& name)
{
	// First, search the for the material
	auto it = m_objModel.materials.find(name);
	if (it == m_objModel.materials.end())
	{
		// Initialize a new material
		ObjMaterial* material = new ObjMaterial();
		material->name = name;
		material->opacity = 1.0f;
		material->diffuseColor = Vector3f(1.0f, 1.0f, 1.0f);
		material->ambientColor = Vector3f(0.0f);
		material->specularColor = Vector3f(0.0f);
		material->specularExponent = 1.0f;
		material->illuminationModel = 2;
		m_objModel.materials[name] = material;
	}
	return m_objModel.materials.at(name);
}

ObjMesh* ObjModelImporter::GetOrCreateMesh(const String& name)
{
	auto it = m_objModel.meshes.find(name);
	if (it == m_objModel.meshes.end())
	{
		m_objModel.meshes[name] = new ObjMesh();
		m_objModel.meshes[name]->material = name;
	}
	return m_objModel.meshes.at(name);
}

//
//
////-----------------------------------------------------------------------------
//// Creation and deletion
////-----------------------------------------------------------------------------
//
//// Initialize an OBJ model.
//void objModelInit(ObjModel* model)
//{
//	strcpy(model->name, "");
//	strcpy(model->objPath, "");
//	strcpy(model->mtlPath, "");
//
//	model->numMeshes = 0;
//	model->meshes = NULL;
//	model->numMaterials = 0;
//	model->materials = NULL;
//}
//
//// Destroy an OBJ model's internal data.
//void objModelDestroy(ObjModel* model)
//{
//	model->numMeshes = 0;
//	free(model->meshes);
//	model->meshes = NULL;
//	model->numMaterials = 0;
//	free(model->materials);
//	model->materials = NULL;
//}
//
//
////-----------------------------------------------------------------------------
//// Loading from OBJ file
////-----------------------------------------------------------------------------
//
//static int readFaceIndex(uint32* outFaceIndex, uint32* firstChar, char* str)
//{
//	char* indexStr = (str + *firstChar);
//
//	if (indexStr[0] == '\0')
//		return 0;
//	
//	// Find the index of the next slash.
//	uint32 length = strcspn(indexStr, "/");
//	if (length == 0)
//	{
//		*firstChar += 1;
//		*outFaceIndex = 0;
//		return 1;
//	}
//
//	// Convert the index string to a number.
//	char* endPtr;
//	*firstChar += length;
//	if (indexStr[length] != '\0')
//		*firstChar += 1;
//	*outFaceIndex = strtoul(indexStr, &endPtr, 10);
//	if (endPtr != indexStr + length)
//		return 0;
//	return 1;
//}
//
//// Load an OBJ model from file, with an optional normal load mode.
//int objModelLoad(ObjModel* model, const char* fileName,
//	ObjMeshNormalMode normalMode)
//{
//	// Open the file.
//	FILE* file = fopen(fileName, "r");
//	if (file == NULL)
//	{
//		perror("Error opening OBJ file");
//		return 0;
//	}
//	
//	objModelInit(model);
//	
//	char directoryPath[256];
//	fileGetDirectoryPath(directoryPath, fileName);
//
//	const size_t maxLineSize = 300;
//	char line[maxLineSize];
//	char* token;
//	char* id;
//	const uint32 maxTokens = 10;
//	uint32 numTokens = 0;
//	char* tokens[maxTokens];
//	char* endptr;
//	uint32 i, j;
//	uint32 faceIndex;
//	uint32 firstChar;
//	char path[256];
//	Vector3f v3;
//	Vector2f v2;
//	ObjFaceVertex faceVertex;
//	ObjTriFace face;
//
//	char mtlFileName[256] = { 0 };
//	char usemtlName[256] = { 0 };
//
//	Array<Vector3f> positions;
//	Array<Vector2f> texCoords;
//	Array<Vector3f> normals;
//	//Array<ObjTriFace> faces;
//
//	// Current loading state:
//	ObjMaterial* currentMaterial = NULL;
//	ObjMesh* currentMesh = NULL;
//	int smoothingGroup = 0;
//
//	// Read the file line by line.
//	while (fgets(line, maxLineSize, file) != NULL) 
//	{
//		// Ignore comments.
//		if (line[0] == '#')
//			continue;
//
//		// Remove newline character.
//		uint32 lineLength = strlen(line);
//		if (lineLength == 0)
//			continue;
//		if (line[lineLength - 1] == '\n')
//			line[lineLength - 1] = '\0';
//
//		// Read the first token.
//		id = strtok(line, " ");
//		if (id == NULL)
//			continue;
//
//		// Read the remaining tokens.
//		numTokens = 0;
//		while ((token = strtok(NULL, " ")) != NULL &&
//			numTokens < maxTokens - 1)
//		{
//			tokens[numTokens++] = token;
//		}
//
//		// Parse the tokens
//		if (strcmp(id, "mtllib") == 0 && numTokens >= 1)
//		{
//			strncpy(mtlFileName, tokens[0], sizeof(mtlFileName));
//			strcpy(path, directoryPath);
//			strcat(path, mtlFileName);
//			objModelLoadMaterials(model, path);
//		}
//		else if (strcmp(id, "usemtl") == 0 && numTokens >= 1)
//		{
//			strncpy(mtlFileName, tokens[0], sizeof(mtlFileName));
//			currentMaterial = objModelFindMaterial(model, tokens[0]);
//			currentMesh = objModelGetOrCreateMesh(model, tokens[0]);
//		}
//		else if (strcmp(id, "s") == 0 && numTokens >= 1)
//		{
//			// smoothing group.
//			if (strcmp(tokens[0], "off") == 0)
//				smoothingGroup = -1;
//			else
//				smoothingGroup = strtol(tokens[0], NULL, 10);
//		}
//		else if (strcmp(id, "o") == 0)
//		{
//			// o [object name]
//		}
//		else if (strcmp(id, "g") == 0)
//		{
//			// g [group name]
//		}
//		else if (strcmp(id, "v") == 0 && numTokens == 3)
//		{
//			// Vertex position
//			v3.x = strtof(tokens[0], &endptr);
//			v3.y = strtof(tokens[1], &endptr);
//			v3.z = strtof(tokens[2], &endptr);
//			positions.push_back(v3);
//			if (normalMode == ObjMeshNormalMode::NORMAL_COMPUTE_VERTEX)
//				normals.push_back(Vector3f::ZERO);
//		}
//		else if (strcmp(id, "vt") == 0 && numTokens >= 2)
//		{
//			// Vertex texture coordinate
//			v2.x = strtof(tokens[0], &endptr);
//			v2.y = strtof(tokens[1], &endptr);
//			texCoords.push_back(v2);
//		}
//		else if (strcmp(id, "vn") == 0 && normalMode == ObjMeshNormalMode::NORMAL_LOAD)
//		{
//			// Vertex normal
//			v3.x = strtof(tokens[0], &endptr);
//			v3.y = strtof(tokens[1], &endptr);
//			v3.z = strtof(tokens[2], &endptr);
//			normals.push_back(v3);
//		}
//		else if (strcmp(id, "f") == 0)
//		{
//			// Polygonal face
//			face = ObjTriFace();
//			
//			uint32 faceNormalIndex = normals.size() + 1;
//			Vector3f faceNormal;
//			
//			Array<ObjTriFace>* meshFaces = (Array<ObjTriFace>*) currentMesh->data;
//
//			// Read each vertex in the face.
//			for (i = 0; i < numTokens; ++i)
//			{
//				faceVertex = ObjFaceVertex();
//				firstChar = 0;
//				j = 0;
//				while (readFaceIndex(&faceIndex, &firstChar, tokens[i]) && j < 3)
//				{
//					faceVertex.indices[j++] = faceIndex;
//				}
//				
//				// Handle negative indices.
//				if (faceVertex.position < 0)
//					faceVertex.position = positions.size() + faceVertex.position + 1;
//				if (faceVertex.texCoord < 0)
//					faceVertex.texCoord = texCoords.size() + faceVertex.texCoord + 1;
//				if (faceVertex.normal < 0)
//					faceVertex.normal = normals.size() + faceVertex.normal + 1;
//				
//				// Override normal index.
//				if (normalMode == ObjMeshNormalMode::NORMAL_COMPUTE_FACE)
//					faceVertex.normal = faceNormalIndex;
//				else if (normalMode == ObjMeshNormalMode::NORMAL_COMPUTE_VERTEX)
//					faceVertex.normal = faceVertex.position;
//
//				if (i < 3)
//					face.vertices[i] = faceVertex;
//
//				if (i == 2)
//				{
//					// Compute the normal for this face.
//					if (normalMode == ObjMeshNormalMode::NORMAL_COMPUTE_FACE ||
//						normalMode == ObjMeshNormalMode::NORMAL_COMPUTE_VERTEX)
//					{
//						faceNormal = Vector3f::Normalize(Vector3f::Cross(
//							positions[face.vertices[2].position - 1] - positions[face.vertices[1].position - 1],
//							positions[face.vertices[0].position - 1] - positions[face.vertices[1].position - 1]));
//					}
//					if (normalMode == ObjMeshNormalMode::NORMAL_COMPUTE_FACE)
//					{
//						normals.push_back(faceNormal);
//					}
//					else if (normalMode == ObjMeshNormalMode::NORMAL_COMPUTE_VERTEX)
//					{
//						normals[face.vertices[0].normal - 1] += faceNormal;
//						normals[face.vertices[1].normal - 1] += faceNormal;
//						normals[face.vertices[2].normal - 1] += faceNormal;
//					}
//
//					meshFaces->push_back(face);
//				}
//				else if (i >= 3)
//				{
//					// Triangulate faces.
//					face.vertices[1] = face.vertices[2];
//					face.vertices[2] = faceVertex;
//					if (normalMode == ObjMeshNormalMode::NORMAL_COMPUTE_VERTEX)
//						normals[faceVertex.normal - 1] += faceNormal;
//					meshFaces->push_back(face);
//				}
//			}
//		}
//	}
//
//	//-------------------------------------------------------------------------
//	// Post-processing.
//
//	// Normalize per-vertex computed normals.
//	if (normalMode == ObjMeshNormalMode::NORMAL_COMPUTE_VERTEX)
//	{
//		for (uint32 i = 0; i < normals.size(); ++i)
//			normals[i].Normalize();
//	}
//	
//	//-------------------------------------------------------------------------
//	// Put vertex data into OBJ data.
//
//	for (i = 0; i < model->numMeshes; ++i)
//	{
//		/*
//		currentMesh = model->meshes + i;
//
//		Array<ObjTriFace>* meshFaces = (Array<ObjTriFace>*) currentMesh->data;
//
//		// Set the first number in data to the number of vertices.
//		currentMesh->numVertices = meshFaces->size() * 3;
//		uint32 bytesPerVertex = sizeof(Vector3f) + sizeof(Vector2f) + sizeof(Vector3f);
//		//currentMesh->data = (char*) malloc(currentMesh->numVertices * bytesPerVertex);
//
//		// Put vertex data into OBJ data.
//		currentMesh->positions = (Vector3f*) malloc(currentMesh->numVertices * bytesPerVertex);
//		currentMesh->texCoords = (Vector2f*) (currentMesh->positions + currentMesh->numVertices);
//		currentMesh->normals   = (Vector3f*) (currentMesh->texCoords + currentMesh->numVertices);
//	
//		// Calculate bounding box along the way.
//		currentMesh->boundingBox.mins.set(FLT_MAX);
//		currentMesh->boundingBox.maxs.set(-FLT_MAX);
//
//		uint32 index = 0;
//		for (uint32 i = 0; i < meshFaces->size(); ++i)
//		{
//			for (uint32 j = 0; j < 3; ++j, ++index)
//			{
//				faceVertex = (*meshFaces)[i].vertices[j];
//				if (faceVertex.position > 0)
//				{
//					currentMesh->positions[index] = positions[faceVertex.position - 1];
//
//					currentMesh->boundingBox.mins.x = minVal(currentMesh->boundingBox.mins.x, currentMesh->positions[index].x);
//					currentMesh->boundingBox.mins.y = minVal(currentMesh->boundingBox.mins.y, currentMesh->positions[index].y);
//					currentMesh->boundingBox.mins.z = minVal(currentMesh->boundingBox.mins.z, currentMesh->positions[index].z);
//					currentMesh->boundingBox.maxs.x = maxVal(currentMesh->boundingBox.maxs.x, currentMesh->positions[index].x);
//					currentMesh->boundingBox.maxs.y = maxVal(currentMesh->boundingBox.maxs.y, currentMesh->positions[index].y);
//					currentMesh->boundingBox.maxs.z = maxVal(currentMesh->boundingBox.maxs.z, currentMesh->positions[index].z);
//				}
//				if (faceVertex.texCoord > 0)
//					currentMesh->texCoords[index] = texCoords[faceVertex.texCoord - 1];
//				if (faceVertex.normal > 0)
//					currentMesh->normals[index] = normals[faceVertex.normal - 1];
//			}
//		}
//	
//		currentMesh->boundingBox.center = (currentMesh->boundingBox.maxs +
//			currentMesh->boundingBox.mins) * 0.5f;
//		currentMesh->boundingBox.halfSize = (currentMesh->boundingBox.maxs -
//			currentMesh->boundingBox.mins) * 0.5f;
//
//		free(currentMesh->data);
//		currentMesh->data = currentMesh->positions;
//		*/
//	}
//
//	fclose(file);
//	return 1;
//}
//
//// Load an OBJ model's list of materials from an MTL file.
//int objModelLoadMaterials(ObjModel* model, const char* mtlFileName)
//{
//	// Open the file.
//	FILE* file = fopen(mtlFileName, "r");
//	if (file == NULL)
//	{
//		perror("Error opening MTL file");
//		return 0;
//	}
//	
//	const size_t maxLineSize = 300;
//	char line[maxLineSize];
//	char* token;
//	char* id;
//	const uint32 maxTokens = 10;
//	uint32 numTokens = 0;
//	char* tokens[maxTokens];
//	char* endPtr;
//
//	ObjMaterial* material = NULL;
//
//	// Read the file line by line.
//	while (fgets(line, maxLineSize, file) != NULL) 
//	{
//		// Ignore comments.
//		if (line[0] == '#')
//			continue;
//
//		// Remove newline character.
//		uint32 lineLength = strlen(line);
//		if (lineLength == 0)
//			continue;
//		if (line[lineLength - 1] == '\n')
//			line[lineLength - 1] = '\0';
//
//		// Read the first token.
//		id = strtok(line, " ");
//		if (id == NULL)
//			continue;
//		
//		// Read the remaining tokens.
//		numTokens = 0;
//		while ((token = strtok(NULL, " ")) != NULL &&
//			numTokens < maxTokens - 1)
//		{
//			tokens[numTokens++] = token;
//		}
//		
//		// Parse the tokens
//		if (strcmp(id, "newmtl") == 0 && numTokens >= 1)
//		{
//			material = objModelGetOrCreateMaterial(model, tokens[0]);
//		}
//		else if (material == NULL)
//		{
//			// error?
//		}
//		else if (strcmp(id, "Kd") == 0 && numTokens == 3)
//		{
//			material->diffuseColor.x = strtof(tokens[0], &endPtr);
//			material->diffuseColor.y = strtof(tokens[1], &endPtr);
//			material->diffuseColor.z = strtof(tokens[2], &endPtr);
//		}
//		else if (strcmp(id, "Ka") == 0 && numTokens == 3)
//		{
//			material->ambientColor.x = strtof(tokens[0], &endPtr);
//			material->ambientColor.y = strtof(tokens[1], &endPtr);
//			material->ambientColor.z = strtof(tokens[2], &endPtr);
//		}
//		else if (strcmp(id, "Ks") == 0 && numTokens == 3)
//		{
//			material->specularColor.x = strtof(tokens[0], &endPtr);
//			material->specularColor.y = strtof(tokens[1], &endPtr);
//			material->specularColor.z = strtof(tokens[2], &endPtr);
//		}
//		else if (strcmp(id, "illum") == 0 && numTokens == 1)
//		{
//			material->illuminationModel = (uint32) strtoul(tokens[0], &endPtr, 10);
//		}
//		else if (strcmp(id, "Ns") == 0 && numTokens == 1)
//		{
//			material->specularExponent = strtof(tokens[0], &endPtr);
//		}
//		else if (strcmp(id, "d") == 0 && numTokens == 1)
//		{
//			material->opacity = strtof(tokens[0], &endPtr);
//		}
//		else if (strcmp(id, "Tr") == 0 && numTokens == 1)
//		{
//			material->opacity = 1.0f - strtof(tokens[0], &endPtr);
//		}
//		else if (strcmp(id, "map_Kd") == 0 && numTokens > 0)
//		{
//			strcpy(material->diffuseMap, tokens[0]);
//		}
//		// TODO: more texture maps
//	}
//	
//	// Close the file.
//	fclose(file);
//	return 1;
//}
//
//
//ObjMaterial* objModelFindMaterial(ObjModel* model, const char* name)
//{
//	uint32 i;
//	for (i = 0; i < model->numMaterials; ++i)
//	{
//		if (strcmp(model->materials[i].name, name) == 0)
//			return model->materials + i;
//	}
//	return NULL;
//}
//
//static void initDefaultMaterial(ObjMaterial* material)
//{
//	*material = ObjMaterial();
//	material->opacity = 1.0f;
//	material->diffuseColor = Vector3f(1.0f, 1.0f, 1.0f);
//	material->ambientColor = Vector3f(0.0f);
//	material->specularColor = Vector3f(0.0f);
//	material->specularExponent = 1.0f;
//	material->illuminationModel = 2;
//}
//
//ObjMaterial* objModelGetOrCreateMaterial(ObjModel* model, const char* name)
//{
//	// First, search the linked list for the material.
//	ObjMaterial* mat = objModelFindMaterial(model, name);
//	if (mat != NULL)
//		return mat;
//
//	// Reallocate the array.
//	uint32 newNumMaterials = model->numMaterials + 1;
//	ObjMaterial* newMaterials = (ObjMaterial*) malloc(newNumMaterials * sizeof(ObjMaterial));
//	memcpy(newMaterials, model->materials, model->numMaterials * sizeof(ObjMaterial));
//	free(model->materials);
//	model->materials = newMaterials;
//	model->numMaterials = newNumMaterials;
//
//	// Initialize the new material.
//	mat = model->materials + (newNumMaterials - 1);
//	initDefaultMaterial(mat);
//	strncpy(mat->name, name, sizeof(mat->name));
//	return mat;
//}
//
//ObjMesh* objModelFindMesh(ObjModel* model, const char* name)
//{
//	uint32 i;
//	for (i = 0; i < model->numMeshes; ++i)
//	{
//		if (model->meshes[i].material >= 0 &&
//			strcmp(model->materials[model->meshes[i].material].name, name) == 0)
//			return model->meshes + i;
//	}
//	return NULL;
//}
//
//ObjMesh* objModelGetOrCreateMesh(ObjModel* model, const char* name)
//{
//	// First, search the linked list for the mesh.
//	ObjMesh* mesh = objModelFindMesh(model, name);
//	if (mesh != NULL)
//		return mesh;
//
//	// Reallocate the array.
//	uint32 newNumMeshes = model->numMeshes + 1;
//	ObjMesh* newMeshes = (ObjMesh*) malloc(newNumMeshes * sizeof(ObjMesh));
//	memcpy(newMeshes, model->meshes, model->numMeshes * sizeof(ObjMesh));
//	free(model->meshes);
//	model->meshes = newMeshes;
//	model->numMeshes = newNumMeshes;
//
//	// Initialize the new mesh.
//	mesh = model->meshes + (newNumMeshes - 1);
//	*mesh = ObjMesh();
//	//mesh->data = new Array<ObjTriFace>;
//
//	ObjMaterial* mat = objModelFindMaterial(model, name);
//	mesh->material = (mat ? (uint32) (mat - model->materials) : -1);
//	return mesh;
//}
//
//
////-----------------------------------------------------------------------------
//// Binary loading and saving
////-----------------------------------------------------------------------------
//
//int objModelLoadBinary(ObjModel* model, const char* fileName)
//{
//	// Open the file.
//	FILE* file = fopen(fileName, "rb");
//	if (file == NULL)
//	{
//		perror("Error opening binary OBJ file");
//		return 0;
//	}
//	
//	ObjMesh* mesh;
//	ObjMaterial* material;
//	uint32 i;
//
//	// Read the number of meshes and materials.
//	fread(&model->numMeshes, sizeof(uint32), 1, file);
//	fread(&model->numMaterials, sizeof(uint32), 1, file);
//	model->meshes = (ObjMesh*) malloc(model->numMeshes * sizeof(ObjMesh));
//	model->materials = (ObjMaterial*) malloc(model->numMaterials * sizeof(ObjMaterial));
//	
//	// Read meshes.
//	for (i = 0; i < model->numMeshes; ++i)
//	{
//		mesh = model->meshes + i;
//		fread(&mesh->material, sizeof(int), 1, file);
//		fread(&mesh->boundingBox, sizeof(BoundingBox), 1, file);
//		fread(&mesh->numVertices, sizeof(uint32), 1, file);
//		mesh->positions = (Vector3f*) malloc(mesh->numVertices * sizeof(Vector3f));
//		mesh->texCoords = (Vector2f*) malloc(mesh->numVertices * sizeof(Vector2f));
//		mesh->normals = (Vector3f*) malloc(mesh->numVertices * sizeof(Vector3f));
//		fread(mesh->positions, sizeof(Vector3f), mesh->numVertices, file);
//		fread(mesh->texCoords, sizeof(Vector2f), mesh->numVertices, file);
//		fread(mesh->normals, sizeof(Vector3f), mesh->numVertices, file);
//	}
//	
//	// Read materials.
//	for (i = 0; i < model->numMaterials; ++i)
//	{
//		material = model->materials + i;
//		fread(material, sizeof(ObjMaterial), 1, file);
//		//fwrite(&material->ambientColor, sizeof(Vector3f), 1, file);
//		//fwrite(&material->diffuseColor, sizeof(Vector3f), 1, file);
//		//fwrite(&material->specularColor, sizeof(Vector3f), 1, file);
//		//fwrite(&material->specularExponent, sizeof(float), 1, file);
//		//fwrite(&material->opacity, sizeof(float), 1, file);
//		//fwrite(&material->illuminationModel, sizeof(uint32), 1, file);
//	}
//
//
//	fclose(file);
//	return 1;
//}
//
//int objModelSaveBinary(const ObjModel* model, const char* fileName)
//{
//	// Open the file.
//	FILE* file = fopen(fileName, "wb");
//	if (file == NULL)
//	{
//		perror("Error saving binary OBJ file");
//		return 0;
//	}
//
//	const ObjMesh* mesh;
//	const ObjMaterial* material;
//	uint32 i;
//
//	// Write the number of meshes and materials.
//	fwrite(&model->numMeshes, sizeof(uint32), 1, file);
//	fwrite(&model->numMaterials, sizeof(uint32), 1, file);
//
//	// Write meshes.
//	for (i = 0; i < model->numMeshes; ++i)
//	{
//		mesh = model->meshes + i;
//		fwrite(&mesh->material, sizeof(int), 1, file);
//		fwrite(&mesh->boundingBox, sizeof(BoundingBox), 1, file);
//		fwrite(&mesh->numVertices, sizeof(uint32), 1, file);
//		fwrite(mesh->positions, sizeof(Vector3f), mesh->numVertices, file);
//		fwrite(mesh->texCoords, sizeof(Vector2f), mesh->numVertices, file);
//		fwrite(mesh->normals, sizeof(Vector3f), mesh->numVertices, file);
//	}
//	
//	// Write materials.
//	for (i = 0; i < model->numMaterials; ++i)
//	{
//		material = model->materials + i;
//		fwrite(material, sizeof(ObjMaterial), 1, file);
//		//fwrite(&material->ambientColor, sizeof(Vector3f), 1, file);
//		//fwrite(&material->diffuseColor, sizeof(Vector3f), 1, file);
//		//fwrite(&material->specularColor, sizeof(Vector3f), 1, file);
//		//fwrite(&material->specularExponent, sizeof(float), 1, file);
//		//fwrite(&material->opacity, sizeof(float), 1, file);
//		//fwrite(&material->illuminationModel, sizeof(uint32), 1, file);
//	}
//
//	fclose(file);
//	return 1;
//}

}
