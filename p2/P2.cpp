#include "geometry/MeshSweeper.h"
#include "P2.h"
#include <string>
#include <iostream>

MeshMap P2::_defaultMeshes;

inline void
P2::buildDefaultMeshes()
{
  _defaultMeshes["None"] = nullptr;
  _defaultMeshes["Box"] = GLGraphics3::box();
  _defaultMeshes["Sphere"] = GLGraphics3::sphere();
}

inline Primitive*
makePrimitive(MeshMapIterator mit)
{
  return new Primitive{mit->second, mit->first};
}

inline void
P2::buildScene()
{
  _current = _scene = new Scene{"Scene 1"};
  _editor = new SceneEditor{*_scene};
  _editor->setDefaultView((float)width() / (float)height());
}

void
P2::initialize()
{
  Application::loadShaders(_program, "shaders/p2.vs", "shaders/p2.fs");
  Assets::initialize();
  buildDefaultMeshes();
  buildScene();
  _renderer = new GLRenderer{*_scene};
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1.0f, 1.0f);
  glEnable(GL_LINE_SMOOTH);
  _program.use();
}

namespace ImGui
{
	void ShowDemoWindow(bool*);
}

inline void
P2::focus(SceneObject* sceneObj)
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
P2::showHierarchy(std::list<cg::Reference<SceneObject>>::const_iterator itSceneObj)
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
		ImGui::SetDragDropPayload("Accept",  &itSceneObj, sizeof(SceneObject));
		ImGui::EndDragDropSource();
	}

	if (ImGui::BeginDragDropTarget())
	{
		if (auto* payload = ImGui::AcceptDragDropPayload("Accept"))
		{
			auto refIt = (std::list<Reference<SceneObject>>::const_iterator*) payload->Data;
			(*(*refIt))->setParent(sceneObj);
		}
		ImGui::EndDragDropTarget();
	}

	// Also verify the flag (because it was crashing on release mode)
	if (open && flag == ImGuiTreeNodeFlags_OpenOnArrow) {
		for (auto it = sceneObj->beginChildren(); it != sceneObj->endChildren(); it++)
			showHierarchy(it);
		ImGui::TreePop();
	}	
}

inline void
P2::hierarchyWindow()
{
	ImGui::Begin("Hierarchy");
	if (ImGui::Button("Create###object"))
		ImGui::OpenPopup("CreateObjectPopup");
	if (ImGui::BeginPopup("CreateObjectPopup"))
	{
		// To make it more readable
		Reference<SceneObject> currentSceneObj = (SceneObject*)_current;

		if (ImGui::MenuItem("Empty Object"))
		{
			if (_current == _scene)
			{
				Reference<SceneObject>(new SceneObject{ ("Object " + std::to_string(++_emptyCount)).c_str(),
					*_scene,
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
				currentSceneObj });
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
						nullptr });					
				}
				else {
					Reference<SceneObject>(new SceneObject{ ("Box " + std::to_string(++_boxCount)).c_str(),
						*_scene,
						makePrimitive(_defaultMeshes.find("Box")),
						nullptr,
						currentSceneObj });
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
						nullptr });
				}
				else {
					Reference<SceneObject>(new SceneObject{ ("Sphere " + std::to_string(++_sphereCount)).c_str(),
						*_scene,
						makePrimitive(_defaultMeshes.find("Sphere")),
						nullptr,
						currentSceneObj });
				}
			}
			ImGui::EndMenu();
		}
		if (ImGui::MenuItem("Camera"))
		{
			// DONE: create a new camera.
			if (_current == _scene)
			{
				Reference<SceneObject>(new SceneObject{ ("Camera " + std::to_string(++_cameraCount)).c_str(),
					*_scene,
					nullptr,
					new Camera,
					nullptr });
			}
			else 
			{
				Reference<SceneObject>(new SceneObject{ ("Camera " + std::to_string(++_cameraCount)).c_str(),
					*_scene,
					nullptr,
					new Camera,
					currentSceneObj });
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
		if (auto* payload = ImGui::AcceptDragDropPayload("Accept"))
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
			for (auto itSceneObj = _scene->beginRoot(); itSceneObj != _scene->endRoot(); itSceneObj++){
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
P2::sceneGui()
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
P2::inspectPrimitive(Primitive& primitive)
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
  ImGui::ColorEdit3("Mesh Color", (float*)&primitive.color);
}

void
P2::inspectCamera(Camera& camera)
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
P2::addComponentButton(SceneObject& object)
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
    if (ImGui::MenuItem("Camera"))
    {
			if (object.findComponent<Camera>() == nullptr)
				object.setCamera(new Camera);
    }
    ImGui::EndPopup();
  }
}

inline void
P2::sceneObjectGui()
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
P2::objectGui()
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
P2::inspectorWindow()
{
  ImGui::Begin("Inspector");
  objectGui();
  ImGui::End();
}

inline void
P2::editorViewGui()
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
  ImGui::Checkbox("Show Ground", &_editor->showGround);
}

inline void
P2::assetsWindow()
{
  if (!_showAssets)
    return;
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
    // p3
  }
  ImGui::End();
}

inline void
P2::editorView()
{
  if (!_showEditorView)
    return;
  ImGui::Begin("Editor View Settings");
  editorViewGui();
  ImGui::End();
}

inline void
P2::fileMenu()
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
P2::showOptions()
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
P2::mainMenu()
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
      if (Camera::current() == 0)
        ImGui::MenuItem("Edit View", nullptr, true, false);
      else
      {
        static const char* viewLabels[]{"Editor", "Renderer"};

        if (ImGui::BeginCombo("View", viewLabels[_viewMode]))
        {
          for (auto i = 0; i < IM_ARRAYSIZE(viewLabels); ++i)
          {
            if (ImGui::Selectable(viewLabels[i], _viewMode == i))
              _viewMode = (ViewMode)i;
          }
          ImGui::EndCombo();
        }
      }
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
    ImGui::EndMainMenuBar();
  }
}

void
P2::gui()
{
  mainMenu();
  hierarchyWindow();
  inspectorWindow();
  assetsWindow();
  editorView();

  /*
  static bool demo = true;
  ImGui::ShowDemoWindow(&demo);
  */
}

inline void
drawMesh(GLMesh* mesh, GLuint mode)
{
  glPolygonMode(GL_FRONT_AND_BACK, mode);
  glDrawElements(GL_TRIANGLES, mesh->vertexCount(), GL_UNSIGNED_INT, 0);
}

inline void
P2::drawPrimitive(Primitive& primitive)
{
  auto m = glMesh(primitive.mesh());

  if (nullptr == m)
    return;

  auto t = primitive.transform();
  auto normalMatrix = mat3f{t->worldToLocalMatrix()}.transposed();

  _program.setUniformMat4("transform", t->localToWorldMatrix());
  _program.setUniformMat3("normalMatrix", normalMatrix);
  _program.setUniformVec4("color", primitive.color);
  _program.setUniform("flatMode", (int)0);
  m->bind();
  drawMesh(m, GL_FILL);
  if (primitive.sceneObject() != _current)
    return;
  _program.setUniformVec4("color", _selectedWireframeColor);
  _program.setUniform("flatMode", (int)1);
  drawMesh(m, GL_LINE);
}


inline void
P2::drawCamera_and_Axes(const Reference<SceneObject>& obj) {

	if (obj == _current)
	{
		auto c = obj->findComponent<Camera>();

		if (c != nullptr && _viewMode == ViewMode::Editor)
			 _editor->drawCamera(*c);


		auto t = obj->transform();
		if(_viewMode == ViewMode::Editor)
			_editor->drawAxes(t->position(), mat3f{ t->rotation() });
	}

	for (auto it = obj->beginChildren(); it != obj->endChildren(); it++)
		drawCamera_and_Axes(*it);
}

void 
P2::renderAll(Camera* camera)
{
	_program.setUniformMat4("vpMatrix", vpMatrix(camera));
	_program.setUniformVec4("ambientLight", _scene->ambientLight);
	_program.setUniformVec3("lightPosition", camera->transform()->position());

	for (auto itPrim = _scene->beginPrimitives(); itPrim != _scene->endPrimitives(); itPrim++)
	{
		auto p = *itPrim;
		SceneObject* sceneObj = p->sceneObject();

		if (!sceneObj->visible)
			continue;

		drawPrimitive(*p);
	}

	for (auto it = _scene->beginRoot(); it != _scene->endRoot(); it++)
		drawCamera_and_Axes(*it);

}

inline void
P2::renderScene()
{
	if (auto camera = Camera::current())
	{
		_renderer->setCamera(camera);
		_renderer->setImageSize(width(), height());
		_renderer->render();

		_program.use();
	}
	else
		_viewMode = ViewMode::Editor;
}

inline void
P2::preview()
{
	Camera* c = ((SceneObject*)_current)->camera();

	GLsizei h = (int)(c->height());
	GLsizei w = (int)(h * c->aspectRatio());

	// Pagliosa's step  by step
	int viewPort[4];
	glGetIntegerv(GL_VIEWPORT, viewPort);
	glViewport(0, 0, w*30, h*30);
	glScissor(0, 0, w*30, h*30);
	glEnable(GL_SCISSOR_TEST);
	
	_renderer->setCamera(c);
	_renderer->setImageSize(w*30, h*30);
	_renderer->render();

	_program.use();

	glDisable(GL_SCISSOR_TEST);
	glViewport(viewPort[0], viewPort[1], viewPort[2], viewPort[3]);
}

constexpr auto CAMERA_RES = 0.01f;
constexpr auto ZOOM_SCALE = 1.01f;

inline void
P2::renderEditorView()
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
P2::render()
{

  if (_viewMode == ViewMode::Renderer)
		renderScene();
	else if (_viewMode == ViewMode::Editor)
		renderEditorView();

	return;
}

bool
P2::windowResizeEvent(int width, int height)
{
  _editor->camera()->setAspectRatio(float(width) / float(height));
  return true;
}

bool
P2::keyInputEvent(int key, int action, int mods)
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
    case GLFW_KEY_E:
      _moveFlags.enable(MoveBits::Down, active);
      break;
  }
  return false;
}

bool
P2::scrollEvent(double, double yOffset)
{
  if (ImGui::GetIO().WantCaptureMouse || _viewMode == ViewMode::Renderer)
    return false;
  _editor->zoom(yOffset < 0 ? 1.0f / ZOOM_SCALE : ZOOM_SCALE);
  return true;
}

bool
P2::mouseButtonInputEvent(int button, int actions, int mods)
{
  if (ImGui::GetIO().WantCaptureMouse || _viewMode == ViewMode::Renderer)
    return false;
  (void)mods;

  auto active = actions == GLFW_PRESS;

  if (button == GLFW_MOUSE_BUTTON_RIGHT)
    _dragFlags.enable(DragBits::Rotate, active);
  else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
    _dragFlags.enable(DragBits::Pan, active);
  if (_dragFlags)
    cursorPosition(_pivotX, _pivotY);
  return true;
}

bool
P2::mouseMoveEvent(double xPos, double yPos)
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
