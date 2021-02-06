/* 
OpenGL Template for INM376 / IN3005
City University London, School of Mathematics, Computer Science and Engineering
Source code drawn from a number of sources and examples, including contributions from
 - Ben Humphrey (gametutorials.com), Michal Bubner (mbsoftworks.sk), Christophe Riccio (glm.g-truc.net)
 - Christy Quinn, Sam Kellett and others

 For educational use by Department of Computer Science, City University London UK.

 This template contains a skybox, simple terrain, camera, lighting, shaders, texturing

 Potential ways to modify the code:  Add new geometry types, shaders, change the terrain, load new meshes, change the lighting, 
 different camera controls, different shaders, etc.
 
 Template version 4.0a 30/01/2016
 Dr Greg Slabaugh (gregory.slabaugh.1@city.ac.uk) 
*/


#include "game.h"


// Setup includes
#include "HighResolutionTimer.h"
#include "GameWindow.h"

// Game includes
#include "Camera.h"
#include "Skybox.h"
#include "Plane.h"
#include "Shaders.h"
#include "FreeTypeFont.h"
#include "Sphere.h"
#include "MatrixStack.h"
#include "OpenAssetImportMesh.h"
#include "Audio.h"
#include "CatmullRom.h"
#include "Texture.h"
#include "Start.h"


// Constructor
Game::Game()
{
	m_pSkybox = NULL;
	m_pCamera = NULL;
	m_pShaderPrograms = NULL;
	m_pPlanarTerrain = NULL;
	m_pFtFont = NULL;
	m_pBarrelMesh = NULL;
	m_pHorseMesh = NULL;
	m_Bike = NULL;
	m_Eiffel = NULL;
	m_StartLine = NULL;
	m_Lamp = NULL;
	m_pSphere = NULL;
	m_roadSign = NULL;
	m_pHighResolutionTimer = NULL;
	m_pAudio = NULL;
	m_pCatmullRom = NULL;

	m_dt = 0.0;
	m_framesPerSecond = 0;
	m_frameCount = 0;
	m_elapsedTime = 0.0f;
	m_cameraRotation = 0.0f;
	m_cameraUpView;
	m_currentDistance = 0.00f;

	m_t = 0;

}

// Destructor
Game::~Game() 
{ 
	//game objects
	delete m_pCamera;
	delete m_pSkybox;
	delete m_pPlanarTerrain;
	delete m_pFtFont;
	delete m_pBarrelMesh;
	delete m_pHorseMesh;
	delete m_Bike;
	delete m_Eiffel;
	delete m_StartLine;
	delete m_Lamp;
	delete m_pSphere;
	delete m_roadSign;
	delete m_pAudio;
	delete m_pCatmullRom;

	if (m_pShaderPrograms != NULL) {
		for (unsigned int i = 0; i < m_pShaderPrograms->size(); i++)
			delete (*m_pShaderPrograms)[i];
	}
	delete m_pShaderPrograms;

	//setup objects
	delete m_pHighResolutionTimer;
}

// Initialisation:  This method only runs once at startup
void Game::Initialise() 
{
	// Set the clear colour and depth
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(1.0f);
	m_pModelMatrix = new glm::mat4(1);

	/// Create objects
	m_pCamera = new CCamera;
	m_pSkybox = new CSkybox;
	m_pShaderPrograms = new vector <CShaderProgram *>;
	m_pPlanarTerrain = new CPlane;
	m_pFtFont = new CFreeTypeFont;
	m_pBarrelMesh = new COpenAssetImportMesh;
	m_pHorseMesh = new COpenAssetImportMesh;
	m_Bike = new COpenAssetImportMesh;
	m_Eiffel = new COpenAssetImportMesh;
	m_StartLine = new Start;
	m_Lamp = new COpenAssetImportMesh;
	m_pSphere = new CSphere;
	m_roadSign = new CSphere;
	m_pAudio = new CAudio;
	m_pCatmullRom = new CCatmullRom;
	m_MyShader = new CShaderProgram;

	lampNumbers = 20;

	RECT dimensions = m_gameWindow.GetDimensions();

	int width = dimensions.right - dimensions.left;
	int height = dimensions.bottom - dimensions.top;

	// Set the orthographic and perspective projection matrices based on the image size
	m_pCamera->SetOrthographicProjectionMatrix(width, height); 
	m_pCamera->SetPerspectiveProjectionMatrix(45.0f, (float) width / (float) height, 0.5f, 5000.0f);

	// Load shaders
	vector<CShader> shShaders;
	vector<string> sShaderFileNames;
	sShaderFileNames.push_back("mainShader.vert");
	sShaderFileNames.push_back("mainShader.frag");
	sShaderFileNames.push_back("textShader.vert");
	sShaderFileNames.push_back("textShader.frag");
	sShaderFileNames.push_back("sphereShader.vert");
	sShaderFileNames.push_back("sphereShader.frag");
	sShaderFileNames.push_back("lineShader.vert");
	sShaderFileNames.push_back("lineShader.frag");

	for (int i = 0; i < (int) sShaderFileNames.size(); i++) {
		string sExt = sShaderFileNames[i].substr((int) sShaderFileNames[i].size()-4, 4);
		int iShaderType;
		if (sExt == "vert") iShaderType = GL_VERTEX_SHADER;
		else if (sExt == "frag") iShaderType = GL_FRAGMENT_SHADER;
		else if (sExt == "geom") iShaderType = GL_GEOMETRY_SHADER;
		else if (sExt == "tcnl") iShaderType = GL_TESS_CONTROL_SHADER;
		else iShaderType = GL_TESS_EVALUATION_SHADER;
		CShader shader;
		shader.LoadShader("resources\\shaders\\"+sShaderFileNames[i], iShaderType);
		shShaders.push_back(shader);
	}

	// Create the main shader program
	CShaderProgram *pMainProgram = new CShaderProgram;
	pMainProgram->CreateProgram();
	pMainProgram->AddShaderToProgram(&shShaders[0]);
	pMainProgram->AddShaderToProgram(&shShaders[1]);
	pMainProgram->LinkProgram();
	m_pShaderPrograms->push_back(pMainProgram);

	// Create a shader program for fonts
	CShaderProgram *pFontProgram = new CShaderProgram;
	pFontProgram->CreateProgram();
	pFontProgram->AddShaderToProgram(&shShaders[2]);
	pFontProgram->AddShaderToProgram(&shShaders[3]);
	pFontProgram->LinkProgram();
	m_pShaderPrograms->push_back(pFontProgram);

	// Create a shader program for lightIndicator
	CShaderProgram *pSignProgram = new CShaderProgram;
	pSignProgram->CreateProgram();
	pSignProgram->AddShaderToProgram(&shShaders[4]);
	pSignProgram->AddShaderToProgram(&shShaders[5]);
	pSignProgram->LinkProgram();
	m_pShaderPrograms->push_back(pSignProgram);

	// Create a shader program for roadSign
	CShaderProgram *pLineProgram = new CShaderProgram;
	pLineProgram->CreateProgram();
	pLineProgram->AddShaderToProgram(&shShaders[6]);
	pLineProgram->AddShaderToProgram(&shShaders[7]);
	pLineProgram->LinkProgram();
	m_pShaderPrograms->push_back(pLineProgram);
	// You can follow this pattern to load additional shaders

	// Create the skybox
	// Skybox downloaded from http://www.akimbo.in/forum/viewtopic.php?f=10&t=9
	m_pSkybox->Create(2500.0f);
	
	// Create the planar terrain
	m_pPlanarTerrain->Create("resources\\textures\\", "snow.jpg", 1000.0f, 1000.0f, 10.0f); // Texture downloaded from http://www.psionicgames.com/?page_id=26 on 24 Jan 2013

	m_pFtFont->LoadSystemFont("arial.ttf", 50);
	m_pFtFont->SetShaderProgram(pFontProgram);

	// Load some meshes in OBJ format
	m_pBarrelMesh->Load("resources\\models\\Barrel\\Barrel02.obj");  // Downloaded from http://www.psionicgames.com/?page_id=24 on 24 Jan 2013
	m_pHorseMesh->Load("resources\\models\\Horse\\Horse2.obj");  // Downloaded from http://opengameart.org/content/horse-lowpoly on 24 Jan 2013
	
	m_Bike->Load("resources\\models\\Bike\\Srad\\Srad750.obj");
	m_Eiffel->Load("resources\\models\\Eiffel_Tower\\Eiffel_Tower.obj");
	m_StartLine->Create("resources\\textures\\road2.jpg");

	m_Lamp->Load("resources\\models\\Lamp\\lightpost.obj");

	// Create a sphere
	m_pSphere->Create("resources\\textures\\", "dirtpile01.jpg", 25, 25);  // Texture downloaded from http://www.psionicgames.com/?page_id=26 on 24 Jan 2013
	m_roadSign->Create("resources\\textures\\", "dirtpile01.jpg", 25, 25);
	//glEnable(GL_CULL_FACE);

	// Initialise audio and play background music
	m_pAudio->Initialise();
	//m_pAudio->LoadEventSound("Resources\\Audio\\Boing.wav");					// Royalty free sound from freesound.org
	//m_pAudio->LoadMusicStream("Resources\\Audio\\DST-Garote.mp3");	// Royalty free music from http://www.nosoapradio.us/
	m_pAudio->PlayMusicStream();
	
	m_pCatmullRom->CreateCentreline();
	m_pCatmullRom->CreateOffsetCurves();
	m_pCatmullRom->CreateTrack();
	m_pHighResolutionTimer->Start();
}


// Render method runs repeatedly in a loop
void Game::Render() 
{
	// Clear the buffers and enable depth testing (z-buffering)
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// Set up a matrix stack
	glutil::MatrixStack modelViewMatrixStack;
	modelViewMatrixStack.SetIdentity();

	// Use the main shader program 
	CShaderProgram *pMainProgram = (*m_pShaderPrograms)[0];
	pMainProgram->UseProgram();
	pMainProgram->SetUniform("bUseTexture", true);
	pMainProgram->SetUniform("sampler0", 0);
	pMainProgram->SetUniform("CubeMapTex", 1);
	

	// Set the projection matrix
	pMainProgram->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());

	// Call LookAt to create the view matrix and put this on the modelViewMatrix stack. 
	// Store the view matrix and the normal matrix associated with the view matrix for later (they're useful for lighting -- since lighting is done in eye coordinates)
	modelViewMatrixStack.LookAt(m_pCamera->GetPosition(), m_pCamera->GetView(), m_pCamera->GetUpVector());
	glm::mat4 viewMatrix = modelViewMatrixStack.Top();
	glm::mat3 viewNormalMatrix = m_pCamera->ComputeNormalMatrix(viewMatrix);

	
	// Set light and materials in main shader program
	glm::vec4 lightPosition1 = glm::vec4(-100, 100, -100, 1); // Position of light source *in world coordinates*
	glm::vec4 lightPosition2(100, 50, 0, 1); // Position of light source *in world coordinates*
	pMainProgram->SetUniform("light1.position", viewMatrix*lightPosition1); // Position of light source *in eye coordinates*
	pMainProgram->SetUniform("light1.La", glm::vec3(1.0f));		// Ambient colour of light
	pMainProgram->SetUniform("light1.Ld", glm::vec3(1.0f));		// Diffuse colour of light
	pMainProgram->SetUniform("light1.Ls", glm::vec3(1.0f));		// Specular colour of light
	pMainProgram->SetUniform("material1.Ma", glm::vec3(1.0f));	// Ambient material reflectance
	pMainProgram->SetUniform("material1.Md", glm::vec3(0.0f));	// Diffuse material reflectance
	pMainProgram->SetUniform("material1.Ms", glm::vec3(0.0f));	// Specular material reflectance
	pMainProgram->SetUniform("material1.shininess", 15.0f);		// Shininess material property

	// Render the skybox and terrain with full ambient reflectance 
	modelViewMatrixStack.Push();
		pMainProgram->SetUniform("renderSkybox", true);
		// Translate the modelview matrix to the camera eye point so skybox stays centred around camera
		glm::vec3 vEye = m_pCamera->GetPosition();
		modelViewMatrixStack.Translate(vEye);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		//m_pSkybox->Render(vpos.x, vpos.y - 50, vpos.z, 30, 30, 30);
		m_pSkybox->Render();
		glEnable(GL_LIGHTING);
		pMainProgram->SetUniform("renderSkybox", false);
	modelViewMatrixStack.Pop();
	
	vector<string> LightName;
	LightName.push_back("light2");
	LightName.push_back("light3");
	LightName.push_back("light4");
	LightName.push_back("light5");
	LightName.push_back("light6");
	LightName.push_back("light7");
	LightName.push_back("light8");
	LightName.push_back("light9");
	LightName.push_back("light10");
	LightName.push_back("light11");
	LightName.push_back("light12");
	LightName.push_back("light13");
	LightName.push_back("light14");
	LightName.push_back("light15");
	LightName.push_back("light16");
	LightName.push_back("light17");
	LightName.push_back("light18");
	LightName.push_back("light19");
	LightName.push_back("light20");
	LightName.push_back("light21");

	std::vector<glm::vec3> leftPoints = m_pCatmullRom->GetLeftOffsetCurve();
	int numPoints = (int)leftPoints.size();

	for (int i = 0; i < lampNumbers; ++i)
	{
		int index = (numPoints / 20) * (i - 20) + numPoints; 
		glm::vec3 leftPoint = leftPoints[index];

		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(leftPoint);
		modelViewMatrixStack.Rotate(glm::vec3(0,1,0), 180.0f);
		modelViewMatrixStack.Scale(5.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_Lamp->Render();
		modelViewMatrixStack.Pop();

	}

	for (int y = 0; y < lampNumbers; y++)
	{
		int index = (numPoints / 20) * (y - 20) + numPoints;
		glm::vec3 leftPoint = leftPoints[index];

		string Lighttext = LightName[y] + ".position";
		string LDtext = LightName[y] + ".Ld";
		string LStext = LightName[y] + ".Ls";
		string Directiontext = LightName[y] + ".direction";
		string Exponenttext = LightName[y] + ".exponent";
		string Cutofftext = LightName[y] + ".cutoff";
		glm::vec3 lightPoint;
		glm::vec3 lightPoint2 = glm::vec3(0, 20, 0);
		lightPoint = leftPoint + lightPoint2;
		pMainProgram->SetUniform(Lighttext, viewMatrix * glm::vec4(lightPoint, 1)); // Position of light source *in eye coordinates*
		pMainProgram->SetUniform(LDtext, glm::vec3(10, 10, 0));		// Diffuse colour of light
		pMainProgram->SetUniform(LStext, glm::vec3(10, 10, 0));		// Specular colour of light
		pMainProgram->SetUniform(Directiontext, glm::normalize(viewNormalMatrix*glm::vec3(0, -1, 0)));
		pMainProgram->SetUniform(Exponenttext, 20.0f);
		pMainProgram->SetUniform(Cutofftext, 30.0f);
	}

	// Render the planar terrain
	modelViewMatrixStack.Push();
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pPlanarTerrain->Render();
	modelViewMatrixStack.Pop();

	// Turn on diffuse + specular materials
	pMainProgram->SetUniform("material1.Ma", glm::vec3(0.5f));	// Ambient material reflectance
	pMainProgram->SetUniform("material1.Md", glm::vec3(0.5f));	// Diffuse material reflectance
	pMainProgram->SetUniform("material1.Ms", glm::vec3(1.0f));	// Specular material reflectance

	
	// Render the bike 
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(m_Bike->GetPosition() + glm::vec3(0.0f, 2.5f, 0.0f));
	//modelViewMatrixStack *= m_Bike->getRotation();
	modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 180.6f);
	modelViewMatrixStack.Scale(4.0f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_Bike->Render();
	pMainProgram->SetUniform("bUseTexture", true);
	modelViewMatrixStack.Pop();
	

	// Render the barrel 
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(10.0f, 0.0f, 0.0f));
	modelViewMatrixStack.Scale(5.0f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pBarrelMesh->Render();
	modelViewMatrixStack.Pop();

	// Render the sphere
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(15.0f, 2.0f, 0.0f));
	modelViewMatrixStack.Scale(2.0f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	// To turn off texture mapping and use the sphere colour only (currently white material), uncomment the next line
	//pMainProgram->SetUniform("bUseTexture", false);
	m_pSphere->Render();
	modelViewMatrixStack.Pop();

	// Eiffel Tower
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(-350.0f, -4.0f, 320.0f));
	modelViewMatrixStack.Rotate(glm::vec3(5.0f, 0.0f, 0.0f), 4.7f);
	modelViewMatrixStack.Scale(0.005f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_Eiffel->Render();
	pMainProgram->SetUniform("bUseTexture", true);
	modelViewMatrixStack.Pop();

	//startline
	//modelViewMatrixStack.Push();
	//modelViewMatrixStack.Translate(100.0f, -8.0f, 0.0f);
	//modelViewMatrixStack.Scale(15.0f);
	//modelViewMatrixStack.Rotate(glm::vec3(10.0f, 180.0f, 100.0f), 180.0f);
	//pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	//pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	//m_StartLine->Render();
	//modelViewMatrixStack.Pop();

	//render centreline
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(0.0f, 0.0f, 0.0f));
	pMainProgram->SetUniform("bUseTexture", true); // turn off texturing
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix",
	m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pCatmullRom->RenderCentreline();
	modelViewMatrixStack.Pop();

	//renderOffSetCurves
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(0.0f, 0.0f, 0.0f));
	pMainProgram->SetUniform("bUseTexture", true); // turn off texturing
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix",
	m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pCatmullRom->RenderOffsetCurves();
	modelViewMatrixStack.Pop();

	//renderTrack
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(0.0f, 0.0f, 0.0f));
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pCatmullRom->RenderTrack();
	modelViewMatrixStack.Pop();

	std::vector<glm::vec3> centreLinePoints = m_pCatmullRom->GetCentrelinePoints();
	int centreLinePoints1 = (int)centreLinePoints.size();

	for (int i = 0; i < lampNumbers; ++i)
	{

		int indexCenter = (centreLinePoints1 / 20) * (i - 20) + centreLinePoints1;
		glm::vec3 centerPoints = centreLinePoints[indexCenter];

		CShaderProgram *pSignProgram = (*m_pShaderPrograms)[2];
		pSignProgram->UseProgram();

		pSignProgram->SetUniform("material1.Ma", glm::vec3(0.0f, 1.0f, 0.0f));
		pSignProgram->SetUniform("material1.Md", glm::vec3(0.0f, 1.0f, 0.0f));
		pSignProgram->SetUniform("material1.Ms", glm::vec3(1.0f, 1.0f, 1.0f));
		pSignProgram->SetUniform("material1.shininess", 50.0f);
		pSignProgram->SetUniform("light1.La", glm::vec3(0.15f, 0.15f, 0.15f));
		pSignProgram->SetUniform("light1.Ld", glm::vec3(1.0f, 1.0f, 1.0f));
		pSignProgram->SetUniform("light1.Ls", glm::vec3(1.0f, 1.0f, 1.0f));
		pSignProgram->SetUniform("t", m_t);

		modelViewMatrixStack.Push(); 
		modelViewMatrixStack.Translate(centerPoints);
			modelViewMatrixStack.Scale(0.5f);
			pSignProgram->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());
			pSignProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
			pSignProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
			m_roadSign->Render();
		 modelViewMatrixStack.Pop();
	}

	for (int i = 0; i < 20; ++i)
	{
		int index = (numPoints / 20) * (i - 20) + numPoints;
		glm::vec3 leftPoint = leftPoints[index];

		CShaderProgram *pLightProgram = (*m_pShaderPrograms)[3];
		pLightProgram->UseProgram();

		pLightProgram->SetUniform("material1.Ma", glm::vec3(1.0f, 1.0f, 0.0f));
		pLightProgram->SetUniform("material1.Md", glm::vec3(1.0f, 1.0f, 0.0f));
		pLightProgram->SetUniform("material1.Ms", glm::vec3(1.0f, 1.0f, 1.0f));
		pLightProgram->SetUniform("material1.shininess", 50.0f);
		pLightProgram->SetUniform("light1.La", glm::vec3(0.15f, 0.15f, 0.15f));
		pLightProgram->SetUniform("light1.Ld", glm::vec3(1.0f, 1.0f, 1.0f));
		pLightProgram->SetUniform("light1.Ls", glm::vec3(1.0f, 1.0f, 1.0f));

		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(leftPoint + glm::vec3(0, 18, 0));
		modelViewMatrixStack.Scale(1.8f);
		pLightProgram->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());
		pLightProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pLightProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		//pLineProgram->SetUniform("lines", centerPoints);
		m_pSphere->Render();
		modelViewMatrixStack.Pop();

	}

	//m_pCatmullRom->RenderStartLine();
	// Draw the 2D graphics after the 3D graphics
	DisplayFrameRate();

	// Swap buffers to show the rendered image
	SwapBuffers(m_gameWindow.Hdc());		
}


// Update method runs repeatedly with the Render method
void Game::Update()
{	
	m_currentDistance += m_dt * 0.07f;

	cameraPos = glm::vec3();

	glm::vec3 up = glm::vec3(0, 3, 0);

	m_pCatmullRom->Sample(m_currentDistance, cameraPos, up);
	m_pCatmullRom->Sample(m_currentDistance + 1, playerPos, up);

	glm::vec3 tangentCamera = glm::normalize(playerPos - cameraPos);

	m_pCatmullRom->Sample(m_currentDistance + 3, playerLooking, up);

	glm::vec3 T = glm::normalize(playerLooking - playerPos);
	glm::vec3 N = glm::normalize(glm::cross(T, glm::vec3(0, 1, 0)));
	glm::vec3 B = glm::normalize(glm::cross(N, T));

	glm::mat4 m_orientation = glm::mat4(glm::mat3(T, B, N));
	//m_Bike->setRotation(m_orientation);

	glm::vec3 thirdCameraPos = cameraPos - glm::vec3(0, -20, 0);
	glm::vec3 sideCameraPos = playerPos - glm::vec3(-10, -5, 0);

	glm::vec3 otherThing = cameraPos + 20.0f * tangentCamera;

	//m_Bike->setPosition(otherThing);
	m_Bike->Update(m_dt);

	if (TopCameraEnabled)
	{
		glm::vec3 m_position = glm::vec3(0.0f, 1000.0f, 0.0f);
		glm::vec3 m_view = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 m_upVector = glm::vec3(1.0f, 0.0f, 0.0f);

		m_pCamera->Set(m_position, m_view, m_upVector);
	}
	else if (FreeModeCameraEnabled)
	{
		m_pCamera->Update(m_dt);
	}
	else if (ThirdPersonCameraEnabled)
	{
		m_pCamera->Set(thirdCameraPos, otherThing, up);
	}
	else if (SideCameraEnabled)
	{
		m_pCamera->Set(sideCameraPos, otherThing, up);
	}
	else if (BikeCameraEnabled)
	{
		glm::vec3 lol = m_Bike->GetPosition();
		m_pCamera->Set(lol, otherThing, up);
		m_Bike->Update(m_dt);
	}
}

void Game::DisplayFrameRate()
{

	CShaderProgram *fontProgram = (*m_pShaderPrograms)[1];

	RECT dimensions = m_gameWindow.GetDimensions();
	int height = dimensions.bottom - dimensions.top;

	// Increase the elapsed time and frame counter
	m_elapsedTime += m_dt;
	m_frameCount++;

	// Now we want to subtract the current time by the last time that was stored
	// to see if the time elapsed has been over a second, which means we found our FPS.
	if (m_elapsedTime > 1000)
    {
		m_elapsedTime = 0;
		m_framesPerSecond = m_frameCount;

		// Reset the frames per second
		m_frameCount = 0;
    }

	if (m_framesPerSecond > 0) {
		// Use the font shader program and render the text
		fontProgram->UseProgram();
		glDisable(GL_DEPTH_TEST);
		fontProgram->SetUniform("matrices.modelViewMatrix", glm::mat4(1));
		fontProgram->SetUniform("matrices.projMatrix", m_pCamera->GetOrthographicProjectionMatrix());
		fontProgram->SetUniform("vColour", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		m_pFtFont->Render(2, height - 20, 20, "Press 1 for Top View");
		m_pFtFont->Render(2, height - 40, 20, "Press 2 for Free View");
		m_pFtFont->Render(2, height - 60, 20, "Press 3 for Third View");
		m_pFtFont->Render(2, height - 80, 20, "Press 4 for Side Person View");
		m_pFtFont->Render(700, height - 20, 20, "FPS: %d", m_framesPerSecond);
	}
}

// The game loop runs repeatedly until game over
void Game::GameLoop()
{
	/*
	// Fixed timer
	dDt = pHighResolutionTimer->Elapsed();
	if (dDt > 1000.0 / (double) Game::FPS) {
		pHighResolutionTimer->Start();
		Update();
		Render();
	}
	*/
	
	
	// Variable timer
	m_pHighResolutionTimer->Start();
	Update();
	Render();
	m_dt = m_pHighResolutionTimer->Elapsed();
	

}


WPARAM Game::Execute() 
{
	m_pHighResolutionTimer = new CHighResolutionTimer;
	m_gameWindow.Init(m_hInstance);

	if(!m_gameWindow.Hdc()) {
		return 1;
	}

	Initialise();

	m_pHighResolutionTimer->Start();

	
	MSG msg;

	while(1) {													
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) { 
			if(msg.message == WM_QUIT) {
				break;
			}

			TranslateMessage(&msg);	
			DispatchMessage(&msg);
		} else if (m_appActive) {
			GameLoop();
		} 
		else Sleep(200); // Do not consume processor power if application isn't active
	}

	m_gameWindow.Deinit();

	return(msg.wParam);
}

LRESULT Game::ProcessEvents(HWND window,UINT message, WPARAM w_param, LPARAM l_param) 
{
	LRESULT result = 0;

	switch (message) {


	case WM_ACTIVATE:
	{
		switch(LOWORD(w_param))
		{
			case WA_ACTIVE:
			case WA_CLICKACTIVE:
				m_appActive = true;
				m_pHighResolutionTimer->Start();
				break;
			case WA_INACTIVE:
				m_appActive = false;
				break;
		}
		break;
		}

	case WM_SIZE:
			RECT dimensions;
			GetClientRect(window, &dimensions);
			m_gameWindow.SetDimensions(dimensions);
		break;

	case WM_PAINT:
		PAINTSTRUCT ps;
		BeginPaint(window, &ps);
		EndPaint(window, &ps);
		break;

	case WM_KEYDOWN:
		switch(w_param) {
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		case '1':
			TopCameraEnabled = true;
			FreeModeCameraEnabled = false;
			ThirdPersonCameraEnabled = false;
			SideCameraEnabled = false;
			BikeCameraEnabled = false;
			break;
		case '2':
			TopCameraEnabled = false;
			FreeModeCameraEnabled = true;
			ThirdPersonCameraEnabled = false;
			SideCameraEnabled = false;
			BikeCameraEnabled = false;
			break;
		case '3':
			TopCameraEnabled = false;
			FreeModeCameraEnabled = false;
			ThirdPersonCameraEnabled = true;
			SideCameraEnabled = false;
			BikeCameraEnabled = false;
			break;
		case '4':
			TopCameraEnabled = false;
			FreeModeCameraEnabled = false;
			ThirdPersonCameraEnabled = false;
			SideCameraEnabled = true;
			BikeCameraEnabled = false;
			break;
		case '5':
			TopCameraEnabled = false;
			FreeModeCameraEnabled = false;
			ThirdPersonCameraEnabled = false;
			SideCameraEnabled = false;
			BikeCameraEnabled = true;
			break;
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		result = DefWindowProc(window, message, w_param, l_param);
		break;
	}

	return result;
}

Game& Game::GetInstance() 
{
	static Game instance;

	return instance;
}

void Game::SetHinstance(HINSTANCE hinstance) 
{
	m_hInstance = hinstance;
}

LRESULT CALLBACK WinProc(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
	return Game::GetInstance().ProcessEvents(window, message, w_param, l_param);
}

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE, PSTR, int) 
{
	Game &game = Game::GetInstance();
	game.SetHinstance(hinstance);

	return game.Execute();
}
