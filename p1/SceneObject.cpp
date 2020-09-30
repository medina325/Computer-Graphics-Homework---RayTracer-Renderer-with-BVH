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
// OVERVIEW: SceneObject.cpp
// ========
// Source file for scene object.
//
// Author(s): Paulo Pagliosa (and your name)
// Last revision: 25/08/2018

#include "SceneObject.h"
#include "Scene.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// SceneObject implementation
// ===========
void
SceneObject::setParent(const Reference<SceneObject>& p)
{
	if (_parent == nullptr) {
		_parent = p;
	}
	else
	{	
		p->addChild(this);
		_parent->removeChild(this);
		_parent = p;
	}
}

void
SceneObject::setPrimitive(const Reference<Primitive>& p)
{
	// Sets primitive atribute of this scene object
	_primitive = p;

	// Add the primitive to the list of components
	_components.push_back(p);

	// Associates this scene object with the primitive
	_primitive->setSceneObject(this);

	// Add the primitive to the list of primitives to be rendered
	_scene->addPrimitive(_primitive);
}

void 
SceneObject::removeChild(const Reference<SceneObject>& child)
{
	// Remove recursively child's children's primitives (yet to be finished)
	//if (child->sizeChildren() > 0)
	//	for (auto it = child->beginChildren(); it != child->endChildren(); it++)
	//		child->r(*it);
	
	if (child->primitive() != nullptr)
		_scene->removePrimitive(child->primitive());
	_children.remove(child);
}

} // end namespace cg
