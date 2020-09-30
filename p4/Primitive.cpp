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
// OVERVIEW: Primitive.cpp
// ========
// Source file for primitive.
//
// Author(s): Paulo Pagliosa (and your name)
// Last revision: 30/10/2018

#include "Primitive.h"
#include "Transform.h"
#include "BVH.h"

namespace cg
{ // begin namespace cg

	bool
		Primitive::intersectTriangle(const Ray& localRay, TriangleMesh::Triangle triangle, Intersection& hit) const
	{
		vec3f p0 = _mesh->data().vertices[triangle.v[0]];
		vec3f p1 = _mesh->data().vertices[triangle.v[1]];
		vec3f p2 = _mesh->data().vertices[triangle.v[2]];

		auto e1 = p1 - p0;
		auto e2 = p2 - p0;
		auto s1 = localRay.direction.cross(e2);

		float x = s1.dot(e1);

		if (math::isZero(abs(x)))
			return false;

		auto S = localRay.origin - p0;
		auto s2 = S.cross(e1);

		auto invX = 1 / x;
		float tl;

		if ((tl = s2.dot(e2) * invX) < 0)
			return false;

		float b1, b2;
		if ((b1 = s1.dot(S) * invX) < 0)
			return false;
		if ((b2 = s2.dot(localRay.direction) * invX) < 0)
			return false;
		if (b1 + b2 > 1)
			return false;

		hit.distance = tl;
		hit.b = { 1 - b1 - b2, b1, b2 };

		return true;
	}
bool
Primitive::intersect(const Ray& ray, Intersection& hit) const
{
  if (_mesh == nullptr)
    return false;

  auto t = const_cast<Primitive*>(this)->transform();
  Ray localRay(t->worldToLocalMatrix().transform(ray.origin), t->worldToLocalMatrix().transformVector(ray.direction));
  auto d = math::inverse(localRay.direction.length());

  float tMin;
  float tMax;

  localRay.direction *= d;

  if (_mesh->bounds().intersect(localRay, tMin, tMax))
  {
		auto nTriangles = _mesh->data().numberOfTriangles;
		auto triangles = _mesh->data().triangles;

		for (auto i = 0; i < nTriangles; i++)		
			if (intersectTriangle(localRay, triangles[i], hit))
				hit.triangleIndex = i;
  }
	return hit.distance != math::Limits<float>::inf() ? true : false;
}

} // end namespace cg
