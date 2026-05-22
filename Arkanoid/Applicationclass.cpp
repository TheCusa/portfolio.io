////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.cpp
////////////////////////////////////////////////////////////////////////////////
#define NOMINMAX
#include "applicationclass.h"
#include "D3dclass.h"


ApplicationClass::ApplicationClass()
{
	m_Direct3D = nullptr;
	m_Camera = nullptr;
	racket = nullptr;
	m_ColorShader = nullptr;
	newModel = nullptr;
	ball = nullptr;

	//rectangles specs
	itemHorizontalSpacing = 0.05f;
	itemVerticalSpacing = 0.2f;
	itemWidth = 0.5f;
	itemHeight = 0.2f;

	//racket specs
	racketHeight = 0.1;
	racketWidth = 1.f;

	ballRadius = 0.1f;


	itemRows = 3;

	minXVisible = 0.f;
	maxXVisible = 0.f;
	minYVisible = 0.f;
	maxYVisible = 0.f;
	safeVisibleWidth = 0.f;

	racketXPosition = 0.f;
	racketYPosition = 0.f;

	 ballSpeedX = 4.f;
	 ballSpeedY = 4.f;

	ballPosition = XMFLOAT3 { 0.f, 0.f, 0.f };
	ballVelocity = XMFLOAT3(ballSpeedX, ballSpeedY, 0.f);
	isBallAttached = true;

	directionEffect = 1.f;



	unsigned seed = randomDevice();
	randomGenerator = std::mt19937(seed);
	colorDistribution = std::uniform_real_distribution<float>(0.0f, 1.0f);
}


ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
}


ApplicationClass::~ApplicationClass()
{
}


bool ApplicationClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	//Calculating safe area width
	float fovY = XM_PIDIV4; //vertical fov
	float aspect = (float)screenWidth / (float)screenHeight;
	float cameraZ = -5.0f; 
	float objZ = 0.0f;      
	float zDepth = abs(cameraZ - objZ);

	float halfHeight = tanf(fovY / 2.0f) * zDepth;
	float halfWidth = halfHeight * aspect;
	float visibleWidth = halfWidth * 2.0f;
	float visibleHeight = halfHeight * 2.0f;

	bool result;
	float safeAreaMargin = 0.025f; // 2,5%
	 safeVisibleWidth = visibleWidth * (1.0f - 2 * safeAreaMargin);
	 float safeVisibleHeight = visibleHeight * (1.0f - 2 * safeAreaMargin);

	float halfSafeWidth = safeVisibleWidth / 2;
	float halfSafeHeight = safeVisibleHeight / 2;

	maxColumnsItems = safeVisibleWidth / (itemWidth + itemHorizontalSpacing);

	minXVisible = -halfSafeWidth;
	maxXVisible = halfSafeWidth;

	minYVisible = -halfSafeHeight;
	maxYVisible = halfSafeHeight;

	// Create and initialize the Direct3D object.
	m_Direct3D = new D3DClass;


	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);

	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}
	// Create the camera object.
	m_Camera = new CameraClass;

	// Set the initial position of the camera.
	m_Camera->SetPosition(0.0f, 0.0f, cameraZ);

	// Create and initialize the model object.
	racket = new ModelClass;
	multipleInitializing(itemRows  , blocks);

	result = racket->Initialize(m_Direct3D->GetDevice(), racketWidth, racketHeight, XMFLOAT4(0.f, 1.f, 0.f, 1.f));
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}


	ball = new ModelClass;
	result = ball->Initialize(m_Direct3D->GetDevice(), ballRadius, ballRadius, XMFLOAT4(1.f, 0.f, 0.f, 1.f)); 
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the ball model.", L"Error", MB_OK);
		return false;
	}

	// Create and initialize the color shader object.
	m_ColorShader = new ColorShaderClass;

	result = m_ColorShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
		return false;
	}
	return true;
}


void ApplicationClass::Shutdown()
{
	// Release the color shader object.
	if (m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = 0;
	}

	// Release the model object.
	if (racket)
	{
		racket->Shutdown();
		delete racket;
		racket = 0;
	}
	for (auto& block : blocks)
	{
		if (block.model)
		{
			block.model->Shutdown();
			delete block.model;
			block.model = nullptr;
		}
	}
	blocks.clear();
	if (ball)
	{
		ball->Shutdown();
		delete ball;
		ball = nullptr;
	}


	// Release the camera object.
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}
	// Release the Direct3D object.
		if (m_Direct3D)
		{
			m_Direct3D->Shutdown();
			delete m_Direct3D;
			m_Direct3D = 0;
		}
	return;
}


bool ApplicationClass::Frame(float deltaTime)
{
	bool result;

	movement(deltaTime);
	updateBall(deltaTime);
	// Render the graphics scene.
	result = Render();
	if (!result)
	{
		return false;
	}
	return true;
}


bool ApplicationClass::Render()
{
	XMMATRIX btmCenteredWorldMatrix, viewMatrix, projectionMatrix;
	bool result;
	XMMATRIX worldMatrix2;
	
	racketYPosition = minYVisible;
	// Clear the buffers to begin the scene.
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	btmCenteredWorldMatrix = XMMatrixTranslation(racketXPosition, racketYPosition, 0.0f);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.

	// Render the model using the color shader.
	 multipleRendering(blocks,viewMatrix, projectionMatrix);
	racket->Render(m_Direct3D->GetDeviceContext());
	result = m_ColorShader->Render(m_Direct3D->GetDeviceContext(), racket->GetIndexCount(), btmCenteredWorldMatrix, viewMatrix, projectionMatrix);

	// drawing the ball
	XMMATRIX ballWorld = XMMatrixTranslation(ballPosition.x, ballPosition.y, ballPosition.z);
	ball->Render(m_Direct3D->GetDeviceContext());
	m_ColorShader->Render(m_Direct3D->GetDeviceContext(), ball->GetIndexCount(), ballWorld, viewMatrix, projectionMatrix);


	if (!result)
	{
		return false;
	}
	
	

	// Present the rendered scene to the screen.
	m_Direct3D->EndScene();
	return true;
}
void ApplicationClass::movement(float deltaTime)
{
	float maxRacketMovement = maxXVisible - racketWidth / 2;
	float minRacketMovement = minXVisible + racketWidth / 2;
	float speed = 2.5f;
	if (GetAsyncKeyState(VK_LEFT))
	{
		racketXPosition -= speed * deltaTime;

	}
	if (GetAsyncKeyState(VK_RIGHT))
	{
		racketXPosition += speed * deltaTime;

	}
	if (racketXPosition > maxRacketMovement)
	{
		racketXPosition = maxRacketMovement;

	}
	if (racketXPosition < minRacketMovement)
	{
		racketXPosition = minRacketMovement;
	}
}
void ApplicationClass::updateBall(float deltaTime)
{
	XMFLOAT3 collisionNormal;

	
	if (isBallAttached)
	{
		// Positioning the ball on top of the racket
		ballPosition.x = racketXPosition;
		ballPosition.y = racketYPosition + (racketHeight / 2) + ballRadius;
	}
	else
	{
		// moving the ball if not attached
		ballPosition.x += ballVelocity.x* deltaTime;
		ballPosition.y += ballVelocity.y * deltaTime;

		// Bounces on screen boundaries
		if (ballPosition.x + ballRadius > maxXVisible || ballPosition.x - ballRadius < minXVisible)
		{
			ballVelocity.x *= -1;

			// Correcting position if exceeding boundaries
			if (ballPosition.x + ballRadius > maxXVisible) ballPosition.x = maxXVisible - ballRadius;
			if (ballPosition.x - ballRadius < minXVisible) ballPosition.x = minXVisible + ballRadius;
		}

		if (ballPosition.y + ballRadius > maxYVisible)
		{
			ballVelocity.y *= -1;
			
			ballPosition.y = maxYVisible - ballRadius;
		}

		//reloading level if ball exceed lower boundaries or all the blocks are destroyed
		else if ((ballPosition.y - ballRadius) < (minYVisible - racketHeight) || !blockChecking())
		{
			resetLevel();
			return;
		}

		// checking block collision
		for (auto& block : blocks)
		{
			if (!block.active) continue;

			if (checkCollisionAndGetNormal(ballPosition, ballRadius * 2, block.position, itemWidth, itemHeight, collisionNormal))
			{
				block.active = false;
				// Reflect velocity based on collision normal
				ballVelocity = reflect(ballVelocity, collisionNormal);
				
				break;
			}
		}

		// Check for collision with the racket
		XMFLOAT3 racketPos = { racketXPosition, racketYPosition, 0.0f };
		if (checkCollisionAndGetNormal(ballPosition, ballRadius * 2, racketPos, racketWidth, racketHeight, collisionNormal))
		{
			// Check if the collision is on the top of the racket
			if (collisionNormal.y > 0)
			{
				// Calculate the relative impact point
				float relativeImpactX = ballPosition.x - racketXPosition;

				// Normalize the impact point relative to the racket's width
				
				float normalizedImpact = relativeImpactX / (racketWidth / 2.0f);

				// Adjust the horizontal velocity based on the normalized impact point
				
				ballVelocity.x = normalizedImpact * directionEffect;

				// Invert the vertical velocity for the bounce
				ballVelocity.y *= -1.0f;
			}
			else
			{
				// If the collision is on the side, just invert the horizontal velocity
				ballVelocity = reflect(ballVelocity, collisionNormal);
			}
		}
	}

	// Launching the ball while pressing spacebar
	if (isBallAttached && GetAsyncKeyState(VK_SPACE))
	{
		isBallAttached = false;
		ballVelocity = XMFLOAT3(0.f, ballSpeedY, 0.f); // starting direction
	}
	

}
void ApplicationClass::multipleInitializing( int rows, std::vector<Block>& blocks)
{
	
	int columns = maxColumnsItems;
	float startX = minXVisible + itemWidth / 2;
	float startY = maxYVisible - itemHeight / 2;

	//initializing items and saving their positions
	for (int j = 0; j < rows; ++j)
	{
		for (int i = 0; i < columns; ++i)
		{
			

			// Generating random color
			float r = colorDistribution(randomGenerator);
			float g = colorDistribution(randomGenerator);
			float b = colorDistribution(randomGenerator);

			ModelClass* model = new ModelClass;
			model->Initialize(m_Direct3D->GetDevice(), itemWidth, itemHeight, XMFLOAT4(r, g, b, 1.f));

			XMFLOAT3 pos = {
				startX + i * (itemWidth + itemHorizontalSpacing),
				startY - j * (itemHeight + itemVerticalSpacing),
				0.f
			};

			blocks.push_back({ model, pos, true });
		}
	}
}


void ApplicationClass::multipleRendering(std::vector<Block>& m_blocks, XMMATRIX& viewMatrix, XMMATRIX& projectionMatrix)
{
	float verticalSpacing=0.f;
	for (auto& block : m_blocks)
	{
		if (!block.active) continue;

		XMMATRIX world = XMMatrixTranslation(block.position.x, block.position.y, block.position.z);

		block.model->Render(m_Direct3D->GetDeviceContext());
		m_ColorShader->Render(m_Direct3D->GetDeviceContext(), block.model->GetIndexCount(), world, viewMatrix, projectionMatrix);
	}
	

}


bool ApplicationClass::checkCollisionAndGetNormal(const XMFLOAT3& ballPos, float ballSize, const XMFLOAT3& rectPos, float rectWidth, float rectHeight, XMFLOAT3& normal)
{
	float halfBall = ballSize / 2.0f;
	float halfRectWidth = rectWidth / 2.0f;
	float halfRectHeight = rectHeight / 2.0f;

	float leftA = ballPos.x - halfBall;
	float rightA = ballPos.x + halfBall;
	float topA = ballPos.y + halfBall;
	float bottomA = ballPos.y - halfBall;

	float leftB = rectPos.x - halfRectWidth;
	float rightB = rectPos.x + halfRectWidth;
	float topB = rectPos.y + halfRectHeight;
	float bottomB = rectPos.y - halfRectHeight;

	// AABB collision check
	if (leftA > rightB || rightA < leftB || topA < bottomB || bottomA > topB)
	{
		return false;
	}

	// Determine collision normal
	float overlapX = std::min(rightA, rightB) - std::max(leftA, leftB);
	float overlapY = std::min(topA, topB) - std::max(bottomA, bottomB);

	if (overlapX < overlapY)
	{
		// Horizontal collision
		if (ballPos.x < rectPos.x)
		{
			normal = XMFLOAT3(-1.0f, 0.0f, 0.0f); //Collision to the left
		}
		else
		{
			normal = XMFLOAT3(1.0f, 0.0f, 0.0f); // Collision to the right
		}
	}
	else
	{
		// Vertical collision
		if (ballPos.y < rectPos.y)
		{
			normal = XMFLOAT3(0.0f, -1.0f, 0.0f); 
		}
		else
		{
			normal = XMFLOAT3(0.0f, 1.0f, 0.0f); 
		}
	}

	return true;
}
XMFLOAT3 ApplicationClass::reflect(const XMFLOAT3& velocity, const XMFLOAT3& normal)
{
	XMVECTOR v = XMLoadFloat3(&velocity);
	XMVECTOR n = XMLoadFloat3(&normal);
	XMVECTOR reflected = XMVector3Reflect(v, n);
	XMFLOAT3 result;
	XMStoreFloat3(&result, reflected);
	return result;
}
void ApplicationClass::resetLevel()
{
	
	// ball reset
	isBallAttached = true;
	ballVelocity = XMFLOAT3(0.f, 0.f, 0.f);

	// cleaning old blocks
	for (auto& block : blocks) {
		if (block.model) {
			block.model->Shutdown();
			delete block.model;
			block.model = nullptr;
		}
	}
	std::vector<Block>().swap(blocks); // forced deallocation

	// block creation
	multipleInitializing(itemRows, blocks);

	// blocs activation
	for (auto& block : blocks) {
		block.active = true;
	}
}
bool ApplicationClass::blockChecking()
{
	for (auto& block : blocks)
	{
		if (block.active == true)
		{ 
			return true;
		}
		
	}
	return false;
}