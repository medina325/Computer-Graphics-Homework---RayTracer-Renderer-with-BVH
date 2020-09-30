#include "geometry/MeshSweeper.h"
#include "P4.h"

MeshMap P4::_defaultMeshes;

inline auto
normalize(const vec4f& p)
{
  return vec3f{p} * math::inverse(p.w);
}

inline auto
viewportToNDC(int x, int y) // equacao 5.24
{
  GLint v[4];

  glGetIntegerv(GL_VIEWPORT, v);

  const auto xn = float(x - v[0]) * 2.0f / float(v[2]) - 1.0f;
  const auto yn = float(y - v[1]) * 2.0f / float(v[3]) - 1.0f;

  return vec4f{xn, yn, -1, 1}; // retornando coordenada de recorte na real
}

inline Ray
P4::makeRay(int x, int y) const
{
  auto c = _editor->camera();
  mat4f m{vpMatrix(c)};

  m.invert(); // coordenada de recorte para camera para global

  auto p = normalize(m * viewportToNDC(x, height() - y)); // p eh o ponto onde o mouse esta em coordenadas globais 
  auto t = c->transform();
  Ray r{t->position(), -t->forward()}; // t->position = C em coordenadas globais e forward aponta para z (para tras da camera)

  if (c->projectionType() == Camera::Perspective)
    r.direction = (p - r.origin).versor();
  else
    r.origin = p - r.direction * c->nearPlane();
  return r;
}

inline void
P4::buildDefaultMeshes()
{
  _defaultMeshes["None"] = nullptr;
  _defaultMeshes["Box"] = GLGraphics3::box();
  _defaultMeshes["Sphere"] = GLGraphics3::sphere();
}

inline Primitive*
makePrimitive(MeshMapIterator mit)
{
  return new Primitive(mit->second, mit->first);
}

inline void
P4::slenderScene()
{
	_scene->backgroundColor.setRGB(0, 0, 0);
	_scene->ambientLight.setRGB(255, 255, 255);

	auto& meshes = Assets::meshes();
	if (!meshes.empty())
	{
		SceneObject *slender, *lamp_left, *lamp_right;
		auto endMeshes = meshes.end();
		Primitive* p;
		for (auto mit = meshes.begin(); mit != endMeshes; mit++)
		{
			if (strcmp(mit->first.c_str(), "Slender_Man.obj") == 0)
			{
				Assets::loadMesh(mit);
				p = makePrimitive(mit);
				slender = new SceneObject("Slender", *_scene, p, nullptr, nullptr, nullptr);
				slender->primitive()->material.diffuse.setRGB(100, 0, 0);
				slender->primitive()->material.specular.setRGB(0, 0, 0);
				slender->primitive()->material.ambient.setRGB(0, 0, 0);
			}
			if (strcmp(mit->first.c_str(), "Lamp.obj") == 0)
			{
				Assets::loadMesh(mit);
				p = makePrimitive(mit);
				lamp_left = new SceneObject("Lamp_left", *_scene, p, nullptr, nullptr, nullptr);
				lamp_left->transform()->translate({ -8.6f, 0.f, -3.1f });
				lamp_left->transform()->setLocalScale({ 1.1f, 1.1f, 1.1f });
				lamp_left->primitive()->material.ambient.setRGB(0, 0, 0);
				lamp_left->primitive()->material.diffuse.setRGB(75, 60, 81);
				lamp_left->primitive()->material.specular.setRGB(255, 255, 255);

				p = makePrimitive(mit);
				lamp_right = new SceneObject("Lamp_right", *_scene, p, nullptr, nullptr, nullptr);
				lamp_right->transform()->translate({ 7.8f, 0.f, -3.1f });
				lamp_right->transform()->setLocalScale({ 1.1f, 1.1f, 1.1f });
				lamp_right->transform()->setEulerAngles({ 0.f, 180.f, 0.f });
				lamp_right->primitive()->material.ambient.setRGB(0, 0, 0);
				lamp_right->primitive()->material.diffuse.setRGB(75, 60, 81);
				lamp_right->primitive()->material.specular.setRGB(255, 255, 255);
			}
		}
	}

	auto l1 = new SceneObject("Spot", *_scene, nullptr, nullptr, nullptr, new Light(Light::Spot));
	l1->transform()->translate({ -3.1f, 22.f, -0.9f });
	l1->transform()->setEulerAngles({ -140.f, -0.5f, 0.f });
	l1->light()->setAngGama(45.f);

	auto l2 = new SceneObject("Point", *_scene, nullptr, nullptr, nullptr, new Light(Light::Point));
	l2->transform()->translate({ 0.f, 12.f, 4.8f });
	l2->transform()->setEulerAngles({ 16.f, 0.f, 0.f });

	auto moon = new SceneObject("Moon", *_scene, makePrimitive(_defaultMeshes.find("Sphere")),
		nullptr, nullptr, nullptr);
	moon->transform()->translate({ 0.f, 25.f, -19.f });
	moon->transform()->setLocalScale({ 13.f, 11.f, 1.f });
	moon->primitive()->material.specular.setRGB(0, 0, 0);
	moon->primitive()->material.diffuse.setRGB(0, 0, 0);
	moon->primitive()->material.spot.setRGB(0, 0, 0);
	moon->primitive()->material.ambient.setRGB(255, 255, 255);

	auto c1 = new SceneObject("Camera 1", *_scene, nullptr, new Camera, nullptr, nullptr);
	c1->transform()->translate({ 0.f, 9.2f, 16.f });
	c1->transform()->setEulerAngles({ 13.f, 0.f, 0.f });

	Camera::setCurrent(c1->camera());
}

inline void
P4::mirrorScene()
{
	_scene->backgroundColor.setRGB(0, 0, 0);

	for (int i = 0; 5 > i; i++)
	{
		auto ballsToTheLeft = new SceneObject("leftBalll", *_scene, makePrimitive(_defaultMeshes.find("Sphere")),
			nullptr, nullptr, nullptr); 
		auto ballsToTheRight = new SceneObject("rightBall", *_scene, makePrimitive(_defaultMeshes.find("Sphere")),
				nullptr, nullptr, nullptr);
		auto ballsUp = new SceneObject("upBall", *_scene, makePrimitive(_defaultMeshes.find("Sphere")),
			nullptr, nullptr, nullptr);
		auto ballsDown = new SceneObject("downBall", *_scene, makePrimitive(_defaultMeshes.find("Sphere")),
			nullptr, nullptr, nullptr);
		auto ballsMiddle = new SceneObject("middleBall", *_scene, makePrimitive(_defaultMeshes.find("Sphere")),
			nullptr, nullptr, nullptr);

		float j = (float)i;
		ballsToTheLeft->transform()->translate({ j * -2.2f, 0.f, 0.f });
		ballsToTheRight->transform()->translate({ j * 2.2f, 0.f, 0.f });
		ballsUp->transform()->translate({ 0.f, j * 2.2f, 0.f });
		ballsDown->transform()->translate({ 0.f, j * -2.2f, 0.f });
		ballsMiddle->transform()->translate({ j * -2.2f, 0.f, j * 2.2f});

		ballsToTheLeft->primitive()->material.specular.setRGB(100, 250, 80);
		ballsToTheRight->primitive()->material.specular.setRGB(170, 20, 200);
		ballsUp->primitive()->material.specular.setRGB(220, 190, 0);
		ballsDown->primitive()->material.specular.setRGB(200, 100, 10);
		ballsMiddle->primitive()->material.specular.setRGB(123, 101, 200);

		ballsToTheLeft->primitive()->material.diffuse.setRGB(100, 100, 100);
		ballsToTheRight->primitive()->material.diffuse.setRGB(100, 100, 100);
		ballsUp->primitive()->material.diffuse.setRGB(100, 100, 100);
		ballsDown->primitive()->material.diffuse.setRGB(100, 100, 100);
		ballsMiddle->primitive()->material.diffuse.setRGB(100, 100, 100);

		ballsToTheRight->setParent(ballsToTheLeft);
		ballsUp->setParent(ballsToTheLeft);
		ballsDown->setParent(ballsToTheLeft);
		ballsMiddle->setParent(ballsToTheLeft);
	}


	auto l1 = new SceneObject("Directional", *_scene, nullptr, nullptr, nullptr, new Light(Light::Directional));
	l1->transform()->setEulerAngles({ 52.f, 0.f, 0.f });
	l1->light()->color.setRGB(155, 0, 255);


	// auto l2 = new SceneObject("Spot", *_scene, nullptr, nullptr, nullptr, new Light(Light::Spot));
	// l2->transform()->translate({ 0.f, 0.f, 13.f });
	// l2->light()->setAngGama(12.f);
	// l2->light()->color.setRGB(0, 255, 0);

	auto c = new SceneObject("Camera", *_scene, nullptr, new Camera, nullptr, nullptr);
	c->transform()->translate({ 4.5f, 7.5f, 21.f });
	c->transform()->setLocalEulerAngles({ -16.f, 2.4f, 0.f });

	Camera::setCurrent(c->camera());
}

inline void
P4::buildScene()
{
	_current = _scene = new Scene{ "Scene 1" };
	_editor = new SceneEditor{ *_scene };
	_renderer = new GLRenderer{ *_scene };
	_rayTracer = new RayTracer{ *_scene };
	_editor->setDefaultView((float)width() / (float)height());

	if (buildFlag == 0)
		slenderScene();
	else if (buildFlag == 1)
		mirrorScene();		
}

void
P4::initialize()
{
  Application::loadShaders(_programGouraud, "shaders/P4.vs", "shaders/P4.fs");
  Assets::initialize();
  buildDefaultMeshes();
  buildScene();
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1.0f, 1.0f);
  glEnable(GL_LINE_SMOOTH);
	_programGouraud.use();
}

inline void
P4::focus(SceneObject* sceneObj)
{
	std::string name = sceneObj->name();
	std::string focus = "Focus on the ";
	std::string texto = focus + name;
	if (ImGui::Button((texto).c_str()))
	{
		vec3f sceneObjPosition = sceneObj->transform()->position();
		_editor->camera()->transform()->setPosition(sceneObjPosition + vec3f(0, 0, 10.f));
	}
}

void
P4::showHierarchy(std::list<cg::Reference<SceneObject>>::const_iterator itSceneObj)
{
	auto sceneObj = *itSceneObj;
	ImGuiTreeNodeFlags flag;

	flag = sceneObj->sizeChildren() > 0 ? ImGuiTreeNodeFlags_OpenOnArrow : ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

	auto open = ImGui::TreeNodeEx(sceneObj,
		_current == sceneObj ? flag | ImGuiTreeNodeFlags_Selected : flag,
		sceneObj->name()
	);

	if (ImGui::IsItemClicked())
		_current = sceneObj;

	if (ImGui::BeginDragDropSource()) {
		ImGui::Text(sceneObj->name());
		ImGui::SetDragDropPayload("Accept", &itSceneObj, sizeof(SceneObject));
		ImGui::EndDragDropSource();
	}

	if (ImGui::BeginDragDropTarget())
	{
		if (auto * payload = ImGui::AcceptDragDropPayload("Accept"))
		{
			auto refIt = (std::list<Reference<SceneObject>>::const_iterator*) payload->Data;
			(*(*refIt))->setParent(sceneObj);
		}
		ImGui::EndDragDropTarget();
	}

	if (open && flag == ImGuiTreeNodeFlags_OpenOnArrow) {
		for (auto it = sceneObj->beginChildren(); it != sceneObj->endChildren(); it++)
			showHierarchy(it);
		ImGui::TreePop();
	}
}

inline void
P4::hierarchyWindow()
{
	ImGui::Begin("Hierarchy");
	if (ImGui::Button("Create###object"))
		ImGui::OpenPopup("CreateObjectPopup");
	if (ImGui::BeginPopup("CreateObjectPopup"))
	{
		Reference<SceneObject> currentSceneObj = (SceneObject*)_current;

		if (ImGui::MenuItem("Empty Object"))
		{
			if (_current == _scene)
			{
				Reference<SceneObject>(new SceneObject{ ("Object " + std::to_string(++_emptyCount)).c_str(),
					*_scene,
					nullptr,
					nullptr,
					nullptr,
					nullptr });
			}
			else
			{
				Reference<SceneObject>(new SceneObject{ ("Object " + std::to_string(++_emptyCount)).c_str(),
					*_scene,
					nullptr,
					nullptr,
					currentSceneObj,
					nullptr });
			}
		}
		if (ImGui::BeginMenu("3D Object"))
		{
			if (ImGui::MenuItem("Box"))
			{
				if (_current == _scene)
				{
					Reference<SceneObject>(new SceneObject{ ("Box " + std::to_string(++_boxCount)).c_str(),
						*_scene,
						makePrimitive(_defaultMeshes.find("Box")),
						nullptr,
						nullptr,
						nullptr });
				}
				else {
					Reference<SceneObject>(new SceneObject{ ("Box " + std::to_string(++_boxCount)).c_str(),
						*_scene,
						makePrimitive(_defaultMeshes.find("Box")),
						nullptr,
						currentSceneObj,
						nullptr });
				}
			}
			if (ImGui::MenuItem("Sphere"))
			{
				if (_current == _scene)
				{
					Reference<SceneObject>(new SceneObject{ ("Sphere " + std::to_string(++_sphereCount)).c_str(),
						*_scene,
						makePrimitive(_defaultMeshes.find("Sphere")),
						nullptr,
						nullptr,
						nullptr });
				}
				else {
					Reference<SceneObject>(new SceneObject{ ("Sphere " + std::to_string(++_sphereCount)).c_str(),
						*_scene,
						makePrimitive(_defaultMeshes.find("Sphere")),
						nullptr,
						currentSceneObj,
						nullptr });
				}
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Light"))
		{
			_lightCount = (int)_scene->sizeLights() + 1;
			if (ImGui::MenuItem("Directional Light"))
			{
				if (_current == _scene)
				{
					Reference<SceneObject>(new SceneObject{ ("Light " + std::to_string(_lightCount)).c_str(),
						*_scene,
						nullptr,
						nullptr,
						nullptr,
						new Light(Light::Type::Directional) });
				}
				else {
					Reference<SceneObject>(new SceneObject{ ("Light " + std::to_string(_lightCount)).c_str(),
						*_scene,
						nullptr,
						nullptr,
						currentSceneObj,
						new Light(Light::Type::Directional) });
				}
			}
			if (ImGui::MenuItem("Point Light"))
			{
				if (_current == _scene)
				{
					Reference<SceneObject>(new SceneObject{ ("Light " + std::to_string(_lightCount)).c_str(),
						*_scene,
						nullptr,
						nullptr,
						nullptr,
						new Light(Light::Type::Point) });
				}
				else {
					Reference<SceneObject>(new SceneObject{ ("Light " + std::to_string(_lightCount)).c_str(),
						*_scene,
						nullptr,
						nullptr,
						currentSceneObj,
						new Light(Light::Type::Point) });
				}
			}
			if (ImGui::MenuItem("Spotlight"))
			{
				if (_current == _scene)
				{
					Reference<SceneObject>(new SceneObject{ ("Light " + std::to_string(_lightCount)).c_str(),
						*_scene,
						nullptr,
						nullptr,
						nullptr,
						new Light(Light::Type::Spot) });
				}
				else {
					Reference<SceneObject>(new SceneObject{ ("Light " + std::to_string(_lightCount)).c_str(),
						*_scene,
						nullptr,
						nullptr,
						currentSceneObj,
						new Light(Light::Type::Spot) });
				}
			}
			ImGui::EndMenu();
		}
		if (ImGui::MenuItem("Camera"))
		{
			if (_current == _scene)
			{
				Reference<SceneObject>(new SceneObject{ ("Camera " + std::to_string(++_cameraCount)).c_str(),
					*_scene,
					nullptr,
					new Camera,
					nullptr,
					nullptr });
			}
			else
			{
				Reference<SceneObject>(new SceneObject{ ("Camera " + std::to_string(++_cameraCount)).c_str(),
					*_scene,
					nullptr,
					new Camera,
					currentSceneObj,
					nullptr });
			}
		}
		ImGui::EndPopup();
	}

	ImGui::SameLine();

	if (ImGui::Button("Delete"))
	{
		if (dynamic_cast<SceneObject*>(_current))
		{
			auto sceneObj = (SceneObject*)_current;

			if (sceneObj->parent()->parent() == nullptr)
			{
				_scene->removeRoot(sceneObj);
			}
			else {
				sceneObj->parent()->removeChild(sceneObj);
			}
		}
		_current = _scene;
	}

	if (dynamic_cast<SceneObject*>(_current))
	{
		ImGui::SameLine();
		SceneObject* sceneObj = (SceneObject*)_current;
		focus(sceneObj);
	}

	ImGui::Separator();

	ImGuiTreeNodeFlags flag{ ImGuiTreeNodeFlags_OpenOnArrow };

	auto open = ImGui::TreeNodeEx(_scene,
		_current == _scene ? flag | ImGuiTreeNodeFlags_Selected : flag,
		_scene->name());

	if (ImGui::IsItemClicked())
		_current = _scene;

	if (ImGui::BeginDragDropTarget())
	{
		if (auto * payload = ImGui::AcceptDragDropPayload("Accept"))
		{
			auto refIt = (std::list<Reference<SceneObject>>::const_iterator*) payload->Data;
			(*(*refIt))->setParent(_scene->root());
		}
		ImGui::EndDragDropTarget();
	}

	if (open)
	{
		// For all scene objects
		if (_scene->sizeRoot() > 0)
		{
			for (auto itSceneObj = _scene->beginRoot(); itSceneObj != _scene->endRoot(); itSceneObj++) {
				showHierarchy(itSceneObj);
			}
		}
		ImGui::TreePop();
	}
	ImGui::End();
}

namespace ImGui
{ // begin namespace ImGui

void
ObjectNameInput(NameableObject* object)
{
  const int bufferSize{128};
  static NameableObject* current;
  static char buffer[bufferSize];

  if (object != current)
  {
    strcpy_s(buffer, bufferSize, object->name());
    current = object;
  }
  if (ImGui::InputText("Name", buffer, bufferSize))
    object->setName(buffer);
}

inline bool
ColorEdit3(const char* label, Color& color)
{
  return ImGui::ColorEdit3(label, (float*)&color);
}

inline bool
DragVec3(const char* label, vec3f& v)
{
  return DragFloat3(label, (float*)&v, 0.1f, 0.0f, 0.0f, "%.2g");
}

void
TransformEdit(Transform* transform)
{
  vec3f temp;

  temp = transform->localPosition();
  if (ImGui::DragVec3("Position", temp))
    transform->setLocalPosition(temp);
  temp = transform->localEulerAngles();
  if (ImGui::DragVec3("Rotation", temp))
    transform->setLocalEulerAngles(temp);
  temp = transform->localScale();
  if (ImGui::DragVec3("Scale", temp))
    transform->setLocalScale(temp);
}

} // end namespace ImGui

inline void
P4::sceneGui()
{
  auto scene = (Scene*)_current;

  ImGui::ObjectNameInput(_current);
  ImGui::Separator();
  if (ImGui::CollapsingHeader("Colors"))
  {
    ImGui::ColorEdit3("Background", scene->backgroundColor);
    ImGui::ColorEdit3("Ambient Light", scene->ambientLight);
  }
}

inline void
P4::inspectShape(Primitive& primitive)
{
  char buffer[16];

  snprintf(buffer, 16, "%s", primitive.meshName());
  ImGui::InputText("Mesh", buffer, 16, ImGuiInputTextFlags_ReadOnly);
  if (ImGui::BeginDragDropTarget())
  {
    if (auto* payload = ImGui::AcceptDragDropPayload("PrimitiveMesh"))
    {
      auto mit = *(MeshMapIterator*)payload->Data;
      primitive.setMesh(mit->second, mit->first);
    }
    ImGui::EndDragDropTarget();
  }
  ImGui::SameLine();
  if (ImGui::Button("...###PrimitiveMesh"))
    ImGui::OpenPopup("PrimitiveMeshPopup");
  if (ImGui::BeginPopup("PrimitiveMeshPopup"))
  {
    auto& meshes = Assets::meshes();

    if (!meshes.empty())
    {
      for (auto mit = meshes.begin(); mit != meshes.end(); ++mit)
        if (ImGui::Selectable(mit->first.c_str()))
          primitive.setMesh(Assets::loadMesh(mit), mit->first);
      ImGui::Separator();
    }
    for (auto mit = _defaultMeshes.begin(); mit != _defaultMeshes.end(); ++mit)
      if (ImGui::Selectable(mit->first.c_str()))
        primitive.setMesh(mit->second, mit->first);
    ImGui::EndPopup();
  }
}

inline void
P4::inspectMaterial(Material& material)
{
  ImGui::ColorEdit3("Ambient", material.ambient);
  ImGui::ColorEdit3("Diffuse", material.diffuse);
  ImGui::ColorEdit3("Spot", material.spot);
  ImGui::DragFloat("Shine", &material.shine, 1, 0, 1000.0f);
  ImGui::ColorEdit3("Specular", material.specular);
}

inline void
P4::inspectPrimitive(Primitive& primitive)
{
  //const auto flag = ImGuiTreeNodeFlags_NoTreePushOnOpen;

  //if (ImGui::TreeNodeEx("Shape", flag))
    inspectShape(primitive);
  //if (ImGui::TreeNodeEx("Material", flag))
    inspectMaterial(primitive.material);
}

inline void
P4::inspectLight(Light& light)
{
	bool selected;
	static const char* lightTypes[]{ "Directional", "Point", "Spot" };
	auto lt = light.type();

	if (ImGui::BeginCombo("Type", lightTypes[lt]))
	{
		for (auto i = 0; i < IM_ARRAYSIZE(lightTypes); ++i)
		{
			selected = lt == i;

			if (ImGui::Selectable(lightTypes[i], selected))
				lt = (Light::Type)i;
			if (selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	light.setType(lt);

	ImGui::ColorEdit3("Color", light.color);

	if (light.type() != Light::Type::Directional)
	{
		static const char* falloffTypes[]{ "NoFalloff", "LinearFalloff", "QuadFalloff" };
		auto fL = light.fL();
		if (ImGui::BeginCombo("Falloff Types", falloffTypes[fL]))
		{
			for (auto i = 0; i < IM_ARRAYSIZE(falloffTypes); ++i)
			{
				selected = fL == i;

				if (ImGui::Selectable(falloffTypes[i], selected))
					fL = (Light::Falloff)i;
				if (selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		light.setFl(fL);

		if (light.type() == Light::Type::Spot)
		{
			auto gama = light.angGama();

			if (ImGui::SliderFloat("Opening Angle",
				&gama,
				0.f,
				90.f,
				"%.0f deg",
				1.0f))
				light.setAngGama(gama <= 0.f ? 0.f : gama);

			int eL = light.eL();
			if (ImGui::DragInt("el", &eL, 1, 0, 2))
				light.setEl(eL);
		}
	}
}

void
P4::inspectCamera(Camera& camera)
{
  static const char* projectionNames[]{"Perspective", "Orthographic"};
  auto cp = camera.projectionType();

  if (ImGui::BeginCombo("Projection", projectionNames[cp]))
  {
    for (auto i = 0; i < IM_ARRAYSIZE(projectionNames); ++i)
    {
      auto selected = cp == i;

      if (ImGui::Selectable(projectionNames[i], selected))
        cp = (Camera::ProjectionType)i;
      if (selected)
        ImGui::SetItemDefaultFocus();
    }
    ImGui::EndCombo();
  }
  camera.setProjectionType(cp);
  if (cp == View3::Perspective)
  {
    auto fov = camera.viewAngle();

    if (ImGui::SliderFloat("View Angle",
      &fov,
      MIN_ANGLE,
      MAX_ANGLE,
      "%.0f deg",
      1.0f))
      camera.setViewAngle(fov <= MIN_ANGLE ? MIN_ANGLE : fov);
  }
  else
  {
    auto h = camera.height();

    if (ImGui::DragFloat("Height",
      &h,
      MIN_HEIGHT * 10.0f,
      MIN_HEIGHT,
      math::Limits<float>::inf()))
      camera.setHeight(h <= 0 ? MIN_HEIGHT : h);
  }

  float n;
  float f;

  camera.clippingPlanes(n, f);

  if (ImGui::DragFloatRange2("Clipping Planes",
    &n,
    &f,
    MIN_DEPTH,
    MIN_DEPTH,
    math::Limits<float>::inf(),
    "Near: %.2f",
    "Far: %.2f"))
  {
    if (n <= 0)
      n = MIN_DEPTH;
    if (f - n < MIN_DEPTH)
      f = n + MIN_DEPTH;
    camera.setClippingPlanes(n, f);
  }
}

inline void
P4::addComponentButton(SceneObject& object)
{
	if (ImGui::Button("Add Component"))
		ImGui::OpenPopup("AddComponentPopup");
	if (ImGui::BeginPopup("AddComponentPopup"))
	{
		if (ImGui::MenuItem("Primitive"))
		{
			if (object.findComponent<Primitive>() == nullptr)
				object.setPrimitive(makePrimitive(_defaultMeshes.find("None")));
		}
		if (ImGui::BeginMenu("Light"))
		{
			if (ImGui::MenuItem("Directional"))
			{
				if (object.findComponent<Light>() == nullptr)
					object.setLight(new Light(Light::Type::Directional));
			}
			else if (ImGui::MenuItem("Point"))
			{
				if (object.findComponent<Light>() == nullptr)
					object.setLight(new Light(Light::Type::Point));
			}
			else if (ImGui::MenuItem("Spot"))
			{
				if (object.findComponent<Light>() == nullptr)
					object.setLight(new Light(Light::Type::Spot));
			}
			ImGui::EndMenu();
		}

		if (ImGui::MenuItem("Camera"))
		{
			if (object.findComponent<Camera>() == nullptr)
				object.setCamera(new Camera);
		}
		ImGui::EndPopup();
	}
}

inline void
P4::sceneObjectGui()
{
	auto object = (SceneObject*)_current;

	addComponentButton(*object);
	ImGui::Separator();
	ImGui::ObjectNameInput(object);
	ImGui::SameLine();
	ImGui::Checkbox("###visible", &object->visible);
	ImGui::Separator();
	if (ImGui::CollapsingHeader(object->transform()->typeName())) {
		auto t = object->transform();
		ImGui::TransformEdit(t);
		object->transform()->localToWorldMatrix();
	}

	for (auto it = object->beginComponents(); it != object->endComponents(); it++)
	{
		Component* component = *it;
		if (auto p = dynamic_cast<Primitive*>(component))
		{
			auto notDelete{ true };
			auto open = ImGui::CollapsingHeader(p->typeName(), &notDelete);

			if (!notDelete)
			{
				object->scene()->removePrimitive(p);
				object->removeComponent(p);
				it = object->beginComponents();
			}
			else if (open)
				inspectPrimitive(*p);
		}
		else if (auto l = dynamic_cast<Light*>(component))
		{
			auto notDelete{ true };
			auto open = ImGui::CollapsingHeader(l->typeName(), &notDelete);

			if (!notDelete)
			{
				object->scene()->removeLight(l);
				object->removeComponent(l);
				it = object->beginComponents();
			}
			else if (open)
				inspectLight(*l);
		}
		else if (auto c = dynamic_cast<Camera*>(component))
		{
			auto notDelete{ true };
			auto open = ImGui::CollapsingHeader(c->typeName(), &notDelete);

			if (!notDelete)
			{
				object->removeComponent(c);
				it = object->beginComponents();
			}
			else if (open)
			{
				auto isCurrent = c == Camera::current();

				ImGui::Checkbox("Current", &isCurrent);
				Camera::setCurrent(isCurrent ? c : nullptr);
				inspectCamera(*c);
			}
		}
	}
}

inline void
P4::objectGui()
{
  if (_current == nullptr)
    return;
  if (dynamic_cast<SceneObject*>(_current))
  {
    sceneObjectGui();
    return;
  }
  if (dynamic_cast<Scene*>(_current))
    sceneGui();
}

inline void
P4::inspectorWindow()
{
  ImGui::Begin("Inspector");
  objectGui();
  ImGui::End();
}

inline void
P4::editorViewGui()
{
  if (ImGui::Button("Set Default View"))
    _editor->setDefaultView(float(width()) / float(height()));
  ImGui::Separator();

  auto t = _editor->camera()->transform();
  vec3f temp;

  temp = t->localPosition();
  if (ImGui::DragVec3("Position", temp))
    t->setLocalPosition(temp);
  temp = t->localEulerAngles();
  if (ImGui::DragVec3("Rotation", temp))
    t->setLocalEulerAngles(temp);
  inspectCamera(*_editor->camera());
  ImGui::Separator();
  {
    static int sm;

    ImGui::Combo("Shading Mode", &sm, "None\0Flat\0Gouraud\0\0");
    // TODO

    static Color edgeColor;
    static bool showEdges;

    ImGui::ColorEdit3("Edges", edgeColor);
    ImGui::SameLine();
    ImGui::Checkbox("###showEdges", &showEdges);
  }
  ImGui::Separator();
  ImGui::Checkbox("Show Ground", &_editor->showGround);
}

inline void
P4::assetsWindow()
{
  ImGui::Begin("Assets");
  if (ImGui::CollapsingHeader("Meshes"))
  {
    auto& meshes = Assets::meshes();

    for (auto mit = meshes.begin(); mit != meshes.end(); ++mit)
    {
      auto meshName = mit->first.c_str();
      auto selected = false;

      ImGui::Selectable(meshName, &selected);
      if (ImGui::BeginDragDropSource())
      {
        Assets::loadMesh(mit);
        ImGui::Text(meshName);
        ImGui::SetDragDropPayload("PrimitiveMesh", &mit, sizeof(mit));
        ImGui::EndDragDropSource();
      }
    }
  }
  ImGui::Separator();
  if (ImGui::CollapsingHeader("Textures"))
  {
    // next semester
  }
  ImGui::End();
}

inline void
P4::editorView()
{
  if (!_showEditorView)
    return;
  ImGui::Begin("Editor View Settings");
  editorViewGui();
  ImGui::End();
}

inline void
P4::fileMenu()
{
  if (ImGui::MenuItem("New"))
  {
    // TODO
  }
  if (ImGui::MenuItem("Open...", "Ctrl+O"))
  {
    // TODO
  }
  ImGui::Separator();
  if (ImGui::MenuItem("Save", "Ctrl+S"))
  {
    // TODO
  }
  if (ImGui::MenuItem("Save As..."))
  {
    // TODO
  }
  ImGui::Separator();
  if (ImGui::MenuItem("Exit", "Alt+F4"))
  {
    shutdown();
  }
}

inline bool
showStyleSelector(const char* label)
{
  static int style = 1;

  if (!ImGui::Combo(label, &style, "Classic\0Dark\0Light\0"))
    return false;
  switch (style)
  {
    case 0: ImGui::StyleColorsClassic();
      break;
    case 1: ImGui::StyleColorsDark();
      break;
    case 2: ImGui::StyleColorsLight();
      break;
  }
  return true;
}

inline void
P4::showOptions()
{
  ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.6f);
  showStyleSelector("Color Theme##Selector");
  ImGui::ColorEdit3("Selected Wireframe", _selectedWireframeColor);
  ImGui::PopItemWidth();

	if (dynamic_cast<SceneObject*>(_current))
	{
		SceneObject* sceneObj = (SceneObject*)_current;
		if (auto c = sceneObj->findComponent<Camera>())
		{
			ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.6f);
			Color color = *(c->color());
			ImGui::ColorEdit3("Selected Color", color);
			c->setColor(color);
			ImGui::PopItemWidth();
		}
	}
}

inline void
P4::mainMenu()
{
  if (ImGui::BeginMainMenuBar())
  {
    if (ImGui::BeginMenu("File"))
    {
      fileMenu();
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("View"))
    {
     // if (Camera::current() == 0)
       // ImGui::MenuItem("Edit View", nullptr, true, false);
      //else
      //{
        static const char* viewLabels[]{"Editor", "Renderer"};

        if (ImGui::BeginCombo("View", viewLabels[_viewMode]))
        {
          for (auto i = 0; i < IM_ARRAYSIZE(viewLabels); ++i)
          {
            if (ImGui::Selectable(viewLabels[i], _viewMode == i))
              _viewMode = (ViewMode)i;
          }
          ImGui::EndCombo();
          // TODO: change mode only if scene has changed
          if (_viewMode == ViewMode::Editor)
            _image = nullptr;
        }
      //}
      ImGui::Separator();
      ImGui::MenuItem("Assets Window", nullptr, &_showAssets);
      ImGui::MenuItem("Editor View Settings", nullptr, &_showEditorView);
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Tools"))
    {
      if (ImGui::BeginMenu("Options"))
      {
        showOptions();
        ImGui::EndMenu();
      }
      ImGui::EndMenu();
    }
		
		if (ImGui::BeginMenu("Examples"))
		{
			static const char* examplesLabels[]{ "Slender Scene", "Mirrors Scene" };
			if (ImGui::BeginCombo("Scene Types", examplesLabels[buildFlag]))
			{
				for (auto j = 0; j < IM_ARRAYSIZE(examplesLabels); ++j)
				{
					bool selected = buildFlag == j;

					if (ImGui::Selectable(examplesLabels[j], selected))
					{
						buildFlag = (int)j;
						buildScene();
						_programGouraud.use();
					}

					if (selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}

void
P4::gui()
{
  mainMenu();
  if (_viewMode == ViewMode::Renderer)
    return;
  hierarchyWindow();
  inspectorWindow();
  assetsWindow();
  editorView();
}

inline void
drawMesh(GLMesh* mesh, GLuint mode)
{
  glPolygonMode(GL_FRONT_AND_BACK, mode);
  glDrawElements(GL_TRIANGLES, mesh->vertexCount(), GL_UNSIGNED_INT, 0);
}

inline void
P4::drawPrimitive(Primitive& primitive)
{
	auto m = glMesh(primitive.mesh());

	if (nullptr == m)
		return;

	auto t = primitive.transform();
	auto normalMatrix = mat3f{ t->worldToLocalMatrix() }.transposed();

	_programGouraud.setUniformMat4("u_transform", t->localToWorldMatrix());
	_programGouraud.setUniformMat3("u_normalMatrix", normalMatrix);
	_programGouraud.setUniformVec4("u_Od", primitive.material.diffuse);
	_programGouraud.setUniformVec4("u_Oa", primitive.material.ambient);
	_programGouraud.setUniformVec4("u_Os", primitive.material.spot);
	_programGouraud.setUniform("u_shine", primitive.material.shine);
	_programGouraud.setUniformVec3("u_cameraPosition", _editor->camera()->transform()->position());
	_programGouraud.setUniform("u_flatMode", (int)0);

	m->bind();
	drawMesh(m, GL_FILL);

	if (primitive.sceneObject() != _current)
		return;
	_programGouraud.setUniformVec4("u_Od", _selectedWireframeColor);
	_programGouraud.setUniform("u_flatMode", (int)1);
	drawMesh(m, GL_LINE);
}

void
P4::drawEditorStuff(const Reference<SceneObject>& obj) {

	if (obj == _current)
	{
		if (_viewMode == ViewMode::Editor)
		{
			auto c = obj->findComponent<Camera>();
			if (c != nullptr)
				_editor->drawCamera(*c);

			auto l = obj->findComponent<Light>();
			if (l != nullptr)
				_editor->drawLight(*l);

			auto t = obj->transform();
			_editor->drawAxes(t->position(), mat3f{ t->rotation() });
		}
	}

	for (auto it = obj->beginChildren(); it != obj->endChildren(); it++)
		drawEditorStuff(*it);
}

void
P4::renderAll(Camera* camera)
{
	_programGouraud.setUniformMat4("u_vpMatrix", vpMatrix(camera));
	_programGouraud.setUniformVec4("u_ambientLight", _scene->ambientLight);
	_programGouraud.setUniform("u_lightCount", (int)_scene->sizeLights());

	for (auto it = _scene->beginRoot(); it != _scene->endRoot(); it++)
		drawEditorStuff(*it);

	for (auto itPrim = _scene->beginPrimitives(); itPrim != _scene->endPrimitives(); itPrim++)
	{
		Primitive* p = *itPrim;
		SceneObject* sceneObj = p->sceneObject();

		if (!sceneObj->visible)
			continue;

		int i;
		std::list<cg::Reference<Light>>::const_iterator l;
		auto sizeLights = _scene->sizeLights();
		for (i = 0, l = _scene->beginLights(); auto sizeLights = _scene->sizeLights() > i && 10 > i; i++, l++)
		{
			vec3f lightPosition = (*l)->transform()->position();
			Color cL = (*l)->color;
			vec3f dL = (*l)->transform()->rotation() * vec3f(0, -1, 0);

			if ((*l)->type() == Light::Type::Spot)
			{
				vec3f eixoCone = ((*l)->transform()->rotation() * vec3f(0, 0, -1)).normalize();
				std::string eixoConeStr = "u_lights[" + std::to_string(i) + "].eixoCone";
				_programGouraud.setUniformVec3(eixoConeStr.c_str(), eixoCone);
			}

			std::string typeStr = "u_lights[" + std::to_string(i) + "].type";
			std::string lightColorStr = "u_lights[" + std::to_string(i) + "].lightColor";
			std::string lightPositionStr = "u_lights[" + std::to_string(i) + "].lightPosition";
			std::string dlStr = "u_lights[" + std::to_string(i) + "].dL";
			std::string angGamaStr = "u_lights[" + std::to_string(i) + "].angGama";
			std::string fallOffStr = "u_lights[" + std::to_string(i) + "].falloff";
			std::string elStr = "u_lights[" + std::to_string(i) + "].eL";

			_programGouraud.setUniform(typeStr.c_str(), (*l)->type());
			_programGouraud.setUniformVec4(lightColorStr.c_str(), cL);
			_programGouraud.setUniformVec3(lightPositionStr.c_str(), lightPosition);
			_programGouraud.setUniformVec3(dlStr.c_str(), dL);
			_programGouraud.setUniform(angGamaStr.c_str(), (*l)->angGama());
			_programGouraud.setUniform(fallOffStr.c_str(), (*l)->fL());
			_programGouraud.setUniform(elStr.c_str(), (*l)->eL());

		}
		drawPrimitive(*p);
	}
}

inline void
P4::renderScene()
{
  if (auto camera = Camera::current())
  {
    if (_image == nullptr)
    {
      const auto w = width(), h = height();
			
      _image = new GLImage{w, h};
      _rayTracer->setImageSize(w, h);
      _rayTracer->setCamera(camera);
      _rayTracer->renderImage(*_image);
    }
    _image->draw(0, 0);
  }
}

inline void
P4::preview()
{
	Camera* c = ((SceneObject*)_current)->camera();

	GLsizei h = (int)(_editor->camera()->height());
	GLsizei w = (int)(h * _editor->camera()->aspectRatio());

	int viewPort[4];
	glGetIntegerv(GL_VIEWPORT, viewPort);
	glViewport(0, 0, w * 30, h * 30);
	glScissor(0, 0, w * 30, h * 30);
	glEnable(GL_SCISSOR_TEST);

	_renderer->setCamera(c);
	_renderer->setImageSize(w * 30, h * 30);
	_renderer->render();

	_programGouraud.use();

	glDisable(GL_SCISSOR_TEST);
	glViewport(viewPort[0], viewPort[1], viewPort[2], viewPort[3]);
}

constexpr auto CAMERA_RES = 0.01f;
constexpr auto ZOOM_SCALE = 1.01f;

inline void
P4::renderEditorView()
{
	if (_moveFlags)
	{
		const auto delta = _editor->orbitDistance() * CAMERA_RES;
		auto d = vec3f::null();

		if (_moveFlags.isSet(MoveBits::Forward))
			d.z -= delta;
		if (_moveFlags.isSet(MoveBits::Back))
			d.z += delta;
		if (_moveFlags.isSet(MoveBits::Left))
			d.x -= delta;
		if (_moveFlags.isSet(MoveBits::Right))
			d.x += delta;
		if (_moveFlags.isSet(MoveBits::Up))
			d.y += delta;
		if (_moveFlags.isSet(MoveBits::Down))
			d.y -= delta;
		_editor->pan(d);
	}
	_editor->newFrame();

	// moved to scene editor??????
	renderAll(_editor->camera());

	if (dynamic_cast<SceneObject*>(_current))
		if (((SceneObject*)_current)->findComponent<Camera>() != nullptr)
			preview();
}

void
P4::render()
{
	if (_viewMode == ViewMode::Renderer)
		renderScene();
	else if (_viewMode == ViewMode::Editor)
		renderEditorView();

	return;
}

bool
P4::windowResizeEvent(int width, int height)
{
  _editor->camera()->setAspectRatio(float(width) / float(height));
  _viewMode = ViewMode::Editor;
  _image = nullptr;
  return true;
}

bool
P4::keyInputEvent(int key, int action, int mods)
{
  auto active = action != GLFW_RELEASE && mods == GLFW_MOD_ALT;

  switch (key)
  {
    case GLFW_KEY_W:
      _moveFlags.enable(MoveBits::Forward, active);
      break;
    case GLFW_KEY_S:
      _moveFlags.enable(MoveBits::Back, active);
      break;
    case GLFW_KEY_A:
      _moveFlags.enable(MoveBits::Left, active);
      break;
    case GLFW_KEY_D:
      _moveFlags.enable(MoveBits::Right, active);
      break;
    case GLFW_KEY_Q:
      _moveFlags.enable(MoveBits::Up, active);
      break;
    case GLFW_KEY_Z:
      _moveFlags.enable(MoveBits::Down, active);
      break;
  }
  return false;
}

bool
P4::scrollEvent(double, double yOffset)
{
  if (ImGui::GetIO().WantCaptureMouse)
    return false;
  _editor->zoom(yOffset < 0 ? 1.0f / ZOOM_SCALE : ZOOM_SCALE);
  return true;
}

bool
P4::mouseButtonInputEvent(int button, int actions, int mods)
{
  if (ImGui::GetIO().WantCaptureMouse)
    return false;
  (void)mods;

  auto active = actions == GLFW_PRESS;

  if (button == GLFW_MOUSE_BUTTON_LEFT)
  {
		
		auto minDistance = math::Limits<float>::inf();
    if (active)
    {
      cursorPosition(_pivotX, _pivotY);

      const auto ray = makeRay(_pivotX, _pivotY);

			auto endPrimitives = _scene->beginPrimitives();
			for (auto p = _scene->beginPrimitives(); p != endPrimitives; p++)
			{
				if (!(*p)->sceneObject()->visible)
					continue;

				Intersection hit;
				hit.distance = math::Limits<float>::inf();
				if ((*p)->intersect(ray, hit))
				{
					if (hit.distance < minDistance)
					{
						minDistance = hit.distance;
						_current = (*p)->sceneObject();
					}
					
				}
			}
    }
    return minDistance != math::Limits<float>::inf() ? true : false;
  }
  if (button == GLFW_MOUSE_BUTTON_RIGHT)
    _dragFlags.enable(DragBits::Rotate, active);
  else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
    _dragFlags.enable(DragBits::Pan, active);
  if (_dragFlags)
    cursorPosition(_pivotX, _pivotY);
  return true;
}

bool
P4::mouseMoveEvent(double xPos, double yPos)
{
  if (!_dragFlags)
    return false;
  _mouseX = (int)xPos;
  _mouseY = (int)yPos;

  const auto dx = (_pivotX - _mouseX);
  const auto dy = (_pivotY - _mouseY);

  _pivotX = _mouseX;
  _pivotY = _mouseY;
  if (dx != 0 || dy != 0)
  {
    if (_dragFlags.isSet(DragBits::Rotate))
    {
      const auto da = -_editor->camera()->viewAngle() * CAMERA_RES;
      isKeyPressed(GLFW_KEY_LEFT_ALT) ?
        _editor->orbit(dy * da, dx * da) :
        _editor->rotateView(dy * da, dx * da);
    }
    if (_dragFlags.isSet(DragBits::Pan))
    {
      const auto dt = -_editor->orbitDistance() * CAMERA_RES;
      _editor->pan(-dt * math::sign(dx), dt * math::sign(dy), 0);
    }
  }
  return true;
}
