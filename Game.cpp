#include "Game.h"

#include "WICTextureLoader.h"
#include "DDSTextureLoader.h"

#define max(a,b) (((a) > (b)) ? (a):(b))
#define min(a,b) (((a) < (b)) ? (a):(b))

Game::Game(HINSTANCE hInstance)
	: DXCore(
		hInstance,		   // The application's handle
		"DirectX Game",	   // Text for the window's title bar
		1280,			   // Width of the window's client area
		720,			   // Height of the window's client area
		true)			   // Show extra stats (fps) in title bar?
{
	// Initialize fields
	vertexBuffer = 0;
	indexBuffer = 0;
	baseVertexShader = 0;
	basePixelShader = 0;
	camera = 0;
	

#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.");
#endif
}


Game::~Game()
{
	delete camera;

	delete basePixelShader;
	delete baseVertexShader;
	delete skyVertexShader;
	delete skyPixelShader;

	delete sphereMesh;
	delete cubeMesh;

	delete materialEarth;
	delete materialCobbleStone;
	delete materialRed;
	delete materialYellow;
	delete materialSkyBox;
	delete materialSnowTracks;
	delete materialEmpty;

	delete skyBoxEntity;
	for(auto& se: sphereEntities) delete se;
	for (auto& fe : flatEntities) delete fe;
	
	rasterizer->Release();

	skyDepthState->Release();
	skyRasterizerState->Release();

	sampler->Release();
	
	earthDayMapSRV->Release();
	earthNormalMapSRV->Release();
	cobbleStoneSRV->Release();
	cobbleStoneNormalSRV->Release();
	plainRedSRV->Release();
	plainYellowSRV->Release();
	plainNormalMapSRV->Release();
	snowTracksSRV->Release();
	snowTracksNormalSRV->Release();

	skySRV->Release();
	


}


void Game::Init()
{
	//Initialize helper methods
	CameraInitialize();
	ShadersInitialize();
	ModelsInitialize();
	LoadTextures();
	MaterialsInitialize();
	SkyBoxInitialize();
	GameEntityInitialize();


	//Setup rasterizer state 
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.DepthClipEnable = false;

	device->CreateRasterizerState(&rasterizerDesc, &rasterizer);

	//Setup blend state 
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	//device->CreateBlendState(&blendDescDR, &blendDR);

	//TODO : DEPTH STENCIL STATE
	//TODO : Set the OMdepthstate also change depth target in Set Render target

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	//device->CreateDepthStencilState(&depthStencilDescDR, &depthStateDR);

	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Game::CameraInitialize()
{
	camera = new Camera(0, 0, -4);
	camera->UpdateProjectionMatrix((float)width / height);
}

void Game::ShadersInitialize()
{
	baseVertexShader = new SimpleVertexShader(device, context);
	if (!baseVertexShader->LoadShaderFile(L"Debug/BaseVertexShader.cso"))
		baseVertexShader->LoadShaderFile(L"BaseVertexShader.cso");

	basePixelShader = new SimplePixelShader(device, context);
	if (!basePixelShader->LoadShaderFile(L"Debug/BasePixelShader.cso"))
		basePixelShader->LoadShaderFile(L"BasePixelShader.cso");

	skyVertexShader = new SimpleVertexShader(device, context);
	if (!skyVertexShader->LoadShaderFile(L"Debug/SkyBoxVertexShader.cso"))
		skyVertexShader->LoadShaderFile(L"SkyBoxVertexShader.cso");

	skyPixelShader = new SimplePixelShader(device, context);
	if (!skyPixelShader->LoadShaderFile(L"Debug/SkyBoxPixelShader.cso"))
		skyPixelShader->LoadShaderFile(L"SkyBoxPixelShader.cso");
}

void Game::ModelsInitialize()
{
	sphereMesh = new Mesh("Models/sphere.obj", device);
	cubeMesh = new Mesh("Models/cube.obj", device);
}

void Game::LoadTextures()
{
	CreateWICTextureFromFile(device, context, L"Textures/earth_daymap.jpg", 0, &earthDayMapSRV);
	CreateWICTextureFromFile(device, context, L"Textures/earth_normal_map.tif", 0, &earthNormalMapSRV);
	CreateWICTextureFromFile(device, context, L"Textures/rock.jpg", 0, &cobbleStoneSRV);
	CreateWICTextureFromFile(device, context, L"Textures/rockNormals.jpg", 0, &cobbleStoneNormalSRV);
	CreateWICTextureFromFile(device, context, L"Textures/red.jpg", 0, &plainRedSRV);
	CreateWICTextureFromFile(device, context, L"Textures/yellow.jpg", 0, &plainYellowSRV);
	CreateWICTextureFromFile(device, context, L"Textures/plainNormal.png", 0, &plainNormalMapSRV);
	CreateWICTextureFromFile(device, context, L"Textures/snowTracks.tif", 0, &snowTracksSRV);
	CreateWICTextureFromFile(device, context, L"Textures/snowTracksNormal.tif", 0, &snowTracksNormalSRV);
}

void Game::MaterialsInitialize()
{
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	device->CreateSamplerState(&samplerDesc, &sampler);


	materialEarth = new Material(basePixelShader, baseVertexShader, earthDayMapSRV, earthNormalMapSRV, sampler);
	materialCobbleStone = new Material(basePixelShader, baseVertexShader, cobbleStoneSRV, cobbleStoneNormalSRV, sampler);
	materialRed = new Material(basePixelShader, baseVertexShader, plainRedSRV, plainNormalMapSRV, sampler);
	materialYellow = new Material(basePixelShader, baseVertexShader, plainYellowSRV, plainNormalMapSRV, sampler);
	materialSnowTracks = new Material(basePixelShader, baseVertexShader, snowTracksSRV, snowTracksNormalSRV, sampler);
	materialEmpty = new Material(basePixelShader, baseVertexShader, 0, plainNormalMapSRV, sampler);

	materialSkyBox = new Material(skyPixelShader, skyVertexShader, skySRV, plainNormalMapSRV, sampler);
	
}

void Game::SkyBoxInitialize()
{
	CreateDDSTextureFromFile(device, L"Textures/SunnyCubeMap.dds", 0, &skySRV);

	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_FRONT;
	rasterizerDesc.DepthClipEnable = true;
	device->CreateRasterizerState(&rasterizerDesc, &skyRasterizerState);

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	device->CreateDepthStencilState(&depthStencilDesc, &skyDepthState);
}

void Game::GameEntityInitialize()
{
	skyBoxEntity = new GameEntity(cubeMesh, materialSkyBox);
	
	GameEntity* sphere0 = new GameEntity(sphereMesh, materialCobbleStone);
	GameEntity* sphere1 = new GameEntity(sphereMesh, materialCobbleStone);
	GameEntity* sphere2 = new GameEntity(sphereMesh, materialCobbleStone);
	GameEntity* sphere3 = new GameEntity(sphereMesh, materialCobbleStone);
	GameEntity* sphere4 = new GameEntity(sphereMesh, materialCobbleStone);
	GameEntity* sphere5 = new GameEntity(sphereMesh, materialCobbleStone);
	GameEntity* sphere6 = new GameEntity(sphereMesh, materialCobbleStone);
	GameEntity* sphere7 = new GameEntity(sphereMesh, materialCobbleStone);
	GameEntity* sphere8 = new GameEntity(sphereMesh, materialCobbleStone);

	sphereEntities.push_back(sphere0);
	sphereEntities.push_back(sphere1);
	sphereEntities.push_back(sphere2);
	sphereEntities.push_back(sphere3);
	sphereEntities.push_back(sphere4);
	sphereEntities.push_back(sphere5);
	sphereEntities.push_back(sphere6);
	sphereEntities.push_back(sphere7);
	sphereEntities.push_back(sphere8);

	sphereEntities[0]->SetPosition(0, 0, 2);
	sphereEntities[1]->SetPosition(2, 0, 2);
	sphereEntities[2]->SetPosition(-2, 0, 2);
	sphereEntities[3]->SetPosition(0, 0, 0);
	sphereEntities[4]->SetPosition(2, 0, 0);
	sphereEntities[5]->SetPosition(-2, 0, 0);
	sphereEntities[6]->SetPosition(0, 0, -2);
	sphereEntities[7]->SetPosition(2, 0, -2);
	sphereEntities[8]->SetPosition(-2, 0, -2);


	GameEntity* flat0 = new GameEntity(cubeMesh, materialEmpty);
	GameEntity* flat1 = new GameEntity(cubeMesh, materialEmpty);
	GameEntity* flat2 = new GameEntity(cubeMesh, materialEmpty);
	GameEntity* flat3 = new GameEntity(cubeMesh, materialEmpty);
	
	flatEntities.push_back(flat0);
	flatEntities.push_back(flat1);
	flatEntities.push_back(flat2);
	flatEntities.push_back(flat3);

	flatEntities[0]->SetScale(5.0f, 0.01f, 5.0f);
	flatEntities[1]->SetScale(5.0f, 0.01f, 5.0f);
	flatEntities[2]->SetScale(5.0f, 0.01f, 5.0f);
	flatEntities[3]->SetScale(5.0f, 0.01f, 5.0f);

	flatEntities[0]->SetPosition(0, -1.5f, 0);
	flatEntities[1]->SetPosition(4.5f, 0, 0);
	flatEntities[2]->SetPosition(0, 0, 4.5f);
	flatEntities[3]->SetPosition(-4.5f, 0, 0);

	flatEntities[1]->SetRotation(0, 0, -1.6f);
	flatEntities[2]->SetRotation(1.6f, 0, 0);
	flatEntities[3]->SetRotation(0, 0, 1.6f);
}

void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();

	// Update the projection matrix assuming the
	// camera exists
	if (camera)
		camera->UpdateProjectionMatrix((float)width / height);
}

void Game::Update(float deltaTime, float totalTime)
{
	camera->Update(deltaTime);

	//Update Spheres
	for (int i = 0; i <= 8; i++)
	{
		sphereEntities[i]->UpdateWorldMatrix();
	}

	//Update Flats
	for (int i = 0; i <= 3; i++)
	{
		flatEntities[i]->UpdateWorldMatrix();
	}


	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();
}

void Game::Draw(float deltaTime, float totalTime)
{
	// Background color (Cornflower Blue in this case) for clearing
	const float color[4] = { 0.6f, 0.6f, 0.6f, 0.0f };

	// Clear the render target and depth buffer (erases what's on the screen)
	//  - Do this ONCE PER FRAME
	//  - At the beginning of Draw (before drawing *anything*)
	context->ClearRenderTargetView(backBufferRTV, color);
	context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	context->RSSetState(rasterizer);

	//Render Spheres
	for (int i = 0; i <= 8 ; i++) 
	{
		render.RenderProcess(sphereEntities[i], vertexBuffer, indexBuffer, baseVertexShader, basePixelShader, camera, context);
	}

	//Render Flats
	for (int i = 0; i <= 3; i++)
	{
		render.RenderProcess(flatEntities[i], vertexBuffer, indexBuffer, baseVertexShader, basePixelShader, camera, context);
	}

	context->RSSetState(NULL);

	render.RenderSkyBox(cubeMesh, vertexBuffer, indexBuffer, skyVertexShader, skyPixelShader, camera, context, skyRasterizerState, skyDepthState, skySRV);

	swapChain->Present(0, 0);
}

#pragma region Mouse Input

// --------------------------------------------------------
// Helper method for mouse clicking.  We get this information
// from the OS-level messages anyway, so these helpers have
// been created to provide basic mouse input if you want it.
// --------------------------------------------------------
void Game::OnMouseDown(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...

	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;

	// Caputure the mouse so we keep getting mouse move
	// events even if the mouse leaves the window.  we'll be
	// releasing the capture once a mouse button is released
	SetCapture(hWnd);
}

// --------------------------------------------------------
// Helper method for mouse release
// --------------------------------------------------------
void Game::OnMouseUp(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...

	// We don't care about the tracking the cursor outside
	// the window anymore (we're not dragging if the mouse is up)
	ReleaseCapture();
}

// --------------------------------------------------------
// Helper method for mouse movement.  We only get this message
// if the mouse is currently over the window, or if we're 
// currently capturing the mouse.
// --------------------------------------------------------
void Game::OnMouseMove(WPARAM buttonState, int x, int y)
{
	// Check left mouse button
	if (buttonState & 0x0001) {
		float xDiff = (x - prevMousePos.x) * 0.005f;
		float yDiff = (y - prevMousePos.y) * 0.005f;
		camera->Rotate(yDiff, xDiff);
	}

	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;
}

// --------------------------------------------------------
// Helper method for mouse wheel scrolling.  
// WheelDelta may be positive or negative, depending 
// on the direction of the scroll
// --------------------------------------------------------
void Game::OnMouseWheel(float wheelDelta, int x, int y)
{
	// Add any custom code here...
}

#pragma endregion
