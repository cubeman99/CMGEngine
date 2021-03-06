#ifndef _CMG_GRAPHICS_MODEL_H_
#define _CMG_GRAPHICS_MODEL_H_

#include <cmgGraphics/cmg_graphics.h>


// A model made up of multiple mesh/material pairs and can optionally include
// a skeleton
class Model : public cmg::ResourceImpl<Model>
{
public:
	using sptr = cmg::shared_ptr<Model>;

	Model();
	~Model();

	bool HasSkeleton() const;
	Skeleton& GetSkeleton();
	const Skeleton& GetSkeleton() const;
	uint32 GetMeshCount() const;
	const Mesh* GetMesh(uint32 index) const;
	const Material* GetMaterial(uint32 index) const;
	Mesh* GetMesh(uint32 index);
	Material* GetMaterial(uint32 index);

	void SetMeshCount(uint32 count);
	void SetMesh(uint32 index, Mesh* mesh);
	void SetMaterial(uint32 index, Material* material);
	void SetSkeleton(const Skeleton& skeleton);

	static Error LoadModel(const Path& path, Model*& outModel);
	static Error SaveModel(const Path& path, const Model* model);
	Error Decode(File& file);
	Error Encode(File& file) const;

protected:
	virtual Error UnloadImpl();
	virtual Error LoadImpl();

private:
	Skeleton m_skeleton;
	Array<Mesh*> m_meshes;
	Array<Material*> m_materials;
};


#endif // _CMG_GRAPHICS_MODEL_H_