//***************************************************************************************
// LightHelper.h by Frank Luna (C) 2011 All Rights Reserved.
//
// Helper classes for lighting.
// File modified from original.
//***************************************************************************************

#ifndef JOJ_MATERIAL_H
#define JOJ_MATERIAL_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#if JPLATFORM_WINDOWS

#include <Windows.h>
#include "math/jmath.h"

namespace joj
{
	struct SimpleMaterial
	{
		SimpleMaterial() { ZeroMemory(this, sizeof(this)); }

		JFloat4 ambient;
		JFloat4 diffuse;
		JFloat4 specular; // w = SpecPower
		JFloat4 Reflect;
	};
}

#endif // JPLATFORM_WINDOWS

#endif // JOJ_MATERIAL_H