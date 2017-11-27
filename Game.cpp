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
	delete PBRVertexShader;
	delete PBRPixelShader;
	delete PBRMatPixelShader;

	delete sphereMesh;
	delete cubeMesh;

	delete materialAluminiumInsulator;
	delete materialGold;
	delete materialGunMetal;
	delete materialLeather;
	delete materialSuperHeroFabric;
	delete materialSkyBox;

	delete skyBoxEntity;
	/*for(auto& se: sphereEntities) delete se;
	for (auto& fe : flatEntities) delete fe;*/
	delete pbrSphere;
	delete pbrSphere1;
	delete pbrSphere2;
	delete pbrSphere3;
	delete pbrSphere4;

	for (size_t i = 0; i < 6; i++)
		for (size_t j = 0; j < 6; j++)
		{
			delete pbrSpheres[i][j];
		}

	rasterizer->Release();

	skyDepthState->Release();
	skyRasterizerState->Release();

	sampler->Release();
	
	AluminiumInsulator_Albedo->Release();
	AluminiumInsulator_Normal->Release();
	AluminiumInsulator_Metallic->Release();
	AluminiumInsulator_Rough->Release();

	Gold_Albedo->Release();
	Gold_Normal->Release();
	Gold_Metallic->Release();
	Gold_Rough->Release();

	GunMetal_Albedo->Release();
	GunMetal_Normal->Release();
	GunMetal_Metallic->Release();
	GunMetal_Rough->Release();

	Leather_Albedo->Release();
	Leather_Normal->Release();
	Leather_Metallic->Release();
	Leather_Rough->Release();

	SuperHeroFabric_Albedo->Release();
	SuperHeroFabric_Normal->Release();
	SuperHeroFabric_Metallic->Release();
	SuperHeroFabric_Rough->Release();

	skySRV->Release();
	


}


void Game::Init()
{
	//Initialize helper methods
	CameraInitialize();
	ShadersInitialize();
	ModelsInitialize();
	LoadTextures();
	SkyBoxInitialize();
	MaterialsInitialize();
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
	camera = new Camera(0.0f, 1.0f, -14.0f);
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

	PBRVertexShader = new SimpleVertexShader(device, context);
	if (!PBRVertexShader->LoadShaderFile(L"Debug/PBRVertexShader.cso"))
		PBRVertexShader->LoadShaderFile(L"PBRVertexShader.cso");

	PBRPixelShader = new SimplePixelShader(device, context);
	if (!PBRPixelShader->LoadShaderFile(L"Debug/PBRPixelShader.cso"))
		PBRPixelShader->LoadShaderFile(L"PBRPixelShader.cso");

	PBRMatPixelShader = new SimplePixelShader(device, context);
	if (!PBRMatPixelShader->LoadShaderFile(L"Debug/PBRMatPixelShader.cso"))
		PBRMatPixelShader->LoadShaderFile(L"PBRMatPixelShader.cso");
}

void Game::ModelsInitialize()
{
	sphereMesh = new Mesh("Models/sphere.obj", device);
	cubeMesh = new Mesh("Models/cube.obj", device);
}

void Game::LoadTextures()
{
	CreateWICTextureFromFile(device, context, L"Textures/AluminiumInsulator_Albedo.png", 0, &AluminiumInsulator_Albedo);
	CreateWICTextureFromFile(device, context, L"Textures/AluminiumInsulator_Normal.png", 0, &AluminiumInsulator_Normal);
	CreateWICTextureFromFile(device, context, L"Textures/AluminiumInsulator_Metallic.png", 0, &AluminiumInsulator_Metallic);
	CreateWICTextureFromFile(device, context, L"Textures/AluminiumInsulator_Rough.png", 0, &AluminiumInsulator_Rough);
	
	CreateWICTextureFromFile(device, context, L"Textures/Gold_Albedo.png", 0, &Gold_Albedo);
	CreateWICTextureFromFile(device, context, L"Textures/Gold_Normal.png", 0, &Gold_Normal);
	CreateWICTextureFromFile(device, context, L"Textures/Gold_Metallic.png", 0, &Gold_Metallic);
	CreateWICTextureFromFile(device, context, L"Textures/Gold_Rough.png", 0, &Gold_Rough);

	CreateWICTextureFromFile(device, context, L"Textures/GunMetal_Albedo.png", 0, &GunMetal_Albedo);
	CreateWICTextureFromFile(device, context, L"Textures/GunMetal_Normal.png", 0, &GunMetal_Normal);
	CreateWICTextureFromFile(device, context, L"Textures/GunMetal_Metallic.png", 0, &GunMetal_Metallic);
	CreateWICTextureFromFile(device, context, L"Textures/GunMetal_Rough.png", 0, &GunMetal_Rough);

	CreateWICTextureFromFile(device, context, L"Textures/Leather_Albedo.png", 0, &Leather_Albedo);
	CreateWICTextureFromFile(device, context, L"Textures/Leather_Normal.png", 0, &Leather_Normal);
	CreateWICTextureFromFile(device, context, L"Textures/Leather_Metallic.png", 0, &Leather_Metallic);
	CreateWICTextureFromFile(device, context, L"Textures/Leather_Rough.png", 0, &Leather_Rough);

	CreateWICTextureFromFile(device, context, L"Textures/SuperHeroFabric_Albedo.png", 0, &SuperHeroFabric_Albedo);
	CreateWICTextureFromFile(device, context, L"Textures/SuperHeroFabric_Normal.png", 0, &SuperHeroFabric_Normal);
	CreateWICTextureFromFile(device, context, L"Textures/SuperHeroFabric_Metallic.png", 0, &SuperHeroFabric_Metallic);
	CreateWICTextureFromFile(device, context, L"Textures/SuperHeroFabric_Rough.png", 0, &SuperHeroFabric_Rough);
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

	materialAluminiumInsulator = new Material(AluminiumInsulator_Albedo, AluminiumInsulator_Normal, AluminiumInsulator_Metallic, AluminiumInsulator_Rough, sampler);
	materialGold = new Material(Gold_Albedo, Gold_Normal, Gold_Metallic, Gold_Rough, sampler);
	materialGunMetal = new Material(GunMetal_Albedo, GunMetal_Normal, GunMetal_Metallic, GunMetal_Rough, sampler);
	materialLeather = new Material(Leather_Albedo, Leather_Normal, Leather_Metallic, Leather_Rough, sampler);
	materialSuperHeroFabric = new Material(SuperHeroFabric_Albedo, SuperHeroFabric_Normal, SuperHeroFabric_Metallic, SuperHeroFabric_Rough, sampler);

	materialSkyBox = new Material(skySRV, NULL, NULL, NULL, sampler);
	
}

void Game::GameEntityInitialize()
{
	skyBoxEntity = new GameEntity(cubeMesh, materialSkyBox);

	pbrSphere = new GameEntity(sphereMesh, materialAluminiumInsulator);
	pbrSphere->SetPosition(2.0f, 3.0f, -8.0f);

	pbrSphere1 = new GameEntity(sphereMesh, materialGold);
	pbrSphere1->SetPosition(2.0f, 2.0f, -8.0f);

	pbrSphere2 = new GameEntity(sphereMesh, materialGunMetal);
	pbrSphere2->SetPosition(2.0f, 1.0f, -8.0f);

	pbrSphere3 = new GameEntity(sphereMesh, materialLeather);
	pbrSphere3->SetPosition(2.0f, 0.0f, -8.0f);

	pbrSphere4 = new GameEntity(sphereMesh, materialSuperHeroFabric);
	pbrSphere4->SetPosition(2.0f, -1.0f, -8.0f);

	for (size_t i = 0; i < 6; i++)
		for (size_t j = 0; j < 6; j++)
		{
			pbrSpheres[i][j] = new GameEntity(sphereMesh);
		}

	float x = -6.0f; float y = 6.0f;
	for (size_t i = 0; i < 6; i++)
	{
		if (y == 0.0f)
			y = 6.0f;
		for (size_t j = 0; j < 6; j++)
		{
			if (x == 0.0f)
				x = -6.0f;

			pbrSpheres[i][j]->SetPosition(x, y, 0.0f);
			x += 1.0f;
		}
		y -= 1.0f;
	}
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

	
	pbrSphere->UpdateWorldMatrix();
	pbrSphere1->UpdateWorldMatrix();
	pbrSphere2->UpdateWorldMatrix();
	pbrSphere3->UpdateWorldMatrix();
	pbrSphere4->UpdateWorldMatrix();

	for (size_t i = 0; i < 6; i++)
		for (size_t j = 0; j < 6; j++)
		{
			pbrSpheres[i][j]->UpdateWorldMatrix();
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

	XMFLOAT3 lightPos[4] = { XMFLOAT3(10.0f, 10.0f, -10.0f), XMFLOAT3(10.0f, -10.0f, -10.0f), XMFLOAT3(-10.0f, 10.0f, -10.0f), XMFLOAT3(-10.0f, -10.0f, -10.0f) };
	
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	float r = 0.0f;
	for (size_t i = 0; i < 6; i++)
	{
		float m = 0.0f;
		for (size_t j = 0; j < 6; j++)
		{

			render.PBRRenderProcess(pbrSpheres[i][j], vertexBuffer, indexBuffer, PBRVertexShader, PBRPixelShader, camera, context, m, r);

			m += 0.2f;
		}
		r += 0.2f;
	}
	
	render.PBRMatRenderProcess(pbrSphere, vertexBuffer, indexBuffer, PBRVertexShader, PBRMatPixelShader, camera, context);
	render.PBRMatRenderProcess(pbrSphere1, vertexBuffer, indexBuffer, PBRVertexShader, PBRMatPixelShader, camera, context);
	render.PBRMatRenderProcess(pbrSphere2, vertexBuffer, indexBuffer, PBRVertexShader, PBRMatPixelShader, camera, context);
	render.PBRMatRenderProcess(pbrSphere3, vertexBuffer, indexBuffer, PBRVertexShader, PBRMatPixelShader, camera, context);
	render.PBRMatRenderProcess(pbrSphere4, vertexBuffer, indexBuffer, PBRVertexShader, PBRMatPixelShader, camera, context);
	//vertexBuffer = pbrSphere->GetMesh()->GetVertexBuffer();
	//indexBuffer = pbrSphere->GetMesh()->GetIndexBuffer();

	//PBRVertexShader->SetMatrix4x4("world", *pbrSphere->GetWorldMatrix());
	//PBRVertexShader->SetMatrix4x4("view", camera->GetView());
	//PBRVertexShader->SetMatrix4x4("projection", camera->GetProjection());

	//PBRVertexShader->CopyAllBufferData();
	//PBRVertexShader->SetShader();

	//PBRMatPixelShader->SetShaderResourceView("albedoSRV", pbrSphere->GetMaterial()->GetAlbedoSRV());
	//PBRMatPixelShader->SetShaderResourceView("normalSRV", pbrSphere->GetMaterial()->GetNormalSRV());
	//PBRMatPixelShader->SetShaderResourceView("metallicSRV", pbrSphere->GetMaterial()->GetMetallicSRV());
	//PBRMatPixelShader->SetShaderResourceView("roughSRV", pbrSphere->GetMaterial()->GetRoughSRV());

	//PBRMatPixelShader->SetSamplerState("basicSampler", pbrSphere->GetMaterial()->GetMaterialSampler());
	//PBRMatPixelShader->SetFloat("ao", 1.0f);

	//PBRMatPixelShader->SetFloat3("lightPos1", XMFLOAT3(10.0f, 10.0f, -10.0f));
	//PBRMatPixelShader->SetFloat3("lightPos2", XMFLOAT3(10.0f, -10.0f, -10.0f));
	//PBRMatPixelShader->SetFloat3("lightPos3", XMFLOAT3(-10.0f, -10.0f, -10.0f));
	//PBRMatPixelShader->SetFloat3("lightPos4", XMFLOAT3(-10.0f, 10.0f, -10.0f));
	////PBRPixelShader->SetData("lightPos", &lightPos, sizeof(lightPos));
	//PBRMatPixelShader->SetFloat3("lightCol", XMFLOAT3(300.0f, 300.0f, 300.0f));

	//PBRMatPixelShader->SetFloat3("camPos", camera->GetPosition());

	//PBRMatPixelShader->CopyAllBufferData();
	//PBRMatPixelShader->SetShader();

	//context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	//context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//context->DrawIndexed(pbrSphere->GetMesh()->GetIndexCount(), 0, 0);

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
