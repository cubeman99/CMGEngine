#include "TestApp.h"
#include <gl/GL.h>


static const Path g_assetsPath("../assets/");



Particle::Particle()
{
	m_position = Vector3f::ZERO;
	m_velocity = Vector3f::ZERO;
	m_acceleration = Vector3f::ZERO;
	m_netForce = Vector3f::ZERO;
	m_mass = 1.0f;
}

void Particle::Update(float timeDelta)
{
	m_acceleration = m_netForce * (1.0f / m_mass);
	m_velocity += m_acceleration * timeDelta;
	m_position += m_velocity * timeDelta;
}




TestApp::TestApp() :
	m_texture(nullptr)
{

}

void TestApp::OnInitialize()
{
	m_renderParams.EnableBlend(true);
	m_renderParams.EnablePolygonSmooth(false);
	m_renderParams.EnableLineSmooth(false);
	m_renderParams.SetClearColor(Color::BLACK);
	m_renderParams.SetPolygonMode(PolygonMode::k_fill);
	m_renderParams.SetClearBits(ClearBits::k_color_buffer_bit | ClearBits::k_depth_buffer_bit);
	m_renderParams.SetBlendFunction(BlendFunc::k_source_alpha, BlendFunc::k_one_minus_source_alpha);
	
	// 2D mode
	m_renderParams.EnableCullFace(false);
	m_renderParams.EnableDepthBufferWrite(false);
	m_renderParams.EnableDepthTest(false);
	m_renderParams.EnableNearFarPlaneClipping(false);

	//GetWindow()->ConfineMouseToWindow(true);
	//GetWindow()->SetMouseVisibility(false);

	m_circle.center = Vector2f(0, 0);
	m_circle.radius = 0.6f;
	m_square.center = Vector2f(0, 0);
	m_square.radius = 0.5f;

	m_cameraTransform.position = Vector3f(0, 1, 0);

	float r = 0.4f;
	Array<Vector3f> vertices;
	vertices.push_back(Vector3f(-r, -r, -r));
	vertices.push_back(Vector3f(r, -r, -r));
	vertices.push_back(Vector3f(r, -r, r));
	vertices.push_back(Vector3f(-r, -r, r));
	vertices.push_back(Vector3f(-r, r, -r));
	vertices.push_back(Vector3f(r, r, -r));
	vertices.push_back(Vector3f(r, r, r));
	vertices.push_back(Vector3f(-r, r, r));
	unsigned int indices[] = {
		0,1, 1,2, 2,3, 3,0,
		4,5, 5,6, 6,7, 7,4,
		0,4, 1,5, 2,6, 3,7,
	};
	
	for (unsigned int i = 0; i < vertices.size(); i++)
	{
		Particle* p = new Particle();
		p->m_position = vertices[i];
		p->m_position.Rotate(Vector3f(1, 0.8f, 2.0f).Normalize(), 1.4f);
		p->m_position += Vector3f(0, 2, -2);
		m_particles.push_back(p);
	}
	for (unsigned int i = 0; i < 24; i += 2)
	{
		Particle* p1 = m_particles[indices[i]];
		Particle* p2 = m_particles[indices[i + 1]];
		float distance = p1->m_position.DistTo(p2->m_position);
		p1->m_connections.push_back(Connection(p2, distance));
		p2->m_connections.push_back(Connection(p1, distance));
	}
}

void TestApp::OnQuit()
{
	delete m_texture;
	m_texture = nullptr;
}

void TestApp::OnResizeWindow(int width, int height)
{
}

void TestApp::OnDropFile(const String& fileName)
{
	if (m_texture != nullptr)
		delete m_texture;

	m_texture = Texture::LoadTexture(fileName);
	
	if (m_texture != nullptr)
		printf("Loaded texture %s\n", fileName.c_str());
	else
		printf("Error loading texture %s\n", fileName.c_str());
}

void TestApp::OnDropText(const String& text)
{
}


void TestApp::OnUpdate(float timeDelta)
{
	Keyboard* keyboard = GetKeyboard();
	
	if (keyboard->IsKeyPressed(Keys::f4))
		GetWindow()->SetFullscreen(!GetWindow()->IsFullscreen());
	
	if (keyboard->IsKeyPressed(Keys::escape))
		Quit();
	
	if (keyboard->IsKeyPressed(Keys::o) && !keyboard->IsKeyDown(Keys::left_control))
		GetWindow()->SetSize(145, 85);

	float rotateAmount = timeDelta * 2.0f;
	float moveAmount = timeDelta * 2.0f;
	if (keyboard->IsKeyDown(Keys::up))
		m_cameraTransform.rotation.Rotate(m_cameraTransform.rotation.GetRight(), rotateAmount);
	if (keyboard->IsKeyDown(Keys::down))
		m_cameraTransform.rotation.Rotate(m_cameraTransform.rotation.GetRight(), -rotateAmount);
	if (keyboard->IsKeyDown(Keys::left))
		m_cameraTransform.rotation.Rotate(Vector3f::UP, rotateAmount);
	if (keyboard->IsKeyDown(Keys::right))
		m_cameraTransform.rotation.Rotate(Vector3f::UP, -rotateAmount);
	if (keyboard->IsKeyDown(Keys::w))
		m_cameraTransform.position += m_cameraTransform.rotation.GetForward() * moveAmount;
	if (keyboard->IsKeyDown(Keys::s))
		m_cameraTransform.position += m_cameraTransform.rotation.GetBack() * moveAmount;
	if (keyboard->IsKeyDown(Keys::a))
		m_cameraTransform.position += m_cameraTransform.rotation.GetLeft() * moveAmount;
	if (keyboard->IsKeyDown(Keys::d))
		m_cameraTransform.position += m_cameraTransform.rotation.GetRight() * moveAmount;

	float circleMoveSpeed = timeDelta * 1.0f;
	if (keyboard->IsKeyDown(Keys::up))
		m_circle.center.y += circleMoveSpeed;
	if (keyboard->IsKeyDown(Keys::down))
		m_circle.center.y -= circleMoveSpeed;
	if (keyboard->IsKeyDown(Keys::left))
		m_circle.center.x -= circleMoveSpeed;
	if (keyboard->IsKeyDown(Keys::right))
		m_circle.center.x += circleMoveSpeed;
	if (keyboard->IsKeyDown(Keys::z))
		m_circle.radius *= 1.01f;
	if (keyboard->IsKeyDown(Keys::x))
		m_circle.radius /= 1.01f;


	// Update particle forces.
	for (auto it = m_particles.begin(); it != m_particles.end(); it++)
	{
		Particle* p1 = *it;
		p1->m_netForce = Vector3f::ZERO;

		p1->m_netForce += Vector3f(0, -10, 0) * p1->m_mass;

		for (auto it2 = p1->m_connections.begin(); it2 != p1->m_connections.end(); it2++)
		{
			float k = 500;

			Connection connection = *it2;
			Particle* p2 = connection.particle;
			Vector3f relPosition = p2->m_position - p1->m_position;
			float offset = relPosition.Length() - connection.distance;
			Vector3f force = offset * Vector3f::Normalize(relPosition) * k;
			p1->m_netForce += force;
		}
	}

	// Update particles.
	for (auto it = m_particles.begin(); it != m_particles.end(); it++)
	{
		Particle* p = *it;
		(*it)->Update(timeDelta);

		if (p->m_position.y < 0.0f)
			p->m_position.y = 0.0f;
	}
}

void TestApp::OnRender()
{
	Graphics2D g(GetWindow());

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_DEPTH_TEST);

	
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDepthMask(false);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    //glDisable(GL_DEPTH_CLAMP);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	Vector2f winSize((float) GetWindow()->GetWidth(),(float) GetWindow()->GetHeight());

	m_cameraProjection = Matrix4f::CreatePerspective(1.5f, GetWindow()->GetAspectRatio(), 0.01f, 100.0f);
	
	Keyboard* keyboard = GetKeyboard();
	
	//if (m_texture != nullptr)
	{
		Matrix4f projection = Matrix4f::CreateOrthographic(0, winSize.x, winSize.y, 0, -1.0f, 1.0f);
		glMatrixMode(GL_PROJECTION);
		//Matrix4f viewMatrix = m_cameraTransform.GetInvMatrix();
		Matrix4f viewMatrix = m_cameraTransform.GetMatrix();
		Matrix4f viewMatrixOrig = m_cameraTransform.GetMatrix();
		viewMatrix.InvertAffine();

		Matrix4f identity = (viewMatrix * viewMatrixOrig);


		Matrix4f viewProjection = m_cameraProjection * viewMatrix;
		glLoadMatrixf(viewProjection.data());

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		
		Matrix4f lookAtMatrix;
		Vector3f point = Vector3f(2, 1, -3);
		//lookAtMatrix.InitLookAt(m_cameraTransform.position - point, Vector3f::UP);
		lookAtMatrix.InitLookAt(m_particles[0]->m_position - point, Vector3f::UP);
		Vector3f right = lookAtMatrix.GetXBasis();
		Vector3f up = lookAtMatrix.GetYBasis();
		Vector3f back = lookAtMatrix.GetZBasis();

		Vector3f lookAt = Vector3f::FORWARD * 2.0f;
		lookAt.x += 0.5f;
		lookAt.y += 0.9f;
		m_cameraTransform.rotation.RotateVector(lookAt);
		lookAt += m_cameraTransform.position;

		// Render particles.
		glBegin(GL_QUADS);
		glColor3f(0.3f, 0.3f, 0.3f);
		glVertex3f(-2.0f, 0.0f, -2.0f);
		glVertex3f(2.0f, 0.0f, -2.0f);
		glVertex3f(2.0f, 0.0f, 2.0f);
		glVertex3f(-2.0f, 0.0f, 2.0f);
		glEnd();

		glPointSize(5.0f);

		glBegin(GL_POINTS);
		glColor3f(0,1,0);
		glVertex3fv(lookAt.data());
		glEnd();

		// Render particles.
		glBegin(GL_LINES);
		glColor3f(1,1,0);
		for (auto it = m_particles.begin(); it != m_particles.end(); it++)
		{
			Particle* p1 = *it;
			
			for (auto it2 = p1->m_connections.begin(); it2 != p1->m_connections.end(); it2++)
			{
				Particle* p2 = it2->particle;
				glVertex3fv(p1->m_position.data());
				glVertex3fv(p2->m_position.data());
			}
		}
		glEnd();
		

		glBegin(GL_LINES);
		glColor3f(0,0,1);
		glVertex3fv(point.data());
		glVertex3fv((point - back).data());
		glColor3f(0,1,0);
		glVertex3fv(point.data());
		glVertex3fv((point + up).data());
		glEnd();

		/*glBegin(GL_LINES);
		glColor3f(0,0,1);
		glVertex3fv(point.data());
		glVertex3fv((point + back).data());
		glColor3f(0,1,0);
		glVertex3fv(point.data());
		glVertex3fv((point + up).data());
		glColor3f(1,0,0);
		glVertex3fv(point.data());
		glVertex3fv((point + right).data());
		glEnd();*/
		
		/*
		if (keyboard->IsKeyDown(Keys::space))
		{
			glBegin(GL_LINES);
			glColor3f(0,0,1);
			glVertex3fv(m_cameraTransform.position.data());
			glVertex3fv((m_cameraTransform.position - back).data());
			glColor3f(0,1,0);
			glVertex3fv(m_cameraTransform.position.data());
			glVertex3fv((m_cameraTransform.position + up).data());
			glColor3f(1,0,0);
			glVertex3fv(m_cameraTransform.position.data());
			glVertex3fv((m_cameraTransform.position + right).data());
			glEnd();
		}
		else
		{
			glBegin(GL_LINES);
			glColor3f(0,0,1);
			glVertex3fv(m_cameraTransform.position.data());
			glVertex3fv((m_cameraTransform.position + m_cameraTransform.rotation.GetForward()).data());
			glColor3f(0,1,0);
			glVertex3fv(m_cameraTransform.position.data());
			glVertex3fv((m_cameraTransform.position + m_cameraTransform.rotation.GetUp()).data());
			glColor3f(1,0,0);
			glVertex3fv(m_cameraTransform.position.data());
			glVertex3fv((m_cameraTransform.position + m_cameraTransform.rotation.GetRight()).data());
			glEnd();
		}
		*/

		//Vector2f texSize((float) m_texture->GetWidth(), (float) m_texture->GetHeight());
		//float scale = Math::Min(winSize.x / texSize.x, winSize.y / texSize.y);

		//g.SetTransformation(Matrix4f::CreateScale(scale));
		//g.DrawTexture(m_texture, Rect2f(0, 0, texSize.x, texSize.y), Color::WHITE);

		/*glBindTexture(GL_TEXTURE_2D, m_texture->GetGLTextureId());

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glScalef(scale, scale, scale);

		glBegin(GL_QUADS);
		glColor3f(1,1,1);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(0, 0);
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(texSize.x, 0);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(texSize.x, texSize.y);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(0, texSize.y);
		glEnd();*/
	}
	/*
	else
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		glBindTexture(GL_TEXTURE_2D, 0);

		glBegin(GL_QUADS);
		glColor3f(1,1,1);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(-0.5f, -0.5f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(-0.5f, 0.5f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(0.5f, 0.5f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(0.5f, -0.5f);
		glEnd();
	}*/

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glScalef(1.0f / GetWindow()->GetAspectRatio(), 1.0f, 1.0f);

	glBegin(GL_LINE_LOOP);
	glColor3f(0,1,1);
	glVertex2f(m_square.center.x - m_square.radius, m_square.center.y - m_square.radius);
	glVertex2f(m_square.center.x - m_square.radius, m_square.center.y + m_square.radius);
	glVertex2f(m_square.center.x + m_square.radius, m_square.center.y + m_square.radius);
	glVertex2f(m_square.center.x + m_square.radius, m_square.center.y - m_square.radius);
	glEnd();
	
	CircleContour contours[6];
	int numContours = 0;
	PerformContour(m_circle, m_square, contours, numContours);

	if (numContours == 0)
	{
		// Draw the circle
		glBegin(GL_LINE_LOOP);
		glColor3f(0,0.4f,0);
		for (int i = 0; i < 20; i++)
		{
			float angle = (i / 20.0f) * Math::TWO_PI;
			glVertex2f(m_circle.center.x + cos(angle) * m_circle.radius,
				m_circle.center.y - sin(angle) * m_circle.radius);
		}
		glEnd();
	}

	for (int i = 0; i < numContours; i++)
	{
		if (contours[i].fullCircle)
		{
			glBegin(GL_LINE_LOOP);
			glColor3f(1,0,1);
			for (int i = 0; i < 20; i++)
			{
				float angle = (i / 20.0f) * Math::TWO_PI;
				glVertex2f(m_circle.center.x + cos(angle) * m_circle.radius,
					m_circle.center.y - sin(angle) * m_circle.radius);
			}
			glEnd();
		}
		else
		{
			DrawArc(contours[i].endPoint, contours[i].startPoint, m_circle);
		}
	}
}

void TestApp::DrawArc(Vector2f a, Vector2f b, const Bounds2D& circle)
{
	glBegin(GL_LINE_STRIP);
	glColor3f(1,0,1);

	Vector2f normal = (b - a);
	normal = Vector2f(-normal.y, normal.x);

	Vector2f aa = (a - circle.center); aa.Normalize();
	Vector2f bb = (b - circle.center); bb.Normalize();

	float angleBetween = Math::ACos(aa.Dot(bb));
	if ((circle.center - a).Dot(normal) > 0.0f)
		angleBetween = Math::TWO_PI - angleBetween;

	int numSteps = 1 + (int) ((angleBetween * 40) / Math::TWO_PI);
	float angleStep = angleBetween / (float) numSteps;
	Matrix3f rotation = Matrix3f::CreateRotation(circle.center, angleStep);
	
	glVertex2fv(a.data());
	Vector2f v = a;
	for (int i = 0; i < numSteps; i++)
	{
		v = rotation.TransformVector(v);
		glVertex2fv(v.data());
	}
	glVertex2fv(b.data());

	glEnd();
}


Vector2f TestApp::ComputeIntersectionPoint(const Bounds2D& circle, int edgeIndex, int pointIndex, Vector2f* corners)
{
	if (edgeIndex < 0)
		edgeIndex += 4;
	if (edgeIndex >= 4)
		edgeIndex -= 4;

	Vector2f point;
	int normalAxis = edgeIndex % 2;
	int tangentAxis = 1 - normalAxis;
	float aa = corners[edgeIndex][normalAxis] - circle.center[normalAxis];
	float sqrSol = (circle.radius * circle.radius) - (aa * aa);

	if (sqrSol > 0.0f)
	{
		point[normalAxis] = corners[edgeIndex][normalAxis];

		point[tangentAxis] = Math::Sqrt(sqrSol);
		if (edgeIndex < 2)
			point[tangentAxis] = -point[tangentAxis];
		if (pointIndex == 1)
			point[tangentAxis] = -point[tangentAxis];
		point[tangentAxis] += circle.center[tangentAxis];
	}

	return point;
}

int TestApp::PerformContour(const Bounds2D& circle, const Bounds2D& square, CircleContour* contours, int& outNumContours)
{
	// Check if the square fully contains the circle.
	if (square.SquareContainsSquare(circle))
	{
		outNumContours = 1;
		contours[0].fullCircle = true;
		return 1;
	}
	
	outNumContours = 0;

	Vector2f corners[4];
	bool circleContainsCorner[4];
	bool squareContainsCorner[4];

	// Compute square corner locations.
	corners[0] = Vector2f(square.center.x - square.radius, square.center.y - square.radius);
	corners[1] = Vector2f(square.center.x - square.radius, square.center.y + square.radius);
	corners[2] = Vector2f(square.center.x + square.radius, square.center.y + square.radius);
	corners[3] = Vector2f(square.center.x + square.radius, square.center.y - square.radius);

	for (int i = 0; i < 4; i++)
	{
		circleContainsCorner[i] = circle.CircleContains(corners[i]);
		squareContainsCorner[i] = circle.SquareContains(corners[i]);
	}
	bool circleBoundsContainsSquare = circle.SquareContainsSquare(square);
	
	// Loop over corners/edges.
	for (int i = 0; i < 4; i++)
	{
		int currCornerIndex = i;
		int prevCornerIndex = (i + 3) % 4;
		int nextCornerIndex = (i + 1) % 4;
		Vector2f corner = corners[i];
		Vector2f normalX = Vector2f(corner.x - square.center.x, 0.0f);
		Vector2f normalY = Vector2f(0.0f, corner.y - square.center.y);
		bool onInside = ((circle.center - corner).Dot(normalX) < 0.0f &&
						(circle.center - corner).Dot(normalY) < 0.0f);

		// Corner case.
		if (squareContainsCorner[currCornerIndex] &&
			!circleContainsCorner[currCornerIndex] &&
			onInside)
		{
			contours[outNumContours++] = CircleContour(
				ComputeIntersectionPoint(circle, i, 0, corners),
				ComputeIntersectionPoint(circle, i - 1, 1, corners));
		}

		// Opposite edge case.
		if (squareContainsCorner[currCornerIndex] &&
			squareContainsCorner[nextCornerIndex] &&
			!squareContainsCorner[prevCornerIndex] &&
			(onInside || (circleContainsCorner[currCornerIndex] && circleContainsCorner[nextCornerIndex])))
		{
			contours[outNumContours++] = CircleContour(
				ComputeIntersectionPoint(circle, i - 1, 0, corners),
				ComputeIntersectionPoint(circle, i + 1, 1, corners));
		}

		// Adjacent edge case.
		else if (squareContainsCorner[currCornerIndex] &&
			((!squareContainsCorner[prevCornerIndex] || !onInside) && !circleContainsCorner[prevCornerIndex]) &&
			((!squareContainsCorner[nextCornerIndex] || !onInside) && !circleContainsCorner[nextCornerIndex]) &&
			!circleBoundsContainsSquare &&
			(onInside || circleContainsCorner[currCornerIndex]))
		{
			contours[outNumContours++] = CircleContour(
				ComputeIntersectionPoint(circle, i - 1, 0, corners),
				ComputeIntersectionPoint(circle, i, 1, corners));
		}
	}
	
	if (outNumContours == 0)
	{
		// Loop over edges.
		for (int i = 0; i < 4; i++)
		{
			int currCornerIndex = i;
			int nextCornerIndex = (i + 1) % 4;
			Vector2f corner = corners[i];
			Vector2f nextCorner = corners[(i + 1) % 4];
		
			// Single edge case.
			Vector2f tangent = Vector2f::Normalize(nextCorner - corner);
			Vector2f normal(tangent.y, -tangent.x);
			Vector2f edgeCenter = (corner + nextCorner) * 0.5f;
			int normalAxis = i % 2;

			if (!circleContainsCorner[currCornerIndex] &&
				!circleContainsCorner[nextCornerIndex] &&
				Math::Abs((circle.center - edgeCenter).Dot(normal)) < circle.radius &&
				Math::Abs((circle.center - edgeCenter).Dot(tangent)) < square.radius)
			{
				contours[outNumContours++] = CircleContour(
					ComputeIntersectionPoint(circle, i, 0, corners),
					ComputeIntersectionPoint(circle, i, 1, corners));
				break;
			}
		}
	}

	return outNumContours;
}



