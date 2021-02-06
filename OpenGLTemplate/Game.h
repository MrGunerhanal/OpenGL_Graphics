#pragma once

#include "Common.h"
#include "GameWindow.h"

// Classes used in game.  For a new class, declare it here and provide a pointer to an object of this class below.  Then, in Game.cpp, 
// include the header.  In the Game constructor, set the pointer to NULL and in Game::Initialise, create a new object.  Don't forget to 
// delete the object in the destructor.   
class CCamera;
class CSkybox;
class CShader;
class CShaderProgram;
class CPlane;
class CFreeTypeFont;
class CHighResolutionTimer;
class CSphere;
class COpenAssetImportMesh;
class CAudio;
class CCatmullRom;
class CTexture;
class Light;
class Start;

class Game {
private:
	// Three main methods used in the game.  Initialise runs once, while Update and Render run repeatedly in the game loop.


	glm::vec3 cameraPos;
	// Some other member variables
	double m_dt;
	int m_framesPerSecond;
	bool m_appActive;
	float m_currentDistance;

	glm::vec3 playerPos;
	glm::vec3 playerLooking;
	
	float playerTOffset = 0;
	float lampNumbers;
	
	float m_t;
	glm::vec3 m_supermanPosition;
	glm::mat4 m_supermanOrientation;

	//bool FirstPersonCameraEnabled;
	bool TopCameraEnabled;
	bool FreeModeCameraEnabled;
	bool ThirdPersonCameraEnabled;
	bool SideCameraEnabled;
	bool BikeCameraEnabled;

	GLuint m_uiVAO;
	GLuint m_uCube;
	glm::mat4 *m_pModelMatrix;

	// Pointers to game objects.  They will get allocated in Game::Initialise()
	CSkybox *m_pSkybox;
	CCamera *m_pCamera;
	vector <CShaderProgram *> *m_pShaderPrograms;
	CPlane *m_pPlanarTerrain;
	CFreeTypeFont *m_pFtFont;
	COpenAssetImportMesh *m_pBarrelMesh;
	COpenAssetImportMesh *m_pHorseMesh;
	COpenAssetImportMesh *m_Bike;
	COpenAssetImportMesh *m_Eiffel;
	COpenAssetImportMesh *m_Superman;
	COpenAssetImportMesh *m_Lamp;
	CSphere *m_pSphere;
	CSphere *m_roadSign;
	Start *m_StartLine;
	CHighResolutionTimer *m_pHighResolutionTimer;
	CAudio *m_pAudio;
	CCatmullRom *m_pCatmullRom;
	Light *m_lighter;
	CShaderProgram *m_MyShader;

	static const int FPS = 60;
	GameWindow m_gameWindow;
	HINSTANCE m_hInstance;
	int m_frameCount;
	double m_elapsedTime;
	float m_cameraRotation;
	glm::vec3 m_cameraUpView;

public:
	Game();
	~Game();
	static Game& GetInstance();
	LRESULT ProcessEvents(HWND window,UINT message, WPARAM w_param, LPARAM l_param);
	void SetHinstance(HINSTANCE hinstance);
	WPARAM Execute();

private:	
	void Initialise();
	void Update();
	void Render();
	void DisplayFrameRate();
	void GameLoop();
};
