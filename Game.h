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
	void IBLStuff();

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
	SimplePixelShader* ConvolutionPixelShader;
	SimplePixelShader* PrefilterMapPixelShader;
	SimplePixelShader* IntegrateBRDFPixelShader;

	//IBL
	ID3D11Texture2D* skyIBLtex;
	ID3D11RenderTargetView* skyIBLRTV[6];
	ID3D11ShaderResourceView* skyIBLSRV;
	
	ID3D11Texture2D* envMaptex;
	ID3D11RenderTargetView* envMapRTV[6];
	ID3D11ShaderResourceView* envMapSRV;

	ID3D11Texture2D* brdfLUTtex;
	ID3D11RenderTargetView* brdfLUTRTV;
	ID3D11ShaderResourceView* brdfLUTSRV;

	// Sampler for wrapping textures
	ID3D11SamplerState* sampler;

	//Albedo Shader Resource Views(SRVs)
	ID3D11ShaderResourceView* AluminiumInsulator_Albedo;
	ID3D11ShaderResourceView* Gold_Albedo;
	ID3D11ShaderResourceView* GunMetal_Albedo;
	ID3D11ShaderResourceView* Leather_Albedo;
	ID3D11ShaderResourceView* SuperHeroFabric_Albedo;
	ID3D11ShaderResourceView* CamoFabric_Albedo;
	ID3D11ShaderResourceView* GlassVisor_Albedo;
	ID3D11ShaderResourceView* IronOld_Albedo;
	ID3D11ShaderResourceView* Rubber_Albedo;
	ID3D11ShaderResourceView* Wood_Albedo;

	//Normal Shader Resource Views(SRVs)
	ID3D11ShaderResourceView* AluminiumInsulator_Normal;
	ID3D11ShaderResourceView* Gold_Normal;
	ID3D11ShaderResourceView* GunMetal_Normal;
	ID3D11ShaderResourceView* Leather_Normal;
	ID3D11ShaderResourceView* SuperHeroFabric_Normal;
	ID3D11ShaderResourceView* CamoFabric_Normal;
	ID3D11ShaderResourceView* GlassVisor_Normal;
	ID3D11ShaderResourceView* IronOld_Normal;
	ID3D11ShaderResourceView* Rubber_Normal;
	ID3D11ShaderResourceView* Wood_Normal;

	//Metallic Shader Resource Views(SRVs)
	ID3D11ShaderResourceView* AluminiumInsulator_Metallic;
	ID3D11ShaderResourceView* Gold_Metallic;
	ID3D11ShaderResourceView* GunMetal_Metallic;
	ID3D11ShaderResourceView* Leather_Metallic;
	ID3D11ShaderResourceView* SuperHeroFabric_Metallic;
	ID3D11ShaderResourceView* CamoFabric_Metallic;
	ID3D11ShaderResourceView* GlassVisor_Metallic;
	ID3D11ShaderResourceView* IronOld_Metallic;
	ID3D11ShaderResourceView* Rubber_Metallic;
	ID3D11ShaderResourceView* Wood_Metallic;

	//Roughness Shader Resource Views(SRVs)
	ID3D11ShaderResourceView* AluminiumInsulator_Rough;
	ID3D11ShaderResourceView* Gold_Rough;
	ID3D11ShaderResourceView* GunMetal_Rough;
	ID3D11ShaderResourceView* Leather_Rough;
	ID3D11ShaderResourceView* SuperHeroFabric_Rough;
	ID3D11ShaderResourceView* CamoFabric_Rough;
	ID3D11ShaderResourceView* GlassVisor_Rough;
	ID3D11ShaderResourceView* IronOld_Rough;
	ID3D11ShaderResourceView* Rubber_Rough;
	ID3D11ShaderResourceView* Wood_Rough;

	//AO Shader Resource Views(SRVs)


	//Sky Box Stuff
	ID3D11ShaderResourceView* skySRV;
	ID3D11RasterizerState* skyRasterizerState;
	ID3D11DepthStencilState* skyDepthState;

	ID3D11ShaderResourceView* skyIR;

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
	Material* materialCamoFabric;
	Material* materialGlassVisor;
	Material* materialIronOld;
	Material* materialRubber;
	Material* materialWood;

	//Game Entity Class
	GameEntity* skyBoxEntity;
	std::vector<GameEntity*> sphereEntities;
	std::vector<GameEntity*> flatEntities;
	GameEntity* pbrSphere;
	GameEntity* pbrSphere1;
	GameEntity* pbrSphere2;
	GameEntity* pbrSphere3;
	GameEntity* pbrSphere4;
	GameEntity* pbrSphere5;
	GameEntity* pbrSphere6;
	GameEntity* pbrSphere7;
	GameEntity* pbrSphere8;
	GameEntity* pbrSphere9;

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

