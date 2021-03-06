#include "PhysicsApp.h"
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



PhysicsApp::PhysicsApp()
{

}

void PhysicsApp::OnInitialize()
{
	InputManager* inputManager = GetInputManager();

	Joystick* joystick = inputManager->AddDevice<Joystick>();



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
	m_font = SpriteFont::LoadSpriteFont(Path(ASSETS_PATH "font_console.png"), 16, 8, 12, 0);

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

	m_debugDraw = new DebugDraw();
		
	m_cameraTransform.position = Vector3f(0, 4, 3);
	m_cameraTransform.rotation = Quaternion(Vector3f::UNITX, -0.3f);

	Reset();
}

void PhysicsApp::Reset()
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
	m_physicsEngine.AddBody(body);
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
	body->SetCollider(new PolygonCollider(polyVerts.size(), polyVerts.data()));
	//body->SetMass(1.0f);
	//body->SetInverseInertiaTensor(Matrix3f::CreateScale(1.0f));
	m_physicsEngine.AddBody(body);
	m_testBody2 = body;

	//float wallWidth = 0.1f;
	//float wallHeight = 2.0f;
	//float groundHalfSize = 1.5f;
	//float groundHeight = 0.1f;
	float wallWidth = 0.5f;
	float wallHeight = 2.0f;
	float groundHalfSize = 34.0f;
	float groundHeight = 0.3f;
	
	// Floor
	body = new RigidBody();
	body->SetInverseMass(0.0f);
	body->SetInverseInertiaTensor(Matrix3f::ZERO);
	//body->SetInverseInertiaTensor(Matrix3f::CreateScale(1.0f, 1.0f, 1.0f));
	body->SetPosition(Vector3f(0, -groundHeight * 0.5f, 0));
	//body->SetOrientation(Quaternion(Vector3f::UNITZ, 0.0f));
	//body->SetOrientation(Quaternion(Vector3f::UNITZ, 0.1f));
	body->SetAngularVelocity(Vector3f(0.0f, 0.0f, 0.0f));
	body->SetCollider(new BoxCollider(Vector3f(groundHalfSize, groundHeight * 0.5f, groundHalfSize)));
	m_physicsEngine.AddBody(body);
	
	// Ramp
	body = new RigidBody();
	body->SetInverseMass(0.0f);
	body->SetPosition(Vector3f(0.0f, 1.0f, -12.0f));
	body->SetOrientation(Quaternion(Vector3f::UNITX, 0.3f));
	body->SetAngularVelocity(Vector3f(0.0f, 0.0f, 0.0f));
	body->SetCollider(new BoxCollider(Vector3f(4.0f, 0.3f, 6.0f)));
	m_physicsEngine.AddBody(body);

	// Walls
	body = new RigidBody();
	body->SetInverseMass(0.0f);
	body->SetPosition(Vector3f(-groundHalfSize + (wallWidth * 0.5f), wallHeight * 0.5f, 0));
	body->SetCollider(new BoxCollider(Vector3f(wallWidth * 0.5f, wallHeight * 0.5f, groundHalfSize)));
	m_physicsEngine.AddBody(body);
	body = new RigidBody();
	body->SetInverseMass(0.0f);
	body->SetPosition(Vector3f(groundHalfSize - (wallWidth * 0.5f), wallHeight * 0.5f, 0));
	body->SetCollider(new BoxCollider(Vector3f(wallWidth * 0.5f, wallHeight * 0.5f, groundHalfSize)));
	m_physicsEngine.AddBody(body);
	body = new RigidBody();
	body->SetInverseMass(0.0f);
	body->SetPosition(Vector3f(0, wallHeight * 0.5f, groundHalfSize - (wallWidth * 0.5f)));
	body->SetCollider(new BoxCollider(Vector3f(groundHalfSize - wallWidth, wallHeight * 0.5f, wallWidth * 0.5f)));
	m_physicsEngine.AddBody(body);
	body = new RigidBody();
	body->SetInverseMass(0.0f);
	body->SetPosition(Vector3f(0, wallHeight * 0.5f, -groundHalfSize + (wallWidth * 0.5f)));
	body->SetCollider(new BoxCollider(Vector3f(groundHalfSize - wallWidth, wallHeight * 0.5f, wallWidth * 0.5f)));
	m_physicsEngine.AddBody(body);

	// Create walls.
	//body = new RigidBody();
	//body->SetInverseMass(0.0f);
	//body->SetInverseInertiaTensor(Matrix3f::ZERO);
	//body->SetPosition(Vector3f(0, 0, 0));
	//body->SetOrientation(Quaternion::IDENTITY);
	//body->SetPrimitive(&m_physGround);
	//m_physicsEngine.AddBody(body);

	m_gjk.done = true;
	m_gjk.result = false;
	m_gjk.simplex.Clear();
	m_gjk.shapeA = m_testBody1->GetCollider();
	m_gjk.shapeB = m_testBody2->GetCollider();
}

void PhysicsApp::OnQuit()
{
	delete m_debugDraw;
	m_debugDraw = nullptr;
}

void PhysicsApp::OnResizeWindow(int width, int height)
{
}

void PhysicsApp::OnUpdate(float timeDelta)
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

		if (keyboard->IsKeyPressed(Keys::space) ||
			keyboard->IsKeyPressed(Keys::m))
		{
			m_gjk.simplex.Clear();
			m_gjk.direction = Vector3f(1, 1, 1);

			m_gjk.done = false;
			m_gjk.result = false;
			m_gjk.shapeA = m_testBody1->GetCollider();
			m_gjk.shapeB = m_testBody2->GetCollider();
			m_gjk.iteration = 0;

			m_gjk.nextPoint = GJK::GetSupportMinkowskiDiff(
				m_gjk.direction, m_gjk.shapeA, m_gjk.shapeB);
			m_gjk.simplex.Add(m_gjk.nextPoint);
			m_gjk.direction = -m_gjk.nextPoint.p; // Search towards the origin.
			
			printf("----------------------------------------------------\n");
			printf("Beginning new GJK\n");
		}
	}
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

	// Enter: reset test body.
	if (keyboard->IsKeyPressed(Keys::enter))
	{
		m_testBody1->SetPosition(Vector3f(0, 2, 14));
		//m_testBody1->SetOrientation(Quaternion(Vector3f::UNITZ, 0.0f));
		m_testBody1->SetOrientation(Quaternion::IDENTITY);
		m_testBody1->SetPosition(Vector3f(0, 3.0f, 0));
		m_testBody1->SetVelocity(Vector3f(0, 1, 0));
		m_testBody1->SetAngularVelocity(Vector3f(0.0f, 0.0f, 0.0f));
	}
	// Backspace: Clear all bodies.
	if (keyboard->IsKeyPressed(Keys::backspace))
	{
		m_physicsEngine.ClearBodies();
		Reset();
		//for (unsigned int i = 0; i < m_physicsEngine.GetNumBodies(); ++i)
		//{
		//	RigidBody* body = m_physicsEngine.GetBody(i);
		//	if (body != m_testBody1 && body != m_testBody2)
		//	{
		//		m_physicsEngine.RemoveBody(body);
		//		--i;
		//	}
		//}
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

void PhysicsApp::UpdateDebugBodyControls(RigidBody* body, float timeDelta)
{
	Keyboard* keyboard = GetKeyboard();
	Mouse* mouse = GetMouse();

	Quaternion bodyOrientation = m_testBody1->GetOrientation();
	Vector3f bodyPos = m_testBody1->GetPosition();
	
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

	m_testBody1->SetOrientation(bodyOrientation);
	m_testBody1->SetPosition(bodyPos);
}

void PhysicsApp::OnRender()
{
	glEnable(GL_DEPTH_TEST);
    glDepthMask(true);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    //glDepthMask(false);
    //glDisable(GL_CULL_FACE);
    //glDisable(GL_DEPTH_TEST);
    //glDisable(GL_DEPTH_CLAMP);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
	glViewport(0, 0, GetWindow()->GetWidth(), GetWindow()->GetHeight());
	glClearColor(0, 0, 0, 1);
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

void PhysicsApp::DrawSimplex(const Simplex& simplex)
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

void PhysicsApp::DrawBoxEdge(const CollisionBox& box, unsigned int vertexIndex1, unsigned int vertexIndex2, const Color& color)
{
	Vector3f v1 = box.GetVertex(vertexIndex1);
	Vector3f v2 = box.GetVertex(vertexIndex2);

	glLineWidth(2.0f);
	glDisable(GL_DEPTH_TEST);
	glBegin(GL_LINES);
		glColor4ubv(color.data());
		//glColor4ubv(c.data());
		glVertex3fv(v1.data());
		glVertex3fv(v2.data());
	glEnd();
	glEnable(GL_DEPTH_TEST);
	glLineWidth(1.0f);
}

void PhysicsApp::DrawBoxFace(const Vector3f& halfSize, const Matrix4f& transform, unsigned int faceIndex, const Color& color)
{
	unsigned int axis = faceIndex / 2;
	unsigned int axis2 = (axis + 1) % 3;
	unsigned int axis3 = (axis + 2) % 3;

	Vector3f lightDir = Vector3f(-1.4f, -1, -1.8f);
	lightDir.Normalize();

	Vector3f normal = Vector3f::ZERO;
	normal[axis] = (faceIndex % 2 == 0 ? 1 : -1.0f);

	Matrix3f m = transform.Get3x3().GetTranspose();
	lightDir = m.TransformVector(lightDir);

	float lightAmount = (normal.Dot(-lightDir) + 1) * 0.5f;
	lightAmount = 0.1f + (lightAmount * 0.9f);	Color c = Color::Lerp(Color::BLACK, color, lightAmount);

	Vector3f vertex = normal * halfSize[axis];
	vertex[axis2] = -halfSize[axis2];
	vertex[axis3] = -halfSize[axis3];
	
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(transform.data());

	glBegin(GL_QUADS);
		glColor4ubv(c.data());
		glVertex3fv(vertex.data());
		vertex[axis2] += 2.0f * halfSize[axis2];
		glVertex3fv(vertex.data());
		vertex[axis3] += 2.0f * halfSize[axis3];
		glVertex3fv(vertex.data());
		vertex[axis2] -= 2.0f * halfSize[axis2];
		glVertex3fv(vertex.data());
	glEnd();
}


void PhysicsApp::DrawDebugBoxInfo(const CollisionBox& box)
{
	
	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPointSize(8.0f);
	/*
	glBegin(GL_POINTS);
	{
		// Draw vertices.
		glColor3f(1,0,1);
		for (unsigned int i = 0; i < 8; i++)
			glVertex3fv(box.GetVertex(i).data());
	
		// Draw center point.
		//glColor3f(0,1,1);
		//glVertex3fv(box.body->GetPosition().data());
	}
	glEnd();*/

	// Draw projections.
	/*Vector3f center = box.body->GetPosition();
	glBegin(GL_LINES);
	glColor3f(0,0,1);
	Vector3f axes[3];
	axes[0] = box.GetAxis(0);
	axes[1] = box.GetAxis(1);
	axes[2] = box.GetAxis(2);
	for (unsigned int i = 0; i < 3; i++)
	{
		Vector3f axis = axes[i];
		axis.Normalize();

		float minProj, maxProj;
		box.ProjectOntoAxis(axis, minProj, maxProj);
		
		glColor3f(0,1,0);
		glVertex3fv((axis * minProj).data());
		glColor3f(0,0,1);
		glVertex3fv((axis * maxProj).data());
	}
	glEnd();*/

	glEnable(GL_DEPTH_TEST);
}

void PhysicsApp::DrawContactPoint(const Contact& contact)
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

void PhysicsApp::DrawContactEdge(const Vector3f& endPoint1, const Vector3f& endPoint2)
{
	Color color = Color::CYAN;

	glLineWidth(3.0f);
	//glDisable(GL_DEPTH_TEST);
	glBegin(GL_LINES);
		glColor4ubv(color.data());
		glVertex3fv(endPoint1.data());
		glVertex3fv(endPoint2.data());
	glEnd();
	//glEnable(GL_DEPTH_TEST);
	glLineWidth(1.0f);
}

void PhysicsApp::DrawRigidBody(RigidBody* body)
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

void PhysicsApp::DrawPrimitive(CollisionPrimitive* primitive, const Color& color, const Matrix4f& modelMatrix)
{
	CollisionPrimitive::primitive_type primitiveType = primitive->GetType();

	// Draw object primitive.
	if (primitiveType == CollisionPrimitiveType::k_box)
	{
		CollisionBox* box = (CollisionBox*) primitive;
		DrawMesh(m_meshCube, color, modelMatrix * primitive->GetBodyToShape() * 
			Matrix4f::CreateScale(box->halfSize));

		//glDisable(GL_DEPTH_TEST);
		//DrawOutlinedBox(box->halfSize, modelMatrix * primitive->GetBodyToShape(), Color::GREEN);
		//glEnable(GL_DEPTH_TEST);

		DrawDebugBoxInfo(*box);
	}
	else if (primitiveType == CollisionPrimitiveType::k_sphere)
	{
		CollisionSphere* sphere = (CollisionSphere*) primitive;
		DrawMesh(m_meshSphere, color, modelMatrix * primitive->GetBodyToShape() * 
			Matrix4f::CreateScale(sphere->radius));

		// Draw some points on the sphere so we can see rotation easier.
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf((modelMatrix * primitive->GetBodyToShape() * 
			Matrix4f::CreateScale(sphere->radius + 0.0008f)).data());
		glPointSize(2.0f);
		glBegin(GL_POINTS);
			glColor3f(0, 0, 0);
			glVertex3fv(Vector3f(1,0,0).data());
			glVertex3fv(Vector3f(-1,0,0).data());
			glVertex3fv(Vector3f(0,1,0).data());
			glVertex3fv(Vector3f(0,-1,0).data());
			glVertex3fv(Vector3f(0,0,1).data());
			glVertex3fv(Vector3f(0,0,-1).data());
			glVertex3fv(Vector3f(-1,-1,-1).Normalize().data());
			glVertex3fv(Vector3f(-1,-1,1).Normalize().data());
			glVertex3fv(Vector3f(-1,1,-1).Normalize().data());
			glVertex3fv(Vector3f(-1,1,1).Normalize().data());
			glVertex3fv(Vector3f(1,-1,-1).Normalize().data());
			glVertex3fv(Vector3f(1,-1,1).Normalize().data());
			glVertex3fv(Vector3f(1,1,-1).Normalize().data());
			glVertex3fv(Vector3f(1,1,1).Normalize().data());
		glEnd();
	}
}

void PhysicsApp::DrawMesh(Mesh* mesh, const Color& color, const Matrix4f& modelMatrix)
{
	glUseProgram(m_shader->GetGLProgram());
	
	Color m_lightColor = Color::WHITE;
	Color m_ambientLight = Color::BLACK;
	Vector3f m_lightDirection = Vector3f(-1.3f, -1.6f, -1.1f).Normalize();

	Matrix4f viewProjection =  m_cameraProjection * m_cameraTransform.GetInvMatrix();
	Matrix4f mvp = viewProjection * modelMatrix;

	int uniformLocation = -1;
	if (m_shader->GetUniformLocation("u_isLit", uniformLocation))
		glUniform1i(uniformLocation, 1);
	if (m_shader->GetUniformLocation("u_color", uniformLocation))
		glUniform4fv(uniformLocation, 1, color.ToVector4f().data());
	if (m_shader->GetUniformLocation("u_mvp", uniformLocation))
		glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, mvp.data());
	if (m_shader->GetUniformLocation("u_model", uniformLocation))
		glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, modelMatrix.data());
	if (m_shader->GetUniformLocation("u_lightColor", uniformLocation))
		glUniform3fv(uniformLocation, 1, m_lightColor.ToVector3f().data());
	if (m_shader->GetUniformLocation("u_ambientLight", uniformLocation))
		glUniform3fv(uniformLocation, 1, m_ambientLight.ToVector3f().data());
	if (m_shader->GetUniformLocation("u_lightDir", uniformLocation))
		glUniform3fv(uniformLocation, 1, m_lightDirection.data());

	//glEnable(GL_CULL_FACE);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glColor4ubv(Color::GREEN.data());
	glBindVertexArray(mesh->GetVertexData()->GetVertexBuffer()->GetGLVertexArray());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->GetIndexData()->GetIndexBuffer()->GetGLIndexBuffer());
	glDrawElements(GL_TRIANGLES, mesh->GetIndexData()->GetCount(), GL_UNSIGNED_INT, (unsigned int*) 0);
	glBindVertexArray(0);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//glDisable(GL_CULL_FACE);
	
	glUseProgram(0);
}


