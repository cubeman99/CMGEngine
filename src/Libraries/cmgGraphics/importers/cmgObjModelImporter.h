#ifndef _CMG_GRAPHICS_OBJ_MODEL_IMPORTER_H_
#define _CMG_GRAPHICS_OBJ_MODEL_IMPORTER_H_

#include <cmgCore/cmg_core.h>
#include <cmgGraphics/cmgModel.h>


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

	String ambientMap;				// map_Ka
	String diffuseMap;				// map_Kd
	String specularMap;				// map_Ks
	String specularHighlightMap;	// map_Ns
	String alphaMap;				// map_d
	String bumpMap;					// map_bump/bump
	String displacementMap;			// disp
	String stencilDecalMap;			// decal
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
	Array<Vector3f> positions;
	Array<Vector2f> texCoords;
	Array<Vector3f> normals;
	Array<ObjFace> faces;
	//BoundingBox boundingBox;
};


//-----------------------------------------------------------------------------
// ObjModel - Struct for loading an OBJ model into memory.
//-----------------------------------------------------------------------------
struct ObjModel
{
	String name;
	String objPath;
	String mtlPath;
	Array<ObjMesh> meshes;
	Map<String, ObjMaterial> materials;
};



class ObjModelImporter
{
public:
	ObjModelImporter();

	Error ImportModel(const Path& path, Model*& outModel);


private:
	void BeginNewObject();
	void CompleteObject();
	ObjFaceVertex ParseFaceVertex(const String& token);
	void AddVertex(const ObjFaceVertex& faceVertex);
	void CalcNormals();

	Model* m_model;
	Array<Vector3f> m_positions;
	Array<Vector2f> m_texCoords;
	Array<Vector3f> m_normals;
	Array<VertexPosTexNorm> m_vertices;
	Array<uint32> m_indices;
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

#endif // _CMG_GRAPHICS_OBJ_MODEL_IMPORTER_H_