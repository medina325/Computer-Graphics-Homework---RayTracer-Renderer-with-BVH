#ifndef __P2_h
#define __P2_h


#include "GLRenderer.h"
#include "SceneEditor.h"
#include "core/Flags.h"
#include "SceneObject.h"

using namespace cg;

class P2: public GLWindow
{
public:
  P2(int width, int height):
    GLWindow{"cg2019 - P2", width, height},
    _program{"P2"},
		_emptyCount{0},
		_boxCount{0},
		_sphereCount{0},
		_cameraCount{0}
  {
    // do nothing
  }

  /// Initialize the app.
  void initialize() override;

  /// Update the GUI.
  void gui() override;

  /// Render the scene.
  void render() override;

private:
  enum ViewMode
  {
    Editor = 0,
    Renderer = 1
  };

  enum class MoveBits
  {
    Left = 1,
    Right = 2,
    Forward = 4,
    Back = 8,
    Up = 16,
    Down = 32
  };

  enum class DragBits
  {
    Rotate = 1,
    Pan = 2
  };

	int _boxCount;
	int _emptyCount;
	int _sphereCount;
	int _cameraCount;
  GLSL::Program _program;
  Reference<Scene> _scene;
  Reference<SceneEditor> _editor;
  Reference<GLRenderer> _renderer;
  
  SceneNode* _current{};
  Color _selectedWireframeColor{255, 102, 0};
  Flags<MoveBits> _moveFlags{};
  Flags<DragBits> _dragFlags{};
  int _pivotX;
  int _pivotY;
  int _mouseX;
  int _mouseY;
  bool _showAssets{true};
  bool _showEditorView{true};
  ViewMode _viewMode{ViewMode::Editor};

  static MeshMap _defaultMeshes;

  void buildScene();

  void mainMenu();
  void fileMenu();
  void showOptions();

	void focus(SceneObject*);
	void showHierarchy(std::list<cg::Reference<SceneObject>>::const_iterator itSceneObj);
  void hierarchyWindow();
  void inspectorWindow();
  void assetsWindow();
  void editorView();
  void sceneGui();
  void sceneObjectGui();
  void objectGui();
  void editorViewGui();
  void inspectPrimitive(Primitive&);
  void inspectCamera(Camera&);
  void addComponentButton(SceneObject&);

  void drawPrimitive(Primitive&);
	void drawCamera_and_Axes(const Reference<SceneObject>&);
	void renderAll(Camera* vp);
	void renderScene();
	void preview();
	void renderEditorView();

  bool windowResizeEvent(int, int) override;
  bool keyInputEvent(int, int, int) override;
  bool scrollEvent(double, double) override;
  bool mouseButtonInputEvent(int, int, int) override;
  bool mouseMoveEvent(double, double) override;

  static void buildDefaultMeshes();

}; // P2

#endif // __P2_h
