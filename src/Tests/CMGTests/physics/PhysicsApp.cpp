#include "PhysicsApp.h"
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
	Vector3f chassisSize;
	float wheelTrack	= 1.400f;
	float wheelBase		= 2.400f;
	float mass			= 1000.0f; // kg
	float wheelRadius	= 0.3f;
	chassisSize.x		= 1.630f;
	chassisSize.z		= 4.200f;
	chassisSize.y		= 1.340f;

	RigidBody* body;

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

	m_physSphere = CollisionSphere(0.2f);
	m_physCube = CollisionBox(Vector3f(0.2f, 0.2f, 0.2f));
	m_physGround = CollisionBox(Vector3f(4, 0.1f, 4));

	body = new RigidBody();
	body->SetMass(2.0f);
	body->SetOrientation(Quaternion::IDENTITY);
	//body->SetInverseInertiaTensor(Matrix3f::CreateScale(1.0f, 1.0f, 1.0f));
	body->SetPosition(Vector3f(0, 3.0f, 0));
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
	body->SetPrimitive(new CollisionBox(Vector3f(0.2f, 0.1f, 0.2f)));
	//body->SetCollider(new BoxCollider(Vector3f(0.6f, 0.5f, 0.2f)));
	body->SetCollider(new SphereCollider(0.6f));
	body->SetMass(4.0f);
	body->SetMass(mass);
	body->SetInverseMass(0.0f);
	body->SetInverseInertiaTensor(Matrix3f::ZERO);
	m_physicsEngine.AddBody(body);
	m_testBody1 = body;

	body = new RigidBody();
	body->SetInverseMass(0.0f);
	body->SetInverseInertiaTensor(Matrix3f::ZERO);
	//body->SetInverseInertiaTensor(Matrix3f::CreateScale(1.0f, 1.0f, 1.0f));
	body->SetPosition(Vector3f(0, 2.0f, 0));
	//body->SetOrientation(Quaternion(Vector3f::UNITZ, 0.0f));
	//body->SetOrientation(Quaternion(Vector3f::UNITY, Math::HALF_PI * 0.5f));
	//body->SetAngularVelocity(Vector3f(1.9f, 1.11f,-0.7f));
	//body->SetPrimitive(new CollisionBox(Vector3f(0.2f, 0.2f, 0.8f)));
	body->SetPrimitive(new CollisionBox(Vector3f(0.2f, 0.1f, 0.2f)));
	body->SetCollider(new BoxCollider(Vector3f(0.5f, 0.3f, 0.4f)));
	m_physicsEngine.AddBody(body);
	m_testBody2 = body;

	/*
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
	body->SetPrimitive(new CollisionBox(Vector3f(groundHalfSize, groundHeight * 0.5f, groundHalfSize)));
	m_physicsEngine.AddBody(body);
	
	// Ramp
	body = new RigidBody();
	body->SetInverseMass(0.0f);
	body->SetPosition(Vector3f(0.0f, 1.0f, -12.0f));
	body->SetOrientation(Quaternion(Vector3f::UNITX, 0.3f));
	body->SetAngularVelocity(Vector3f(0.0f, 0.0f, 0.0f));
	body->SetPrimitive(new CollisionBox(Vector3f(4.0f, 0.3f, 6.0f)));
	m_physicsEngine.AddBody(body);

	// Walls
	body = new RigidBody();
	body->SetInverseMass(0.0f);
	body->SetPosition(Vector3f(-groundHalfSize + (wallWidth * 0.5f), wallHeight * 0.5f, 0));
	body->SetPrimitive(new CollisionBox(Vector3f(wallWidth * 0.5f, wallHeight * 0.5f, groundHalfSize)));
	m_physicsEngine.AddBody(body);
	body = new RigidBody();
	body->SetInverseMass(0.0f);
	body->SetPosition(Vector3f(groundHalfSize - (wallWidth * 0.5f), wallHeight * 0.5f, 0));
	body->SetPrimitive(new CollisionBox(Vector3f(wallWidth * 0.5f, wallHeight * 0.5f, groundHalfSize)));
	m_physicsEngine.AddBody(body);
	body = new RigidBody();
	body->SetInverseMass(0.0f);
	body->SetPosition(Vector3f(0, wallHeight * 0.5f, groundHalfSize - (wallWidth * 0.5f)));
	body->SetPrimitive(new CollisionBox(Vector3f(groundHalfSize - wallWidth, wallHeight * 0.5f, wallWidth * 0.5f)));
	m_physicsEngine.AddBody(body);
	body = new RigidBody();
	body->SetInverseMass(0.0f);
	body->SetPosition(Vector3f(0, wallHeight * 0.5f, -groundHalfSize + (wallWidth * 0.5f)));
	body->SetPrimitive(new CollisionBox(Vector3f(groundHalfSize - wallWidth, wallHeight * 0.5f, wallWidth * 0.5f)));
	m_physicsEngine.AddBody(body);

	// Create walls.
	//body = new RigidBody();
	//body->SetInverseMass(0.0f);
	//body->SetInverseInertiaTensor(Matrix3f::ZERO);
	//body->SetPosition(Vector3f(0, 0, 0));
	//body->SetOrientation(Quaternion::IDENTITY);
	//body->SetPrimitive(&m_physGround);
	//m_physicsEngine.AddBody(body);
	*/
		
	m_cameraTransform.position = Vector3f(0, 4, 3);
	m_cameraTransform.rotation.Rotate(Vector3f::UNITX, -0.3f);

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

	m_gjk.done = true;
	m_gjk.result = false;
	m_gjk.simplex.Clear();
	m_gjk.shapeA = m_testBody1->GetCollider();
	m_gjk.shapeB = m_testBody2->GetCollider();

	m_debugDraw = new DebugDraw();
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
		//m_physicsEngine.ClearBodies();
	}

	if (keyboard->IsKeyPressed(Keys::f))
	{
		RigidBody* body = new RigidBody();
		body->SetPosition(Vector3f(0, 4.0f, -12.0f));
		body->SetVelocity(Vector3f(0, 0, 0));
		body->SetAngularVelocity(Vector3f(0.0f, 0.0f, 0.0f));
		body->SetMass(400.0f);
		body->SetPrimitive(new CollisionBox(Vector3f(1, 1, 1)));
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
		CollisionPrimitive* primitive = nullptr;
		if (RandomFloat() > 0.5f)
			primitive = new CollisionBox(halfSize);
		else
			primitive = new CollisionSphere(halfSize.x);
		float density = 200.0f;
		float volume = primitive->GetVolume();

		RigidBody* body = new RigidBody();
		body->SetPosition(Vector3f(0, 4.0f, 0) + 5.0f * Vector3f(RandomFloatClamped(), RandomFloatClamped(), RandomFloatClamped()));
		body->SetOrientation(randomOrientation);
		body->SetVelocity(Vector3f(0, 0, 0));
		body->SetAngularVelocity(Vector3f(0.0f, 0.0f, 0.0f));
		body->SetMass(volume * density);
		body->SetPrimitive(primitive);
		m_physicsEngine.AddBody(body);
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
    glDisable(GL_CULL_FACE);
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
	//for (auto it = m_physicsEngine.bodies_begin();
	//	it != m_physicsEngine.bodies_end(); it++)
	//{
	//	DrawRigidBody(*it);
	//}

	m_debugDraw->SetViewProjection(viewProjection);
	m_debugDraw->SetShaded(true);
	m_debugDraw->DrawFilledCollider(m_testBody1->GetCollider(), Color::RED);
	m_debugDraw->DrawFilledCollider(m_testBody2->GetCollider(), Color::BLUE);

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

	// Draw Minkowski Difference point cloud.
	/*CollisionBox* boxA = (CollisionBox*) m_gjk.shapeA;
	CollisionBox* boxB = (CollisionBox*) m_gjk.shapeB;
	glPointSize(4.0f);
	glBegin(GL_POINTS);
	glColor3f(1.0f, 0.5f, 1.0f);
	for (unsigned int i = 0; i < 8; ++i)
	{
		for (unsigned int j = 0; j < 8; ++j)
		{
			Vector3f minkowskiPoint = boxA->GetVertex(i) - boxB->GetVertex(j);
			glVertex3fv(minkowskiPoint.v);
		}
	}
	glEnd();*/

	Vector3f simplexPointColors[4] = {
		Vector3f(1,0,0), Vector3f(0,1,0), Vector3f(0,0,1), Vector3f(1,1,1) 
	};
	
	Vector3f a = m_gjk.simplex.GetPoint(0).p;
	Vector3f b = m_gjk.simplex.GetPoint(1).p;
	Vector3f c = m_gjk.simplex.GetPoint(2).p;
	Vector3f d = m_gjk.simplex.GetPoint(3).p;
	Vector3f ab = b - a;
	Vector3f ac = c - a;
	Vector3f ad = d - a;
	Vector3f ao = -a;
	Vector3f abc = -ab.Cross(ac);
	Vector3f acd = ac.Cross(ad);
	Vector3f adb = ad.Cross(ab);

	// Draw contact point
	if (m_gjk.result && m_epa.passed)
	{
		glPointSize(8.0f);
		glBegin(GL_POINTS);
		glColor3f(1.0f, 1.0f, 0.0f);
		glVertex3fv(m_epa.contactPoint.v);
		glEnd();
		
		glLineWidth(4.0f);
		glBegin(GL_LINES);
		glColor3f(1.0f, 1.0f, 0.5f);
		glVertex3fv(m_epa.contactPoint.v);
		glVertex3fv((m_epa.contactPoint + m_epa.normal * m_epa.depth).v);
		glEnd();
	}

	// Draw simplex points.
	if (m_gjk.simplex.GetNumPoints() >= 1)
	{
		glPointSize(8.0f);
		glBegin(GL_POINTS);
		glColor3f(1.0f, 0.0f, 0.0f);
		for (unsigned int i = 0; i < m_gjk.simplex.GetNumPoints(); ++i)
		{
			glColor3fv(simplexPointColors[i].v);
			glVertex3fv(m_gjk.simplex.GetPoint(i).p.v);
		}
		glEnd();
	}
	
	// Draw simplex lines.
	if (m_gjk.simplex.GetNumPoints() >= 2)
	{
		glLineWidth(4.0f);
		glBegin(GL_LINES);
		glColor3f(1.0f, 0.25f, 0.25f);
		for (unsigned int i = 0; i < m_gjk.simplex.GetNumPoints(); ++i)
		{
			for (unsigned int j = i + 1; j < m_gjk.simplex.GetNumPoints(); ++j)
			{
				glVertex3fv(m_gjk.simplex.GetPoint(i).p.v);
				glVertex3fv(m_gjk.simplex.GetPoint(j).p.v);
			}
		}
		glEnd();
	}
	
	if (m_gjk.simplex.GetNumPoints() == 3)
	{
		glBegin(GL_LINES);
		glColor3f(1,0,1);
			glVertex3fv(a.v);
			glVertex3fv((a + abc).v);
		glColor3f(0,1,0);
			glVertex3fv(a.v);
			glVertex3fv((a + abc.Cross(ab)).v);
		glColor3f(0,0,1);
			glVertex3fv(a.v);
			glVertex3fv((a + abc.Cross(-ac)).v);
		glEnd();

	}

	// Draw simplex faces.
	if (m_gjk.simplex.GetNumPoints() >= 3)
	{
		unsigned int count = (m_gjk.simplex.GetNumPoints() == 3 ? 1 : 4);
		
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

				glVertex3fv(m_gjk.simplex.GetPoint(i0).p.v);
				glVertex3fv(m_gjk.simplex.GetPoint(i1).p.v);
				glVertex3fv(m_gjk.simplex.GetPoint(i2).p.v);
		}
		glEnd();
		glEnable(GL_CULL_FACE);
	}

	
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
				glVertex3fv(body->GetPosition().data());
			glEnd();
			glEnable(GL_DEPTH_TEST);
		}
	}

	// Draw contacts.
	for (auto it = m_physicsEngine.collisions_begin();
		it != m_physicsEngine.collisions_end(); it++)
	{
		collisionData = *it;

		for (unsigned int j = 0; j < collisionData.numContacts; j++)
		{
			const Contact& contact = collisionData.contacts[j];
		
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			Color contactColor = Color::BLUE;
			if (contact.contactType == ContactType::k_debug)
				contactColor = Color::MAGENTA;

			DrawContactPoint(contact.contactPoint, contact.contactNormal);

			if (contact.contactType == ContactType::k_vertex_face)
			{
				/*
				RigidBody* refBody = contact.bodyB;
				RigidBody* incBody = contact.bodyA;
				Matrix4f modelMatrix = Matrix4f::CreateTranslation(refBody->GetPosition()) *
					Matrix4f::CreateRotation(refBody->GetOrientation());
				glEnable(GL_CULL_FACE);
				DrawBoxFace(
					((CollisionBox*) refBody->GetPrimitive())->halfSize,
					modelMatrix, contact.features.faceIndex[1], Color::Lerp(Color(255, 150, 0), Color::WHITE, 0.3f));
				modelMatrix = Matrix4f::CreateTranslation(incBody->GetPosition()) *
					Matrix4f::CreateRotation(incBody->GetOrientation());
				DrawBoxFace(
					((CollisionBox*) incBody->GetPrimitive())->halfSize,
					modelMatrix, contact.features.faceIndex[0], Color::Lerp(Color::RED, Color::WHITE, 0.3f));
				glDisable(GL_CULL_FACE);*/
			}
			else if (contact.contactType == ContactType::k_edge_edge)
			{
				/*
				CollisionBox* boxA = (CollisionBox*) contact.bodyA->GetPrimitive();
				CollisionBox* boxB = (CollisionBox*) contact.bodyB->GetPrimitive();
				DrawContactEdge(boxA->GetVertex(contact.features.edges[0].vertexIndex[0]),
								boxA->GetVertex(contact.features.edges[0].vertexIndex[1]));
				DrawContactEdge(boxB->GetVertex(contact.features.edges[1].vertexIndex[0]),
								boxB->GetVertex(contact.features.edges[1].vertexIndex[1]));
				*/
			}
		}
	}
	
	//CollisionBox* firstBox = (CollisionBox*) m_testBody1->GetPrimitive();
	//CollisionBox* secondBox = (CollisionBox*) m_testBody2->GetPrimitive();
	//DrawSATViewGrid(firstBox, secondBox);
}

void PhysicsApp::DrawSphere(float radius, const Matrix4f& transform, const Color& color)
{
	Matrix4f modelMatrix = transform * Matrix4f::CreateScale(radius);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(modelMatrix.data());

	glColor4ubv(color.data());
	glBindVertexArray(m_meshSphere->GetVertexData()->GetVertexBuffer()->GetGLVertexArray());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_meshSphere->GetIndexData()->GetIndexBuffer()->GetGLIndexBuffer());
	glDrawElements(GL_TRIANGLES, m_meshSphere->GetIndexData()->GetCount(), GL_UNSIGNED_INT, (unsigned int*) 0);
	glBindVertexArray(0);
}

void PhysicsApp::DrawOutlinedBox(const Vector3f& halfSize, const Matrix4f& transform, const Color& color)
{
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//DrawBox(halfSize, transform, color);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//DrawBox(halfSize, transform, color);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(transform.m);

	glBegin(GL_LINES);
		glColor4ubv(color.data());
		glVertex3f(-halfSize.x,  halfSize.y, -halfSize.z);
		glVertex3f( halfSize.x,  halfSize.y, -halfSize.z);
		glVertex3f( halfSize.x,  halfSize.y, -halfSize.z);
		glVertex3f( halfSize.x,  halfSize.y,  halfSize.z);
		glVertex3f( halfSize.x,  halfSize.y,  halfSize.z);
		glVertex3f(-halfSize.x,  halfSize.y,  halfSize.z);
		glVertex3f(-halfSize.x,  halfSize.y,  halfSize.z);
		glVertex3f(-halfSize.x,  halfSize.y, -halfSize.z);
	
		glVertex3f(-halfSize.x, -halfSize.y, -halfSize.z);
		glVertex3f( halfSize.x, -halfSize.y, -halfSize.z);
		glVertex3f( halfSize.x, -halfSize.y, -halfSize.z);
		glVertex3f( halfSize.x, -halfSize.y,  halfSize.z);
		glVertex3f( halfSize.x, -halfSize.y,  halfSize.z);
		glVertex3f(-halfSize.x, -halfSize.y,  halfSize.z);
		glVertex3f(-halfSize.x, -halfSize.y,  halfSize.z);
		glVertex3f(-halfSize.x, -halfSize.y, -halfSize.z);
	
		glVertex3f(-halfSize.x,  halfSize.y, -halfSize.z);
		glVertex3f(-halfSize.x, -halfSize.y, -halfSize.z);
		glVertex3f( halfSize.x,  halfSize.y, -halfSize.z);
		glVertex3f( halfSize.x, -halfSize.y, -halfSize.z);
		glVertex3f( halfSize.x,  halfSize.y,  halfSize.z);
		glVertex3f( halfSize.x, -halfSize.y,  halfSize.z);
		glVertex3f(-halfSize.x,  halfSize.y,  halfSize.z);
		glVertex3f(-halfSize.x, -halfSize.y,  halfSize.z);
	glEnd();
}

void PhysicsApp::DrawBox(const Vector3f& halfSize, const Matrix4f& transform, const Color& color)
{
	for (unsigned int i = 0; i < 6; i++)
		DrawBoxFace(halfSize, transform, i, color);
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

void PhysicsApp::DrawContactPoint(const Vector3f& contactPoint, const Vector3f& contactNormal)
{
	Color color = Color::BLUE;

	glDisable(GL_DEPTH_TEST);
	glPointSize(8.0f);
	glBegin(GL_POINTS);
		glColor4ubv(color.data());
		glVertex3fv(contactPoint.data());
	glEnd();
	
	glLineWidth(1.0f);
	glBegin(GL_LINES);
		glColor4ubv(color.data());
		glVertex3fv(contactPoint.data());
		glVertex3fv((contactPoint + contactNormal * 0.5f).data());
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



struct SATInfo
{
	Vector3f axes[15];
	
	float smallestOverlap;
	unsigned int smallestOverlapAxis;
	unsigned int axisIndex;
	Vector3f axis;
	bool isColliding;
	
	CollisionBox* one;
	CollisionBox* two;
	CollisionData collisionData;

	SATInfo()
	{}

	SATInfo(PhysicsEngine* physicsEngine, CollisionBox* one, CollisionBox* two) :
		one(one),
		two(two)
	{

		// Face axes from object one->
		axes[0] = one->GetAxis(0);
		axes[1] = one->GetAxis(1);
		axes[2] = one->GetAxis(2);

		// Face axes from object two->
		axes[3] = two->GetAxis(0);
		axes[4] = two->GetAxis(1);
		axes[5] = two->GetAxis(2);

		// Edge-edge axes.
		axes[6] = one->GetAxis(0).Cross(two->GetAxis(0));
		axes[7] = one->GetAxis(0).Cross(two->GetAxis(1));
		axes[8] = one->GetAxis(0).Cross(two->GetAxis(2));
		axes[9] = one->GetAxis(1).Cross(two->GetAxis(0));
		axes[10] = one->GetAxis(1).Cross(two->GetAxis(1));
		axes[11] = one->GetAxis(1).Cross(two->GetAxis(2));
		axes[12] = one->GetAxis(2).Cross(two->GetAxis(0));
		axes[13] = one->GetAxis(2).Cross(two->GetAxis(1));
		axes[14] = one->GetAxis(2).Cross(two->GetAxis(2));
		
		// Perform the Seperating Axis Test for each axis.
		float minOne, maxOne, minTwo, maxTwo;
		smallestOverlap = FLT_MAX;
		isColliding = true;

		for (unsigned int i = 0; i < 15; ++i)
		{
			if (axes[i].LengthSquared() < 0.001f)
				continue;
			axes[i].Normalize();
			axis = axes[i];

			// Project each box onto the axis and determine the overlap.
			one->ProjectOntoAxis(axis, minOne, maxOne);
			two->ProjectOntoAxis(axis, minTwo, maxTwo);
			float overlap = Math::Min(maxOne - minTwo, maxTwo - minOne);
		
			// If there is no overlap, then the two boxes are not colliding.
			if (overlap < 0.0f)
				isColliding = false;

			// Else, keep track of the smallest overlap.
			if (overlap < smallestOverlap)
			{
				smallestOverlap = overlap;
				smallestOverlapAxis = i;
			}
		}

		axisIndex = smallestOverlapAxis;
		axis = axes[smallestOverlapAxis];

		collisionData.firstBody = one->body;
		collisionData.secondBody = two->body;
		collisionData.numContacts = 0;
		physicsEngine->GetCollisionDetector()->CollideBoxAndBox(*one, *two, &collisionData);
	}
};

static SATInfo satInfo;

void PhysicsApp::DrawSATViewGrid(CollisionBox* one, CollisionBox* two)
{
	satInfo = SATInfo(&m_physicsEngine, one, two);

	Vector2f windowSize((float) GetWindow()->GetWidth(), (float) GetWindow()->GetHeight());
	Vector2f panelSize(windowSize.x / 5.0f, windowSize.y / 3.0f);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	

	for (unsigned int i = 0; i < 15; i++)
	{
		unsigned int x = (i / 3);
		unsigned int y = (i % 3);

		Vector2f topLeft = panelSize * Vector2f((float) x, (float) y);
		if (i >= 3 && i < 6)
			DrawSATViewPort(two, one, i, topLeft, panelSize);
		else
			DrawSATViewPort(one, two, i, topLeft, panelSize);
	}
}

void PhysicsApp::DrawSATViewPort(
		CollisionBox* one,
		CollisionBox* two,
		unsigned int axisIndex,
		const Vector2f& viewPortTopLeft,
		const Vector2f& viewPortSize)
{
	// Create a basis vectors, where up represents the SAT axis.
	Vector3f axis = satInfo.axes[axisIndex];
	Vector3f basisY = axis;
	Vector3f basisX = one->GetAxis(0);
	if (Math::Abs(basisY.Dot(basisX)) > 0.9f)
		basisX = one->GetAxis(1);
	basisX.Normalize();
	Vector3f basisZ = basisX.Cross(basisY);
	basisZ.Normalize();
	basisX = basisY.Cross(basisZ);
	basisX.Normalize();
	Matrix4f worldToView(Matrix3f(basisX, basisY, basisZ));

	// Determine the extents of the two bodies along the X/Y plane.
	Vector3f oneMin, oneMax;
	Vector3f twoMin, twoMax;
	one->ProjectOntoAxis(basisX, oneMin.x, oneMax.x);
	one->ProjectOntoAxis(basisY, oneMin.y, oneMax.y);
	one->ProjectOntoAxis(basisZ, oneMin.z, oneMax.z);
	two->ProjectOntoAxis(basisX, twoMin.x, twoMax.x);
	two->ProjectOntoAxis(basisY, twoMin.y, twoMax.y);
	two->ProjectOntoAxis(basisZ, twoMin.z, twoMax.z);

	Vector3f mins, maxs;
	mins.x = Math::Min(oneMin.x, twoMin.x);
	mins.y = Math::Min(oneMin.y, twoMin.y);
	mins.z = Math::Min(oneMin.z, twoMin.z);
	maxs.x = Math::Max(oneMax.x, twoMax.x);
	maxs.y = Math::Max(oneMax.y, twoMax.y);
	maxs.z = Math::Max(oneMax.z, twoMax.z);

	Vector3f halfSize = (maxs - mins) * 0.5f * 1.3f;
	Vector3f center = (mins + maxs) * 0.5f;
	float halfSizeAspectRatio = halfSize.x / halfSize.y;
	float viewPortAspectRatio = viewPortSize.x / viewPortSize.y;
	if (halfSizeAspectRatio > viewPortAspectRatio)
		halfSize.y = halfSize.x / viewPortAspectRatio;
	else
		halfSize.x = halfSize.y * viewPortAspectRatio;
	halfSize.z *= 2.0f;

	halfSize.z = 100;
	Matrix4f viewToProjection = Matrix4f::CreateOrthographic(
		center.x - halfSize.x, center.x + halfSize.x,
		center.y - halfSize.y, center.y + halfSize.y,
		center.z - halfSize.z, center.z + halfSize.z);
	Matrix4f worldToProjection = viewToProjection * worldToView.GetTranspose();
	
	glViewport(
		(int) viewPortTopLeft.x,
		(int) viewPortTopLeft.y,
		(int) viewPortSize.x,
		(int) viewPortSize.y);
	
	Vector3f c = (axis + Vector3f::ONE) * 0.5f;
	//glClearColor(c.x, c.y, c.z, 1);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	
	// Draw bodies.

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(worldToProjection.data());
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Render each body.
	DrawBox(one->halfSize, one->body->m_bodyToWorld * one->GetBodyToShape(), Color::RED);
	DrawBox(two->halfSize, two->body->m_bodyToWorld * one->GetBodyToShape(), Color::BLUE);


	// Draw outline
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	float b = 0.99f;
	glLineWidth(1.0f);
	glBegin(GL_LINE_LOOP);
		glColor3f(1,1,1);
		glVertex2f(-b, -b);
		glVertex2f(b, -b);
		glVertex2f(b, b);
		glVertex2f(-b, b);
	glEnd();

	
	if (satInfo.smallestOverlapAxis == axisIndex)
	{
		b = 0.9f;
		glLineWidth(3.0f);
		glBegin(GL_LINE_LOOP);
			if (satInfo.isColliding)
				glColor3f(1,1,0);
			else
				glColor3f(0.5f,0.5f,0);
			glVertex2f(-b, -b);
			glVertex2f(b, -b);
			glVertex2f(b, b);
			glVertex2f(-b, b);
		glEnd();
	}
}




void PhysicsApp::DrawRigidBody(RigidBody* body)
{
	// Setup the model matrix.
	Matrix4f modelMatrix = Matrix4f::CreateTranslation(body->GetPosition()) *
		Matrix4f::CreateRotation(body->GetOrientation());

	// Create a random color, seeded by the object ID.
	unsigned char minColor = 100;
	unsigned char colorAdd = 256 - minColor;
	srand(body->GetId());
	Color color;
	color.r = minColor + (rand() % colorAdd);
	color.g = minColor + (rand() % colorAdd);
	color.b = minColor + (rand() % colorAdd);
	color.a = 255;

	// Draw the object primitives.
	for (auto it = body->primitives_begin(); it != body->primitives_end(); ++it)
	{
		DrawPrimitive(*it, color, modelMatrix);
	}
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


