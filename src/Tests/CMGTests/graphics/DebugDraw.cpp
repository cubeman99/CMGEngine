#include "DebugDraw.h"
#include "physics/PrimitiveMeshes.h"


//-----------------------------------------------------------------------------
// Constructor & destructor
//-----------------------------------------------------------------------------

DebugDraw::DebugDraw() :
	m_viewProjection(Matrix4f::IDENTITY),
	m_shaded(false)
{
	String vertexSource = "#version 410"
		"\n"   "layout (location = 0) in vec3 vertPosition;"
		"\n"   "layout (location = 2) in vec3 vertNormal;"
		"\n"   "uniform mat4 u_mvp;"
		"\n"   "uniform mat4 u_model;"
		"\n"   "out vec3 v_normal;"
		"\n"   "void main()"
		"\n"   "{"
		"\n"   "	gl_Position = u_mvp * vec4(vertPosition, 1);"
		"\n"   "	v_normal = (u_model * vec4(vertNormal, 0)).xyz;"
		"\n"   "}";
	String fragmentSource = "#version 410"
		"\n"   "uniform vec4 u_color = vec4(1,1,1,1);"
		"\n"   "uniform vec3 u_lightDirection;"
		"\n"   "in vec3 v_normal;"
		"\n"   "out vec4 o_color;"
		"\n"   "void main()"
		"\n"   "{"
		"\n"   "	float lightAmount = dot(normalize(v_normal), -u_lightDirection);"
		"\n"   "	lightAmount = (lightAmount + 1.0) * 0.5;"
		"\n"   "	o_color = vec4(u_color.rgb * lightAmount, u_color.a);"
		"\n"   "}";

	m_shaderShadedColor = new Shader();
	m_shaderShadedColor->AddStage(ShaderType::k_vertex_shader, vertexSource);
	m_shaderShadedColor->AddStage(ShaderType::k_fragment_shader, fragmentSource);
	m_shaderShadedColor->CompileAndLink();
	
	vertexSource = "#version 410"
		"\n"   "layout (location = 0) in vec3 vertPosition;"
		"\n"   "uniform mat4 u_mvp;"
		"\n"   "void main()"
		"\n"   "{"
		"\n"   "	gl_Position = u_mvp * vec4(vertPosition, 1);"
		"\n"   "}";
	fragmentSource = "#version 410"
		"\n"   "uniform vec4 u_color = vec4(1,1,1,1);"
		"\n"   "out vec4 o_color;"
		"\n"   "void main()"
		"\n"   "{"
		"\n"   "	o_color = u_color;"
		"\n"   "}";

	m_shaderSolidColor = new Shader();
	m_shaderSolidColor->AddStage(ShaderType::k_vertex_shader, vertexSource);
	m_shaderSolidColor->AddStage(ShaderType::k_fragment_shader, fragmentSource);
	m_shaderSolidColor->CompileAndLink();


	VertexAttributeInfo attribs[4];
	attribs[0] = VertexAttributeInfo(VertexAttrib::k_position, AttributeType::k_vec3, nullptr);
	attribs[1] = VertexAttributeInfo(VertexAttrib::k_normal, AttributeType::k_vec3, nullptr);
	attribs[2] = VertexAttributeInfo(VertexAttrib::k_tex_coord, AttributeType::k_vec2, nullptr);
	attribs[3] = VertexAttributeInfo(VertexAttrib::k_color, AttributeType::k_vec3, nullptr);

	Primitives::InitializeCube();
	Primitives::InitializeSphere();
	Primitives::InitializeCylinder();
	Primitives::InitializeCone();
	
	attribs[0].data = Primitives::GetCubePositions();
	attribs[1].data = Primitives::GetCubeNormals();
	attribs[2].data = Primitives::GetCubeTexCoords();
	attribs[3].data = Primitives::GetCubeColors();
	m_meshCube = new Mesh();
	m_meshCube->SetPrimitiveType(VertexPrimitiveType::k_triangles);
	m_meshCube->GetVertexData()->BufferVertices(Primitives::GetCubeVertexCount(), attribs, 4);
	m_meshCube->GetIndexData()->BufferIndices(0, nullptr);
	m_meshCube->SetIndices(0, Primitives::GetCubeVertexCount());
		
	attribs[0].data = Primitives::GetSpherePositions();
	attribs[1].data = Primitives::GetSphereNormals();
	attribs[2].data = Primitives::GetSphereTexCoords();
	attribs[3].data = Primitives::GetSphereColors();
	m_meshSphere = new Mesh();
	m_meshSphere->SetPrimitiveType(VertexPrimitiveType::k_triangles);
	m_meshSphere->GetVertexData()->BufferVertices(Primitives::GetSphereVertexCount(), attribs, 4);
	m_meshSphere->GetIndexData()->BufferIndices(Primitives::GetSphereIndexCount(), Primitives::GetSphereIndices());
	m_meshSphere->SetIndices(0, Primitives::GetSphereIndexCount());
		
	attribs[0].data = Primitives::GetCylinderPositions();
	attribs[1].data = Primitives::GetCylinderNormals();
	attribs[2].data = Primitives::GetCylinderTexCoords();
	attribs[3].data = Primitives::GetCylinderColors();
	m_meshCylinder = new Mesh();
	m_meshCylinder->SetPrimitiveType(VertexPrimitiveType::k_triangles);
	m_meshCylinder->GetVertexData()->BufferVertices(Primitives::GetCylinderVertexCount(), attribs, 4);
	m_meshCylinder->GetIndexData()->BufferIndices(Primitives::GetCylinderIndexCount(), Primitives::GetCylinderIndices());
	m_meshCylinder->SetIndices(0, Primitives::GetCylinderIndexCount());
		
	attribs[0].data = Primitives::GetConePositions();
	attribs[1].data = Primitives::GetConeNormals();
	attribs[2].data = Primitives::GetConeTexCoords();
	attribs[3].data = Primitives::GetConeColors();
	m_meshCone = new Mesh();
	m_meshCone->SetPrimitiveType(VertexPrimitiveType::k_triangles);
	m_meshCone->GetVertexData()->BufferVertices(Primitives::GetConeVertexCount(), attribs, 4);
	m_meshCone->GetIndexData()->BufferIndices(Primitives::GetConeIndexCount(), Primitives::GetConeIndices());
	m_meshCone->SetIndices(0, Primitives::GetConeIndexCount());
	
	m_meshSphereLowRes = nullptr;
}

DebugDraw::~DebugDraw()
{
	delete m_shaderSolidColor;
	m_shaderSolidColor = nullptr;
	delete m_shaderShadedColor;
	m_shaderShadedColor = nullptr;

	delete m_meshSphere;
	m_meshSphere = nullptr;
	delete m_meshSphereLowRes;
	m_meshSphereLowRes = nullptr;
	delete m_meshCube;
	m_meshCube = nullptr;
	delete m_meshCylinder;
	m_meshCylinder = nullptr;
}


//-----------------------------------------------------------------------------
// Render options
//-----------------------------------------------------------------------------

void DebugDraw::SetLineWidth(float lineWidth)
{
	m_lineWidth = lineWidth;
	glLineWidth(m_lineWidth);
}

void DebugDraw::SetPointSize(float pointSize)
{
	m_pointSize = pointSize;
	glPointSize(m_pointSize);
}

void DebugDraw::SetShaded(bool shaded, const Vector3f& lightDirection)
{
	m_shaded = shaded;
	m_lightDirection = lightDirection;
	m_lightDirection.Normalize();
}

void DebugDraw::SetViewProjection(const Matrix4f& viewProjection)
{
	m_viewProjection = viewProjection;
}


//-----------------------------------------------------------------------------
// Colliders
//-----------------------------------------------------------------------------

void DebugDraw::DrawWireCollider(const Collider* collider, const Color& color)
{
	ColliderType type = collider->GetType();
	if (type == ColliderType::k_box)
		DrawWireBoxCollider((const BoxCollider*) collider, color);
	else if (type == ColliderType::k_sphere)
		DrawWireSphereCollider((const SphereCollider*) collider, color);
	else if (type == ColliderType::k_cylinder)
		DrawWireCylinderCollider((const CylinderCollider*) collider, color);
	else if (type == ColliderType::k_capsule)
		DrawWireCapsuleCollider((const CapsuleCollider*) collider, color);
	else if (type == ColliderType::k_cone)
		DrawWireConeCollider((const ConeCollider*) collider, color);
	else if (type == ColliderType::k_polygon)
		DrawWirePolygonCollider((const PolygonCollider*) collider, color);
}

void DebugDraw::DrawFilledCollider(const Collider* collider, const Color& color)
{
	ColliderType type = collider->GetType();
	if (type == ColliderType::k_box)
		DrawFilledBoxCollider((const BoxCollider*) collider, color);
	else if (type == ColliderType::k_sphere)
		DrawFilledSphereCollider((const SphereCollider*) collider, color);
	else if (type == ColliderType::k_cylinder)
		DrawFilledCylinderCollider((const CylinderCollider*) collider, color);
	else if (type == ColliderType::k_capsule)
		DrawFilledCapsuleCollider((const CapsuleCollider*) collider, color);
	else if (type == ColliderType::k_cone)
		DrawFilledConeCollider((const ConeCollider*) collider, color);
	else if (type == ColliderType::k_polygon)
		DrawFilledPolygonCollider((const PolygonCollider*) collider, color);
}

void DebugDraw::DrawWireBoxCollider(const BoxCollider* box, const Color& color)
{
	DrawWireBox(box->GetShapeToWorld(),
		box->GetHalfSize(), color);
}

void DebugDraw::DrawFilledBoxCollider(const BoxCollider* box, const Color& color)
{
	DrawFilledBox(box->GetShapeToWorld(),
		box->GetHalfSize(), color);
}

void DebugDraw::DrawWireSphereCollider(const SphereCollider* sphere, const Color& color)
{
	DrawWireSphere(sphere->GetShapeToWorld(),
		sphere->GetRadius(), color);
}

void DebugDraw::DrawFilledSphereCollider(const SphereCollider* sphere, const Color& color)
{
	DrawFilledSphere(sphere->GetShapeToWorld(),
		sphere->GetRadius(), color);
}

void DebugDraw::DrawWireCylinderCollider(const CylinderCollider* cylinder, const Color& color)
{
	DrawWireCylinder(cylinder->GetShapeToWorld(),
		cylinder->GetRadius(), cylinder->GetHalfHeight(), color);
}

void DebugDraw::DrawFilledCylinderCollider(const CylinderCollider* cylinder, const Color& color)
{
	DrawFilledCylinder(cylinder->GetShapeToWorld(),
		cylinder->GetRadius(), cylinder->GetHalfHeight(), color);
}

void DebugDraw::DrawWireCapsuleCollider(const CapsuleCollider* capsule, const Color& color)
{
	DrawWireCapsule(capsule->GetShapeToWorld(),
		capsule->GetRadius(), capsule->GetHalfHeight(), color);
}

void DebugDraw::DrawFilledCapsuleCollider(const CapsuleCollider* capsule, const Color& color)
{
	DrawFilledCapsule(capsule->GetShapeToWorld(),
		capsule->GetRadius(), capsule->GetHalfHeight(), color);
}

void DebugDraw::DrawWireConeCollider(const ConeCollider* cone, const Color& color)
{
	DrawWireCone(cone->GetShapeToWorld(),
		cone->GetRadius(), cone->GetHeight(), color);
}

void DebugDraw::DrawFilledConeCollider(const ConeCollider* cone, const Color& color)
{
	DrawFilledCone(cone->GetShapeToWorld(),
		cone->GetRadius(), cone->GetHeight(), color);
}

void DebugDraw::DrawWirePolygonCollider(const PolygonCollider* poly, const Color& color)
{
	BeginDrawWire();
	glUseProgram(0);
	glLoadMatrixf(poly->GetShapeToWorld().m);
	glBegin(GL_LINE_LOOP);
	glColor4ubv(color.data());
	for (auto it = poly->vertices_begin(); it != poly->vertices_end(); ++it)
		glVertex3fv(it->v);
	glEnd();
}

void DebugDraw::DrawFilledPolygonCollider(const PolygonCollider* poly, const Color& color)
{
	BeginDrawFill();
	glUseProgram(0);
	glLoadMatrixf(poly->GetShapeToWorld().m);
	glBegin(GL_TRIANGLE_FAN);
	glColor4ubv(color.data());
	for (auto it = poly->vertices_begin(); it != poly->vertices_end(); ++it)
		glVertex3fv(it->v);
	glEnd();
}

void DebugDraw::DrawDrawConvexMeshCollider(const ConvexMeshCollider* mesh, const Color& color)
{
	BeginImmediate(mesh->GetShapeToWorld());
	glBegin(GL_LINES);
	glColor4ubv(color.data());
	for (unsigned int i = 0; i < mesh->GetNumFaces(); ++i)
	{
		const ConvexMeshFace& face = mesh->GetFace(i);
		const ConvexMeshHalfEdge* edge = face.edge;
		for (unsigned int j = 0; j < face.numEdges; ++j)
		{
			glVertex3fv(face.edge->tail->position.v);
			glVertex3fv(face.edge->twin->tail->position.v);
			edge = edge->next;
		}
	}
	glEnd();
}

void DebugDraw::DrawFilledConvexMeshCollider(const ConvexMeshCollider* mesh, const Color& color)
{
	BeginImmediate(mesh->GetShapeToWorld());
	glBegin(GL_TRIANGLES);
	glColor4ubv(color.data());
	for (unsigned int i = 0; i < mesh->GetNumFaces(); ++i)
	{
		const ConvexMeshFace& face = mesh->GetFace(i);
		const ConvexMeshHalfEdge* edge = face.edge->next->next;
		glColor4ubv(GetShadedColor(mesh->GetShapeToWorld().
			Rotate(face.normal), color).data());
		for (unsigned int j = 2; j < face.numEdges; ++j)
		{
			glVertex3fv(face.edge->tail->position.v);
			glVertex3fv(face.edge->next->tail->position.v);
			glVertex3fv(edge->tail->position.v);
			edge = edge->next;
		}
	}
	glEnd();
}


//-----------------------------------------------------------------------------
// Primitives
//-----------------------------------------------------------------------------

void DebugDraw::DrawWireBox(const Matrix4f& modelMatrix,
	const Vector3f& halfSize, const Color& color)
{
}

void DebugDraw::DrawFilledBox(const Matrix4f& modelMatrix,
	const Vector3f& halfSize, const Color& color)
{
	DrawMesh(m_meshCube, modelMatrix *
		Matrix4f::CreateScale(halfSize), color);
}

void DebugDraw::DrawWireSphere(const Matrix4f& modelMatrix,
	float radius, const Color& color)
{
}

void DebugDraw::DrawFilledSphere(const Matrix4f& modelMatrix,
	float radius, const Color& color)
{
	DrawMesh(m_meshSphere, modelMatrix *
		Matrix4f::CreateScale(radius), color);
}

void DebugDraw::DrawWireCylinder(const Matrix4f& modelMatrix,
	float radius, float halfHeight, const Color& color)
{
}

void DebugDraw::DrawFilledCylinder(const Matrix4f& modelMatrix,
	float radius, float halfHeight, const Color& color)
{
	DrawMesh(m_meshCylinder, modelMatrix *
		Matrix4f::CreateScale(radius, halfHeight, radius), color);
}

void DebugDraw::DrawWireCylinder(const Matrix4f& modelMatrix,
	const Vector3f& a, const Vector3f& b, float radius, const Color& color)
{
}

void DebugDraw::DrawFilledCylinder(const Matrix4f& modelMatrix,
	const Vector3f& p0, const Vector3f& p1, float radius, const Color& color)
{
	// Create basis vectors
	float height = p0.DistTo(p1);
	Vector3f up = (p1 - p0) / height;
	Vector3f right = (Math::Abs(up.x) < Math::Abs(up.z) ? Vector3f::UNITX : Vector3f::UNITZ);
	right = up.Cross(right).Normalize();
	Vector3f back = right.Cross(up).Normalize();
	
	Matrix4f cylinderModelMatrix = Matrix4f::IDENTITY;
	cylinderModelMatrix.c0.xyz = right * radius;
	cylinderModelMatrix.c1.xyz = up * (height * 0.5f);
	cylinderModelMatrix.c2.xyz = back * radius;
	cylinderModelMatrix.c3.xyz = (p0 + p1) * 0.5f;
	
	DrawMesh(m_meshCylinder, modelMatrix *
		cylinderModelMatrix, color);
}

void DebugDraw::DrawWireCapsule(const Matrix4f& modelMatrix,
	float radius, float halfHeight, const Color& color)
{
}

void DebugDraw::DrawFilledCapsule(const Matrix4f& modelMatrix,
	float radius, float halfHeight, const Color& color)
{
	DrawMesh(m_meshCylinder, modelMatrix *
		Matrix4f::CreateScale(radius, halfHeight, radius), color);
	DrawMesh(m_meshSphere, modelMatrix *
		Matrix4f::CreateTranslation(0.0f, halfHeight, 0.0f) *
		Matrix4f::CreateScale(radius), color);
	DrawMesh(m_meshSphere, modelMatrix *
		Matrix4f::CreateTranslation(0.0f, -halfHeight, 0.0f) *
		Matrix4f::CreateScale(radius), color);
}

void DebugDraw::DrawWireCapsule(const Matrix4f& modelMatrix,
	const Vector3f& a, const Vector3f& b, float radius, const Color& color)
{
}

void DebugDraw::DrawFilledCapsule(const Matrix4f& modelMatrix,
	const Vector3f& p0, const Vector3f& p1, float radius, const Color& color)
{
	// Create basis vectors
	float height = p0.DistTo(p1);
	Vector3f up = (p1 - p0) / height;
	Vector3f right = (Math::Abs(up.x) < Math::Abs(up.z) ? Vector3f::UNITX : Vector3f::UNITZ);
	right = up.Cross(right).Normalize();
	Vector3f back = right.Cross(up).Normalize();
	
	Matrix4f cylinderModelMatrix = Matrix4f::IDENTITY;
	cylinderModelMatrix.c0.xyz = right;
	cylinderModelMatrix.c1.xyz = up;
	cylinderModelMatrix.c2.xyz = back;
	cylinderModelMatrix.c3.xyz = (p0 + p1) * 0.5f;
	
	DrawFilledCapsule(modelMatrix *
		cylinderModelMatrix, radius, height * 0.5f, color);
}

void DebugDraw::DrawWireCone(const Matrix4f& modelMatrix, float radius, float height, const Color& color)
{
}

void DebugDraw::DrawFilledCone(const Matrix4f& modelMatrix, float radius, float height, const Color& color)
{
	DrawMesh(m_meshCone, modelMatrix *
		Matrix4f::CreateScale(radius, height, radius), color);
}



//-----------------------------------------------------------------------------
// Internal functions
//-----------------------------------------------------------------------------

Color DebugDraw::GetShadedColor(const Vector3f& normal, const Color& color)
{
	float lightAmount = normal.Dot(-m_lightDirection);
	lightAmount = (lightAmount + 1.0f) * 0.5f;
	return Color::Lerp(Color::BLACK, color, lightAmount);
}

void DebugDraw::BeginImmediate(const Matrix4f& transform)
{
	glUseProgram(0);

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(m_viewProjection.m);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(transform.m);

	glLineWidth(m_lineWidth);
	glPointSize(m_pointSize);
}

void DebugDraw::BeginDrawWire()
{
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(m_viewProjection.m);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glLineWidth(m_lineWidth);
}

void DebugDraw::BeginDrawFill()
{
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(m_viewProjection.m);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

}

void DebugDraw::DrawMesh(Mesh* mesh, const Matrix4f& modelMatrix, const Color& color)
{
	Matrix4f mvp = m_viewProjection * modelMatrix;
	Shader* shader = (m_shaded ? m_shaderShadedColor : m_shaderSolidColor);
	// Activate the shader
	glUseProgram(shader->GetGLProgram());

	// Update uniforms
	int uniformLocation = -1;
	if (shader->GetUniformLocation("u_color", uniformLocation))
		glUniform4fv(uniformLocation, 1, color.ToVector4f().v);
	if (shader->GetUniformLocation("u_mvp", uniformLocation))
		glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, mvp.m);
	if (shader->GetUniformLocation("u_model", uniformLocation))
		glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, modelMatrix.m);
	if (shader->GetUniformLocation("u_lightDirection", uniformLocation))
		glUniform3fv(uniformLocation, 1, m_lightDirection.v);

	// Draw polygons.

	//glEnable(GL_CULL_FACE);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glColor4ubv(Color::GREEN.data());
	glBindVertexArray(mesh->GetVertexData()->
		GetVertexBuffer()->GetGLVertexArray());

	if (mesh->GetIndexData()->GetCount() == 0)
	{
		// Draw non indexed.
		glDrawArrays(GL_TRIANGLES, mesh->GetFirstIndex(),
			mesh->GetNumIndices());
	}
	else
	{
		// Draw indexed.
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
			mesh->GetIndexData()->GetIndexBuffer()->GetGLIndexBuffer());
		glDrawElements(GL_TRIANGLES, mesh->GetNumIndices(),
			GL_UNSIGNED_INT, ((unsigned int*) 0) + mesh->GetFirstIndex());
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	glBindVertexArray(0);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//glDisable(GL_CULL_FACE);
}

