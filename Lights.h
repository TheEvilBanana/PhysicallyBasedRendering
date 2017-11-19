#pragma once

#include <DirectXMath.h>

using namespace DirectX;

struct DirectionalLight {
	XMFLOAT4 diffuseColor;
	XMFLOAT3 direction;
	float pad;

	void SetLightValues(XMFLOAT4 _diffuseColor, XMFLOAT3 _direction, float _pad) {
		diffuseColor = _diffuseColor;
		direction = _direction;
		pad = _pad;
	}
};

struct AmbientLight {
	XMFLOAT4 ambientColor;

	void SetLightValues(XMFLOAT4 _ambientColor) {
		ambientColor = _ambientColor;
	}
};

struct PointLight {
	XMFLOAT4 diffuseColor;
	XMFLOAT3 position;
	float range;
	XMFLOAT3 attenuate;
	float pad;

	void SetLightValues(XMFLOAT4 _diffuseColor, XMFLOAT3 _position, float _range, XMFLOAT3 _attenuate, float _pad) 
	{
		diffuseColor = _diffuseColor;
		position = _position;
		range = _range;
		attenuate = _attenuate;
		pad = _pad;
	}
};

struct SpotLight {
	XMFLOAT4 diffuseColor;
	XMFLOAT3 position;
	float range;
	XMFLOAT3 direction;
	float spot;
	XMFLOAT3 attenuate;
	float pad;

	void SetLightValues(XMFLOAT4 _diffuseColor, XMFLOAT3 _position, float _range, XMFLOAT3 _direction, float _spot, XMFLOAT3 _attenuate, float _pad)
	{
		diffuseColor = _diffuseColor;
		position = _position;
		range = _range;
		direction = _direction;
		spot = _spot;
		attenuate = _attenuate;
		pad = _pad;
	}
};