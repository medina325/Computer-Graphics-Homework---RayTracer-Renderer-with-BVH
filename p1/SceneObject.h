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
// OVERVIEW: SceneObject.h
// ========
// Class definition for scene object.
//
// Author(s): Paulo Pagliosa (and your name)
// Last revision: 25/08/2018

#ifndef __SceneObject_h
#define __SceneObject_h

#include "SceneNode.h"
#include "Transform.h"
#include "Primitive.h"
#include <list>
//#include <iostream>

namespace cg
{ // begin namespace cg

// Forward definition
class Scene;


using SceneObjList = std::list<Reference<SceneObject>>;
 
// NAO DÁ PRA FAZER DE REFERENCE, POIS NAO RECONHECE REFERENCE<PRIMITIVE> DAÍ
using ComponentList = std::list<Component*>;

////////////////////////////////////////////////////////////////////
//
// SceneObject: scene object class
// ===========
class SceneObject: public SceneNode
{
public:
  bool visible{true};

  /// Constructs an scene object with a primitive definied
  SceneObject(const char* name, Scene* scene, Reference<Primitive> p, SceneObject* parent):
    SceneNode{name},
    _scene{scene},
		_parent{parent}
  {
		setPrimitive(p);
    makeUse(&_transform);
		_components.push_back(&_transform);
  }

	// Constructs an empty scene object
	SceneObject(const char* name, Scene* scene, SceneObject* parent):
		SceneNode{name},
		_scene{scene},
		_parent{parent}
	{
		_primitive = nullptr;
		makeUse(&_transform);
		_components.push_back(&_transform);
	}

  /// Returns the scene which this scene object belong to.
  auto scene() const
  {
    return _scene;
  }

  /// Returns the parent of this scene object.
  auto parent() const
  {
    return _parent;
  }

  /// Sets the parent of this scene object.
  void setParent(const Reference<SceneObject>& parent);

  /// Returns the transform of this scene object.
  auto transform()
  {
    return &_transform;
  }

	auto primitive() const
	{
		return _primitive;
	}

	// Sets the primitive of this scene objects
	void setPrimitive(const Reference<Primitive>& p);

	// Below there are only methods for the list of children scene objects
	auto beginChildren() const
	{
		return _children.begin();
	}

	auto endChildren() const
	{
		return _children.end();
	}

	auto rBeginChildren() const
	{
		return _children.rbegin();
	}
	void addChild(const Reference<SceneObject>& child)
	{
		_children.push_back(child);
	}

	void removeChild(const Reference<SceneObject>& child);


	size_t sizeChildren()
	{
		return _children.size();
	}
	
private:
  Scene* _scene;
	Reference<SceneObject> _parent;
	Transform _transform;
	Reference<Primitive> _primitive;

	SceneObjList _children;
	ComponentList _components;

  friend class Scene;

}; // SceneObject


/// Returns the transform of a component.
inline Transform*
Component::transform() // declared in Component.h
{
  return sceneObject()->transform();
}

/// Returns the parent of a transform.
inline Transform*
Transform::parent() const // declared in Transform.h
{
  if (auto p = sceneObject()->parent())
    return p->transform();
   return nullptr;
}

} // end namespace cg

#endif // __SceneObject_h
