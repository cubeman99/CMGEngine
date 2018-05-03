#include "CollisionTester.h"
#include <cmgCore/cmgRandom.h>
#include <gl/GL.h>


#define ASSETS_PATH "C:/Workspace/C++/Framework Projects/CMGEngine/assets/"
//#define ASSETS_PATH "assets/"



static float RandomFloat()
{
	return ((float) rand() / (float) RAND_MAX);
}

static float RandomFloatClamped()
{
	return RandomFloat() - RandomFloat();
}



CollisionTestApp::CollisionTestApp()
{

}

void CollisionTestApp::OnInitialize()
{
	// Load meshes.
	m_meshCube		= Primitives::CreateCube();
	m_meshSphere	= Primitives::CreateIcoSphere(1.0f, 3);
	
	// Load shader.
	String fsSource, vsSource;
	File::OpenAndGetContents(Path(ASSETS_PATH "shaders/test.fs"), fsSource);
	File::OpenAndGetContents(Path(ASSETS_PATH "shaders/test.vs"), vsSource);
	m_shader = new Shader();
	m_shader->AddStage(ShaderType::k_vertex_shader, vsSource);
	m_shader->AddStage(ShaderType::k_fragment_shader, fsSource);
	m_shader->CompileAndLink();

	// Load font.
	m_font = SpriteFont::LoadFont(Path(ASSETS_PATH "font_console.png"), 16, 8, 12, 0);

	m_simulationPaused = false;
	m_simulationSpeed = 1.0f;

	m_renderParams.EnableBlend(true);
	m_renderParams.EnablePolygonSmooth(false);
	m_renderParams.EnableLineSmooth(false);
	m_renderParams.SetClearColor(Color::BLACK);
	m_renderParams.SetPolygonMode(PolygonMode::k_fill);
	m_renderParams.SetClearBits(ClearBits::k_color_buffer_bit | ClearBits::k_depth_buffer_bit);
	m_renderParams.SetBlendFunction(BlendFunc::k_source_alpha, BlendFunc::k_one_minus_source_alpha);
	
	// 2D mode
	//m_renderParams.EnableCullFace(false);
	//m_renderParams.EnableDepthBufferWrite(false);
	//m_renderParams.EnableDepthTest(false);
	//m_renderParams.EnableNearFarPlaneClipping(false);
	m_frameTimer.Start();

	m_showContacts = true;
	m_showMinkowskiDifference = false;

	m_debugDraw = new DebugDraw();
		
	m_cameraTransform.position = Vector3f(0, 4, 3);
	m_cameraTransform.rotation = Quaternion(Vector3f::UNITX, -0.3f);

	Reset();
}

void CollisionTestApp::Reset()
{
	RigidBody* body;

	Vector3f chassisSize;
	float wheelTrack	= 1.400f;
	float wheelBase		= 2.400f;
	float mass			= 1000.0f; // kg
	float wheelRadius	= 0.3f;
	chassisSize.x		= 1.630f;
	chassisSize.z		= 4.200f;
	chassisSize.y		= 1.340f;

	body = new RigidBody();
	body->SetMass(2.0f);
	body->SetOrientation(Quaternion::IDENTITY);
	//body->SetInverseInertiaTensor(Matrix3f::CreateScale(1.0f, 1.0f, 1.0f));
	body->SetPosition(Vector3f(0, 4.0f, 0));
	//body->SetOrientation(Quaternion(Vector3f::UNITY, -Math::HALF_PI));
	//body->SetAngularVelocity(Vector3f(0.0f, 0.0f, 2.0f));
	//body->SetPrimitive(new CollisionBox(Vector3f(0.3f, 0.2f, 0.2f)));
	//body->SetPrimitive(new CollisionBox(Vector3f(0.4f, 0.1f, 0.2f)));
	//body->AddPrimitive(new CollisionBox(Vector3f(0.2f, 0.2f, 0.2f) * 0.2f), Vector3f(0, -0.5f, 0));
	//body->AddPrimitive(new CollisionSphere(0.1f), Vector3f(0, -0.5f, 0));
	//body->AddPrimitive(new CollisionBox(Vector3f(0.2f, 0.2f, 0.2f)), Vector3f(0, -0.5f, 0));
	//body->AddPrimitive(new CollisionBox(Vector3f(0.4f, 0.2f, 0.1f)), Vector3f(0, 0.5f, 0));

	float roofWidth = chassisSize.x * 0.9f;
	Vector3f roofSize(chassisSize.x * 0.9f, chassisSize.y * 0.5f, chassisSize.z * 0.6f);
	Vector3f baseSize(chassisSize.x, chassisSize.y * 0.5f, chassisSize.z);
	Vector3f roofPos(0, roofSize.y * 0.5f, chassisSize.z * 0.15f);
	Vector3f basePos(0, -baseSize.y * 0.5f, 0);
	//body->AddPrimitive(new CollisionBox(roofSize * 0.5f), roofPos);
	//body->AddPrimitive(new CollisionBox(baseSize * 0.5f), basePos);
	//body->AddPrimitive(new CollisionSphere(wheelRadius), Vector3f(-wheelTrack, -chassisSize.y, -wheelBase) * 0.5f);
	//body->AddPrimitive(new CollisionSphere(wheelRadius), Vector3f(-wheelTrack, -chassisSize.y, wheelBase) * 0.5f);
	//body->AddPrimitive(new CollisionSphere(wheelRadius), Vector3f(wheelTrack, -chassisSize.y, -wheelBase) * 0.5f);
	//body->AddPrimitive(new CollisionSphere(wheelRadius), Vector3f(wheelTrack, -chassisSize.y, wheelBase) * 0.5f);
	//body->SetPrimitive(new CollisionBox(Vector3f(0.2f, 0.1f, 0.2f)));
	body->SetCollider(new BoxCollider(Vector3f(0.8f, 0.4f, 0.6f)));
	//body->SetCollider(new SphereCollider(0.6f));
	//body->SetCollider(new CylinderCollider(0.8f, 1.0f));
	//body->SetCollider(new ConeCollider(0.6f, 2.0f));
	//body->SetCollider(new ConeCollider(1.0f, 1.0f));
	//body->SetCollider(new CapsuleCollider(0.6f, 1.0f));
	body->SetMass(4.0f);
	body->SetMass(mass);
	body->SetInverseMass(0.0f);
	body->SetInverseInertiaTensor(Matrix3f::ZERO);
	body->SetMass(1.0f);
	body->SetInverseInertiaTensor(Matrix3f::CreateScale3(1.0f));
	//body->SetOrientation(Quaternion(Vector3f(0.2f, 0.1f, 0.3f).Normalize(), 0.05f));
	//body->SetOrientation(Quaternion(Vector3f::UNITZ, 0.2f));
	//body->SetAngularVelocity(Vector3f(0.0f, 6.0f, 0.0f));
	//body->SetOrientation(Quaternion(Vector3f::UNITX, Math::HALF_PI));
	body->CalcInertia();
	m_testBody1 = body;

	body = new RigidBody();
	body->SetInverseMass(0.0f);
	body->SetInverseInertiaTensor(Matrix3f::ZERO);
	//body->SetInverseInertiaTensor(Matrix3f::CreateScale(1.0f, 1.0f, 1.0f));
	body->SetPosition(Vector3f(0, 1.0f, 0));
	//body->SetPosition(Vector3f(0, 0.3f, 0));
	//body->SetOrientation(Quaternion(Vector3f::UNITZ, 0.0f));
	//body->SetOrientation(Quaternion(Vector3f::UNITY, Math::HALF_PI * 0.5f));
	//body->SetAngularVelocity(Vector3f(1.9f, 1.11f,-0.7f));
	//body->SetPrimitive(new CollisionBox(Vector3f(0.2f, 0.2f, 0.8f)));
	//body->SetPrimitive(new CollisionBox(Vector3f(0.2f, 0.1f, 0.2f)));
	//body->SetCollider(new BoxCollider(Vector3f(4.0f, 0.3f, 4.0f)));
	std::vector<Vector3f> polyVerts;
	polyVerts.push_back(Vector3f(-2, 0, -3));
	polyVerts.push_back(Vector3f(-2, 0, 3));
	polyVerts.push_back(Vector3f(2, 0, 3));
	polyVerts.push_back(Vector3f(2, 0, -3));
	body->SetCollider(new BoxCollider(Vector3f(3, 4, 1)));
	//body->SetCollider(new PolygonCollider(polyVerts.size(), polyVerts.data()));
	//body->SetMass(1.0f);
	//body->SetInverseInertiaTensor(Matrix3f::CreateScale(1.0f));
	m_testBody2 = body;


	m_gjk.done = true;
	m_gjk.result = false;
	m_gjk.simplex.Clear();
	m_gjk.shapeA = m_testBody1->GetCollider();
	m_gjk.shapeB = m_testBody2->GetCollider();
}

void CollisionTestApp::OnQuit()
{
	delete m_debugDraw;
	m_debugDraw = nullptr;
}

void CollisionTestApp::OnResizeWindow(int width, int height)
{
}

void CollisionTestApp::OnUpdate(float timeDelta)
{
	Keyboard* keyboard = GetKeyboard();
	Mouse* mouse = GetMouse();

	// Escape: Quit
	if (keyboard->IsKeyPressed(Keys::escape))
	{
		Quit();
		return;
	}

	if (m_frameTimer.GetElapsedSeconds() >= 1.0f)
	{
		//m_physicsEngine.GetProfiler()->Print(std::cout);
		m_frameTimer.Start();
	}

	bool simulateSingleFrame = false;
	// P: Pause/resume simulation.
	if (keyboard->IsKeyPressed(Keys::p))
		m_simulationPaused = !m_simulationPaused;
	// Page Up: increase simulation speed.
	if (keyboard->IsKeyPressed(Keys::page_up))
		m_simulationSpeed *= 1.5f;
	// Page Down: decrease simulation speed.
	if (keyboard->IsKeyPressed(Keys::page_down))
		m_simulationSpeed /= 1.5f;
	// Right bracket: simulate a single frame.
	if (keyboard->IsKeyPressed(Keys::right_bracket))
		simulateSingleFrame = true;
	// C: toggle visible contacts
	if (keyboard->IsKeyPressed(Keys::c))
		m_showContacts = !m_showContacts;

	if (keyboard->IsKeyPressed(Keys::semicolon))
		m_showMinkowskiDifference = !m_showMinkowskiDifference;
	
	if (m_gjk.done)
	{
		if (m_gjk.result && keyboard->IsKeyPressed(Keys::h))
		{
			m_epa = EPA::PerformEPA(
				m_testBody1->GetCollider(),
				m_testBody2->GetCollider(),
				m_gjk.simplex);

			printf("depth = %f\n", m_epa.depth);
			std::cout << "normal = " << m_epa.normal << std::endl;
			std::cout << "passed = " << m_epa.passed << std::endl;
			std::cout << "contact point = " << m_epa.contactPoint << std::endl;
		}

		if (m_showMinkowskiDifference)
		{
			std::vector<Vector3f> vertices;
			CreateMinkowskiDifferencePointCloud(vertices,
				m_testBody1->GetCollider(),
				m_testBody2->GetCollider());
			m_quickHull.Clear();
			m_quickHull.DoQuickHull(vertices.size(), vertices.data());
			m_quickHull.m_horizon.clear();
			//while (m_quickHull.MergeFaces())
				//;
		}

		if (keyboard->IsKeyPressed(Keys::space))
		{
			std::vector<Vector3f> vertices;
			for (unsigned int i = 0; i < 20; ++i)
			{
				Vector3f v;
				v.x = RandomFloatClamped();
				v.y = RandomFloatClamped();
				v.z = RandomFloatClamped();
				v *= 10;
				vertices.push_back(v);
			}

			Vector3f points[] = 
			{
				Vector3f(-1.0f, 0.0f, 0.0f),
				Vector3f(1.0f, 0.0f, -1.0f),
				Vector3f(1.0f, 0.0f, 1.0f),
				Vector3f(1.0f, 1.0f, 0.0f),
				Vector3f(0.0f, 0.7f, 0.7f),
				Vector3f(-0.4f, 0.7f, -2),
			};

			vertices.clear();
			CreateMinkowskiDifferencePointCloud(vertices,
				m_testBody1->GetCollider(),
				m_testBody2->GetCollider());

			m_quickHull.Clear();

			if (keyboard->IsKeyDown(Keys::left_control))
			{
				m_quickHull.DoQuickHull(vertices.size(), vertices.data());
				m_quickHull.m_horizon.clear();
			}
			else
			{
				m_quickHull.BuildInitialHull(vertices.size(), vertices.data());
			}
		}
		if (keyboard->IsKeyPressed(Keys::m))
		{
			QuickHullVertex* vertex = m_quickHull.NextConflictVertex();
			if (vertex != nullptr)
				m_quickHull.AddVertexToHull(vertex);
			else
				m_quickHull.m_horizon.clear();
		}
		if (keyboard->IsKeyPressed(Keys::comma))
		{
			if (keyboard->IsKeyDown(Keys::left_control))
			{
				while (m_quickHull.MergeFaces())
					;
			}
			else
				m_quickHull.MergeFaces();
		}
		//if (keyboard->IsKeyPressed(Keys::space) ||
		//	keyboard->IsKeyPressed(Keys::m))
		//{
		//	m_gjk.simplex.Clear();
		//	m_gjk.direction = Vector3f(1, 1, 1);

		//	m_gjk.done = false;
		//	m_gjk.result = false;
		//	m_gjk.shapeA = m_testBody1->GetCollider();
		//	m_gjk.shapeB = m_testBody2->GetCollider();
		//	m_gjk.iteration = 0;

		//	m_gjk.nextPoint = GJK::GetSupportMinkowskiDiff(
		//		m_gjk.direction, m_gjk.shapeA, m_gjk.shapeB);
		//	m_gjk.simplex.Add(m_gjk.nextPoint);
		//	m_gjk.direction = -m_gjk.nextPoint.p; // Search towards the origin.
		//	
		//	printf("----------------------------------------------------\n");
		//	printf("Beginning new GJK\n");
		//}
	}
	/*
	else if (keyboard->IsKeyPressed(Keys::space) ||
			keyboard->IsKeyDown(Keys::m))
	{
		//bool gjk = GJK::TestIntersection(
			//m_testBody1->GetPhysicsMesh()->GetShape(0),
			//m_testBody2->GetPhysicsMesh()->GetShape(0));
		//printf("GJK RESULT = %d\n", (int) gjk);
		
		if (m_gjk.iteration > 0)
		{
			if (GJK::DoSimplex(m_gjk.simplex, m_gjk.direction))
			{
				m_gjk.done = true;
				m_gjk.result = true;
				printf("GJK passed after %u iterations\n", m_gjk.iteration);
			}
		}

		++m_gjk.iteration;
			
		m_gjk.nextPoint = GJK::GetSupportMinkowskiDiff(
			m_gjk.direction, m_gjk.shapeA, m_gjk.shapeB);
			
		if (m_gjk.nextPoint.p.Dot(m_gjk.direction) < 0.0f)
		{
			m_gjk.done = true;
			m_gjk.result = false;
			printf("GJK failed after %u iterations\n", m_gjk.iteration);
			m_gjk.simplex.Add(m_gjk.nextPoint);
		}

		if (!m_gjk.done)
		{
			m_gjk.simplex.Add(m_gjk.nextPoint);
			printf("%u. Simplex size = %u\n", m_gjk.iteration, m_gjk.simplex.GetNumPoints());
		}
	}
	*/

	
	// F3: Print profiling info.
	if (keyboard->IsKeyPressed(Keys::f3))
		m_physicsEngine.GetProfiler()->Print(std::cout);
	
	//if (keyboard->IsKeyPressed(Keys::i))
	//{
	//	std::cout << "m_testBody1->SetPosition(Vector3f" << m_testBody1->GetPosition() << ");" << std::endl;
	//	std::cout << "m_testBody1->SetOrientation(Quaternion" << m_testBody1->GetOrientation() << ");" << std::endl;
	//	std::cout << "m_testBody2->SetPosition(Vector3f" << m_testBody2->GetPosition() << ");" << std::endl;
	//	std::cout << "m_testBody2->SetOrientation(Quaternion" << m_testBody2->GetOrientation() << ");" << std::endl;
	//}
	
	float debugMoveSpeed = 1.0f;
	if (keyboard->IsKeyDown(Keys::i))
		m_testBody1->SetPosition(m_testBody1->GetPosition() + debugMoveSpeed * Vector3f::UNITY * timeDelta);
	if (keyboard->IsKeyDown(Keys::k))
		m_testBody1->SetPosition(m_testBody1->GetPosition() - debugMoveSpeed * Vector3f::UNITY * timeDelta);
	if (keyboard->IsKeyDown(Keys::l))
		m_testBody1->SetPosition(m_testBody1->GetPosition() + debugMoveSpeed * Vector3f::UNITX * timeDelta);
	if (keyboard->IsKeyDown(Keys::j))
		m_testBody1->SetPosition(m_testBody1->GetPosition() - debugMoveSpeed * Vector3f::UNITX * timeDelta);
	
	if (keyboard->IsKeyPressed(Keys::t))
	{
		m_simulationPaused = true;
		m_testBody1->SetPosition(Vector3f(0, 2.4051f, 0));
		m_testBody1->SetOrientation(Quaternion(0.0135548f, 0.963575f, -0.0560558f, -0.261147f));
		m_testBody2->SetPosition(Vector3f(0, 2, 0));
		m_testBody2->SetOrientation(Quaternion(0, 0, 0, 1));
	}
	if (keyboard->IsKeyPressed(Keys::y))
	{
		m_simulationPaused = true;
		m_testBody1->SetPosition(Vector3f(0.0512f, 2.36f, -0.064f));
		m_testBody1->SetOrientation(Quaternion(-0.00093867f, 0.119242f, -0.0154401f, 0.992745f));
	}

	// Enter: reset setup.
	if (keyboard->IsKeyPressed(Keys::enter))
	{
		m_physicsEngine.ClearBodies();
		Reset();
	}

	if (keyboard->IsKeyPressed(Keys::f))
	{
		RigidBody* body = new RigidBody();
		body->SetPosition(Vector3f(0, 4.0f, -12.0f));
		body->SetVelocity(Vector3f(0, 0, 0));
		body->SetAngularVelocity(Vector3f(0.0f, 0.0f, 0.0f));
		body->SetMass(400.0f);
		body->SetCollider(new BoxCollider(Vector3f(1, 1, 1)));
		m_physicsEngine.AddBody(body);
	}

	// B: Spawn random rigid body.
	if (keyboard->IsKeyPressed(Keys::b))
	{
		Quaternion randomOrientation(
			RandomFloatClamped(), RandomFloatClamped(),
			RandomFloatClamped(), RandomFloatClamped());
		randomOrientation.Normalize();
		Vector3f halfSize = Vector3f(
			0.2f + RandomFloat() * 0.8f,
			0.2f + RandomFloat() * 0.8f,
			0.2f + RandomFloat() * 0.8f) * 1.5f;
		Collider* collider = nullptr;
		unsigned int shapeIndex = Random::NextInt(0, 5);
		if (shapeIndex == 0)
			collider = new BoxCollider(halfSize);
		else if (shapeIndex == 1)
			collider = new SphereCollider(halfSize.x);
		else if (shapeIndex == 2)
			collider = new CylinderCollider(halfSize.x, halfSize.y);
		else if (shapeIndex == 3)
			collider = new CapsuleCollider(halfSize.x, halfSize.y);
		else if (shapeIndex == 4)
			collider = new ConeCollider(halfSize.x, halfSize.y * 2.0f);
		float density = 200.0f;
		float volume = collider->GetVolume();

		RigidBody* body = new RigidBody();
		body->SetPosition(Vector3f(0, 4.0f, 0) + 5.0f * Vector3f(RandomFloatClamped(), RandomFloatClamped(), RandomFloatClamped()));
		body->SetOrientation(randomOrientation);
		body->SetVelocity(Vector3f(0, 0, 0));
		body->SetAngularVelocity(Vector3f(0.0f, 0.0f, 0.0f));
		body->SetMass(volume * density);
		//body->SetInverseInertiaTensor(Matrix3f::CreateScale3(1.0f));
		body->SetCollider(collider);
		m_physicsEngine.AddBody(body);
	}
	

	// B: Spawn random rigid body.
	if (keyboard->IsKeyPressed(Keys::n))
	{
		for (unsigned int i = 0; i < 10; ++i)
		{
			RigidBody* body = new RigidBody();
			body->SetPosition(Vector3f(-5, 1.0f + (2.0f * i), 0.0f));
			body->SetMass(10.0f);
			body->SetCollider(new BoxCollider(Vector3f(1.0f)));
			m_physicsEngine.AddBody(body);
		}
	}
	
	// F4: Toggle fullscreen mode
	if (keyboard->IsKeyPressed(Keys::f4))
		GetWindow()->SetFullscreen(!GetWindow()->IsFullscreen());
	
	float thrust = 5.0f;
	Vector3f bodyForward = m_testBody1->GetOrientation().GetForward();
	Vector3f bodyRight = m_testBody1->GetOrientation().GetRight();
	if (keyboard->IsKeyDown(Keys::up))
		m_testBody1->m_velocity += bodyForward * thrust * timeDelta;
	if (keyboard->IsKeyDown(Keys::down))
		m_testBody1->m_velocity -= bodyForward * thrust * timeDelta;
	if (keyboard->IsKeyDown(Keys::right))
		m_testBody1->m_angularVelocity.y -= 3.0f * timeDelta;
	if (keyboard->IsKeyDown(Keys::left))
		m_testBody1->m_angularVelocity.y += 3.0f * timeDelta;
	
	float rotateAmount = timeDelta * 2.0f;
	float moveAmount = timeDelta * 12.0f;
	if (!keyboard->IsKeyDown(Keys::left_control) &&
		!keyboard->IsKeyDown(Keys::right_control))
	{
		if (keyboard->IsKeyDown(Keys::w))
			m_cameraTransform.position += m_cameraTransform.rotation.GetForward() * moveAmount;
		if (keyboard->IsKeyDown(Keys::s))
			m_cameraTransform.position += m_cameraTransform.rotation.GetBack() * moveAmount;
		if (keyboard->IsKeyDown(Keys::a))
			m_cameraTransform.position += m_cameraTransform.rotation.GetLeft() * moveAmount;
		if (keyboard->IsKeyDown(Keys::d))
			m_cameraTransform.position += m_cameraTransform.rotation.GetRight() * moveAmount;
		if (keyboard->IsKeyDown(Keys::e))
			m_cameraTransform.position += Vector3f::UP * moveAmount;
		if (keyboard->IsKeyDown(Keys::q))
			m_cameraTransform.position += Vector3f::DOWN * moveAmount;
	}
	
	if (keyboard->IsKeyDown(Keys::left_shift))
		UpdateDebugBodyControls(m_testBody2, timeDelta);
	else
		UpdateDebugBodyControls(m_testBody1, timeDelta);

	if (mouse->IsButtonDown(MouseButtons::left))
	{
		MouseState currState = mouse->GetMouseState();
		MouseState prevState = mouse->GetPrevMouseState();
		Vector2f mouseDelta((float) (currState.x - prevState.x), (float) (currState.y - prevState.y));
		
		rotateAmount = 0.005f;
		m_cameraTransform.rotation.Rotate(Vector3f::UP, -mouseDelta.x * rotateAmount);
		m_cameraTransform.rotation.Rotate(m_cameraTransform.rotation.GetLeft(), mouseDelta.y * rotateAmount);

		//GetWindow()->SetMouseVisibility(false);
		//GetWindow()->SetMousePosition(1920 / 2, 1080 / 2);
	}
	else
	{
		//GetWindow()->SetMouseVisibility(true);
	}

	if (!m_simulationPaused || simulateSingleFrame)
		m_physicsEngine.Simulate(timeDelta * m_simulationSpeed);
	else
		m_physicsEngine.DebugDetectCollisions();
}

void CollisionTestApp::UpdateDebugBodyControls(RigidBody* body, float timeDelta)
{
	Keyboard* keyboard = GetKeyboard();
	Mouse* mouse = GetMouse();

	Quaternion bodyOrientation = body->GetOrientation();
	Vector3f bodyPos = body->GetPosition();
	
	float rotateAmount = timeDelta * 1.8f;
	float moveAmount = timeDelta * 10.0f;
	
	if (keyboard->IsKeyDown(Keys::left_control) ||
		keyboard->IsKeyDown(Keys::right_control))
	{
		if (keyboard->IsKeyDown(Keys::num8))
			bodyOrientation.Rotate(Vector3f::RIGHT, -rotateAmount);
		if (keyboard->IsKeyDown(Keys::num5))
			bodyOrientation.Rotate(Vector3f::RIGHT, rotateAmount);
		if (keyboard->IsKeyDown(Keys::num4))
			bodyOrientation.Rotate(Vector3f::UP, -rotateAmount);
		if (keyboard->IsKeyDown(Keys::num6))
			bodyOrientation.Rotate(Vector3f::UP, rotateAmount);
		if (keyboard->IsKeyDown(Keys::num7))
			bodyOrientation.Rotate(Vector3f::FORWARD, -rotateAmount);
		if (keyboard->IsKeyDown(Keys::num9))
			bodyOrientation.Rotate(Vector3f::FORWARD, rotateAmount);
	}
	else
	{
		if (keyboard->IsKeyDown(Keys::num8))
			bodyPos += Vector3f::UP * moveAmount;
		if (keyboard->IsKeyDown(Keys::num5))
			bodyPos += Vector3f::DOWN * moveAmount;
		if (keyboard->IsKeyDown(Keys::num4))
			bodyPos += Vector3f::LEFT * moveAmount;
		if (keyboard->IsKeyDown(Keys::num6))
			bodyPos += Vector3f::RIGHT * moveAmount;
		if (keyboard->IsKeyDown(Keys::num7))
			bodyPos += Vector3f::BACK * moveAmount;
		if (keyboard->IsKeyDown(Keys::num9))
			bodyPos += Vector3f::FORWARD * moveAmount;
	}
	
	Vector3f forward = m_cameraTransform.rotation.GetForward();
	forward.y = 0.0f;
	forward.Normalize();
	Vector3f right = m_cameraTransform.rotation.GetRight();

	if (keyboard->IsKeyDown(Keys::left_control))
	{
		if (keyboard->IsKeyDown(Keys::w))
			bodyPos += forward * moveAmount;
		if (keyboard->IsKeyDown(Keys::s))
			bodyPos -= forward * moveAmount;
		if (keyboard->IsKeyDown(Keys::a))
			bodyPos -= right * moveAmount;
		if (keyboard->IsKeyDown(Keys::d))
			bodyPos += right * moveAmount;
		if (keyboard->IsKeyDown(Keys::e))
			bodyPos += Vector3f::UP * moveAmount;
		if (keyboard->IsKeyDown(Keys::q))
			bodyPos += Vector3f::DOWN * moveAmount;
		
		if (mouse->IsButtonDown(MouseButtons::left) ||
			mouse->IsButtonDown(MouseButtons::right))
		{
			MouseState currState = mouse->GetMouseState();
			MouseState prevState = mouse->GetPrevMouseState();
			Vector2f mouseDelta((float) (currState.x - prevState.x), (float) (currState.y - prevState.y));
		
			rotateAmount = 0.005f;
			bodyOrientation.Rotate(Vector3f::UP, mouseDelta.x * rotateAmount);
			bodyOrientation.Rotate(m_cameraTransform.rotation.GetLeft(), -mouseDelta.y * rotateAmount);
		}
	}

	body->SetOrientation(bodyOrientation);
	body->SetPosition(bodyPos);
}

static void GetColliderVertices(std::vector<Vector3f>& vertices, Collider* collider)
{
	ColliderType type = collider->GetType();
	if (type == ColliderType::k_box)
	{
		BoxCollider* box = (BoxCollider*) collider;
		vertices.resize(8);
		for (unsigned int i = 0; i < 8; ++i)
		{
			Vector3f v = box->GetHalfSize();
			if (i & 1) v.x = -v.x;
			if (i & 2) v.y = -v.y;
			if (i & 4) v.z = -v.z;
			vertices[i] = box->GetShapeToWorld().TransformAffine(v);
		}
	}
}

void CollisionTestApp::CreateMinkowskiDifferencePointCloud(
	std::vector<Vector3f>& points, Collider* colliderA, Collider* colliderB)
{
	std::vector<Vector3f> verticesA;
	GetColliderVertices(verticesA, colliderA);
	std::vector<Vector3f> verticesB;
	GetColliderVertices(verticesB, colliderB);

	points.resize(verticesA.size() * verticesB.size());

	unsigned int i, j;
	unsigned int index = 0;
	for (i = 0; i < verticesA.size(); ++i)
	{
		for (j = 0; j < verticesB.size(); ++j)
		{
			points[index++] = verticesA[i] - verticesB[j];
		}
	}
}

void CollisionTestApp::OnRender()
{
	glEnable(GL_DEPTH_TEST);
    glDepthMask(true);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
    //glDepthMask(false);
    //glDisable(GL_CULL_FACE);
    //glDisable(GL_DEPTH_TEST);
    //glDisable(GL_DEPTH_CLAMP);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	Vector3f backgroundColor = Vector3f(0.5f, 0.6f, 0.7f);

	glViewport(0, 0, GetWindow()->GetWidth(), GetWindow()->GetHeight());
	glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_cameraProjection = Matrix4f::CreatePerspective(
		1.5f, GetWindow()->GetAspectRatio(), 0.01f, 100.0f);
	Matrix4f viewMatrix = m_cameraTransform.GetInvMatrix();
	Matrix4f viewProjection = m_cameraProjection * viewMatrix;

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(viewProjection.data());
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	m_debugDraw->SetViewProjection(viewProjection);
	m_debugDraw->SetShaded(true);

	CollisionData collisionData;
	//collisionData.firstBody = m_testBody1;
	//collisionData.secondBody = m_testBody2;
	//collisionData.numContacts = 0;
	//CollisionBox* firstBox = (CollisionBox*) m_testBody1->GetPrimitive();
	//CollisionBox* secondBox = (CollisionBox*) m_testBody2->GetPrimitive();
	//m_physicsEngine.CollideBoxAndBox(*firstBox, *secondBox, &collisionData);
	//firstBox = (CollisionBox*) collisionData.firstBody->GetPrimitive();
	//secondBox = (CollisionBox*) collisionData.secondBody->GetPrimitive();
	/*collisionData.numContacts = 0;
	collisionData.firstBody = m_testBody1;
	collisionData.secondBody = m_testBody2;
	m_physicsEngine.CollideBoxAndBox(*firstBox, *secondBox, &collisionData);
	RigidBody* firstBody = collisionData.firstBody;*/
	
	// Draw the grid.
	DrawGrid();

	m_testBody1->CalculateDerivedData();
	m_testBody2->CalculateDerivedData();

	// Draw the two bodies.
	DrawRigidBody(m_testBody1);
	DrawRigidBody(m_testBody2);

	DrawQuickHull(&m_quickHull);

	// Draw all rigid bodies.
	for (auto it = m_physicsEngine.bodies_begin();
		it != m_physicsEngine.bodies_end(); it++)
	{
		DrawRigidBody(*it);
	}

	//m_debugDraw->DrawFilledCollider(m_testBody1->GetCollider(), Color::RED);
	//m_debugDraw->DrawFilledCollider(m_testBody2->GetCollider(), Color::BLUE);

	glUseProgram(0);
	glClear(GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Draw simplex.
	m_gjk.result = GJK::TestIntersection(
		m_testBody1->GetCollider(),
		m_testBody2->GetCollider(),
		&m_gjk.simplex);
	m_gjk.done = true;
	if (m_gjk.result)
	{
		m_epa = EPA::PerformEPA(
			m_testBody1->GetCollider(),
			m_testBody2->GetCollider(),
			m_gjk.simplex);
	}

	//DrawSimplex(m_gjk.simplex);
		
	// Draw center of masses.
	for (auto it = m_physicsEngine.bodies_begin();
		it != m_physicsEngine.bodies_end(); it++)
	{
		RigidBody* body = *it;
		if (body->GetInverseMass() > 0.0f)
		{
			glDisable(GL_DEPTH_TEST);
			glPointSize(3.0f);
			glBegin(GL_POINTS);
				glColor4ubv(Color::MAGENTA.data());
				glVertex3fv(body->GetCenterOfMassWorld().data());
			glEnd();
			glEnable(GL_DEPTH_TEST);
		}
	}

	// Draw contacts.
	if (m_showContacts)
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//for (auto it = m_physicsEngine.collisions_begin();
			//it != m_physicsEngine.collisions_end(); it++)
		for (auto it = m_physicsEngine.GetCollisionCache()->collisions_begin();
			it != m_physicsEngine.GetCollisionCache()->collisions_end(); it++)
		{
			collisionData = it->second;

			for (unsigned int j = 0; j < collisionData.numContacts; j++)
			{
				const Contact& contact = collisionData.contacts[j];

				Color contactColor = Color::BLUE;
				if (contact.contactType == ContactType::k_debug)
					contactColor = Color::MAGENTA;

				DrawContactPoint(contact);
			}
		}
	}
	
	//CollisionBox* firstBox = (CollisionBox*) m_testBody1->GetPrimitive();
	//CollisionBox* secondBox = (CollisionBox*) m_testBody2->GetPrimitive();
	//DrawSATViewGrid(firstBox, secondBox);
}

void CollisionTestApp::DrawGrid()
{
	int resolution = 20;
	float squareSize = 1.0f;
	float extent = squareSize * (resolution / 2);

	m_debugDraw->BeginImmediate();
	glBegin(GL_LINES);
	glColor4ubv(Color::GREEN.data());
	glVertex3f(0.0f, -0.5f, 0.0f);
	glVertex3f(0.0f, 0.5f, 0.0f);
	for (int i = 0; i <= resolution; ++i)
	{
		float x = (i - (resolution / 2)) * squareSize;

		Color color = Color::GRAY;
		if (i == 0 || i == resolution)
			color = Color::BLACK;
		else if (i == resolution / 2)
			color = Color::WHITE;
		glColor4ubv(color.data());

		if (i == resolution / 2)
			glColor4ubv(Color::BLUE.data());
		glVertex3f(x, 0.0f, -extent);
		glVertex3f(x, 0.0f, extent);
		
		if (i == resolution / 2)
			glColor4ubv(Color::RED.data());
		glVertex3f(-extent, 0.0f, x);
		glVertex3f(extent, 0.0f, x);
	}
	glEnd();
}

void CollisionTestApp::DrawSimplex(const Simplex& simplex)
{
	// Draw the origin.
	float originRadius = 0.3f;
	glLineWidth(1.0f);
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(-originRadius, 0, 0); glVertex3f(originRadius, 0, 0);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0, -originRadius, 0); glVertex3f(0, originRadius, 0);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0, 0, -originRadius); glVertex3f(0, 0, originRadius);
	glEnd();

	Vector3f simplexPointColors[4] = {
		Vector3f(1,0,0), // red
		Vector3f(0,1,0), // green
		Vector3f(0,0,1), // blue
		Vector3f(1,1,0), // yellow
	};
	
	Vector3f ab = simplex.b.p - simplex.a.p;
	Vector3f ac = simplex.c.p - simplex.a.p;
	Vector3f ad = simplex.d.p - simplex.a.p;
	//Vector3f ao = -simplex.a.p;
	Vector3f abc = -ab.Cross(ac);
	Vector3f acd = ac.Cross(ad);
	Vector3f adb = ad.Cross(ab);

	// Draw contact point
	if (m_gjk.result && m_epa.passed)
	{
		glPointSize(8.0f);
		glBegin(GL_POINTS);
		//glColor3f(1.0f, 1.0f, 0.0f);
		//glVertex3fv(m_epa.contactPoint.v);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3fv(m_epa.contactPointA.v);
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3fv(m_epa.contactPointB.v);
		glEnd();
		
		glLineWidth(4.0f);
		glBegin(GL_LINES);
		glColor3f(1.0f, 1.0f, 0.5f);
		glVertex3fv(m_epa.contactPoint.v);
		glVertex3fv((m_epa.contactPoint + m_epa.normal * m_epa.depth).v);
		glEnd();
	}

	// Draw simplex points.
	if (simplex.GetNumPoints() >= 1)
	{
		glPointSize(8.0f);
		glBegin(GL_POINTS);
		glColor3f(1.0f, 0.0f, 0.0f);
		for (unsigned int i = 0; i < simplex.numPoints; ++i)
		{
			glColor3fv(simplexPointColors[i].v);
			glVertex3fv(simplex.points[i].p.v);
		}
		glEnd();
	}
	
	// Draw simplex lines.
	if (simplex.numPoints >= 2)
	{
		glLineWidth(4.0f);
		glBegin(GL_LINES);
		glColor3f(1.0f, 0.25f, 0.25f);
		for (unsigned int i = 0; i < simplex.numPoints; ++i)
		{
			for (unsigned int j = i + 1; j < simplex.numPoints; ++j)
			{
				glVertex3fv(simplex.points[i].p.v);
				glVertex3fv(simplex.points[j].p.v);
			}
		}
		glEnd();
	}
	
	if (simplex.numPoints == 3)
	{
		glBegin(GL_LINES);
		glColor3f(1,0,1);
			glVertex3fv(simplex.a.p.v);
			glVertex3fv((simplex.a.p + abc).v);
		glColor3f(0,1,0);
			glVertex3fv(simplex.a.p.v);
			glVertex3fv((simplex.a.p + abc.Cross(ab)).v);
		glColor3f(0,0,1);
			glVertex3fv(simplex.a.p.v);
			glVertex3fv((simplex.a.p + abc.Cross(-ac)).v);
		glEnd();
	}

	// Draw simplex faces.
	if (simplex.numPoints >= 3)
	{
		unsigned int count = (simplex.numPoints == 3 ? 1 : 4);
		
		glDisable(GL_CULL_FACE);
		glBegin(GL_TRIANGLES);
		glColor3f(1.0f, 0.5f, 0.5f);
		if (m_gjk.result)
			glColor3f(0.5f, 1.0f, 0.5f);
		for (unsigned int i = 0; i < count; ++i)
		{
			unsigned int i0 = (i + 0) % 4;
			unsigned int i1 = (i + 1) % 4;
			unsigned int i2 = (i + 2) % 4;

			glVertex3fv(simplex.points[i0].p.v);
			glVertex3fv(simplex.points[i1].p.v);
			glVertex3fv(simplex.points[i2].p.v);
		}
		glEnd();
		glEnable(GL_CULL_FACE);
	}
}

void CollisionTestApp::DrawContactPoint(const Contact& contact)
{
	Color color = Color::BLUE;
	
	Vector3f localToGlobalA = contact.bodyA->GetBodyToWorld().TransformAffine(contact.localPositionA);
	Vector3f localToGlobalB = contact.bodyB->GetBodyToWorld().TransformAffine(contact.localPositionB);
	//localToGlobalA = contact.worldPositionA;
	//localToGlobalB = contact.worldPositionB;

	glDisable(GL_DEPTH_TEST);
	glPointSize(8.0f);
	glBegin(GL_POINTS);
		//glColor4ubv(color.data());
		//glVertex3fv(contact.contactPoint.v);
		glColor4ubv(Color::RED.data());
		glVertex3fv(localToGlobalA.v);
		glColor4ubv(Color::BLUE.data());
		glVertex3fv(localToGlobalB.v);
	glEnd();

	Color normalColor = Color::YELLOW;
	if (contact.penetration < 0.0f)
		normalColor = Color::CYAN;
	
	glLineWidth(1.0f);
	glBegin(GL_LINES);
		glColor4ubv(Color::RED.data());
		glVertex3fv(localToGlobalA.v);
		glColor4ubv(Color::BLUE.data());
		glVertex3fv(localToGlobalB.v);
		glColor4ubv(normalColor.data());
		glVertex3fv(localToGlobalB.v);
		glVertex3fv((localToGlobalB - contact.contactNormal * contact.penetration).data());
		//glColor4ubv(color.data());
		//glVertex3fv(contact.contactPoint.v);
		//glVertex3fv((contact.contactPoint + contact.contactNormal * contact.penetration).data());
	glEnd();
	glEnable(GL_DEPTH_TEST);
}

void CollisionTestApp::DrawRigidBody(RigidBody* body)
{
	// Setup the model matrix.
	Matrix4f modelMatrix = Matrix4f::CreateTranslation(body->GetPosition()) *
		Matrix4f::CreateRotation(body->GetOrientation());

	// Create a random color, seeded by the object ID.
	unsigned char minColor = 100;
	unsigned char colorAdd = 256 - minColor;
	
	RandomNumberGenerator random(body->GetId());

	Color color;
	color.r = minColor + random.NextInt(0, colorAdd);
	color.g = minColor + random.NextInt(0, colorAdd);
	color.b = minColor + random.NextInt(0, colorAdd);
	color.a = 255;

	if (body->GetCollider() != nullptr)
		m_debugDraw->DrawFilledCollider(body->GetCollider(), color);

	// Draw the object primitives.
	//for (auto it = body->primitives_begin(); it != body->primitives_end(); ++it)
	//{
		//DrawPrimitive(*it, color, modelMatrix);
	//}
	//CollisionPrimitive* primitive = body->GetPrimitive();
	//DrawPrimitive(primitive, color, modelMatrix);
}


void CollisionTestApp::DrawQuickHull(QuickHull* hull)
{
	unsigned int i;
	QuickHullFace* face;
	QuickHullEdge* edge;
	QuickHullVertex* vertex;

	m_debugDraw->BeginImmediate(Matrix4f::IDENTITY);

	m_debugDraw->SetLineWidth(1.0f);
	m_debugDraw->SetPointSize(8.0f);

	Color colorFace = Color::WHITE;
	Color colorEdge = Color::BLACK;
	Color colorHullVertex = Color::BLACK;
	Color colorConflictVertex = Color::BLUE;
	Color colorHorizon = Color::RED;
	
	// Draw hull vertices.
	glBegin(GL_POINTS);
	for (vertex = hull->m_hullVertices.Head();
		vertex != nullptr; vertex = vertex->next)
	{
		glColor4ubv(colorHullVertex.data());

		//if (m_quickHull.m_mergedFace != nullptr &&
			//m_quickHull.m_mergedFace->FindConvexVertex() == vertex)
			//glColor4ubv(Color::GREEN.data());

		glVertex3fv(vertex->position.v);
	}
	glEnd();

	// Draw conflict vertices.
	for (vertex = hull->m_conflictVertices.Head();
		vertex != nullptr; vertex = vertex->next)
	{
		Vector3f posOnFace = vertex->position -
			(vertex->distToFace * vertex->closestFace->normal);

		glBegin(GL_POINTS);
			glColor4ubv(colorConflictVertex.data());
			glVertex3fv(vertex->position.v);
		glEnd();
		glBegin(GL_LINES);
			glColor4ubv(colorConflictVertex.data());
			glVertex3fv(vertex->position.v);
			glVertex3fv(posOnFace.v);
		glEnd();
	}

	// Draw horizon edges.
	m_debugDraw->SetLineWidth(5.0f);
	glBegin(GL_LINES);
	glColor4ubv(colorHorizon.data());
	unsigned int index = 0;
	for (auto it = hull->m_horizon.begin(); it != hull->m_horizon.end(); ++it)
	{
		float t = ((float) index) / ((float) hull->m_horizon.size() - 1);
		edge = *it;
		glColor4ubv(Color::Lerp(Color::RED, Color::YELLOW, t).data());
		glVertex3fv(edge->tail->position.v);
		glVertex3fv(edge->twin->tail->position.v);
		++index;
	}
	glEnd();

	// Draw merged edges.
	m_debugDraw->SetLineWidth(5.0f);
	for (auto it = hull->m_mergedEdges.begin(); it != hull->m_mergedEdges.end(); ++it)
	{
		edge = *it;
		glBegin(GL_LINES);
			glColor4ubv(Color::MAGENTA.data());
			glVertex3fv(edge->tail->position.v);
			glVertex3fv(edge->twin->tail->position.v);
		glEnd();
		glBegin(GL_POINTS);
			glColor4ubv(Color::MAGENTA.data());
			glVertex3fv(edge->twin->face->centroid.v);
		glEnd();
		++index;
	}

	// Draw face outlines.
	m_debugDraw->SetLineWidth(1.0f);
	for (face = hull->m_hullFaces.Head();
		face != nullptr; face = face->next)
	{
		glBegin(GL_LINES);
		glColor4ubv(colorEdge.data());
		for (edge = face->edge, i = 0; i < face->numEdges;
			edge = edge->next, ++i)
		{
			glColor4ubv(colorEdge.data());
			QuickHullConvexity convexity = edge->CalcConvexity(m_quickHull.m_tolerance);
			if (convexity == QuickHullConvexity::k_coplanar)
				glColor4ubv(Color::DARK_MAGENTA.data());
			if (convexity == QuickHullConvexity::k_concave)
				glColor4ubv(Color::RED.data());
			//if (m_quickHull.m_mergedFace == face)
				//glColor4ubv(Color::CYAN.data());
			glVertex3fv(edge->tail->position.v);
			glVertex3fv(edge->next->tail->position.v);
		}
		glEnd();
	}	

	// Draw filled faces.
	for (face = hull->m_hullFaces.Head();
		face != nullptr; face = face->next)
	{
		glBegin(GL_TRIANGLE_FAN);
		Color faceColor = colorFace;
		//if (m_quickHull.m_mergedFace == face)
			//faceColor = Color(255, 210, 210);
		glColor4ubv(m_debugDraw->GetShadedColor(face->normal, faceColor).data());
		for (edge = face->edge, i = 0; i < face->numEdges;
			edge = edge->next, ++i)
		{
			glVertex3fv(edge->tail->position.v);
		}
		glEnd();

		
		glBegin(GL_TRIANGLE_FAN);
		faceColor = Color::DARK_MAGENTA;
		glColor4ubv(m_debugDraw->GetShadedColor(-face->normal, faceColor).data());
		for (edge = face->edge, i = 0; i < face->numEdges;
			edge = edge->prev, ++i)
		{
			glVertex3fv(edge->tail->position.v);
		}
		glEnd();
	}

	//// Draw face normals.
	//for (face = hull->m_hullFaces.Head();
	//	face != nullptr; face = face->next)
	//{
	//	Vector3f center = face->GetCenter();

	//	glBegin(GL_POINTS);
	//	glColor4ubv(Color::CYAN.data());
	//	glVertex3fv(center.v);
	//	glEnd();
	//	glBegin(GL_LINES);
	//	glColor4ubv(Color::CYAN.data());
	//	glVertex3fv(center.v);
	//	glVertex3fv((center + face->normal).v);
	//	glEnd();
	//}
}
