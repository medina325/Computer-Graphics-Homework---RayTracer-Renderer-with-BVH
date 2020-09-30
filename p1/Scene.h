//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018 Orthrus Group.                               |
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
// Last revision: 25/08/2018

#ifndef __Scene_h
#define __Scene_h

#include "SceneObject.h"
#include "graphics/Color.h"
#include <list>

namespace cg
{ // begin namespace cg

using SceneObjList = std::list<Reference<SceneObject>>;
using PrimitiveList = std::list<Reference<Primitive>>;
/////////////////////////////////////////////////////////////////////
//
// Scene: scene class
// =====
class Scene: public SceneNode
{
public:
  Color backgroundColor{Color::gray};

  /// Constructs an empty scene.
  Scene(const char* name):
    SceneNode{name}
  {
    // do nothing
  }

	auto beginRoot() const
	{
		return _root.begin();
	}

	auto endRoot() const
	{
		return _root.end();
	}

	auto rBeginRoot() const
	{
		return _root.rbegin();
	}

	auto sizeRoot()
	{
		return _root.size();
	}

	void addRoot(const Reference<SceneObject>& root)
	{
		_root.push_back(root);
	}

	void removeRoot(Reference<SceneObject> root)
	{
		// Remove recursively root's children's primitives (yet to be finished)
		//if (root->sizeChildren() > 0)
		//	for (auto it = root->beginChildren(); it != root->endChildren(); it++)
		//		root->removeChild(*it);

		if (root->primitive() != nullptr)
			_primitives.remove(root->primitive());
		_root.remove(root);

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

	auto sizePrimitives()
	{
		return _primitives.size();
	}
private:
	SceneObjList _root;
	PrimitiveList _primitives;
}; // Scene

} // end namespace cg

#endif // __Scene_h
