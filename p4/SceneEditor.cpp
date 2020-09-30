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
// OVERVIEW: SceneEditor.cpp
// ========
// Source file for scene editor.
//
// Author(s): Paulo Pagliosa (and your name)
// Last revision: 23/09/2019

#include "SceneEditor.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// SceneEditor impementation
// ===========
void
SceneEditor::setDefaultView(float aspect)
{
  auto t = _camera->transform();

  t->reset();
  t->setLocalPosition({0, 0, 10});
  _camera->reset(aspect);
}

void
SceneEditor::zoom(float s)
{
  if (s > 0)
    if (_camera->projectionType() == Camera::Perspective)
      _camera->setViewAngle(_camera->viewAngle() / s);
    else
      _camera->setHeight(_camera->height() / s);
}

void
SceneEditor::rotateView(float ax, float ay)
{
  auto t = _camera->transform();
  auto q = quatf{ay, vec3f{0, 1, 0}} * t->rotation();

  t->setRotation(quatf{ax, q.rotate({1, 0, 0})} * q);
}

void
SceneEditor::orbit(float ax, float ay)
{
  auto t = _camera->transform();
  auto p = t->position() - t->forward() * _orbitDistance;

  rotateView(ax, ay);
  t->setPosition(p + t->forward() * _orbitDistance);
}

void
SceneEditor::pan(const vec3f& d)
{
  _camera->transform()->translate(d, Transform::Space::Local);
}

void
SceneEditor::newFrame()
{
  const auto& bc = _scene->backgroundColor;

  glClearColor(bc.r, bc.g, bc.b, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  setView(_camera->transform()->position(), vpMatrix(_camera));
  if (showGround)
    drawXZPlane(10, 1);
}

void
SceneEditor::drawCamera(Camera& camera)
{
	float F, B;
	camera.clippingPlanes(F, B);

	// Changing the scale
	F *= 54.f;
	B *= 0.003f;

	float H, W;
	if (camera.projectionType() == Camera::ProjectionType::Perspective)
		H = 2 * F * tan(math::toRadians(camera.viewAngle()) / 2);
	else if (camera.projectionType() == Camera::ProjectionType::Parallel)
		H = camera.height() / 13.4f;

	W = H * camera.aspectRatio();

	Transform t1 = *(camera.transform()),
		t2 = t1,
		t3 = t1,
		t4 = t1,
		t5 = t1,
		t6 = t1,
		t7 = t1,
		t8 = t1;

	if (camera.projectionType() == Camera::ProjectionType::Perspective)
	{
		t1.translate(vec3f(-W / 2, H / 2, -F));
		t2.translate(vec3f(W / 2, H / 2, -F));
		t3.translate(vec3f(W / 2, -H / 2, -F));
		t4.translate(vec3f(-W / 2, -H / 2, -F));
		t5.translate(vec3f(-W / 2, B * H / F, -B));
		t6.translate(vec3f(W / 2, B * H / F, -B));
		t7.translate(vec3f(W / 2, -B * H / F, -B));
		t8.translate(vec3f(-W / 2, -B * H / F, -B));
	}
	else if (camera.projectionType() == Camera::ProjectionType::Parallel)
	{
		t1.translate(vec3f(-W / 2, H / 2, -F));
		t2.translate(vec3f(W / 2, H / 2, -F));
		t3.translate(vec3f(W / 2, -H / 2, -F));
		t4.translate(vec3f(-W / 2, -H / 2, -F));
		t5.translate(vec3f(-W / 2, H / 2, -B));
		t6.translate(vec3f(W / 2, H / 2, -B));
		t7.translate(vec3f(W / 2, -H / 2, -B));
		t8.translate(vec3f(-W / 2, -H / 2, -B));
	}

	vec3f a = t1.position(),
		b = t2.position(),
		c = t3.position(),
		d = t4.position(),
		e = t5.position(),
		f = t6.position(),
		g = t7.position(),
		h = t8.position();

	setVectorColor(*(camera.color()));
	drawLine(a, e);
	drawLine(e, f);
	drawLine(d, h);
	drawLine(c, g);
	drawLine(b, f);
	drawLine(a, b);
	drawLine(b, c);
	drawLine(a, d);
	drawLine(d, c);
	drawLine(e, h);
	drawLine(f, g);
	drawLine(h, g);
}

void
SceneEditor::drawLight(Light& light)
{
	Transform t1 = *(light.transform()),
		t2 = t1,
		t3 = t1,
		t4 = t1,
		t5 = t1,
		t6 = t1,
		t7 = t1,
		t8 = t1;

	setVectorColor(light.color);
	if (light.type() == Light::Type::Point)
	{
		t1.translate(vec3f(0.0f, 1.0f, 0));
		t2.translate(vec3f(0.0f, 3.0f, 0));
		t3.translate(vec3f(-1.f, 2.0f, 0));
		t4.translate(vec3f(1.0f, 2.0f, 0));
		t5.translate(vec3f(-0.5f, 2.5f, 0));
		t6.translate(vec3f(0.5f, 1.5f, 0));
		t7.translate(vec3f(0.5f, 2.5f, 0));
		t8.translate(vec3f(-0.5f, 1.5f, 0));

		vec3f p1 = t1.position();
		vec3f p2 = t2.position();
		vec3f p3 = t3.position();
		vec3f p4 = t4.position();
		vec3f p5 = t5.position();
		vec3f p6 = t6.position();
		vec3f p7 = t7.position();
		vec3f p8 = t8.position();

		drawLine(p1, p2);
		drawLine(p3, p4);
		drawLine(p5, p6);
		drawLine(p7, p8);
	}
	else if (light.type() == Light::Type::Directional)
	{
		auto rotation = light.transform()->rotation();
		t1.translate(vec3f(-0.2f, 3, 0));
		t2.translate(vec3f(0.0f, 3, 0));
		t3.translate(vec3f(0.2f, 3, 0));
		t4.translate(vec3f(0.4f, 3, 0));

		vec3f p1 = t1.position();
		vec3f p2 = t2.position();
		vec3f p3 = t3.position();
		vec3f p4 = t4.position();

		setVectorColor(light.color);

		drawVector(p1, rotation * vec3f(0.0f, -1, 0), 1.f);
		drawVector(p2, rotation * vec3f(0.0f, -1, 0), 1.f);
		drawVector(p3, rotation * vec3f(0.0f, -1, 0), 1.f);
		drawVector(p4, rotation * vec3f(0.0f, -1, 0), 1.f);
	}
	else // Spot
	{
		float raio = -0.005f * tan(math::toRadians(light.angGama()));
		quatf rotation = light.transform()->rotation();

		for (auto i = 100; i > 0; i--)
		{
			Transform t1 = *(light.transform());
			t1.translate(vec3f{ 0,0,-0.01f * i });
			vec3f p1 = t1.position();

			drawCircle(p1, i * raio, rotation * vec3f(0, 0, 1));
		}
	}
}

} // end namespace cg
