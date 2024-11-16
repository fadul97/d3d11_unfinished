//***************************************************************************************
// LightHelper.h by Frank Luna (C) 2011 All Rights Reserved.
//
// Helper classes for lighting.
// File modified from original.
//***************************************************************************************

#ifndef JOJ_LIGHT_H
#define JOJ_LIGHT_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#if JPLATFORM_WINDOWS

#include <Windows.h>
#include "math/jmath.h"

namespace joj
{
	struct DirectionalLight
	{
		DirectionalLight() { ZeroMemory(this, sizeof(this)); }

		JFloat4 ambient;
		JFloat4 diffuse;
		JFloat4 specular;
		JFloat3 direction;
		f32 pad; // pad the last float so we can set an array of lights if we wanted.
	};

	struct PointLight
	{
		PointLight() { ZeroMemory(this, sizeof(this)); }

		JFloat4 ambient;
		JFloat4 diffuse;
		JFloat4 specular;

		// Packed into 4D vector: (Position, Range)
		JFloat3 Position;
		f32 Range;

		// Packed into 4D vector: (A0, A1, A2, pad)
		JFloat3 att;
		f32 pad; // pad the last float so we can set an array of lights if we wanted.
	};

	struct SpotLight
	{
		SpotLight() { ZeroMemory(this, sizeof(this)); }

		JFloat4 ambient;
		JFloat4 diffuse;
		JFloat4 specular;

		// Packed into 4D vector: (Position, Range)
		JFloat3 Position;
		f32 Range;

		// Packed into 4D vector: (direction, Spot)
		JFloat3 direction;
		f32 Spot;

		// Packed into 4D vector: (att, pad)
		JFloat3 att;
		f32 pad; // pad the last float so we can set an array of lights if we wanted.
	};
}

#endif // JPLATFORM_WINDOWS

#endif // JOJ_LIGHT_H