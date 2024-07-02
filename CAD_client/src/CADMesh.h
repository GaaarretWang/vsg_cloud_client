#pragma once
//fb loading
#include "renderGeo_generated.h"

//qt
#include <QtWidgets>

//vsg
#include <vsg/all.h>

//vsgqt
//#include <vsgQt/KeyboardMap.h>
#include <vsgQt/Viewer.h>
#include <vsgQt/Window.h>

//c++
#include <unordered_set>
#include <iostream>

struct MaterialObj
{
    // Structure holding the material
    std::string name = "";
    vsg::ref_ptr<vsg::vec4Value> color;
    vsg::vec4 ambient = vsg::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    vsg::vec4 diffuse = vsg::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    vsg::vec4 specular = vsg::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    vsg::vec4 emission = vsg::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    float shininess = 0.0f;
};

struct TinyModelVertex
{
    TinyModelVertex()
    {
        pos = vsg::vec3(0.0f, 0.0f, 0.0f);
        uv = vsg::vec2(0.0f, 0.0f);
        normal = vsg::vec3(0.0f, 0.0f, 0.0f);
        tangent = vsg::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    };
    TinyModelVertex(vsg::vec3 _pos, vsg::vec2 _uv, vsg::vec3 _normal, vsg::vec4 _tangent)
    {
        pos = _pos;
        uv = _uv;
        normal = _normal;
        tangent = _tangent;
    }
    vsg::vec3 pos;
    vsg::vec2 uv;
    vsg::vec3 normal;
    vsg::vec4 tangent;

    bool operator==(const TinyModelVertex& other) const
    {
        return pos == other.pos && uv == other.uv && normal == other.normal;
    }
};

struct treeNode
{
    std::string id; // string id of the node
    vsg::mat4 originalMatrix;
    vsg::dvec3 position; // node's world coordinate

    std::vector<std::string> kids;// children's id 

    const RenderFlatBuffer::Instance* instance;
    vsg::ref_ptr<vsg::StateGroup> stateGroup;
    vsg::ref_ptr<vsg::GraphicsPipelineConfigurator> pipeline_1; // normal pipeline
    vsg::ref_ptr<vsg::GraphicsPipelineConfigurator> pipeline_2; // highlighted pipeline
    vsg::ref_ptr<vsg::MatrixTransform> transform;
    
    vsg::dbox bounds; // aabb bounding box
};

struct message
{
    std::string id;
    double x = 0.0, y = 0.0, z = 0.0;

    std::string outputString()
    {
        std::string temp = "";
        temp += ("零件编号：" + id + "\n");
        
        temp += ("坐标：（" + std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(z) + "）\n");

        return temp;
    }
};

namespace std
{
    template<typename T>
    struct hash<vsg::t_vec2<T>>
    {
        std::size_t operator()(const vsg::t_vec2<T>& vec) const noexcept
        {
            std::hash<T> hasher;
            std::size_t seed = 0;
            for (size_t i = 0; i < 2; ++i)
            {
                seed ^= hasher(vec[i]) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            return seed;
        }
    };
    template<typename T>
    struct hash<vsg::t_vec3<T>>
    {
        std::size_t operator()(const vsg::t_vec3<T>& vec) const noexcept
        {
            std::hash<T> hasher;
            std::size_t seed = 0;
            for (size_t i = 0; i < 3; ++i)
            {
                seed ^= hasher(vec[i]) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            return seed;
        }
    };
    template<typename T>
    struct hash<vsg::t_vec4<T>>
    {
        std::size_t operator()(const vsg::t_vec4<T>& vec) const noexcept
        {
            std::hash<T> hasher;
            std::size_t seed = 0;
            for (size_t i = 0; i < 4; ++i)
            {
                seed ^= hasher(vec[i]) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            return seed;
        }
    };
    template<>
    struct hash<TinyModelVertex>
    {
        size_t operator()(TinyModelVertex const& vertex) const
        {
            return ((hash<vsg::vec3>()(vertex.pos) ^
                (hash<vsg::vec3>()(vertex.normal) << 1)) >>
                1) ^
                (hash<vsg::vec2>()(vertex.uv) << 1);
        }
    };
} // namespace std

class CADMesh
{
private:
    template<typename T>
    vsg::vec3 toVec3(const flatbuffers::Vector<T>* flat_vector, int begin = 0);
    vsg::vec3 toVec3(const flatbuffers::String* string_vector);
    template<typename T>
    vsg::vec2 toVec2(const flatbuffers::Vector<T>* flat_vector, int begin = 0);

    std::vector<MaterialObj> mMaterials{};
    std::vector<std::string> mTextures{};
    std::vector<uint32_t> mMatIndex{};
    std::unordered_map<std::string, uint32_t> uniqueMaterials; //pos,normal,uv...

    std::unordered_map<std::string, uint32_t> protoIndex;
    std::unordered_map<std::string, uint32_t> protoTriangleNum;
    std::unordered_map<int, uint32_t> materialIndex;
    std::unordered_map<std::string, int> instanceIndex;     //flatbuffer instanceID, i

    std::unordered_map<std::string, treeNode> nodePtr;        //记录ID对应的节点
    std::map<vsg::ref_ptr<vsg::MatrixTransform>, std::string> idPtr;          //记录ID对应的节点

    std::unordered_set<std::string> added;           //记录添加到树中的节点
    flatbuffers::FlatBufferBuilder builder_out;

    //default and highlighted material
    vsg::ref_ptr<vsg::vec4Value> default_color;
    vsg::ref_ptr<vsg::PhongMaterialValue> default_material;

    vsg::ref_ptr<vsg::vec4Value> highlighted_color;
    vsg::ref_ptr<vsg::PhongMaterialValue> highlighted_material;

    //shader settings
    vsg::ref_ptr<vsg::ShaderSet> shader;
    const RenderFlatBuffer::RenderFlatBufferDoc* renderFlatBuffer;


    QTreeWidgetItem** nodeList;
    int nodeIndex;

public:
    int nodenumber = 0;

    std::vector<vsg::ref_ptr<vsg::vec3Array>> verticesVector;
    std::vector<vsg::ref_ptr<vsg::vec3Array>> normalsVector;
    std::vector<vsg::ref_ptr<vsg::vec2Array>> coordinatesVector;
    std::vector<vsg::ref_ptr<vsg::uintArray>> indicesVector;

    //get tree node acoording to id
    treeNode getNode(std::string str) 
    {
        return nodePtr[str];
    }

    // load shader
    vsg::ref_ptr<vsg::ShaderSet> buildShader();
    
    // load .fb file
    void buildnode(const vsg::GeometryInfo& info, const vsg::StateInfo& stateInfo, const std::string& path, bool fullNormal, vsg::ref_ptr<vsg::Group> scene);
    
    // present explode effect
    void explode();
    
    // recover from explode effect
    void recover();
    
    // repaint one part (state 0: normal  state 1: highlighted)
    treeNode repaint(vsg::ref_ptr<vsg::MatrixTransform> selectedNode, int state);
    
    // repaint one part (state 0: normal  state 1: highlighted)
    treeNode repaint(std::string id_selected, int state);

    // repaint the whole model using new material
    void repaintAll(treeNode& node, MaterialObj& mat);
    
    // create a new selected part of the CAD model
    vsg::ref_ptr<vsg::MatrixTransform> presentSingleNode(std::string id_selected);

    // load model tree structure for tree widget
    void presentTreeforQt(QTreeWidgetItem* root);
    void buildQTree(treeNode node);

    // calculate position for every part
    void positionCalculate(treeNode &node, vsg::dmat4 matrix);

    //加载模型
    void buildnode(const std::string& path, bool fullNormal, vsg::ref_ptr<vsg::Group> scene, vsg::ref_ptr<vsg::ShaderSet> shader);
    void buildIntgNode(vsg::ref_ptr<vsg::Group> scene, vsg::ref_ptr<vsg::ShaderSet> shader, vsg::ref_ptr<vsg::ImageInfo>* imageInfos);
    void buildCloudIntgNode(vsg::ref_ptr<vsg::Group> scene, vsg::ref_ptr<vsg::ShaderSet> shader, vsg::ref_ptr<vsg::Data> image);
    void buildPlaneNode(vsg::ref_ptr<vsg::Group> scene, vsg::ref_ptr<vsg::ShaderSet> shader);

    //加载着色器
    vsg::ref_ptr<vsg::ShaderSet> buildShader(std::string vert, std::string frag);
    vsg::ref_ptr<vsg::ShaderSet> buildIntgShader(std::string vert, std::string frag);
};

