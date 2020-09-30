//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018, 2019 Orthrus Group.                         |
//|                                                                 |
//| This software is provided 'as-is', without any express or       |
//| implied warranty. In no event will the authors be held liable   |
//| for any damages arising from the use of this software.          |
//|                                                                 |
//| Permission is granted to anyone to use this software for any    |
//| purpose, including commercial applications, and to alter it and |
//| redistribute it freely, subject to the following restrictions:  |
//|                                                                 |
//| 1. The origin of this software must not be misrepresented; you  |
//| must not claim that you wrote the original software. If you use |
//| this software in a product, an acknowledgment in the product    |
//| documentation would be appreciated but is not required.         |
//|                                                                 |
//| 2. Altered source versions must be plainly marked as such, and  |
//| must not be misrepresented as being the original software.      |
//|                                                                 |
//| 3. This notice may not be removed or altered from any source    |
//| distribution.                                                   |
//|                                                                 |
//[]---------------------------------------------------------------[]
//
// OVERVIEW: Light.h
// ========
// Class definition for light.
//
// Author(s): Paulo Pagliosa (and your name)
// Last revision: 14/10/2019

#ifndef __Light_h
#define __Light_h

#include "Component.h"
#include "graphics/Color.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// Light: light class
// =====
	class Light : public Component
	{
	public:
		enum Type
		{
			Directional = 0,
			Point = 1,
			Spot = 2
		};

		enum Falloff
		{
			NoFalloff = 0,
			LinearFalloff = 1,
			QuadFalloff = 2
		};

		Color color{ Color::white }; // cL
		static constexpr float minAngle = 0;
		static constexpr float maxAngle = 90;

		Light(Type type, Color lightColor = Color::white) :
			Component{ "Light" },
			_type{ type },
			color{lightColor},
			_fL{ Falloff::NoFalloff },
			_eL{ 1 },
			_angGama{ 60.f }
		{
			// do nothing
		}

		auto type() const
		{
			return _type;
		}

		void setType(Type type)
		{
			_type = type;
		}

		auto fL()
		{
			return _fL;
		}

		void setFl(Falloff fL)
		{
			_fL = fL;
		}

		int eL()
		{
			return _eL;
		}

		void setEl(int eL)
		{
			_eL = eL;
		}

		auto angGama()
		{
			return _angGama;
		}

		void setAngGama(float angle)
		{
			if (!math::isEqual(_angGama, angle))
			{
				_angGama = std::min(std::max(angle, minAngle), maxAngle);
			}
		}

	private:
		Type _type;

		// Spot & Point 
		Falloff _fL;

		// Spot
		int _eL;
		float _angGama; // In degrees
	}; // Light

} // end namespace cg

#endif // __Light_h
