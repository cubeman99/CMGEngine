#ifndef _CMG_GRAPHICS_OBJ_MODEL_IMPORTER_H_
#define _CMG_GRAPHICS_OBJ_MODEL_IMPORTER_H_

#include <cmgCore/cmg_core.h>
#include <cmgGraphics/cmgModel.h>

namespace cmg {

// Normal calculation mode
enum class ObjMeshNormalMode
{
	NORMAL_LOAD,					// load normals from file
	NORMAL_COMPUTE_FACE,			// ignore normals from file and compute per-face
	NORMAL_COMPUTE_VERTEX,			// ignore normals from file and compute per-vertex
	NORMAL_COMPUTE_TANGENT_FACE,	// compute full tangent basis per-face (texcoords must exist)
	NORMAL_COMPUTE_TANGENT_VERTEX,	// compute full tangent basis per-vertex (texcoords must exist)
};

//enum ObjIllumModelComp
//{
//	ILLUM_COMP_COLOR,
//	ILLUM_COMP_AMBIENT,
//	ILLUM_COMP_HIGHLIGHT,
//	ILLUM_COMP_REFLECTION,
//	ILLUM_COMP_REFLECTION_FRESNEL,
//	ILLUM_COMP_RAY_TRACE,
//	ILLUM_COMP_TRANSPARENCY_GLASS,
//	ILLUM_COMP_TRANSPARENCY_REFRACTION,
//	ILLUM_COMP_CAST_SHADOWS_ONTO_INVISIBLE_SURFACES,
//};
//
//enum ObjIlluminationModel
//{
//	OBJ_ILLUM_COLOR_ON_AMBIENT_OFF = 0,
//	OBJ_ILLUM_COLOR_ON_AMBIENT_ON,
//	OBJ_ILLUM_HIGHLIGHT_ON,
//	OBJ_ILLUM_REFLECTION_ON_RAY_TRACE_ON,
//	OBJ_ILLUM_TRANSPARENCY_GLASS_ON_REFLECTION_RAY_TRACE_ON,
//	OBJ_ILLUM_REFLECTION_FRESNEL_ON_RAY_TRACE_ON,
//	OBJ_ILLUM_TRANSPARENCY_REFRACTION_ON_REFLECTION_FRESNEL_OFF_AND_RAY_TRACE_ON,
//	OBJ_ILLUM_TRANSPARENCY_REFRACTION_ON_REFLECTION_FRESNEL_ON_AND_RAY_TRACE_ON,
//	OBJ_ILLUM_REFLECTION_ON_AND_RAY_TRACE_OFF,
//	OBJ_ILLUM_TRANSPARENCY_GLASS_ON_REFLECTION_RAY_TRACE_OFF,
//	OBJ_ILLUM_CASTS_SHADOWS_ONTO_INVISIBLE_SURFACES,
//
//	OBJ_ILLUM_SOLID_COLOR = 0,
//	OBJ_ILLUM_LAMBERT,
//	OBJ_ILLUM_PHONG,
//};


//-----------------------------------------------------------------------------
// ObjMaterial - OBJ model material, defined in a MTL file
//-----------------------------------------------------------------------------
struct ObjMaterial
{
	String name;

	Vector3f ambientColor;			// Ka 
	Vector3f diffuseColor;			// Kd
	Vector3f specularColor;			// Ks
	float specularExponent;			// Ns 
	float opacity;					// d/Tr
	uint32 illuminationModel;		// illum

	Texture::sptr ambientMap = nullptr;				// map_Ka
	Texture::sptr diffuseMap = nullptr;				// map_Kd
	Texture::sptr specularMap = nullptr;			// map_Ks
	Texture::sptr specularHighlightMap = nullptr;	// map_Ns
	Texture::sptr alphaMap = nullptr;				// map_d
	Texture::sptr bumpMap = nullptr;				// map_bump/bump
	Texture::sptr displacementMap = nullptr;		// disp
	Texture::sptr stencilDecalMap = nullptr;		// decal
};

union ObjFaceVertex
{
	struct
	{
		int32 indices[3];
	};

	struct
	{
		int32 position;
		int32 texCoord;
		int32 normal;
	};
};

union ObjFace
{
	ObjFaceVertex vertices[3];
};


//-----------------------------------------------------------------------------
// ObjMesh - OBJ model mesh, containing vertex information.
//-----------------------------------------------------------------------------
struct ObjMesh
{
	String material;
	//BoundingBox boundingBox;
	Array<ObjFace> faces;
	Array<VertexPosTexNorm> vertices;
	Array<uint32> indices;
};


//-----------------------------------------------------------------------------
// ObjModel - Struct for loading an OBJ model into memory.
//-----------------------------------------------------------------------------
struct ObjModel
{
	String name;
	String objPath;
	String mtlPath;
	Map<String, ObjMesh*> meshes;
	Map<String, ObjMaterial*> materials;
};



class ObjModelImporter
{
public:
	ObjModelImporter(ResourceLoader<Texture>& textureLoader);

	Error ImportModel(const Path& path, Model*& outModel);

private:
	ObjMesh* BeginNewMesh(const String& name);
	void CompleteMesh(ObjMesh* mesh);
	ObjFaceVertex ParseFaceVertex(const String& token);
	void AddVertex(ObjMesh* mesh, const ObjFaceVertex& faceVertex);
	void CalcNormals(ObjMesh* mesh);
	Error LoadMaterials(const Path& mtlFileName);
	ObjMaterial* GetOrCreateMaterial(const String& name);
	ObjMesh* GetOrCreateMesh(const String& name);

	ResourceLoader<Texture>& m_textureLoader;
	ObjModel m_objModel;
	Model* m_model;
	Array<Vector3f> m_positions;
	Array<Vector2f> m_texCoords;
	Array<Vector3f> m_normals;
};


/*
//-----------------------------------------------------------------------------
// Loading from OBJ file

// Load an OBJ model from file, with an optional normal load mode.
int objModelLoad(ObjModel* model, const char* fileName,
	ObjMeshNormalMode normalMode = ObjMeshNormalMode::NORMAL_LOAD);

// Load an OBJ model's list of materials from an MTL file.
int objModelLoadMaterials(ObjModel* model, const char* mtlFileName);

ObjMaterial* objModelFindMaterial(ObjModel* model, const char* name);

ObjMaterial* objModelGetOrCreateMaterial(ObjModel* model, const char* name);

ObjMesh* objModelFindMesh(ObjModel* model, const char* name);

ObjMesh* objModelGetOrCreateMesh(ObjModel* model, const char* name);


//-----------------------------------------------------------------------------
// Binary loading and saving

// Save an OBJ model to a binary OBJ file.
int objModelSaveBinary(const ObjModel* model, const char* fileName);

// Load an OBJ model from a binary OBJ file.
int objModelLoadBinary(ObjModel* model, const char* fileName);

*/

};

#endif // _CMG_GRAPHICS_OBJ_MODEL_IMPORTER_H_