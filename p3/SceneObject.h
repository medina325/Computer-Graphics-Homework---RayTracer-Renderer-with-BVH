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
// OVERVIEW: SceneObject.h
// ========
// Class definition for scene object.
//
// Author(s): Paulo Pagliosa (and your name)
// Last revision: 23/09/2019

#ifndef __SceneObject_h
#define __SceneObject_h

#include "SceneNode.h"
#include "Transform.h"
#include "Primitive.h"
#include "Camera.h"
#include "Light.h"
#include <list>

namespace cg
{ // begin namespace cg

// Forward definition
	class Scene;

	using SceneObjList = std::list<Reference<SceneObject>>;
	using ComponentList = std::list<Reference<Component>>;
	//
	// SceneObject: scene object class
	// ===========
	class SceneObject : public SceneNode
	{
	public:
		bool visible{ true };

		// Constructs root scene object
		SceneObject(const char* name, Scene& scene) :
			SceneNode{ name },
			_scene{ &scene }
		{
			// do nothing
		}

		/// Constructs an scene object
		SceneObject(const char* name, Scene& scene, Reference<Primitive> primitive,
																								Reference<Camera> camera,
																								Reference<SceneObject> parent,
																								Reference<Light> light) 
			: SceneNode{ name },
				_scene{ &scene }
		{
			setParent(parent);

			if (primitive != nullptr)
				setPrimitive(primitive);
			else
				_primitive = nullptr;

			setCamera(camera);

			setLight(light);

			makeUse(&_transform);
			// Add the transform to the list of components
			if (findComponent<Transform>() == nullptr)
				addComponent(&_transform);
			_transform.update();
		}

		template <typename T>
		T* findComponent() const
		{
			T* result = nullptr;

			auto it = beginComponents();
			while (it != endComponents())
			{
				// Casting implicit (to make it work) 
				Component* component = *it;
				// Verify if the component is of type T
				if (dynamic_cast<T*>(component) != nullptr)
				{
					// We have to cast it explicitly so we can return it
					result = (T*)component;

					// To finish loop
					it = endComponents();
				}
				else
					it++;
			}
			return result;
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

		auto primitive() const
		{
			return _primitive;
		}

		// Sets the primitive of this scene objects
		void setPrimitive(const Reference<Primitive>& p);

		auto camera()
		{
			return _camera;
		}

		// Sets the camera of this scene objects
		void setCamera(const Reference<Camera>& c);

		auto light()
		{
			return _light;
		}

		void setLight(const Reference<Light>& l);

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
		void removeRecursion(const Reference<SceneObject>& child);
		void removeFromParent(const Reference<SceneObject>& child);
		
		auto beginComponents() const
		{
			return _components.begin();
		}

		auto endComponents() const
		{
			return _components.end();
		}

		void addComponent(Component* component)
		{
			// Associates this scene object with the primitive
			component->_sceneObject = this;

			_components.push_back(Component::makeUse(component));
		}

		void removeComponent(const Reference<Component>& component)
		{
			_components.remove(component);
		}

		size_t sizeChildren()
		{
			return _children.size();
		}
		/// Returns the transform of this scene object.
		auto transform() const
		{
			return &_transform;
		}

		auto transform()
		{
			return &_transform;
		}

	private:
		friend class Scene;
		Scene* _scene;
		Reference<SceneObject> _parent = nullptr;
		Transform _transform;
		Reference<Primitive> _primitive = nullptr;
		Reference<Camera> _camera = nullptr;
		Reference<Light> _light = nullptr;

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
