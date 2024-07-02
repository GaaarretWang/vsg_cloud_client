#include "matWindow.h"

class InteractionHandler : public vsg::Inherit<vsg::Visitor, InteractionHandler>
{
public:
    bool have_node = false; // false when the second widget is empty
    treeNode preNode;       //last selected node
    treeNode curNode;       //current selected node
    bool editable = false;  //perform intersection only if editable

    CADMesh* mesh;
    vsg::ref_ptr<vsg::Camera> mainCamera;
    vsg::ref_ptr<vsg::Camera> secondCamera;
    vsgQt::Window* mainWindow;
    vsgQt::Window* secondWindow;
    vsg::ref_ptr<vsg::Group> mainScenegraph;
    vsg::ref_ptr<vsg::Group> secondScenegraph;
    vsg::ref_ptr<vsg::View> secondView;
    vsg::ref_ptr<vsgQt::Viewer> mainViewer;
    vsg::ref_ptr<vsgQt::Viewer> secondViewer;
    QTextEdit* info;

    InteractionHandler(vsg::ref_ptr<vsg::Camera>& in_mainCamera,
                       vsg::ref_ptr<vsg::Camera>& in_secondCamera,
                       vsgQt::Window*& in_mainWindow,
                       vsgQt::Window*& in_secondWindow,
                       vsg::ref_ptr<vsg::Group>& in_scenegraph_1,
                       vsg::ref_ptr<vsg::Group>& in_scenegraph_2,
                       vsg::ref_ptr<vsg::View> in_view,
                       vsg::ref_ptr<vsgQt::Viewer>& in_viewer_1,
                       vsg::ref_ptr<vsgQt::Viewer>& in_viewer_2,
                       CADMesh& in_mesh,
                       QTextEdit*& in_info)
    {
        mainCamera = in_mainCamera;
        secondCamera = in_secondCamera;

        mainWindow = in_mainWindow;
        secondWindow = in_secondWindow;

        mainScenegraph = in_scenegraph_1;
        secondScenegraph = in_scenegraph_2;

        secondView = in_view;
        mainViewer = in_viewer_1;
        secondViewer = in_viewer_2;

        mesh = &in_mesh;
        info = in_info;
    }

    void apply(vsg::ButtonPressEvent& buttonPressEvent) override
    {
        lastPointerEvent = &buttonPressEvent;

        if (buttonPressEvent.button == 1 && editable) // allow mouse intersection with Scene only if editable
        {
            intersection(buttonPressEvent);
        }
    }

    void apply(vsg::PointerEvent& pointerEvent) override
    {
        lastPointerEvent = &pointerEvent;
    }

    void intersection(vsg::PointerEvent& pointerEvent)
    {
        //set up intersector for mouse intersection with the scene
        auto intersector = vsg::LineSegmentIntersector::create(*mainCamera, pointerEvent.x, pointerEvent.y);
        mainScenegraph->accept(*intersector);
        std::cout << "intersection(" << pointerEvent.x << ", " << pointerEvent.y << ") " << intersector->intersections.size() << ")" << std::endl;

        if (intersector->intersections.empty()) return;

        // sort the intersections front to back
        std::sort(intersector->intersections.begin(), intersector->intersections.end(), [](auto& lhs, auto& rhs) { return lhs->ratio < rhs->ratio; });
        auto& firstIntersection = intersector->intersections.front();

        vsg::Node* node_this = nullptr;
        vsg::Node* node_next = nullptr; //used for finding the last MatrixTransform node

        // traverse node path of the first intersection point to find out witch part of the model is selected
        for (auto& node : firstIntersection->nodePath)
        {
            node_this = node_next;
            node_next = const_cast<vsg::Node*>(node);

            //if node_next is a stateGroup, then node_this is the last MatrixTransform node
            if (node_next->className() == "vsg::StateGroup")
            {
                vsg::MatrixTransform* temp = dynamic_cast<vsg::MatrixTransform*>(node_this);
                vsg::ref_ptr<vsg::MatrixTransform> smartPtr(temp);

                //update preNode and curNode
                preNode = curNode;
                curNode = mesh->repaint(smartPtr, 1); // make curNode highlighted color

                if (preNode.transform && preNode.transform != curNode.transform)
                    mesh->repaint(preNode.transform, 0); // make preNode normal color

                mainViewer->compile();

                // show message on info widget
                message m;
                m.id = curNode.id;
                m.x = curNode.position.x;
                m.y = curNode.position.y;
                m.z = curNode.position.z;
                info->setText(QString::fromLocal8Bit(m.outputString().c_str()));

                if (!have_node)
                {
                    secondScenegraph->addChild(mesh->presentSingleNode(curNode.id));
                    have_node = true;
                }
                else
                {
                    for (auto& item : secondScenegraph->children)
                    {
                        if (item->className() == "vsg::MatrixTransform")
                        {
                            item = mesh->presentSingleNode(curNode.id);
                            break;
                        }
                    }
                }

                secondViewer->compile();
            }
        }

        lastIntersection = intersector->intersections.front();
    }

    void select(std::string str)
    {
        //update preNode and curNode
        preNode = curNode;
        curNode = mesh->repaint(str, 1); // make curNode highlighted color

        if (preNode.transform && preNode.transform != curNode.transform)
            mesh->repaint(preNode.transform, 0); // make preNode normal color

        //show message on info widget
        message m;
        m.id = str;
        m.id = curNode.id;
        m.x = curNode.position.x;
        m.y = curNode.position.y;
        m.z = curNode.position.z;
        info->setText(QString::fromLocal8Bit(m.outputString().c_str()));

        mainViewer->compile();
    }

protected:
    vsg::ref_ptr<vsg::PointerEvent> lastPointerEvent;
    vsg::ref_ptr<vsg::LineSegmentIntersector::Intersection> lastIntersection;
};
