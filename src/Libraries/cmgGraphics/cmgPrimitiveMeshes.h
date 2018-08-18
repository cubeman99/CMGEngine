#ifndef _CMG_GRAPHICS_PRIMITIVE_MESHES_H_
#define _CMG_GRAPHICS_PRIMITIVE_MESHES_H_

#include <vector>
#include <cmgMath/types/cmgVector3f.h>
#include <cmgMath/types/cmgRay.h>
#include <cmgGraphics/cmg_graphics.h>



class Primitives
{
public:


	static void				InitializeCube();
	static unsigned int		GetCubeVertexCount();
	static const Vector3f*	GetCubePositions();
	static const Vector3f*	GetCubeNormals();
	static const Vector2f*	GetCubeTexCoords();
	static const Vector3f*	GetCubeColors();
	
	static void				InitializeSphere();
	static unsigned int		GetSphereVertexCount();
	static const Vector3f*	GetSpherePositions();
	static const Vector3f*	GetSphereNormals();
	static const Vector2f*	GetSphereTexCoords();
	static const Vector3f*	GetSphereColors();
	static unsigned int		GetSphereIndexCount();
	static const unsigned int*	GetSphereIndices();
	
	static void				InitializeCylinder();
	static unsigned int		GetCylinderVertexCount();
	static const Vector3f*	GetCylinderPositions();
	static const Vector3f*	GetCylinderNormals();
	static const Vector2f*	GetCylinderTexCoords();
	static const Vector3f*	GetCylinderColors();
	static unsigned int		GetCylinderIndexCount();
	static const unsigned int*	GetCylinderIndices();
	
	static void				InitializeCone();
	static unsigned int		GetConeVertexCount();
	static const Vector3f*	GetConePositions();
	static const Vector3f*	GetConeNormals();
	static const Vector2f*	GetConeTexCoords();
	static const Vector3f*	GetConeColors();
	static unsigned int		GetConeIndexCount();
	static const unsigned int*	GetConeIndices();

	static Mesh* CreateCube();
	static Mesh* CreateIcosahedron(float radoius, bool smooth = true);

	static Mesh* CreateQuad();
	//static void CreateUVSphere(float radius, unsigned int rings, unsigned int segments);
	static Mesh* CreateIcoSphere(float radius, unsigned int subdivisions, bool smooth = true);
	static Mesh* CreateHexaSphere(float radius, unsigned int subdivisions, bool smooth = true);
	static Mesh* CreateQuadSphere(float radius, unsigned int subdivisions, bool smooth = true);
	
	static Mesh* CreateCylinder(float radius, float height, unsigned int numEdges, const Vector3f& axis, bool smooth = true);

private:
	Primitives() {}
};


#endif // _CMG_GRAPHICS_PRIMITIVE_MESHES_H_