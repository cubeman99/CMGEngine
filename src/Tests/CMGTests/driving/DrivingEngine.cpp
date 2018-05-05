#include "DrivingEngine.h"
#include <sstream>
#include  <iomanip>

using namespace std;


//#define ASSETS_PATH "C:/Workspace/C++/Framework Projects/CMGEngine/assets/"
#define ASSETS_PATH "C:/Users/david.jordan/Desktop/CMGEngine/assets/"
//#define ASSETS_PATH "assets/"



DrivingApp::DrivingApp() :
	m_testGraph("Lateral Grip", Color::GREEN),
	m_graphSpeed("Speed (MPH)", Color::YELLOW),
	m_graphLongForce("Long Force", Color::GREEN)
{
	const char* tireNames[4];
	tireNames[Tire::k_front_left] = "FR";
	tireNames[Tire::k_front_right] = "FL";
	tireNames[Tire::k_rear_right] = "RR";
	tireNames[Tire::k_rear_left] = "RL";

	for (unsigned int i = 0; i < 4; i++)
	{
		m_graphLongitudinalGrip[i].SetTitle(String("Long Grip ") + tireNames[i]);
		m_graphLongitudinalGrip[i].SetLineColor(Color::GREEN);
		m_graphLateralGrip[i].SetTitle(String("Lat Grip ") + tireNames[i]);
		m_graphLateralGrip[i].SetLineColor(Color::RED);
	}
}
	
void DrivingApp::OnInitialize()
{
	m_isPaused = false;
	m_simulationSpeed = 1.0f;

	// Longitudinal grip graphs.
	for (float slipRatio = -4.0f; slipRatio <= 4.0f; slipRatio += 0.1f)
	{
		float longGrip = m_car.m_tires[0].m_pacejkaLongitudinal.Calcluate(slipRatio);
		for (unsigned int i = 0; i < 4; i++)
			m_graphLongitudinalGrip[i].AddPoint(slipRatio, longGrip);
	}
	
	// Lateral grip graphs.
	for (float slipAngle = -Math::PI; slipAngle <= Math::PI; slipAngle += 0.1f)
	{
		float latGrip = m_car.m_tires[0].m_pacejkaLateral.Calcluate(slipAngle);
		for (unsigned int i = 0; i < 4; i++)
			m_graphLateralGrip[i].AddPoint(slipAngle, latGrip);
	}

	// Initialize camera.
	m_camera.m_position.Set(0, 8, 3);
	m_camera.m_orientation.SetIdentity();
	m_camera.m_orientation.Rotate(Vector3f::RIGHT, -0.5f);
	m_camera.m_distance = 6.0f;
	m_camera.m_fieldOfView = 1.6f;
	m_camera.m_farPlane = 100.0f;
	m_camera.m_nearPlane = 0.1f;
	m_camera.m_aspectRatio = GetWindow()->GetAspectRatio();
	
	// Load meshes.
	m_meshCube			= Primitives::CreateCube();
	m_meshWheel			= Primitives::CreateCylinder(1.0f, 2.0f, 16, Vector3f::UNITX);
	//m_meshCarChassis	= TestMeshes::LoadMesh(Path(ASSETS_PATH "models/volks.obj"));
	m_meshCarChassis	= TestMeshes::LoadMesh(Path(ASSETS_PATH "models/ae86.obj"));
	m_meshWheel			= TestMeshes::LoadMesh(Path(ASSETS_PATH "models/tire.obj"));
	
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
	
	m_frameIndex = 0;
	m_graphics.Initialize(GetWindow(), m_font);

	for (unsigned int i = 0; i < 4; i++)
	{
		m_trailTires[i].SetColor(Color::WHITE);
		m_trails.push_back(&m_trailTires[i]);
	}
	m_trailCenter.SetColor(Color::WHITE);
	m_trails.push_back(&m_trailCenter);

	m_car.Reset();
}

void DrivingApp::OnQuit()
{
	delete m_font;
	delete m_meshCube;
	delete m_shader;
}

void DrivingApp::OnResizeWindow(int width, int height)
{
	m_camera.m_aspectRatio = GetWindow()->GetAspectRatio();
}

void DrivingApp::OnUpdate(float timeDelta)
{
	Keyboard* keyboard = GetKeyboard();
	Mouse* mouse = GetMouse();
	
	bool simulateSingleFrame = false;

	m_frameIndex++;

	// Escape: Quit game.
	if (keyboard->IsKeyPressed(Keys::escape))
	{
		Quit();
		return;
	}
	// P: Pause/resume simulation.
	if (keyboard->IsKeyPressed(Keys::p))
		m_isPaused = !m_isPaused;
	// Page Up: increase simulation speed.
	if (keyboard->IsKeyPressed(Keys::page_up))
		m_simulationSpeed *= 1.5f;
	// Page Down: decrease simulation speed.
	if (keyboard->IsKeyPressed(Keys::page_down))
		m_simulationSpeed /= 1.5f;
	// Right bracket: simulate a single frame.
	if (keyboard->IsKeyPressed(Keys::right_bracket))
		simulateSingleFrame = true;
	// Insert: toggle camera following.
	if (keyboard->IsKeyPressed(Keys::insert))
		m_enableCameraFollow = !m_enableCameraFollow;
	// F4: Toggle fullscreen mode
	if (keyboard->IsKeyPressed(Keys::f4))
		GetWindow()->SetFullscreen(!GetWindow()->IsFullscreen());
	// Enter: Reset simulation.
	if (keyboard->IsKeyPressed(Keys::enter))
	{
		m_car.Reset();

		m_graphSpeed.Clear();
		m_graphLongForce.Clear();
		for (unsigned int i = 0; i < m_trails.size(); i++)
			m_trails[i]->Clear();
	}

	// Camera movement.
	float rotateAmount = timeDelta * 2.0f;
	float moveAmount = timeDelta * 6.0f;
	//if (keyboard->IsKeyDown(Keys::w))
	//	m_camera.m_position += m_camera.m_orientation.GetForward() * moveAmount;
	//if (keyboard->IsKeyDown(Keys::s))
	//	m_camera.m_position += m_camera.m_orientation.GetBack() * moveAmount;
	//if (keyboard->IsKeyDown(Keys::a))
	//	m_camera.m_position += m_camera.m_orientation.GetLeft() * moveAmount;
	//if (keyboard->IsKeyDown(Keys::d))
	//	m_camera.m_position += m_camera.m_orientation.GetRight() * moveAmount;
	//if (keyboard->IsKeyDown(Keys::e))
	//	m_camera.m_position += Vector3f::UP * moveAmount;
	//if (keyboard->IsKeyDown(Keys::q))
	//	m_camera.m_position += Vector3f::DOWN * moveAmount;
	
	MouseState currState = mouse->GetMouseState();
	MouseState prevState = mouse->GetPrevMouseState();

	// Camera rotation.
	if (mouse->IsButtonDown(MouseButtons::right))
	{
		Vector2f mouseDelta((float) (currState.x - prevState.x), (float) (currState.y - prevState.y));
		
		rotateAmount = 0.005f;
		m_camera.m_orientation.Rotate(Vector3f::UP, -mouseDelta.x * rotateAmount);
		m_camera.m_orientation.Rotate(m_camera.m_orientation.GetLeft(), mouseDelta.y * rotateAmount);
	}
	int wheelDelta = currState.z - prevState.z;
	if (wheelDelta != 0)
	{
		m_camera.m_distance *= Math::Pow(0.85f, (float) wheelDelta);
	}

	// 1: Rotate car 90 degrees to the left.
	if (keyboard->IsKeyPressed(Keys::n1))
		m_car.m_orientation.Rotate(Vector3f::UP, Math::HALF_PI);
	// 2: Rotate car 180 degrees.
	if (keyboard->IsKeyPressed(Keys::n2))
		m_car.m_orientation.Rotate(Vector3f::UP, Math::PI);
	// 3: Rotate car 90 degrees to the right.
	if (keyboard->IsKeyPressed(Keys::n3))
		m_car.m_orientation.Rotate(Vector3f::UP, -Math::HALF_PI);
	// R: Spin the car around its up axis.
	if (keyboard->IsKeyPressed(Keys::r))
		m_car.m_angularVelocity.y += 5;
	// Y: Launch the car into the air.
	if (keyboard->IsKeyPressed(Keys::y))
		m_car.m_velocity.y += 10;

	// Steering controls.
	int turnAmount = 0;
	if (keyboard->IsKeyDown(Keys::right) || keyboard->IsKeyDown(Keys::d))
		turnAmount -= 1;
	if (keyboard->IsKeyDown(Keys::left) || keyboard->IsKeyDown(Keys::a))
		turnAmount += 1;
	if (turnAmount != 0)
	{
		float steeringScale = 1.0f / (1.0f + m_car.m_velocity.Length() * 0.05f);
		float maxWheelAngle = 60.0f * Math::DEG_TO_RAD * steeringScale;
		m_car.m_steeringAngle += turnAmount * timeDelta * 120.0f * steeringScale * Math::DEG_TO_RAD;
		m_car.m_steeringAngle = Math::Clamp(m_car.m_steeringAngle, -maxWheelAngle, maxWheelAngle);
	}
	else
	{
		m_car.m_steeringAngle *= 0.9f;
	}

	// Up: Throttle (gas pedal).
	if (keyboard->IsKeyDown(Keys::up) || keyboard->IsKeyDown(Keys::w))
		m_car.m_throttleAmount = 1.0f;
	else
		m_car.m_throttleAmount = 0.0f;

	// Down: Reverse gas pedal.
	if (keyboard->IsKeyDown(Keys::down) || keyboard->IsKeyDown(Keys::s))
		m_car.m_reverseAmount = 1.0f;
	else
		m_car.m_reverseAmount = 0.0f;

	// Space: hand-break (break pedal).
	if (keyboard->IsKeyDown(Keys::space))
		m_car.m_handBreakAmount = 1.0f;
	else
		m_car.m_handBreakAmount = 0.0f;

	// X: Shift up.
	if (keyboard->IsKeyPressed(Keys::x))
		m_car.ShiftUp();
	// Z: Shift down.
	if (keyboard->IsKeyPressed(Keys::z))
		m_car.ShiftDown();

	// Update camera to follow car.
	if (m_enableCameraFollow && !m_isPaused)
	{
		Vector3f carVelocity = m_car.m_velocity;
		carVelocity.y = 0.0f;
		Vector3f camForward = carVelocity;
		if (carVelocity.Length() < 0.1f)
			camForward = m_car.m_orientation.GetForward();
		else
			camForward.Normalize();
		float camHeight = -m_camera.m_orientation.GetForward().y * m_camera.m_distance;
		float camDist = Math::Sqrt(m_camera.m_distance * m_camera.m_distance - camHeight * camHeight);
		Vector3f cp = (camForward * camDist) - (Vector3f::UP * camHeight);
		cp.Normalize();
		Quaternion camRot = Quaternion::LookAtRotation(cp, Vector3f::UP);
		float lerpAmount = Math::Clamp((carVelocity.Length() - 4.0f) / 40.0f, 0.0f, 1.0f);
		m_camera.m_orientation = Quaternion::Lerp(m_camera.m_orientation, camRot, 0.2f * lerpAmount);
	}
	m_camera.m_position = m_car.m_position;
	m_camera.m_farPlane = 100.0f + m_camera.m_distance * 2.0f;

	// Update car physics.
	if (!m_isPaused || simulateSingleFrame)
	{
		unsigned int numIterations = 5;
		float iterationTimeDelta = (timeDelta * m_simulationSpeed) / numIterations;
		for (unsigned int i = 0; i < numIterations; i++)
			m_car.UpdatePhysics(iterationTimeDelta);
			
		// Update visuals.
		m_graphSpeed.AddPoint((float) m_frameIndex, m_car.m_velocity.Length());
		m_graphLongForce.AddPoint((float) m_frameIndex, m_car.m_netForce.Dot(m_car.GetForwardAxis()));
		for (unsigned int i = 0; i < 4; i++)
		{
			m_graphLongitudinalGrip[i].SetMarkerPosition(m_car.m_tires[i].m_slipRatio);
			m_graphLateralGrip[i].SetMarkerPosition(m_car.m_tires[i].m_slipAngle);
		}
		for (unsigned int i = 0; i < 4; i++)
		{
			Tire& tire = m_car.m_tires[i];
			Vector4f c;
			c.x = Math::Abs(tire.m_lateralGrip);
			c.y = Math::Abs(tire.m_longitudinalGrip);
			c.z = 0.0f;
			c.w = 1.0f;
			m_trailTires[i].AddPoint(tire.m_tireToWorld.TransformAffine(Vector3f::ZERO), c);
		}
		m_speedometer.SetSpindleValue(m_car.m_velocity.Length() * METERS_PER_SECOND_TO_MPH);
		m_tachometer.SetSpindleValue((m_car.m_engineSpeed / 1000.0f) * RADIANS_PER_SECOND_TO_RPM);
		float redLineRpm1000 = (m_car.m_engineRedLineSpeed * RADIANS_PER_SECOND_TO_RPM) / 1000.0f;
		m_tachometer.SetRedLineValue((int) (redLineRpm1000 + 0.5f));
		m_tachometer.SetGearNumber(m_car.m_gearIndex + 1);
	}
	else
	{
		m_car.CalcDerivedData();
	}
}

void DrivingApp::OnRender()
{
	//-------------------------------------------------------------------------
	// Setup render parameters

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
		
	//-------------------------------------------------------------------------
	// Render the simulation
	
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(m_camera.GetViewProjection().data());
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Draw the ground.
	DrawGridFloor(Vector3f(m_camera.m_position.x, 0, m_camera.m_position.z), 1, 10.0f * m_camera.m_distance);

	// Draw trails.
	for (unsigned int i = 0; i < m_trails.size(); i++)
		m_graphics.DrawTrail(*m_trails[i]);

	// Draw the car.
	DrawCar(&m_car);
		
	//-------------------------------------------------------------------------
	// Render the  HUD.
	
	Vector2f windowSize((float) GetWindow()->GetWidth(), (float) GetWindow()->GetHeight());
	Matrix4f orthoProjection = Matrix4f::CreateOrthographic(0, windowSize.x, windowSize.y, 0, -1, 1);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(orthoProjection.data());
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);

	//-------------------------------------------------------------------------
	// Draw car info text

	stringstream strStream;
	strStream.setf(ios::fixed, ios::floatfield);
	strStream.precision(2);

	float carSpeedMph = m_car.m_velocity.Length() * METERS_PER_SECOND_TO_MPH;
	float tireSpeedMps = m_car.m_tires[2].m_angularVelocity * m_car.m_tires[2].m_radius;
	float tireSpeedMph = tireSpeedMps * METERS_PER_SECOND_TO_MPH;
	int engineSpeedRpm = (int) (m_car.m_engineSpeed * RADIANS_PER_SECOND_TO_RPM + 0.5f);
	Vector3f carForward = m_car.GetForwardAxis();

	strStream << "Sim Speed     = x" << m_simulationSpeed << (m_isPaused ? " [PAUSED]" : "") << endl;
	strStream << endl;
	strStream << "Speed         = " << carSpeedMph << " mph (" << m_car.m_velocity.Length() << " m/s)" << endl;
	strStream << "Tire Speed    = " << tireSpeedMph << " mph (" << tireSpeedMps << " m/s)" << endl;
	strStream << endl;
	strStream << "Gear " << (m_car.m_gearIndex + 1) << "        = x" << m_car.GetGear().m_ratio << endl;
	strStream << "Engine Speed  = " << engineSpeedRpm << " rpm (" << m_car.m_engineSpeed << " rad/s)" << endl;
	strStream << "Engine Torque = " << m_car.m_engineTorque << " N*m" << endl;
	strStream << endl;
	strStream << "Weight Transfer Torque = " << m_car.m_weightTransferTorque << " N*m" << endl;
	strStream << endl;
	strStream << "Traction Force = " << m_car.m_tractionForce.Dot(carForward) << " N" << endl;
	strStream << "Drag Force     = " << m_car.m_dragForce.Dot(carForward) << " N" << endl;
	strStream << "RR Force       = " << m_car.m_rollingResistanceForce.Dot(carForward) << " N" << endl;
	strStream << "Net Force      = " << m_car.m_netForce.Dot(carForward) << " N" << endl;
	strStream << endl;
		
	// Draw text.
	String text = strStream.str();
	Vector2f textPos(32, 32);
	m_graphics.DrawString(text, textPos, Color::WHITE);

	//-------------------------------------------------------------------------
	// Draw graphs

	Vector2f graphSize(100, 100);
	float graphSep = 10.0f;
	Vector2f graphSpacing = graphSize + Vector2f(graphSep);
	Vector2f cursor(windowSize.x - graphSpacing.x * 2, graphSep);
	for (unsigned int i = 0; i < 14; i++)
	{
		if (i < 4)
			m_graphics.DrawGraph(m_graphLongitudinalGrip[i % 4], cursor, graphSize);
		else if (i < 8)
			m_graphics.DrawGraph(m_graphLateralGrip[i % 4], cursor, graphSize);
		else if (i < 12)
			DrawTractionCircle(m_car.m_tires[i % 4], cursor + graphSize * 0.5f, graphSize.x * 0.5f);
		else if (i == 12)
			m_graphics.DrawGraph(m_graphSpeed, cursor, graphSize);
		else if (i == 13)
			m_graphics.DrawGraph(m_graphLongForce, cursor, graphSize);

		if (i % 2 == 0)
		{
			cursor.x += graphSpacing.x;
		}
		else
		{
			cursor.x -= graphSpacing.x;
			cursor.y += graphSpacing.y;
		}
	}
	
	//-------------------------------------------------------------------------
	// Gauges and other diagrams

	// Draw speedometer and tachometer.
	float meterRadius = 80;
	Vector2f meterCenter(windowSize.x * 0.5f, windowSize.y - 20 - meterRadius);
	m_speedometer.Draw(m_graphics, meterCenter - Vector2f(meterRadius + 10, 0), meterRadius);
	m_tachometer.Draw(m_graphics, meterCenter + Vector2f(meterRadius + 10, 0), meterRadius);

	// Draw free body diagram.
	DrawFreeBodyDiagram(&m_car, Vector2f(200, windowSize.y - 200), 85.0f);

	// Draw throttle and break bars.
	Vector2f barSize(20, 50);
	Vector2f gasBarPos = meterCenter - Vector2f(meterRadius * 2 + 50, 0);
	Vector2f breakBarPos = meterCenter + Vector2f(meterRadius * 2 + 50, 0);
	Vector2f barLabelOffset(0.0f, (barSize.y * 0.5f) + 20.0f);
	m_graphics.DrawBar(gasBarPos - (barSize * 0.5f), barSize, m_car.m_throttleAmount, Color::GREEN, Color::WHITE);
	m_graphics.DrawBar(breakBarPos - (barSize * 0.5f), barSize, m_car.m_handBreakAmount, Color::RED, Color::WHITE);
	m_graphics.DrawString("Gas", gasBarPos + barLabelOffset, Color::GREEN, TextAlign::CENTERED);
	m_graphics.DrawString("Break", breakBarPos + barLabelOffset, Color::RED, TextAlign::CENTERED);
}

void DrivingApp::DrawWireFrameBox(const Vector3f& halfSize, const Color& color)
{
	Vector3f p = Vector3f::ZERO;
	Vector3f h = halfSize;
	glBegin(GL_LINE_LOOP);
		glColor4ubv(color.data());
		glVertex3f(p.x - h.x, p.y - h.y, p.z + h.z);
		glVertex3f(p.x - h.x, p.y - h.y, p.z - h.z);
		glVertex3f(p.x + h.x, p.y - h.y, p.z - h.z);
		glVertex3f(p.x + h.x, p.y - h.y, p.z + h.z);
	glEnd();
	glBegin(GL_LINE_LOOP);
		glColor4ubv(color.data());
		glVertex3f(p.x - h.x, p.y + h.y, p.z + h.z);
		glVertex3f(p.x - h.x, p.y + h.y, p.z - h.z);
		glVertex3f(p.x + h.x, p.y + h.y, p.z - h.z);
		glVertex3f(p.x + h.x, p.y + h.y, p.z + h.z);
	glEnd();
	glBegin(GL_LINES);
		glColor4ubv(color.data());
		glVertex3f(p.x - h.x, p.y - h.y, p.z + h.z);
		glVertex3f(p.x - h.x, p.y + h.y, p.z + h.z);
		glVertex3f(p.x - h.x, p.y - h.y, p.z - h.z);
		glVertex3f(p.x - h.x, p.y + h.y, p.z - h.z);
		glVertex3f(p.x + h.x, p.y - h.y, p.z - h.z);
		glVertex3f(p.x + h.x, p.y + h.y, p.z - h.z);
		glVertex3f(p.x + h.x, p.y - h.y, p.z + h.z);
		glVertex3f(p.x + h.x, p.y + h.y, p.z + h.z);
	glEnd();
}

void DrivingApp::DrawCar(Car* car)
{
	// Create a quaternion to represent weight transfer.
	Quaternion q(Vector4f(m_car.m_chassisTilt, 0.0f));
	Quaternion weightTransferRotation = Quaternion::IDENTITY;// m_car.m_orientation;
	weightTransferRotation += (q * weightTransferRotation) * (0.7f * 0.5f);
	weightTransferRotation.Normalize();

	Matrix4f meshMatrix = 
		Matrix4f::CreateTranslation(0.0f, 0.0f, 0.05f);
	glMatrixMode(GL_MODELVIEW);
	Matrix4f modelMatrix =
		Matrix4f::CreateTranslation(m_car.m_position) * 
		Matrix4f::CreateRotation(m_car.m_orientation) *
		Matrix4f::CreateRotation(weightTransferRotation) *
		Matrix4f::CreateTranslation(0.0f, m_car.m_chassisSize.y * 0.5f, 0.0f) *
		meshMatrix;
	glLoadMatrixf(modelMatrix.data());
	
	// Draw the chassis mesh.
	DrawMesh(m_meshCarChassis, Color::WHITE, modelMatrix);

	glMatrixMode(GL_MODELVIEW);
	modelMatrix = m_car.m_carToWorld * 
		Matrix4f::CreateTranslation(0.0f, m_car.m_chassisSize.y * 0.5f, 0.0f);
	glLoadMatrixf(modelMatrix.data());
	
	//DrawWireFrameBox(m_car.m_chassisSize * 0.5f, Color::GREEN);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(m_car.m_carToWorld.data());

	// Draw axels.
	Vector3f upOffset(0, car->m_tires[Tire::k_front_right].m_radius, 0);
	Vector3f tirePosFR = car->m_tires[Tire::k_front_right].m_offset + upOffset;
	Vector3f tirePosFL = car->m_tires[Tire::k_front_left].m_offset + upOffset;
	Vector3f tirePosRR = car->m_tires[Tire::k_rear_right].m_offset + upOffset;
	Vector3f tirePosRL = car->m_tires[Tire::k_rear_left].m_offset + upOffset;
	Vector3f frontCenter = (tirePosFR + tirePosFL) * 0.5f;
	Vector3f rearCenter = (tirePosRR + tirePosRL) * 0.5f;
	Vector3f carCenter = (frontCenter + rearCenter) * 0.5f;

	// Draw drive-train.
	glBegin(GL_LINES);
		glColor3f(1,1,1);
		glVertex3fv(tirePosFL.data());
		glVertex3fv(tirePosFR.data());
		glVertex3fv(tirePosRL.data());
		glVertex3fv(tirePosRR.data());
		glVertex3fv(frontCenter.data());
		glVertex3fv(rearCenter.data());
	glEnd();
		
	// Draw center of mass.
	glPointSize(4.0f);
	glBegin(GL_POINTS);
		glColor3f(1,1,1);
		glVertex3f(0, m_car.m_centerOfMassHeight, 0);
	glEnd();

	// Draw steering circle
	/*float turnRadius = 0.0f;
	if (Math::Abs(car->m_steeringAngle) > 0.01f)
	{
		//turnRadius = Math::Abs(car->m_wheelBase / Math::Sin(car->m_steeringAngle));
		turnRadius = Math::Abs(car->m_wheelBase / Math::Tan(car->m_steeringAngle));
		Vector3f turnCenter = rearCenter;
		turnCenter.x -= Math::Sign(car->m_steeringAngle) * turnRadius;
		glBegin(GL_LINES);
			glColor3f(0.5f, 0, 0.5f);
			glVertex3fv(turnCenter.data());
			glVertex3fv(tirePosRL.data());
			glVertex3fv(turnCenter.data());
			glVertex3fv(tirePosRR.data());
			glVertex3fv(turnCenter.data());
			glVertex3fv(tirePosFL.data());
			glVertex3fv(turnCenter.data());
			glVertex3fv(tirePosFR.data());
			glVertex3fv(turnCenter.data());
			glVertex3fv(carCenter.data());
		glEnd();
		glBegin(GL_POINTS);
			glColor3f(1, 0, 1);
			glVertex3fv(turnCenter.data());
		glEnd();
	}*/
		
	// Draw the individual tires.
	for (unsigned int i = 0; i < car->m_numWheels; i++)
	{
		DrawTire(car, i);
	}
}

void DrivingApp::DrawTire(Car* car, unsigned int tireIndex)
{
	const Tire& tire = car->m_tires[tireIndex];
	
	float carWeight = car->m_mass * car->m_gravity;
	float forceScale = 4.0f / carWeight;
	float tireWidth = tire.m_radius * 0.6f;
	
	// Draw the tire mesh.
	Matrix4f modelMatrix = tire.m_tireToWorld *
		Matrix4f::CreateTranslation(0.0f, tire.m_radius, 0.0f) *
		Matrix4f::CreateScale(tire.m_radius);
	DrawMesh(m_meshWheel, Color::WHITE, modelMatrix);

	Vector3f p;
	p.x = 0.0f;
	p.y = tire.m_radius + (Math::Cos(-tire.m_rotationAngle) * tire.m_radius);
	p.z = Math::Sin(-tire.m_rotationAngle) * tire.m_radius;
	
	// Draw line on tire face to show rotation angle.
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(tire.m_tireToWorld.data());
	glLineWidth(2.0f);
	glBegin(GL_LINES);
	glColor3f(0,0,0);
	glVertex3f(-tireWidth * 0.52f, tire.m_radius, 0);
	glVertex3f(-tireWidth * 0.52f, p.y, p.z);
	glVertex3f(tireWidth * 0.52f, tire.m_radius, 0);
	glVertex3f(tireWidth * 0.52f, p.y, p.z);
	glEnd();
	glLineWidth(1.0f);

	// Draw vertical, longitudinal, and lateral force on tire.
	//glDisable(GL_DEPTH_TEST);
	m_graphics.DrawArrow(Vector3f::ZERO, Vector3f(0, -tire.m_weight * forceScale, 0), Vector3f::RIGHT, Color::BLUE, 0.1f, 2.0f);
	if (Math::Abs(tire.m_longitudinalForce) > 1.0f)
		m_graphics.DrawArrow(Vector3f::ZERO, Vector3f(0, 0, -tire.m_longitudinalForce * forceScale), Vector3f::UP, Color::GREEN, 0.1f, 2.0f);
	if (Math::Abs(tire.m_lateralForce) > 1.0f)
		m_graphics.DrawArrow(Vector3f::ZERO, Vector3f(tire.m_lateralForce * forceScale, 0, 0), Vector3f::UP, Color::RED, 0.1f, 2.0f);
	//glEnable(GL_DEPTH_TEST);
}

void DrivingApp::DrawWheel(float radius, float width, float rotation, const Color& color)
{
	// Draw tire box.
	//glBegin(GL_POINTS);
	//	glColor4ubv(color.data());
	//	glVertex3f(0, 0, 0);
	//glEnd();
	//glBegin(GL_LINE_LOOP);
	//	glColor4ubv((color * 0.7f).data());
	//	glVertex3f(-width * 0.5f, 0, -radius);
	//	glVertex3f(width * 0.5f, 0, -radius);
	//	glVertex3f(width * 0.5f, 0, radius);
	//	glVertex3f(-width * 0.5f, 0, radius);
	//glEnd();

	// Draw cylinder
	unsigned int resolution = 15;
	for (unsigned int side = 0; side < 2; side++)
	{
		glBegin(GL_LINE_LOOP);
		glColor4ubv(color.data());
		for (unsigned int i = 0; i < resolution; i++)
		{
			float angle = (i / (float) resolution) * Math::TWO_PI;
			angle += -rotation;
			Vector3f v;
			v.x = (side == 0 ? width : -width) * 0.5f;
			v.y = radius + (Math::Cos(angle) * radius);
			v.z = Math::Sin(angle) * radius;
			glVertex3fv(v.data());
		}
		glEnd();
				
		glBegin(GL_LINES);
		glColor4ubv(color.data());
		for (unsigned int i = 0; i < resolution; i++)
		{
			float angle = (i / (float) resolution) * Math::TWO_PI;
			angle += -rotation;
			Vector3f v;
			v.x = -width * 0.5f;
			v.y = radius + (Math::Cos(angle) * radius);
			v.z = Math::Sin(angle) * radius;
			glVertex3fv(v.data());
			v.x = width * 0.5f;
			glVertex3fv(v.data());
		}
		glEnd();
	}
	
	// Draw line going from center to outside.
	Vector3f p;
	p.x = 0.0f;
	p.y = radius + (Math::Cos(-rotation) * radius);
	p.z = Math::Sin(-rotation) * radius;
	
	glBegin(GL_LINES);
	glColor4ubv(color.data());
	glVertex3f(0, radius, 0);
	glVertex3f(0, p.y, p.z);
	//glVertex3f(-width * 0.5f, radius, 0);
	//glVertex3f(-width * 0.5f, p.y, p.z);
	//glVertex3f(width * 0.5f, radius, 0);
	//glVertex3f(width * 0.5f, p.y, p.z);
	//glVertex3f(-width * 0.5f, radius, 0);
	//glVertex3f(width * 0.5f, radius, 0);
	glEnd();
}

void DrivingApp::DrawFreeBodyDiagram(Car* car, const Vector2f& center, float scale)
{
	car->m_wheelBase;
	Vector2f tirePositions[4];

	Vector2f windowSize((float) GetWindow()->GetWidth(), (float) GetWindow()->GetHeight());
	Matrix4f carToScreen = 
		Matrix4f::CreateOrthographic(0, windowSize.x, windowSize.y, 0, 20 * scale, -20 * scale) *
		Matrix4f::CreateTranslation(center.x, center.y, 0.0f) *
		Matrix4f::CreateScale(scale) *
		Matrix4f::CreateRotation(Vector3f::UNITX, -Math::HALF_PI);
	Matrix4f modelMatrix = Matrix4f::IDENTITY;
	
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(carToScreen.data());
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(modelMatrix.data());
	
	for (unsigned int tireIndex = 0; tireIndex < car->m_numWheels; tireIndex++)
	{
		const Tire& tire = car->m_tires[tireIndex];
		tirePositions[tireIndex] = center + tire.m_offset.GetXZ() * scale;
	}
	
	Vector3f frontCenter = (car->m_tires[Tire::k_front_right].m_offset + car->m_tires[Tire::k_front_left].m_offset) * 0.5f;
	Vector3f rearCenter = (car->m_tires[Tire::k_rear_right].m_offset + car->m_tires[Tire::k_rear_left].m_offset) * 0.5f;

	float forceScale = 0.2f / m_car.m_mass;
	float torqueScale = (1.0f / m_car.m_inertia) * 0.2f;
	float arrowHead = 0.08f;

	// Draw drive-train.
	glBegin(GL_LINES);
	glColor3f(1,1,1);
		glVertex3fv(car->m_tires[Tire::k_front_left].m_offset.data());
		glVertex3fv(car->m_tires[Tire::k_front_right].m_offset.data());
		glVertex3fv(car->m_tires[Tire::k_rear_left].m_offset.data());
		glVertex3fv(car->m_tires[Tire::k_rear_right].m_offset.data());
		glVertex3fv(car->m_tires[Tire::k_front_left].m_offset.data());
		glVertex3fv(car->m_tires[Tire::k_front_right].m_offset.data());
		glVertex3fv(frontCenter.data());
		glVertex3fv(rearCenter.data());
	glEnd();
	
	// Draw center point.
	glPointSize(6.0f);
	glBegin(GL_POINTS);
		glColor4ubv(Color::YELLOW.data());
		glVertex3f(0, 0, 0);
	glEnd();

	Vector3f netForce = Vector3f::ZERO;

	// Draw tires.
	for (unsigned int tireIndex = 0; tireIndex < car->m_numWheels; tireIndex++)
	{
		const Tire& tire = car->m_tires[tireIndex];
		float tireRadius = tire.m_radius;
		float tireWidth = tire.m_radius * 0.31f;

		modelMatrix = tire.m_tireToCar;
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(modelMatrix.data());
		
		// Draw tire outline box.
		glBegin(GL_LINE_LOOP);
			glColor4ubv(Color::WHITE.data());
			if (tire.m_weight <= 0.0f)
				glColor4ubv(Color::RED.data());
			glVertex3f(-tireWidth, 0, -tireRadius);
			glVertex3f(tireWidth, 0, -tireRadius);
			glVertex3f(tireWidth, 0, tireRadius);
			glVertex3f(-tireWidth, 0, tireRadius);
		glEnd();

		// Draw a horizontal line that moves as the tire rotates.
		if (Math::Sin(tire.m_rotationAngle) > 0.0f)
		{
			float rotationLongitude = Math::Cos(tire.m_rotationAngle);
			glBegin(GL_LINES);
				glColor4ubv(Color::WHITE.data());
				if (tire.m_weight <= 0.0f)
					glColor4ubv(Color::RED.data());
				glVertex3f(-tireWidth, 0, -tireRadius * rotationLongitude);
				glVertex3f(tireWidth, 0, -tireRadius * rotationLongitude);
			glEnd();
		}
		
		netForce.x += tire.m_lateralForce;
		netForce.z -= tire.m_longitudinalForce;

		// Draw forces.
		m_graphics.DrawArrow(Vector3f::ZERO, forceScale * Vector3f(
			0.0f, 0.0f, -tire.m_longitudinalForce), Color::GREEN, arrowHead, 2);
		m_graphics.DrawArrow(Vector3f::ZERO, forceScale * Vector3f(
			tire.m_lateralForce, 0.0f, 0.0f), Color::RED, arrowHead, 2);
	}
	
	modelMatrix = Matrix4f::IDENTITY;
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(modelMatrix.data());

	// Draw the net force and torque acting on the car.
	m_graphics.DrawArrow(Vector3f::ZERO, forceScale * netForce, Color::YELLOW, arrowHead, 2);
	m_graphics.DrawArrow(Vector3f::ZERO, forceScale * car->m_dragForce, Color::CYAN, arrowHead, 2);
	m_graphics.DrawArrow(Vector3f(0, 0, -0.2f), torqueScale * Vector3f(-m_car.m_netTorque.y, 0.0f, 0.0f), Color::YELLOW, arrowHead, 2);

	glPointSize(1.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(Matrix4f::CreateOrthographic(0, windowSize.x, windowSize.y, 0, -1, 1).data());
}

void DrivingApp::DrawTractionCircle(const Tire& tire, const Vector2f& center, float radius)
{
	Color colorOutline = Color::WHITE;
	Color colorLong = Color(0, 128, 0);
	Color colorLat = Color(128, 0, 0);
	Color colorForce = Color::YELLOW;

	m_graphics.DrawCircle(center, radius, colorOutline, 20, 2.0f);

	m_graphics.DrawLine(center - Vector2f(radius, 0), center + Vector2f(radius, 0), colorLat, 1.0f);
	m_graphics.DrawLine(center - Vector2f(0, radius), center + Vector2f(0, radius), colorLong, 1.0f);

	m_graphics.DrawLine(center + Vector2f(tire.m_lateralGrip, 0) * radius,
		center + Vector2f(tire.m_lateralGrip, -tire.m_longitudinalGrip) * radius, colorLong, 1.0f);
	m_graphics.DrawLine(center + Vector2f(0,  -tire.m_longitudinalGrip) * radius,
		center + Vector2f(tire.m_lateralGrip, -tire.m_longitudinalGrip) * radius, colorLat, 1.0f);
	m_graphics.DrawLine(center, center + Vector2f(tire.m_lateralGrip, -tire.m_longitudinalGrip) * radius, colorForce, 2.0f);
}

void DrivingApp::DrawGridFloor(const Vector3f& center, float squareSize, float gridRadius)
{
	int majorTick = 5;

	// Snap the grid radius to the square size.
	gridRadius = Math::Ceil(gridRadius / squareSize) * squareSize;

	float startX = center.x - gridRadius;
	float startZ = center.z - gridRadius;
	int indexX = (int) Math::Floor(startX / (squareSize)) ;
	int indexZ = (int) Math::Floor(startZ / (squareSize)) ;
	startX = indexX * squareSize;
	startZ = indexZ * squareSize;
	float endX = startX + (gridRadius * 2.0f);
	float endZ = startZ + (gridRadius * 2.0f);
	float x = startX;
	float z = startZ;

	// Draw a grid of perpendicular lines.
    glDepthMask(false);
	glBegin(GL_LINES);
	for (; x < center.x + gridRadius; x += squareSize, z += squareSize, indexX++, indexZ++)
	{
		// Draw line along z-axis.
		Color color = Color(30, 30, 30);
		if (indexX % majorTick == 0)
			color = Color(80, 80, 80);
		glColor4ubv(color.data());
		glVertex3f(x, center.y, startZ);
		glVertex3f(x, center.y, endZ);
		
		// Draw line along x-axis.
		color = Color(30, 30, 30);
		if (indexZ % majorTick == 0)
			color = Color(80, 80, 80);
		glColor4ubv(color.data());
		glVertex3f(startX, center.y, z);
		glVertex3f(endX, center.y, z);
	}
	glEnd();
    glDepthMask(true);
}


void DrivingApp::DrawMesh(Mesh* mesh, const Color& color, const Matrix4f& modelMatrix)
{
	glUseProgram(m_shader->GetGLProgram());
	
	Color m_lightColor = Color::WHITE;
	Color m_ambientLight = Color::BLACK;
	Vector3f m_lightDirection = Vector3f(-1, -1, -1).Normalize();

	Matrix4f viewProjection =  m_camera.GetViewProjection();
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

	// Draw the car chassis.
	glEnable(GL_CULL_FACE);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glColor4ubv(Color::GREEN.data());
	glBindVertexArray(mesh->GetVertexData()->GetVertexBuffer()->GetGLVertexArray());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->GetIndexData()->GetIndexBuffer()->GetGLIndexBuffer());
	glDrawElements(GL_TRIANGLES, mesh->GetIndexData()->GetCount(), GL_UNSIGNED_INT, (unsigned int*) 0);
	glBindVertexArray(0);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_CULL_FACE);
	
	glUseProgram(0);
}

