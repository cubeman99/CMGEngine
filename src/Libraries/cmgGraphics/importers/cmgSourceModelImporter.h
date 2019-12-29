#ifndef _CMG_GRAPHICS_SOURCE_MODEL_IMPORTER_H_
#define _CMG_GRAPHICS_SOURCE_MODEL_IMPORTER_H_

#include <cmgGraphics/cmgModel.h>
#include <cmgGraphics/cmgAnimationClip.h>

#define SMD_VERTEX_MAX_JOINTS 4

/*
Valve File Formats:
 - Model (compiled from .qc)
 - Model/Animation (.smd)
 - Material (.vmt)
 - Texture (.vtf)
*/

struct SmdJointIndices
{
	uint32 indices[SMD_VERTEX_MAX_JOINTS] = { 0 };
};

struct SmdJointWeights
{
	float weights[SMD_VERTEX_MAX_JOINTS] = { 0.0f };
};

struct SmdVertex
{
	Vector3f position;
	Vector2f texCoord;
	Vector3f normal;
	SmdJointIndices jointIndices;
	SmdJointWeights jointWeights;
};

struct SmdMesh
{
	Array<Vector3f> positions;
	Array<Vector3f> normals;
	Array<Vector2f> texCoords;
	Array<SmdJointIndices> jointIndices;
	Array<SmdJointWeights> jointWeights;
};

struct SmdNode
{
	String name;
	int32 parentIndex = -1;
};

struct SmdNodeTransform
{
	Vector3f translation = Vector3f::ZERO;
	Quaternion rotation = Quaternion::IDENTITY;
};

struct SmdKeyFrame
{
	Array<SmdNodeTransform> poses;
};

struct SmdAnimation
{
	Array<SmdKeyFrame> frames;
};

struct SmdSkeleton
{
	Array<SmdNode> nodes;
};

struct SmdModel
{
	SmdSkeleton skeleton;
	SmdAnimation animation;
	std::map<String, SmdMesh> meshes;
};

class SourceModelImporter
{
public:
	SourceModelImporter();
	~SourceModelImporter();
	
	Error ImportModel(const Path& path, Model*& outModel);
	Error ImportAnimation(const Path& path, AnimationClip*& outAnimation);

	// From Game3D:
	//Model loadModelQC(const FileInfo& fileInfo);
	//Mesh loadMeshSMD(const FileInfo& fileInfo, Array<DirectoryInfo>& materialPaths);
	//AnimationClip loadAnimationSMD(const FileInfo& fileInfo, const Skeleton& skeleton);
	//Material loadMaterialVMT(const FileInfo& fileInfo, const DirectoryInfo& textureDirectory);
	//Texture loadTextureVTF(const FileInfo& fileInfo);

private:
	static Error ReadSkeleton(std::ifstream& file,
		SmdSkeleton& smdSkeleton, Skeleton* skeleton);
	static Error ReadAnimation(std::ifstream& file,
		SmdSkeleton& smdSkeleton, SmdAnimation& smdAnimation,
		Skeleton* skeleton, AnimationClip* animation);
	static Error ReadMeshes(std::ifstream& file,
		SmdModel& smdModel, Model* model);

	static void ReadJoint(std::istringstream& line, SmdSkeleton& skeleton);
	static void ReadNodeTransform(std::istringstream& line, SmdKeyFrame& keyFrame);
	static void ReadVertex(std::istringstream& line, SmdMesh& mesh);
	static String ReadLine(std::ifstream& file, String& line);
};


#endif