#ifndef __P1_h
#define __P1_h

#include "Scene.h"
#include "graphics/Application.h"

using namespace cg;

class P1: public GLWindow
{
private:
	GLSL::Program _program;
	Reference<Scene> _scene;	
	SceneNode* _current{};
	Color selectedWireframeColor{ 0, 0, 0 };
	mat4f _transform{ mat4f::identity() };

	int _boxCount;
	int _emptyCount;
public:
  P1(int width, int height):
    GLWindow{"cg2019 - P1", width, height},
    _program{"P1"},
		_boxCount{0},
		_emptyCount{0}
  {
    // do nothing
  }

  /// Initialize the app.
  void initialize() override;

  /// Update the GUI.
  void gui() override;

  /// Render the scene and it's scene objects.
  void render() override;

private:
	void buildScene();
	void showHierarchy(const Reference<SceneObject>& sceneObj);
	void hierarchyWindow();
	void inspectorWindow();

	void sceneGui();
	void sceneObjectGui();
	void objectGui();

}; // P1

#endif // __P1_h
