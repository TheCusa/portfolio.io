#pragma once
////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _APPLICATIONCLASS_H_
#define _APPLICATIONCLASS_H_


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "d3dclass.h"
#include "cameraclass.h"
#include "modelclass.h"
#include "colorshaderclass.h"
#include <vector>
#include <random>


/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = true;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.3f;


struct Block {
	ModelClass* model;
	XMFLOAT3 position;
	bool active = true;
};
////////////////////////////////////////////////////////////////////////////////
// Class name: ApplicationClass
////////////////////////////////////////////////////////////////////////////////
class ApplicationClass
{
public:
	ApplicationClass();
	ApplicationClass(const ApplicationClass&);
	~ApplicationClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame(float);
	void movement(float);
	void updateBall(float);

	
private:
	bool Render();
	void multipleInitializing(int, std::vector<Block>&);
	void multipleRendering(std::vector<Block>&, XMMATRIX&, XMMATRIX&);
	void resetLevel();
	bool blockChecking();

	XMFLOAT3 reflect(const XMFLOAT3&, const XMFLOAT3&);
	bool checkCollisionAndGetNormal(const XMFLOAT3&, float, const XMFLOAT3&, float, float, XMFLOAT3&);
	
	
private:
	D3DClass* m_Direct3D;
	CameraClass* m_Camera;
	ModelClass* racket;
	ColorShaderClass* m_ColorShader;


	float racketXPosition = 0.0f;
	float racketYPosition = 0.0f;

	float racketWidth;
	float racketHeight;

	 float maxXVisible ;
	 float minXVisible ;
	 float maxYVisible ;
	 float minYVisible ;
	int maxColumnsItems;
	float itemWidth;
	float itemHeight;
	float itemHorizontalSpacing;
	float itemVerticalSpacing;

	float directionEffect;
	float ballRadius;

	float safeVisibleWidth;
	int itemRows;

	ModelClass* ball;
	bool isBallAttached = true;

	float ballSpeedX;
	float ballSpeedY;
	XMFLOAT3 ballPosition;
	XMFLOAT3 ballVelocity;



	std::vector<Block> blocks;
	
	ModelClass* newModel;

	std::mt19937 randomGenerator;
	std::uniform_real_distribution<float> colorDistribution;
	std::random_device randomDevice;
};

#endif