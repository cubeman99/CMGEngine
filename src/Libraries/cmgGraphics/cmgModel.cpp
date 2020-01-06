#include "cmgModel.h"
#include <cmgGraphics/importers/cmgSourceModelImporter.h>
#include <cmgGraphics/importers/cmgObjModelImporter.h>
#include <cmgCore/resource/cmgResourceLoader.h>
#include <cmgGraphics/cmgTexture.h>


struct CMGModelFile
{
	static constexpr char MAGIC[] = "CMGModel";

	// Header (64 bytes)
	struct Header
	{
		char magic[8] = {'C', 'M', 'G', 'M', 'o', 'd', 'e', 'l'};
		uint32 version = 1;
		uint32 meshCount = 0;
		uint8 hasSkeleton = 0;
	} header;
};


Model::Model()
{
}

Model::~Model()
{
	SetMeshCount(0);
}

bool Model::HasSkeleton() const
{
	return (m_skeleton.GetNumJoints() > 0);
}

Skeleton& Model::GetSkeleton()
{
	return m_skeleton;
}

const Skeleton& Model::GetSkeleton() const
{
	return m_skeleton;
}

uint32 Model::GetMeshCount() const
{
	return m_meshes.size();
}

const Mesh* Model::GetMesh(uint32 index) const
{
	return m_meshes[index];
}

const Material* Model::GetMaterial(uint32 index) const
{
	return m_materials[index];
}

Mesh* Model::GetMesh(uint32 index)
{
	return m_meshes[index];
}

Material* Model::GetMaterial(uint32 index)
{
	return m_materials[index];
}

void Model::SetMeshCount(uint32 count)
{
	uint32 oldSize = m_meshes.size();
	for (uint32 i = count; i < oldSize; i++)
	{
		delete m_meshes[i];
		delete m_materials[i];
	}
	m_meshes.resize(count);
	m_materials.resize(count);
	for (uint32 i = oldSize; i < count; i++)
	{
		m_meshes[i] = nullptr;
		m_materials[i] = nullptr;
	}
}

void Model::SetMesh(uint32 index, Mesh* mesh)
{
	m_meshes[index] = mesh;
}

void Model::SetMaterial(uint32 index, Material* material)
{
	m_materials[index] = material;
}

void Model::SetSkeleton(const Skeleton& skeleton)
{
	m_skeleton = skeleton;
}

Error Model::LoadModel(const Path& path, Model*& outModel)
{
	String extension = cmg::string::ToLower(path.GetExtension());

	if (extension == "smd")
	{
		SourceModelImporter importer;
		return importer.ImportModel(path, outModel);
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
		return outModel->Decode(file);
	}
}

Error Model::SaveModel(const Path& path, const Model* model)
{
	File file(path);
	Error error = file.Open(FileAccess::WRITE, FileType::BINARY);
	if (error.Failed())
		return error.Uncheck();
	return model->Encode(file);
}

Error Model::Decode(File& file)
{
	CMGModelFile contents;

	// Read the header
	file.Read(&contents.header, sizeof(contents.header));
	if (memcmp(contents.header.magic, CMGModelFile::MAGIC, 8) != 0)
		return CMG_ERROR(CommonErrorTypes::k_file_corrupt);

	m_meshes.resize(contents.header.meshCount);
	m_materials.resize(contents.header.meshCount);
	m_skeleton.ClearJoints();

	// Skeleton
	if (contents.header.hasSkeleton == 1)
	{
		Skeleton::Decode(file, m_skeleton);
	}

	// Read the mesh data
	for (uint32 i = 0; i < contents.header.meshCount; i++)
	{
		Mesh*& mesh = m_meshes[i];
		mesh = new Mesh();
		mesh->DecodeCMG(file);
	}

	// Read the material data
	for (uint32 i = 0; i < contents.header.meshCount; i++)
	{
		Material*& material = m_materials[i];
		material = nullptr;
	}

	return CMG_ERROR_SUCCESS;
}

Error Model::Encode(File& file) const
{
	CMGModelFile contents;

	// Header
	contents.header.version = 1;
	contents.header.hasSkeleton = (HasSkeleton() ? 1 : 0);
	contents.header.meshCount = (uint32) m_meshes.size();
	file.Write(&contents.header, sizeof(contents.header));

	// Skeleton
	if (HasSkeleton())
	{
		Skeleton::Encode(file, &m_skeleton);
	}

	// Meshes
	for (uint32 i = 0; i < m_meshes.size(); i++)
	{
		auto mesh = m_meshes[i];
		mesh->EncodeCMG(file);
	}
	for (uint32 i = 0; i < m_meshes.size(); i++)
	{
		auto material = m_materials[i];
	}
	return CMG_ERROR_SUCCESS;
}

Error Model::UnloadImpl()
{
	m_meshes.clear();
	m_materials.clear();
	m_skeleton.Unload();
	return CMG_ERROR_SUCCESS;
}

Error Model::LoadImpl()
{
	Path path = GetResourceLoader()->GetResourcePath(GetResourceName());
	if (!path.FileExists())
		return CMG_ERROR_FILE_NOT_FOUND;

	String extension = cmg::string::ToLower(path.GetExtension());
	Model* model = this;

	if (extension == "smd")
	{
		SourceModelImporter importer;
		return importer.ImportModel(path, model);
	}
	else if (extension == "obj")
	{
		cmg::ObjModelImporter importer(
			*GetResourceLoader()->GetResourceManager()->GetResourcePool<Texture>());
		return importer.ImportModel(path, model);
	}
	else
	{
		// Load a CMG model file
		File file(path);
		Error error = file.Open(FileAccess::READ, FileType::BINARY);
		if (error.Failed())
			return error.Uncheck();
		return Decode(file);
	}

	return CMG_ERROR_SUCCESS;
}

