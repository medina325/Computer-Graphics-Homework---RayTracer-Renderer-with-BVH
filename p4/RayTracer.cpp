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
// OVERVIEW: RayTracer.cpp
// ========
// Source file for simple ray tracer.
//
// Author(s): Paulo Pagliosa (and your name)
// Last revision: 16/10/2019

#include "Camera.h"
#include "RayTracer.h"
#include <time.h>

using namespace std;

namespace cg
{ // begin namespace cg

	void
		printElapsedTime(const char* s, clock_t time)
	{
		printf("%sElapsed time: %.4f s\n", s, (float)time / CLOCKS_PER_SEC);
	}


	/////////////////////////////////////////////////////////////////////
	//
	// RayTracer implementation
	// =========
	RayTracer::RayTracer(Scene& scene, Camera* camera) :
		Renderer{ scene, camera },
		_maxRecursionLevel{ 6 },
		_minWeight{ MIN_WEIGHT }
	{
		// TODO: BVH
	}

	void
		RayTracer::render()
	{
		throw std::runtime_error("RayTracer::render() invoked");
	}

	inline float
		windowHeight(Camera* c)
	{
		if (c->projectionType() == Camera::Parallel)
			return c->height();
		return c->nearPlane() * tan(math::toRadians(c->viewAngle() * 0.5f)) * 2;

	}
	void
		RayTracer::renderImage(Image& image)
	{
		auto t = clock();
		const auto& m = _camera->cameraToWorldMatrix();

		// VRC axes
		_vrc.u = m[0];
		_vrc.v = m[1];
		_vrc.n = m[2];
		// init auxiliary mapping variables
		_W = image.width();
		_H = image.height();
		_Iw = math::inverse(float(_W));
		_Ih = math::inverse(float(_H));

		auto height = windowHeight(_camera);

		_W >= _H ? _Vw = (_Vh = height) * _W * _Ih : _Vh = (_Vw = height) * _H * _Iw;
		// init pixel ray
		_pixelRay.origin = _camera->transform()->position();
		_pixelRay.direction = -_vrc.n;
		_camera->clippingPlanes(_pixelRay.tMin, _pixelRay.tMax);
		_numberOfRays = _numberOfHits = 0;
		scan(image);
		printf("\nNumber of rays: %llu", _numberOfRays);
		printf("\nNumber of hits: %llu", _numberOfHits);
		printElapsedTime("\nDONE! ", clock() - t);
	}

	void
		RayTracer::setPixelRay(float x, float y)
		//[]---------------------------------------------------[]
		//|  Set pixel ray                                      |
		//|  @param x coordinate of the pixel                   |
		//|  @param y cordinates of the pixel                   |
		//[]---------------------------------------------------[]
	{
		auto p = imageToWindow(x, y);

		switch (_camera->projectionType())
		{
		case Camera::Perspective:
			_pixelRay.direction = (p - _camera->nearPlane() * _vrc.n).versor();
			break;

		case Camera::Parallel:
			_pixelRay.origin = _camera->transform()->position() + p;
			break;
		}
	}

	void
		RayTracer::scan(Image& image)
	{
		ImageBuffer scanLine{ _W, 1 };

		for (int j = 0; j < _H; j++)
		{
			auto y = (float)j + 0.5f;

			printf("Scanning line %d of %d\r", j + 1, _H);
			for (int i = 0; i < _W; i++)
				scanLine[i] = shoot((float)i + 0.5f, y);
			image.setData(0, j, scanLine);
		}
	}

	Color
		RayTracer::shoot(float x, float y)
		//[]---------------------------------------------------[]
		//|  Shoot a pixel ray                                  |
		//|  @param x coordinate of the pixel                   |
		//|  @param y cordinates of the pixel                   |
		//|  @return RGB color of the pixel                     |
		//[]---------------------------------------------------[]
	{
		// set pixel ray
		setPixelRay(x, y);

		// trace pixel ray
		Color color = trace(_pixelRay, 0, 1.0f);

		// adjust RGB color
		if (color.r > 1.0f)
			color.r = 1.0f;
		if (color.g > 1.0f)
			color.g = 1.0f;
		if (color.b > 1.0f)
			color.b = 1.0f;
		// return pixel color
		return color;
	}

	Color
		RayTracer::trace(const Ray& ray, uint32_t level, float weight)
		//[]---------------------------------------------------[]
		//|  Trace a ray                                        |
		//|  @param the ray                                     |
		//|  @param recursion level                             |
		//|  @param ray weight                                  |
		//|  @return color of the ray                           |
		//[]---------------------------------------------------[]
	{
		if (level > _maxRecursionLevel)
			return Color::black;
		_numberOfRays++;

		Intersection hit;

		return intersect(ray, hit) ? shade(ray, hit, level, weight) : background();
	}

	inline constexpr auto
		rt_eps()
	{
		return 1e-2f;
	}

	bool
		RayTracer::intersect(const Ray& ray, Intersection& hit)
		//[]---------------------------------------------------[]
		//|  Ray/object intersection                            |
		//|  @param the ray (input)                             |
		//|  @param information on intersection (output)        |
		//|  @return true if the ray intersects an object       |
		//[]---------------------------------------------------[]
	{
		hit.object = nullptr;
		hit.distance = math::Limits<float>::inf();

		auto endPrimitives = _scene->endPrimitives();
		for (auto p = _scene->beginPrimitives(); p != endPrimitives; p++)
		{
			auto mesh = (*p)->mesh();
			auto bvh = bvhMap[mesh];
			if (bvh == nullptr)
				bvhMap[mesh] = bvh = new BVH{ *mesh, 16 };

			auto t = (*p)->transform();
			Ray localRay(t->worldToLocalMatrix().transform(ray.origin), t->worldToLocalMatrix().transformVector(ray.direction));
			auto d = math::inverse(localRay.direction.length());
			localRay.direction *= d;

			Intersection hitAux;
			hitAux.distance = math::Limits<float>::inf();
			if (bvh->intersect(localRay, hitAux))
			{
				_numberOfHits++;
				hitAux.distance *= d;

				// Verify which sceneObject is closer
				if (hitAux.distance < hit.distance) 
				{
					hit.distance = hitAux.distance;
					hit.b = hitAux.b;
					hit.triangleIndex = hitAux.triangleIndex;
					hit.object = *p;
				}
			}

			// Brute strength never again
			/*if ((*p)->intersect(ray, hit))
				_numberOfHits++;*/
		}
		return hit.object != nullptr;
	}

	vec3f reflect(vec3f L, vec3f N)
	{
		return (2.f * (float)N.dot(L) * N) * (1 / pow(N.length(), 2)) - L;
	}

	Color
		RayTracer::shade(const Ray& ray, Intersection& hit, int level, float weight)
		//[]---------------------------------------------------[]
		//|  Shade a point P                                    |
		//|  @param the ray (input)                             |
		//|  @param information on intersection (input)         |
		//|  @param recursion level                             |
		//|  @param ray weight                                  |
		//|  @return color at point P                           |
		//[]---------------------------------------------------[]
	{
		TriangleMesh::Data hitData = hit.object->mesh()->data();
		TriangleMesh::Triangle triangle = hitData.triangles[hit.triangleIndex];

		auto t = hit.object->sceneObject()->transform();
		auto normalMatrix = mat3f{ t->worldToLocalMatrix() }.transposed();

		vec3f n0 = hitData.vertexNormals[triangle.v[0]];
		vec3f n1 = hitData.vertexNormals[triangle.v[1]];
		vec3f n2 = hitData.vertexNormals[triangle.v[2]];

		vec3f N = hit.b.x * n0 + hit.b.y * n1 + hit.b.z * n2;
		N = normalMatrix * N;
		N = N.normalize();

		vec3f p0 = hitData.vertices[triangle.v[0]];
		vec3f p1 = hitData.vertices[triangle.v[1]];
		vec3f p2 = hitData.vertices[triangle.v[2]];

		//vec3f intersectP = hit.b.x * p0 + hit.b.y * p1 + hit.b.z * p2;
		//intersectP = t->localToWorldMatrix().transform(intersectP);
		vec3f intersectP = ray.origin + (hit.distance * ray.direction);
		intersectP += rt_eps() * N;

		vec3f V = (intersectP - _camera->transform()->position()).normalize();
		vec3f Rl;

		Color refDif = Color::black,
					refSpec = Color::black;

		auto endLights = _scene->endLights();
		for (auto l = _scene->beginLights(); l != endLights; l++)
		{
			Light* light = *l;
			Ray lightRay = Ray(intersectP, light->type() == Light::Directional ? -(light->transform()->rotation() * vec3f(0, -1, 0)).normalize() :
																																						(light->transform()->position() - intersectP).normalize());
			if (!shadow(lightRay))
			{
				Color iL = light->color;
				vec3f L = light->transform()->rotation() * vec3f(0, -1, 0);

				Light::Type type = light->type();
				if (type != Light::Directional)
				{
					L = intersectP - light->transform()->position();
					float d = L.length();
					L = L.normalize();

					iL = light->color * (1 / pow(d, light->fL()));

					if (type == Light::Spot)
					{
						vec3f Dl = ((light->transform()->rotation() * vec3f(0, 0, -1)).normalize());
						float cosPsi = std::max(Dl.dot(L), 0.f);
						float psi = math::toDegrees(acos(cosPsi));

						iL = psi <= light->angGama() ? iL * pow(cosPsi, light->eL()) : Color::black;
					}
				}

				Rl = (reflect(-L, N)).normalize();
				Color Od = hit.object->material.diffuse,
							Os = hit.object->material.spot;

				refDif += (Od * iL * std::max((-N).dot(L), 0.f));
				refSpec += (Os * iL * pow(std::max((-Rl).dot(V), 0.f), hit.object->material.shine));
			}
		} // end lights for

		Color color = (hit.object->material.ambient * _scene->ambientLight) + refDif + refSpec;

		Color Or = hit.object->material.specular;
		if (Or != Color::black)
		{
			vec3f Rv = reflect(-V, N);
			auto w = weight * std::max(Or.r, std::max(Or.g, Or.b));
			if (w > MIN_WEIGHT)
				color += Or * trace(Ray(intersectP, Rv), ++level, w);
		}

		return color;
	}

	Color
		RayTracer::background() const
		//[]---------------------------------------------------[]
		//|  Background                                         |
		//|  @return background color                           |
		//[]---------------------------------------------------[]
	{
		return _scene->backgroundColor;
	}

	bool
		RayTracer::shadow(const Ray& ray)
		//[]---------------------------------------------------[]
		//|  Verifiy if ray is a shadow ray                     |
		//|  @param the ray (input)                             |
		//|  @return true if the ray intersects an object       |
		//[]---------------------------------------------------[]
	{
		Intersection hit;
		return intersect(ray, hit);
	}

} // end namespace cg
