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
// OVERVIEW: GLRenderer.cpp
// ========
// Source file for OpenGL renderer.
//
// Author(s): Paulo Pagliosa (and your name)
// Last revision: 09/09/2019

#include "GLRenderer.h"

namespace cg
{ // begin namespace cg


//////////////////////////////////////////////////////////
//
// GLRenderer implementation
// ==========
void
GLRenderer::update()
{
  Renderer::update();
  // TODO
}

void
GLRenderer::render()
{
	GLSL::Program::setCurrent(&_programPhong);
	const auto& bc = _scene->backgroundColor;

	glClearColor(bc.r, bc.g, bc.b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	_programPhong.setUniformMat4("u_vpMatrix", vpMatrix(camera()));
	_programPhong.setUniformVec4("u_ambientLight", _scene->ambientLight);
	_programPhong.setUniform("u_lightCount", (int)_scene->sizeLights());

	for (auto itPrim = _scene->beginPrimitives(); itPrim != _scene->endPrimitives(); itPrim++)
	{
		auto primitive = *itPrim;
		SceneObject* sceneObj = primitive->sceneObject();

		if (!sceneObj->visible)
			continue;

		int i;
		std::list<cg::Reference<Light>>::const_iterator l;
		for (i = 0, l = _scene->beginLights(); _scene->sizeLights() > i && 10 > i; i++, l++)
		{
			std::string typeT = "u_lights[" + std::to_string(i) + "].type";
			_programPhong.setUniform(typeT.c_str(), (int)(*l)->type());

			vec3f eixoCone;
			vec3f lightPosition = (*l)->transform()->position();
			Color cL = (*l)->color;
			vec3f dL = (*l)->transform()->rotation() * vec3f(0, -1, 0);

			if ((*l)->type() == Light::Type::Spot)
				eixoCone = ((*l)->transform()->rotation() * vec3f(0, 0, -1)).normalize();

			std::string lightColorT = "u_lights[" + std::to_string(i) + "].lightColor";
			std::string lightPositionT = "u_lights[" + std::to_string(i) + "].lightPosition";
			std::string eixoConeT = "u_lights[" + std::to_string(i) + "].eixoCone";
			std::string dLT = "u_lights[" + std::to_string(i) + "].dL";
			std::string angGamaT = "u_lights[" + std::to_string(i) + "].angGama";
			std::string falloffT = "u_lights[" + std::to_string(i) + "].falloff";
			std::string eLT = "u_lights[" + std::to_string(i) + "].eL";


			_programPhong.setUniformVec4(lightColorT.c_str(), cL);
			_programPhong.setUniformVec3(lightPositionT.c_str(), lightPosition);
			_programPhong.setUniformVec3(eixoConeT.c_str(), eixoCone);
			_programPhong.setUniformVec3(dLT.c_str(), dL);
			_programPhong.setUniform(angGamaT.c_str(), (*l)->angGama());
			_programPhong.setUniform(falloffT.c_str(), (*l)->fL());
			_programPhong.setUniform(eLT.c_str(), (*l)->eL());

		}
		auto mesh = glMesh(primitive->mesh());

		if (nullptr == mesh)
			return;

		auto t = primitive->transform();
		auto normalMatrix = mat3f{ t->worldToLocalMatrix() }.transposed();

		_programPhong.setUniformMat4("u_transform", t->localToWorldMatrix());
		_programPhong.setUniformMat3("u_normalMatrix", normalMatrix);
		_programPhong.setUniformVec4("u_Od", primitive->material.diffuse);
		_programPhong.setUniformVec4("u_Oa", primitive->material.ambient);
		_programPhong.setUniformVec4("u_Os", primitive->material.spot);
		_programPhong.setUniform("u_shine", primitive->material.shine);
		_programPhong.setUniformVec3("u_cameraPosition", camera()->transform()->position());
		_programPhong.setUniform("u_flatMode", (int)0);
		
		mesh->bind();

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawElements(GL_TRIANGLES, mesh->vertexCount(), GL_UNSIGNED_INT, 0);
	}
}

} // end namespace cg
