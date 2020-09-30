//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2019 Orthrus Group.                               |
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
// OVERVIEW: BVH.cpp
// ========
// Source file for BVH.
//
// Author(s): Paulo Pagliosa (and your name)
// Last revision: 18/11/2019

#include "BVH.h"

namespace cg
{ // begin namespace cg

	struct BVH::TriangleInfo
	{
		int index;
		Bounds3f bounds;
		vec3f centroid;

		TriangleInfo() = default;

		TriangleInfo(int index, const Bounds3f& bounds) :
			index{ index },
			bounds{ bounds },
			centroid{ bounds.center() }
		{
			// do nothing
		}

	}; // BVH::TriangleInfo

	struct BVH::Node
	{
		Bounds3f bounds;
		Node* children[2];
		int first;
		int count;

		Node(const Bounds3f& b, int first, int count) :
			bounds{ b },
			first{ first },
			count{ count }
		{
			children[0] = children[1] = nullptr;
		}

		Node(Node* c0, Node* c1) :
			count{ 0 }
		{
			bounds.inflate(c0->bounds);
			bounds.inflate(c1->bounds);
			children[0] = c0;
			children[1] = c1;
		}

		~Node()
		{
			delete children[0];
			delete children[1];
		}

		bool isLeaf() const
		{
			return children[0] == nullptr;
		}

		static void iterate(const Node* node, BVHNodeFunction f);

	}; // BVH::Node

	void
		BVH::Node::iterate(const Node* node, BVHNodeFunction f)
	{
		if (node == nullptr)
			return;

		auto isLeaf = node->isLeaf();

		f({ node->bounds, isLeaf, node->first, node->count });
		if (!isLeaf)
		{
			iterate(node->children[0], f);
			iterate(node->children[1], f);
		}
	}

	inline BVH::Node*
		BVH::makeLeaf(TriangleInfoArray& triangleInfo,
			int start,
			int end,
			TriangleIndexArray& orderedTris)
	{
		Bounds3f bounds;
		auto first = int(orderedTris.size());

		for (int i = start; i < end; ++i)
		{
			bounds.inflate(triangleInfo[i].bounds);
			orderedTris.push_back(_triangles[triangleInfo[i].index]);
		}
		return new Node{ bounds, first, end - start };
	}

	inline auto
		maxDim(const Bounds3f& b)
	{
		auto s = b.size();
		return s.x > s.y && s.x > s.z ? 0 : (s.y > s.z ? 1 : 2);
	}

	BVH::Node*
		BVH::makeNode(TriangleInfoArray& triangleInfo,
			int start,
			int end,
			TriangleIndexArray& orderedTris)
	{
		++_nodeCount;
		if (end - start <= _maxTrisPerNode)
			return makeLeaf(triangleInfo, start, end, orderedTris);

		Bounds3f centroidBounds;

		for (int i = start; i < end; ++i)
			centroidBounds.inflate(triangleInfo[i].centroid);

		auto dim = maxDim(centroidBounds);

		if (centroidBounds.max()[dim] == centroidBounds.min()[dim])
			return makeLeaf(triangleInfo, start, end, orderedTris);

		// Partition tris into two sets and build children
		auto mid = (start + end) / 2;

		std::nth_element(&triangleInfo[start],
			&triangleInfo[mid],
			&triangleInfo[end - 1] + 1,
			[dim](const TriangleInfo& a, const TriangleInfo& b)
			{
				return a.centroid[dim] < b.centroid[dim];
			});
		return new Node{ makeNode(triangleInfo, start, mid, orderedTris),
			makeNode(triangleInfo, mid, end, orderedTris) };
	}

	BVH::BVH(TriangleMesh& mesh, int maxTrisPerNode) :
		_mesh{ &mesh },
		_maxTrisPerNode{ maxTrisPerNode }
	{
		const auto& data = mesh.data();
		int nt{ data.numberOfTriangles };

		if (nt == 0)
			return;
		_triangles.resize(nt);

		TriangleInfoArray triangleInfo(nt);

		for (int i = 0; i < nt; ++i)
		{
			_triangles[i] = i;

			auto t = data.triangles + i;
			Bounds3f b;

			b.inflate(data.vertices[t->v[0]]);
			b.inflate(data.vertices[t->v[1]]);
			b.inflate(data.vertices[t->v[2]]);
			triangleInfo[i] = { i, b };
		}

		TriangleIndexArray orderedTris;

		orderedTris.reserve(nt);
		_root = makeNode(triangleInfo, 0, nt, orderedTris);

		_triangles.swap(orderedTris);

#ifdef _DEBUG
		if (true)
		{
			mesh.bounds().print("Mesh bounds:");
			printf("Mesh triangles: %d\n", nt);
			bounds().print("BVH bounds:");
			printf("BVH nodes: %d\n", _nodeCount);
			iterate([this](const BVHNodeInfo& node)
				{
					if (!node.isLeaf)
						return;
					node.bounds.print("Leaf bounds:");
					printf("Leaf triangles: %d\n", node.numberOfTriangles);
					//for (int i = 0; i < node.numberOfTriangles; ++i)
						//printf("%d ", _triangles[node.firstTriangleIndex + i]);
					//putchar('\n');
				});
			putchar('\n');
		}
#endif // _DEBUG
	}

	BVH::~BVH()
	{
		delete _root;
	}

	Bounds3f
		BVH::bounds() const
	{
		return _root == nullptr ? Bounds3f{} : _root->bounds;
	}

	void
		BVH::iterate(BVHNodeFunction f) const
	{
		Node::iterate(_root, f);
	}

	bool
		BVH::intersectTriangle(const Ray& localRay, TriangleMesh::Triangle triangle, Intersection& hitDistBaric)
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

		if ( (tl = s2.dot(e2) * invX) < 0)
			return false;

		float b1, b2;
		if ( (b1 = s1.dot(S) * invX) < 0)
			return false;
		if ( (b2 = s2.dot(localRay.direction) * invX) < 0)
			return false;
		if (b1 + b2 > 1)
			return false;

		hitDistBaric.distance = tl;
		hitDistBaric.b = { 1 - b1 - b2, b1, b2 };

		return true;
	}

	bool
		BVH::intersect(const Ray& localRay, Intersection& hitAux)
	{
		float tMin, tMax;
		_stack.push(_root);

		while (!_stack.empty())
		{
			Node* aabb = _stack.top();
			_stack.pop();

			if (aabb->bounds.intersect(localRay, tMin, tMax))
			{
				if (aabb->isLeaf())
				{
					auto size = aabb->first + aabb->count;
					for (int i = aabb->first; size > i; i++)
					{
						TriangleMesh::Triangle triangle = _mesh->data().triangles[_triangles[i]];

						Intersection hitDistBaric;
						if (intersectTriangle(localRay, triangle, hitDistBaric))
						{
							// Verify which triangle is closer
							if (hitDistBaric.distance < hitAux.distance) // qual triangulo esta mais proximo
							{
								hitAux.distance = hitDistBaric.distance;
								hitAux.b = hitDistBaric.b;
								hitAux.triangleIndex = _triangles[i];
							}
						}
					}
				}
				else
				{
					_stack.push(aabb->children[0]);
					_stack.push(aabb->children[1]);
				}
			}
		}

		return hitAux.distance != math::Limits<float>::inf() ? true : false;
	}

} // end namespace cg
