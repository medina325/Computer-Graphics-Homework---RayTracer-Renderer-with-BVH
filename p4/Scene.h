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
// OVERVIEW: Scene.h
// ========
// Class definition for scene.
//
// Author(s): Paulo Pagliosa (and your name)
// Last revision: 21/09/2019

#ifndef __Scene_h
#define __Scene_h

#include "SceneObject.h"
#include "graphics/Color.h"

namespace cg
{ // begin namespace cg

	using SceneObjList = std::list<Reference<SceneObject>>;
	using PrimitiveList = std::list<Reference<Primitive>>;
	using LightList = std::list<Reference<Light>>;

	/////////////////////////////////////////////////////////////////////
	//
	// Scene: scene class
	// =====
	class Scene : public SceneNode
	{
	public:
		Color backgroundColor{ Color::gray };
		Color ambientLight{ Color::black };

		/// Constructs an empty scene.
		Scene(const char* name) :
			SceneNode{ name },
			_root{ "x1bRoot", *this }
		{
			SceneObject::makeUse(&_root);
		}

		auto beginRoot() const
		{
			return _rootChildren.begin();
		}

		auto endRoot() const
		{
			return _rootChildren.end();
		}

		auto rBeginRoot() const
		{
			return _rootChildren.rbegin();
		}

		auto sizeRoot()
		{
			return _rootChildren.size();
		}

		void addRoot(const Reference<SceneObject>& root)
		{
			_rootChildren.push_back(root);
		}

		void removeRoot(const Reference<SceneObject>& root)
		{
			// Remove recursively root's children's primitives
			if (root->sizeChildren() > 0)
				for (auto it = root->sizeChildren(); it > 0; it = root->sizeChildren())
					root->removeChild(*(root->beginChildren()));

			if (root->findComponent<Primitive>() != nullptr)
				_primitives.remove(root->primitive());

			if (root->findComponent<Light>() != nullptr)
				_lights.remove(root->light());

			_rootChildren.remove(root);
		}

		void removeFromRoot(const Reference<SceneObject>& root)
		{
			_rootChildren.remove(root);
		}

		//Ps.: Prof. Pagliosa's idea
		// Add a primitive (that knows it's owner) to the primitives to be rendered for this scene
		void addPrimitive(Reference<Primitive> p)
		{
			_primitives.push_back(p);
		}

		void removePrimitive(const Reference<Primitive>& p)
		{
			_primitives.remove(p);
		}
		auto beginPrimitives()
		{
			return _primitives.begin();
		}

		auto endPrimitives()
		{
			return _primitives.end();
		}

		void clearPrimitives()
		{
			_primitives.clear();
		}

		auto sizePrimitives()
		{
			return _primitives.size();
		}

		// Add a light (that knows it's owner) to the list of lights to be used
		void addLight(Reference<Light> l)
		{
			_lights.push_back(l);
		}

		void removeLight(const Reference<Light>& l)
		{
			_lights.remove(l);
		}
		auto beginLights()
		{
			return _lights.begin();
		}

		auto endLights()
		{
			return _lights.end();
		}

		void clearLights()
		{
			_lights.clear();
		}

		auto sizeLights()
		{
			return _lights.size();
		}
		/// Returns the root scene object of this scene.
		auto root() const
		{
			return &_root;
		}

		auto root()
		{
			return &_root;
		}

	private:
		SceneObject _root;
		SceneObjList _rootChildren;
		PrimitiveList _primitives;
		LightList _lights;
	}; // Scene

} // end namespace cg

#endif // __Scene_h
