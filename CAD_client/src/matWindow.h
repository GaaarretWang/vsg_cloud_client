#include "CADMesh.h"
#include <ui_matWindow.h>
#pragma once

class matWindow : public QMainWindow
{
    Q_OBJECT

public:
    matWindow(vsg::ref_ptr<vsg::WindowTraits> in_traits,
              CADMesh& in_mesh,
              vsg::ref_ptr<vsgQt::Viewer>& in_viewer,
              QWidget* parent);

    ~matWindow();

private slots:
    void on_Action_apply();

private:
    void createVsgMatWindow(QWindow* parent);

    void doMaterialUpdate(QTreeWidgetItem* item); // update material for sphere
    void materialUpdate(MaterialObj mat, std::string mode);

    void materialInit(); // set up materials

    CADMesh* mesh;

    //qt
    Ui::matWindow ui;

    //vsg
    vsg::ref_ptr<vsg::WindowTraits> traits = vsg::WindowTraits::create();
    vsg::ref_ptr<vsg::Group> scene = vsg::Group::create();
    vsg::ref_ptr<vsg::View> view = vsg::View::create();
    vsg::ref_ptr<vsgQt::Viewer> viewer = vsgQt::Viewer::create();
    vsg::ref_ptr<vsg::Camera> camera;

    //vsgqt
    vsgQt::Window* window;
    vsg::ref_ptr<vsgQt::Viewer> viewer_mainWindow;

    //material related
    std::unordered_map<std::string, int> indexPtr_phong;
    std::map<int, std::string> namePtr_phong;
    int index = 0;
    MaterialObj materials_phong[24];
    MaterialObj currentMat;

    vsg::ref_ptr<vsg::vec3Array> vertices;
    vsg::ref_ptr<vsg::vec3Array> normals;
    vsg::ref_ptr<vsg::vec2Array> texcoords;
    vsg::ref_ptr<vsg::ushortArray> indices;

    vsg::DataList vertexArrays;
    vsg::ref_ptr<vsg::ShaderSet> shaderSet_phong;
    vsg::ref_ptr<vsg::GraphicsPipelineConfigurator> graphicsPipelineConfig;
    vsg::ref_ptr<vsg::StateGroup> stateGroup;
};
