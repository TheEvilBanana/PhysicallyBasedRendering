#pragma once

#include <DirectXMath.h>

#include "DXCore.h"
#include "Camera.h"
#include "Mesh.h"
#include "Material.h"
#include "GameEntity.h"
#include "Render.h"

using namespace DirectX;

class Game : public DXCore
{
public:
	Game(HINSTANCE hInstance);
	~Game();

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

	// Overridden mouse input helper methods
	void OnMouseDown(WPARAM buttonState, int x, int y);
	void OnMouseUp(WPARAM buttonState, int x, int y);
	void OnMouseMove(WPARAM buttonState, int x, int y);
	void OnMouseWheel(float wheelDelta, int x, int y);

private:

	// Initialization helper methods 
	void CameraInitialize();
	void ShadersInitialize();
	void ModelsInitialize();
	void LoadTextures();
	void MaterialsInitialize();
	void SkyBoxInitialize();
	void GameEntityInitialize();

	// Buffers to hold actual geometry data
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	ID3D11RasterizerState* rasterizer;

	//Declare shader variables
	SimpleVertexShader* baseVertexShader;
	SimplePixelShader* basePixelShader;
	SimpleVertexShader* skyVertexShader;
	SimplePixelShader* skyPixelShader;
	
	SimpleVertexShader* PBRVertexShader;
	SimplePixelShader* PBRPixelShader;
	SimplePixelShader* PBRMatPixelShader;

	// Sampler for wrapping textures
	ID3D11SamplerState* sampler;

	//Albedo Shader Resource Views(SRVs)
	ID3D11ShaderResourceView* AluminiumInsulator_Albedo;
	ID3D11ShaderResourceView* Gold_Albedo;
	ID3D11ShaderResourceView* GunMetal_Albedo;
	ID3D11ShaderResourceView* Leather_Albedo;
	ID3D11ShaderResourceView* SuperHeroFabric_Albedo;

	//Normal Shader Resource Views(SRVs)
	ID3D11ShaderResourceView* AluminiumInsulator_Normal;
	ID3D11ShaderResourceView* Gold_Normal;
	ID3D11ShaderResourceView* GunMetal_Normal;
	ID3D11ShaderResourceView* Leather_Normal;
	ID3D11ShaderResourceView* SuperHeroFabric_Normal;

	//Metallic Shader Resource Views(SRVs)
	ID3D11ShaderResourceView* AluminiumInsulator_Metallic;
	ID3D11ShaderResourceView* Gold_Metallic;
	ID3D11ShaderResourceView* GunMetal_Metallic;
	ID3D11ShaderResourceView* Leather_Metallic;
	ID3D11ShaderResourceView* SuperHeroFabric_Metallic;

	//Roughness Shader Resource Views(SRVs)
	ID3D11ShaderResourceView* AluminiumInsulator_Rough;
	ID3D11ShaderResourceView* Gold_Rough;
	ID3D11ShaderResourceView* GunMetal_Rough;
	ID3D11ShaderResourceView* Leather_Rough;
	ID3D11ShaderResourceView* SuperHeroFabric_Rough;

	//AO Shader Resource Views(SRVs)


	//Sky Box Stuff
	ID3D11ShaderResourceView* skySRV;
	ID3D11RasterizerState* skyRasterizerState;
	ID3D11DepthStencilState* skyDepthState;

	//Mesh Class
	Mesh* sphereMesh;
	Mesh* cubeMesh;

	//Material Class
	Material* materialSkyBox;
	Material* materialAluminiumInsulator;
	Material* materialGold;
	Material* materialGunMetal;
	Material* materialLeather;
	Material* materialSuperHeroFabric;

	//Game Entity Class
	GameEntity* skyBoxEntity;
	std::vector<GameEntity*> sphereEntities;
	std::vector<GameEntity*> flatEntities;
	GameEntity* pbrSphere;
	GameEntity* pbrSphere1;
	GameEntity* pbrSphere2;
	GameEntity* pbrSphere3;
	GameEntity* pbrSphere4;

	GameEntity* pbrSpheres[6][6];

	//Render Class
	Render render;

	//Camera class
	Camera* camera;

	// The matrices to go from model space to screen space
	XMFLOAT4X4 worldMatrix;
	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projectionMatrix;

	// Keeps track of the old mouse position.  Useful for 
	// determining how far the mouse moved in a single frame.
	POINT prevMousePos;
	POINT currentMousePos;
	POINT difference;

};

