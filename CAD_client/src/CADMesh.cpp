#include "CADMesh.h"


template<typename T>
vsg::vec3 CADMesh::toVec3(const flatbuffers::Vector<T>* flat_vector, int begin)
{
    // return vsg::vec3(flat_vector->Get(begin) / 1000, flat_vector->Get(begin + 1) / 1000, flat_vector->Get(begin + 2) / 1000);
    return vsg::vec3(flat_vector->Get(begin), flat_vector->Get(begin + 1), flat_vector->Get(begin + 2));
}

template<typename T>
vsg::vec2 CADMesh::toVec2(const flatbuffers::Vector<T>* flat_vector, int begin)
{
    return vsg::vec2(flat_vector->Get(begin), flat_vector->Get(begin + 1));
}

vsg::vec3 CADMesh::toVec3(const flatbuffers::String* string_vector)
{
    if (string_vector->str().size() == 0)
        return vsg::vec3(0.f, 0.f, 0.f);
    vsg::vec3 vector3;
    std::string word = string_vector->str();
    int num;
    for (int i = 0; i < 3; i++)
    {
        std::stringstream converter;
        converter << std::hex << word.substr(i * 2 + 1, 2);
        converter >> num;
        vector3[i] = num / 255.0;
    }

    return vector3;
}

vsg::ref_ptr<vsg::ShaderSet> CADMesh::buildShader()
{
    //创建phong着色模型的shaderset
    vsg::ref_ptr<vsg::ShaderSet> shaderSet;
    auto options = vsg::Options::create();
    auto phong_vertexShader = vsg::read_cast<vsg::ShaderStage>("../data/shaders/standard.vert", options);
    auto phong_fragShader = vsg::read_cast<vsg::ShaderStage>("../data/shaders/standard_phong.frag", options);
    shaderSet = vsg::createPhongShaderSet();
    if (shaderSet && phong_vertexShader && phong_fragShader)
    {
        // replace shaders
        shaderSet->stages.clear();
        shaderSet->stages.push_back(phong_vertexShader);
        shaderSet->stages.push_back(phong_fragShader);

        // clear prebuilt variants
        shaderSet->variants.clear();
        std::cout << "Replaced phong shader." << std::endl;
    }
    else
    {
        std::cout << "Could not create shaders." << std::endl;
    }

    return shaderSet;

}

void CADMesh::buildnode(const vsg::GeometryInfo& info, const vsg::StateInfo& stateInfo, const std::string& path, bool fullNormal, vsg::ref_ptr<vsg::Group> scene)
{
    //创建着色器
    shader = buildShader();

    default_color = vsg::vec4Value::create(vsg::vec4{ 0.9, 0.9, 0.9, 1.0 });
    default_material = vsg::PhongMaterialValue::create();
    default_material->value().ambient.set(0.9, 0.9, 0.9, 1.0);
    default_material->value().diffuse.set(0.55, 0.55, 0.55, 1.0);
    default_material->value().specular.set(0.7, 0.7, 0.7, 1.0);
    //std::cout << mat->value().shininess;//默认值为100
    default_material->value().shininess = 25;

    highlighted_color = vsg::vec4Value::create(vsg::vec4{ 0.85, 0.0, 0.0, 1.0 });
    highlighted_material = vsg::PhongMaterialValue::create();
    highlighted_material->value().ambient.set(0.85, 0.0, 0.0, 1.0);
    highlighted_material->value().diffuse.set(0.7, 0.4, 0.4, 1.0);
    highlighted_material->value().specular.set(0.7, 0.04, 0.04, 1.0);
    //std::cout << mat->value().shininess;//默认值为100
    highlighted_material->value().shininess = 7.8125;


    uint8_t* buffer_data;
    int buffer_size;
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Failed to open the file." << std::endl;
    }
    file.seekg(0, std::ios::end);
    std::streampos fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<uint8_t> buffer(fileSize);
    file.read(reinterpret_cast<char*>(buffer.data()), fileSize);
    file.close();
    buffer_data = buffer.data();
    buffer_size = fileSize;

    builder_out.PushFlatBuffer(buffer_data, buffer_size);
    renderFlatBuffer = RenderFlatBuffer::GetRenderFlatBufferDoc(builder_out.GetBufferPointer());
    int emptyProtoNum = 0;

    for (int p = 0; p < renderFlatBuffer->ProtoData()->size(); p++)
    {
        //std::cout << "proto" << p << ":" << std::endl;
        const RenderFlatBuffer::Proto* protofbs = renderFlatBuffer->ProtoData()->Get(p);
        if (protofbs->Models()->size() == 0)
        {
            emptyProtoNum++;
            continue;
        }
        protoIndex[protofbs->ProtoID()->str()] = static_cast<uint32_t>(p - emptyProtoNum);
        std::unordered_map<TinyModelVertex, uint32_t> uniqueVertices;
        std::vector<TinyModelVertex> mVertices{};
        std::vector<uint32_t> mIndices{};

        for (int m = 0; m < protofbs->Models()->size(); m++)
        {
            const RenderFlatBuffer::Model* modelfbs = protofbs->Models()->Get(m);
            auto modelGeo = modelfbs->Geo();
            //std::cout << "modelfbs->Type()->str()" << modelfbs->Type()->str() << std::endl;
            if (modelfbs->Type()->str() == "mesh")
            {
                if (fullNormal) {
                    for (int i = 0; i < modelGeo->Index()->size(); i += 1)
                    {
                        TinyModelVertex vertex;
                        int index = modelGeo->Index()->Get(i);
                        vertex.pos = toVec3(modelGeo->Position(), index * 3);
                        vertex.normal = toVec3(modelGeo->Normal(), index * 3);
                        // vertex.uv = toVec2(modelGeo->UV(), index * 2);


                        if (uniqueVertices.count(vertex) == 0) //if unique
                        {
                            uniqueVertices[vertex] = static_cast<uint32_t>(mVertices.size());
                            mVertices.push_back(vertex);
                        }
                        mIndices.push_back(uniqueVertices[vertex]);
                    }
                }
                else {
                    for (int i = 0; i < modelGeo->Index()->size(); i += 3)
                    {
                        std::vector<TinyModelVertex> vertex = {};
                        for (int j = 0; j < 3; j++)
                        {
                            TinyModelVertex v;
                            int index = modelGeo->Index()->Get(i + j);
                            // std::cout <<index << "\t";
                            v.pos = toVec3(modelGeo->Position(), index * 3);
                            // vertex.uv = toVec2(modelGeo->UV(), index * 2);
                            vertex.push_back(v);
                        }
                        vsg::vec3 n = vsg::normalize(vsg::cross((vertex[1].pos - vertex[0].pos), (vertex[2].pos - vertex[0].pos)));
                        vertex[0].normal = n;
                        vertex[1].normal = n;
                        vertex[2].normal = n;

                        for (int j = 0; j < 3; j++)
                        {
                            if (uniqueVertices.count(vertex[j]) == 0) //if unique
                            {
                                uniqueVertices[vertex[j]] = static_cast<uint32_t>(mVertices.size());
                                mVertices.push_back(vertex[j]);
                            }
                            mIndices.push_back(uniqueVertices[vertex[j]]);
                        }
                    }
                }
            }
        }
        int Verticesnumber = mVertices.size();  //顶点、法向个数
        int Indicesnumber = mIndices.size(); //索引个数
        protoTriangleNum[protofbs->ProtoID()->str()] = static_cast<uint32_t>(mIndices.size() / 3);

        vsg::ref_ptr<vsg::vec3Array> vertices = vsg::vec3Array::create(Verticesnumber);
        vsg::ref_ptr<vsg::vec3Array> normals = vsg::vec3Array::create(Verticesnumber);
        //coordinates = vsg::vec2Array::create(Nodenumber);
        vsg::ref_ptr<vsg::uintArray> indices = vsg::uintArray::create(Indicesnumber);

        //读取顶点
        for (int i = 0; i < Verticesnumber; i++)
        {
            vertices->at(i) = vsg::vec3(mVertices[i].pos);
            normals->at(i) = vsg::vec3(mVertices[i].normal);
        }
        //读取索引
        for (int i = 0; i < Indicesnumber; i++)
        {
            indices->at(i) = mIndices[i];
        }
        verticesVector.push_back(vertices);
        normalsVector.push_back(normals);
        indicesVector.push_back(indices);
    }

    for (int i = 0; i < renderFlatBuffer->Bom()->size(); i++)
    {
        auto instance = renderFlatBuffer->Bom()->Get(i);
        instanceIndex[instance->InstanceID()->str()] = i;
    }

    vsg::mat4 matrix = vsg::mat4(1.0f);
    treeNode top;
    top.transform = vsg::MatrixTransform::create();
    top.transform->matrix = matrix;
    top.originalMatrix = matrix;
    top.id = "";
    nodePtr[""] = top;
    idPtr[top.transform] = "";
    nodenumber++;

    //创建树的所有节点
    for (int i = 0; i < renderFlatBuffer->Bom()->size(); i++)
    {
        auto instance = renderFlatBuffer->Bom()->Get(i);
        auto protoId = instance->ProtoID()->str();
        auto currentId = instance->InstanceID()->str();
        if (protoIndex.find(protoId) != protoIndex.end())
        {
            //default pipeline
            auto graphicsPipelineConfig = vsg::GraphicsPipelineConfigurator::create(shader);
            vsg::DataList vertexArrays;
            auto drawCommands = vsg::Commands::create();

            //创建纹理或遮罩
            bool addTexture = 0;
            auto options = vsg::Options::create();
            vsg::Path textureFile("../data/textures/lz.vsgb");
            if (currentId == "52_74" && textureFile && addTexture)
                //if (textureFile && addTexture)
            {
                auto textureData = vsg::read_cast<vsg::Data>(textureFile, options);
                if (!textureData)
                {
                    std::cout << "Could not read texture file : " << textureFile << std::endl;
                }
                graphicsPipelineConfig->assignTexture("diffuseMap", textureData);
            }

            //传入模型几何参数
            graphicsPipelineConfig->assignArray(vertexArrays, "vsg_Vertex", VK_VERTEX_INPUT_RATE_VERTEX, verticesVector[protoIndex[instance->ProtoID()->str()]]);
            graphicsPipelineConfig->assignArray(vertexArrays, "vsg_Normal", VK_VERTEX_INPUT_RATE_VERTEX, normalsVector[protoIndex[instance->ProtoID()->str()]]);
            //graphicsPipelineConfig->assignArray(vertexArrays, "vsg_TexCoord0", VK_VERTEX_INPUT_RATE_VERTEX, texcoords);

            graphicsPipelineConfig->assignArray(vertexArrays, "vsg_Color", VK_VERTEX_INPUT_RATE_INSTANCE, default_color);
            graphicsPipelineConfig->assignDescriptor("material", default_material);
            //绑定索引
            drawCommands->addChild(vsg::BindVertexBuffers::create(graphicsPipelineConfig->baseAttributeBinding, vertexArrays));
            drawCommands->addChild(vsg::BindIndexBuffer::create(indicesVector[protoIndex[instance->ProtoID()->str()]]));
            drawCommands->addChild(vsg::DrawIndexed::create(indicesVector[protoIndex[instance->ProtoID()->str()]]->size(), 1, 0, 0, 0));

            graphicsPipelineConfig->init();
            auto stateGroup = vsg::StateGroup::create();
            graphicsPipelineConfig->copyTo(stateGroup);
            //stateGroup->add(graphicsPipelineConfig);
            stateGroup->addChild(drawCommands);

            treeNode node;
            node.transform = vsg::MatrixTransform::create();
            for (int m = 0; m < 4; m++)
                for (int n = 0; n < 3; n++)
                {
                    node.transform->matrix[m][n] = instance->Matrix()->Get(3 * m + n);
                    node.originalMatrix[m][n] = node.transform->matrix[m][n];
                }
            node.instance = instance;
            node.pipeline_1 = graphicsPipelineConfig;
            node.stateGroup = stateGroup;
            


            node.transform->addChild(node.stateGroup);
            node.id = currentId;


            //highlighted pipeline
            auto graphicsPipelineConfig2 = vsg::GraphicsPipelineConfigurator::create(shader);
            vsg::DataList vertexArrays2;
            //传入模型几何参数
            graphicsPipelineConfig2->assignArray(vertexArrays, "vsg_Vertex", VK_VERTEX_INPUT_RATE_VERTEX, verticesVector[protoIndex[instance->ProtoID()->str()]]);
            graphicsPipelineConfig2->assignArray(vertexArrays, "vsg_Normal", VK_VERTEX_INPUT_RATE_VERTEX, normalsVector[protoIndex[instance->ProtoID()->str()]]);
            //graphicsPipelineConfig2->assignArray(vertexArrays, "vsg_TexCoord0", VK_VERTEX_INPUT_RATE_VERTEX, texcoords);
            graphicsPipelineConfig2->assignArray(vertexArrays, "vsg_Color", VK_VERTEX_INPUT_RATE_INSTANCE, highlighted_color);
            graphicsPipelineConfig2->assignDescriptor("material", highlighted_material);

            graphicsPipelineConfig2->init();

            node.pipeline_2 = graphicsPipelineConfig2;



            //set up reflection
            nodePtr[currentId] = node;
            
            idPtr[node.transform] = currentId;
            std::cout << "渲染id: " << idPtr[node.transform] << std::endl;
            nodenumber++;

            //nodePtr[currentId].bounds = vsg::visit<vsg::ComputeBounds>(nodePtr[currentId].transform).bounds;
            //nodePtr[currentId].transform->bounds = nodePtr[currentId].bounds;

        }
        else
        {
            vsg::mat4 matrix = vsg::mat4(1.0f);
            for (int m = 0; m < 4; m++)
            {
                for (int n = 0; n < 3; n++)
                {
                    matrix[m][n] = instance->Matrix()->Get(3 * m + n);

                    std::cout << matrix[m][n] << " ";

                }

                std::cout << std::endl;
            }
                
            treeNode node;
            node.transform = vsg::MatrixTransform::create();
            node.transform->matrix = matrix;
            node.originalMatrix = matrix;
            node.id = currentId;

            nodePtr[currentId] = node;
            
            //std::cout << "node.transform: " << node.transform << "   ";
            idPtr[node.transform] = currentId;
            //std::cout << "id: " << idPtr[node.transform] << std::endl;


            nodenumber++;
        }
    }

    //处理树中节点的关系
    uint32_t totalTriangleNum = 0;
    for (int i = 0; i < renderFlatBuffer->Bom()->size(); i++)
    {
        auto instance = renderFlatBuffer->Bom()->Get(i);
        auto currentId = instance->InstanceID()->str();
        auto parentId = instance->ParentID()->str();
        auto protoId = instance->ProtoID()->str();
        if (protoIndex.find(protoId) != protoIndex.end())
        {
            totalTriangleNum += protoTriangleNum[protoId];

            while (true)
            {
                if (added.find(currentId) == added.end()) {
                    nodePtr[parentId].kids.push_back(currentId);
                    nodePtr[parentId].transform->addChild(nodePtr[currentId].transform);
                    added.insert(currentId);
                }
                if (parentId == "")
                    break;
                currentId = parentId;
                parentId = renderFlatBuffer->Bom()->Get(instanceIndex[currentId])->ParentID()->str();
            }
        }
    }

    std::cout << "totalTriangleNum = " << totalTriangleNum << std::endl;

    scene->addChild(nodePtr[""].transform);

    //initialize bounding box
    for (int i = 0; i < renderFlatBuffer->Bom()->size(); i++)
    {
        auto instance = renderFlatBuffer->Bom()->Get(i);
        auto currentId = instance->InstanceID()->str();
        nodePtr[currentId].bounds = vsg::visit<vsg::ComputeBounds>(nodePtr[currentId].transform).bounds;
        nodePtr[currentId].transform->bounds = nodePtr[currentId].bounds;

    }
    nodePtr[""].bounds = vsg::visit<vsg::ComputeBounds>(nodePtr[""].transform).bounds;
    nodePtr[""].transform->bounds = nodePtr[""].bounds;

    positionCalculate(nodePtr[""], nodePtr[""].transform->matrix);

}

void CADMesh::explode()
{
    auto renderFlatBuffer = RenderFlatBuffer::GetRenderFlatBufferDoc(builder_out.GetBufferPointer());
    for (int i = 0; i < renderFlatBuffer->Bom()->size(); i++)
    {
        auto instance = renderFlatBuffer->Bom()->Get(i);
        auto currentId = instance->InstanceID()->str();
        auto parentId = instance->ParentID()->str();

        vsg::dvec3 center((nodePtr[currentId].bounds.max + nodePtr[currentId].bounds.min) / 2.0);
        vsg::dvec3 parentCenter((nodePtr[parentId].bounds.max + nodePtr[parentId].bounds.min) / 2.0);

        auto distance = center - parentCenter;
        auto moveDistance = distance * 0.4;
        //std::cout << "moveDistance" << moveDistance << std::endl;
        nodePtr[currentId].transform->matrix = nodePtr[currentId].transform->matrix * vsg::translate(moveDistance);
    }
}

void CADMesh::recover()
{
    auto renderFlatBuffer = RenderFlatBuffer::GetRenderFlatBufferDoc(builder_out.GetBufferPointer());
    for (int i = 0; i < renderFlatBuffer->Bom()->size(); i++)
    {
        auto instance = renderFlatBuffer->Bom()->Get(i);
        auto currentId = instance->InstanceID()->str();
        auto parentId = instance->ParentID()->str();

        nodePtr[currentId].transform->matrix = nodePtr[currentId].originalMatrix;
    }
}

treeNode CADMesh::repaint(vsg::ref_ptr<vsg::MatrixTransform> selectedNode, int state)
{
    std::string id_selected;
    //treeNode node_selected;

    //读取交互信息对应的id和树节点
    id_selected = idPtr[selectedNode];

    if (!id_selected.empty())
    {
        if (state == 0)
        {
            std::cout << "取消上一个选中：" << id_selected << std::endl;
            nodePtr[id_selected].pipeline_1->copyTo(nodePtr[id_selected].stateGroup);

        }
        else if(state == 1)
        {
            std::cout << "选中：" << id_selected << std::endl;

            nodePtr[id_selected].pipeline_2->copyTo(nodePtr[id_selected].stateGroup);

        }
        //for (auto& sc : node_selected.stateGroup->stateCommands)
        //{
        //    if (auto bgp = sc->cast<vsg::BindGraphicsPipeline>())
        //    {
        //        std::cout << "进入1" << std::endl;
        //        for (auto& pipelineState : bgp->pipeline->pipelineStates)
        //        {
        //            if (auto rasterizationState = pipelineState.cast<vsg::RasterizationState>())
        //            {
        //                std::cout << "进入2" << std::endl;
        //                auto alternate_rasterizationState = vsg::RasterizationState::create(*rasterizationState);
        //                alternate_rasterizationState->polygonMode = VK_POLYGON_MODE_LINE;
        //                pipelineState = alternate_rasterizationState;
        //            }
        //            //if (auto colorBlendState = pipelineState.cast<vsg::ColorBlendState>())
        //            //{
        //            //    auto alternate_colorBlendState = vsg::RasterizationState::create(*colorBlendState);
        //            //    //alternate_colorBlendState;
        //            //    pipelineState = alternate_colorBlendState;
        //            //}
        //        }
        //    }
        //}

    }

    return nodePtr[id_selected];

}

treeNode CADMesh::repaint(std::string id_selected, int state)
{
    if (!id_selected.empty())
    {
        if (state == 0)
        {
            std::cout << "取消选中零件id：" << id_selected << "    ";
            std::cout << "对应树节点：" << nodePtr[id_selected].transform << std::endl;

            nodePtr[id_selected].pipeline_1->copyTo(nodePtr[id_selected].stateGroup);

        }
        else if (state == 1)
        {
            std::cout << "选中零件id：" << id_selected << "    ";
            std::cout << "对应树节点：" << nodePtr[id_selected].transform << std::endl;

            nodePtr[id_selected].pipeline_2->copyTo(nodePtr[id_selected].stateGroup);

        }
    }

    return nodePtr[id_selected];

}

vsg::ref_ptr<vsg::MatrixTransform> CADMesh::presentSingleNode(std::string id_selected)
{
    std::cout << "单独绘制零件的id：" << id_selected << "    ";

    ////执行重新绘制的代码----------------------------------------------------------------
    auto graphicsPipelineConfig = vsg::GraphicsPipelineConfigurator::create(shader);
    vsg::DataList vertexArrays;
    //传入模型几何参数
    graphicsPipelineConfig->assignArray(vertexArrays, "vsg_Vertex", VK_VERTEX_INPUT_RATE_VERTEX, verticesVector[protoIndex[nodePtr[id_selected].instance->ProtoID()->str()]]);
    graphicsPipelineConfig->assignArray(vertexArrays, "vsg_Normal", VK_VERTEX_INPUT_RATE_VERTEX, normalsVector[protoIndex[nodePtr[id_selected].instance->ProtoID()->str()]]);
    graphicsPipelineConfig->assignArray(vertexArrays, "vsg_Color", VK_VERTEX_INPUT_RATE_INSTANCE, default_color);
    graphicsPipelineConfig->assignDescriptor("material", default_material);
    graphicsPipelineConfig->init();
    //绑定索引
    auto drawCommands = vsg::Commands::create();
    drawCommands->addChild(vsg::BindVertexBuffers::create(graphicsPipelineConfig->baseAttributeBinding, vertexArrays));
    drawCommands->addChild(vsg::BindIndexBuffer::create(indicesVector[protoIndex[nodePtr[id_selected].instance->ProtoID()->str()]]));
    drawCommands->addChild(vsg::DrawIndexed::create(indicesVector[protoIndex[nodePtr[id_selected].instance->ProtoID()->str()]]->size(), 1, 0, 0, 0));



    auto stateGroup = vsg::StateGroup::create();
    graphicsPipelineConfig->copyTo(stateGroup);
    stateGroup->addChild(drawCommands);


    //transform->matrix = vsg::translate(-radius, 0.0, 0.0);
    auto transform = vsg::MatrixTransform::create();
    transform->matrix = vsg::scale(0.01, 0.01, 0.01);
    transform->addChild(stateGroup);

    return transform;



}

void CADMesh::presentTreeforQt(QTreeWidgetItem* root)
{
    nodeList = new QTreeWidgetItem * [nodenumber + 10];

    nodeIndex = 0;
    root->setText(0, QString::fromLocal8Bit("000"));
    nodeList[0] = root;

    buildQTree(nodePtr[""]);//传入根节点

}

void CADMesh::buildQTree(treeNode node)
{
    if (node.kids.size() != 0) // not leaf node
    {
        int fatherIndex = nodeIndex;

        for (auto& child_id : node.kids)
        {
            QTreeWidgetItem* temp = new QTreeWidgetItem();
            temp->setText(0, QString::fromLocal8Bit(child_id.c_str()));//set up a new node
            nodeList[fatherIndex]->addChild(temp);//add the new node to his father
            nodeList[++nodeIndex] = temp;//add the new node to list

            buildQTree(nodePtr[child_id]);
        }

    }
    else return;

}

void CADMesh::positionCalculate(treeNode &node, vsg::dmat4 matrix)
{
    if (node.kids.size() != 0) //not leaf node
    {
        for (auto& child_id : node.kids)
        {
            vsg::dmat4 newMatrix = matrix * nodePtr[child_id].transform->matrix;

            positionCalculate(nodePtr[child_id], newMatrix);
        }
    }
    else//叶节点
    {
        vsg::dmat4 temp = nodePtr[node.id].transform->matrix;
        nodePtr[node.id].transform->matrix = matrix;
        auto b = vsg::visit<vsg::ComputeBounds>(nodePtr[node.id].transform).bounds;
        node.position = (b.max + b.min) / 2.0;
        nodePtr[node.id].transform->matrix = temp;

        return;
    }
}

vsg::ref_ptr<vsg::ShaderSet> CADMesh::buildIntgShader(std::string vert, std::string frag)
{
    auto options = vsg::Options::create();
    auto phong_vertexShader = vsg::read_cast<vsg::ShaderStage>(vert, options);
    auto phong_fragShader = vsg::read_cast<vsg::ShaderStage>(frag, options);

#define VIEW_DESCRIPTOR_SET 0
#define MATERIAL_DESCRIPTOR_SET 1

    auto shaderSet = vsg::ShaderSet::create(vsg::ShaderStages{phong_vertexShader, phong_fragShader});

    shaderSet->addAttributeBinding("vsg_Vertex", "", 0, VK_FORMAT_R32G32B32_SFLOAT, vsg::vec3Array::create(1));
    shaderSet->addAttributeBinding("vsg_Normal", "", 1, VK_FORMAT_R32G32B32_SFLOAT, vsg::vec3Array::create(1));
    shaderSet->addAttributeBinding("vsg_TexCoord0", "", 2, VK_FORMAT_R32G32_SFLOAT, vsg::vec2Array::create(1));
    shaderSet->addAttributeBinding("vsg_Color", "", 3, VK_FORMAT_R32G32B32A32_SFLOAT, vsg::vec4Array::create(1));

    shaderSet->addAttributeBinding("vsg_position", "VSG_INSTANCE_POSITIONS", 4, VK_FORMAT_R32G32B32_SFLOAT, vsg::vec3Array::create(1));
    shaderSet->addAttributeBinding("vsg_position_scaleDistance", "VSG_BILLBOARD", 4, VK_FORMAT_R32G32B32A32_SFLOAT, vsg::vec4Array::create(1));

    shaderSet->addDescriptorBinding("displacementMap", "VSG_DISPLACEMENT_MAP", MATERIAL_DESCRIPTOR_SET, 6, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_VERTEX_BIT, vsg::floatArray2D::create(1, 1, vsg::Data::Properties{VK_FORMAT_R32_SFLOAT}));
    shaderSet->addDescriptorBinding("cadColor", "", MATERIAL_DESCRIPTOR_SET, 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, vsg::ubvec4Array2D::create(1, 1, vsg::Data::Properties{VK_FORMAT_R8G8B8A8_UNORM}));
    shaderSet->addDescriptorBinding("cadDepth", "", MATERIAL_DESCRIPTOR_SET, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, vsg::ubvec4Array2D::create(1, 1, vsg::Data::Properties{VK_FORMAT_D32_SFLOAT}));
    shaderSet->addDescriptorBinding("planeColor", "", MATERIAL_DESCRIPTOR_SET, 2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, vsg::vec3Array2D::create(1, 1, vsg::Data::Properties{VK_FORMAT_R8G8B8A8_UNORM}));
    shaderSet->addDescriptorBinding("planeDepth", "", MATERIAL_DESCRIPTOR_SET, 3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, vsg::vec4Array2D::create(1, 1, vsg::Data::Properties{VK_FORMAT_D32_SFLOAT}));
    shaderSet->addDescriptorBinding("shadowColor", "", MATERIAL_DESCRIPTOR_SET, 4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, vsg::vec4Array2D::create(1, 1, vsg::Data::Properties{VK_FORMAT_R8G8B8A8_UNORM}));
    shaderSet->addDescriptorBinding("shadowDepth", "", MATERIAL_DESCRIPTOR_SET, 5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, vsg::vec4Array2D::create(1, 1, vsg::Data::Properties{VK_FORMAT_D32_SFLOAT}));

    shaderSet->addDescriptorBinding("material", "", MATERIAL_DESCRIPTOR_SET, 10, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, vsg::PhongMaterialValue::create());

    shaderSet->addDescriptorBinding("lightData", "", VIEW_DESCRIPTOR_SET, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, vsg::vec4Array::create(64));
    shaderSet->addDescriptorBinding("viewportData", "", VIEW_DESCRIPTOR_SET, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, vsg::vec4Value::create(0, 0, 1280, 1024));
    shaderSet->addDescriptorBinding("shadowMaps", "", VIEW_DESCRIPTOR_SET, 2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, vsg::floatArray3D::create(1, 1, 1, vsg::Data::Properties{VK_FORMAT_R32_SFLOAT}));

    shaderSet->addPushConstantRange("pc", "", VK_SHADER_STAGE_VERTEX_BIT, 0, 128);

    shaderSet->optionalDefines = {"VSG_GREYSCALE_DIFFUSE_MAP", "VSG_TWO_SIDED_LIGHTING", "VSG_POINT_SPRITE"};

    shaderSet->definesArrayStates.push_back(vsg::DefinesArrayState{{"VSG_INSTANCE_POSITIONS", "VSG_DISPLACEMENT_MAP"}, vsg::PositionAndDisplacementMapArrayState::create()});
    shaderSet->definesArrayStates.push_back(vsg::DefinesArrayState{{"VSG_INSTANCE_POSITIONS"}, vsg::PositionArrayState::create()});
    shaderSet->definesArrayStates.push_back(vsg::DefinesArrayState{{"VSG_DISPLACEMENT_MAP"}, vsg::DisplacementMapArrayState::create()});
    shaderSet->definesArrayStates.push_back(vsg::DefinesArrayState{{"VSG_BILLBOARD"}, vsg::BillboardArrayState::create()});

    shaderSet->customDescriptorSetBindings.push_back(vsg::ViewDependentStateBinding::create(VIEW_DESCRIPTOR_SET));

    return shaderSet;
}

vsg::ref_ptr<vsg::ShaderSet> CADMesh::buildShader(std::string vert, std::string frag)
{
    //创建phong着色模型的shaderset
    vsg::ref_ptr<vsg::ShaderSet> shaderSet;
    auto options = vsg::Options::create();
    auto phong_vertexShader = vsg::read_cast<vsg::ShaderStage>(vert, options);
    auto phong_fragShader = vsg::read_cast<vsg::ShaderStage>(frag, options);
    shaderSet = vsg::createPhongShaderSet();

    if (shaderSet && phong_vertexShader && phong_fragShader)
    {
        bool shaderDebug = false;
        bool depthClamp = false;

        // replace shaders
        shaderSet->stages.clear();
        shaderSet->stages.push_back(phong_vertexShader);
        shaderSet->stages.push_back(phong_fragShader);
        if (shaderDebug)
        {
            shaderSet->optionalDefines.insert("SHADOWMAP_DEBUG");
            shaderSet->defaultShaderHints = vsg::ShaderCompileSettings::create();
            shaderSet->defaultShaderHints->defines.insert("SHADOWMAP_DEBUG");
        }
        if (depthClamp)
        {
            auto rasterizationState = vsg::RasterizationState::create();
            rasterizationState->depthClampEnable = VK_TRUE;
            shaderSet->defaultGraphicsPipelineStates.push_back(rasterizationState);
        }

        // clear prebuilt variants
        shaderSet->variants.clear();
        std::cout << "Replaced phong shader." << std::endl;
    }
    else
    {
        std::cout << "Could not create shaders." << std::endl;
    }

    return shaderSet;
}

void CADMesh::buildPlaneNode(vsg::ref_ptr<vsg::Group> scene, vsg::ref_ptr<vsg::ShaderSet> shader)
{
    //2. 设置材质参数
#if (1) //金属
    auto plane_colors = vsg::vec4Value::create(vsg::vec4{1.0, 1.0, 1.0, 1.0f});
    auto plane_mat = vsg::PhongMaterialValue::create();
    plane_mat->value().ambient.set(0.24725, 0.1995, 0.0745, 1.0f);
    plane_mat->value().diffuse.set(0.75164, 0.60648, 0.22648, 1.0f);
    plane_mat->value().specular.set(0.628281, 0.555802, 0.366065, 1.0f);
    //std::cout << mat->value().shininess;//默认值为100
    plane_mat->value().shininess = 100;

#elif (0) //塑料
    auto plane_colors = vsg::vec4Value::create(vsg::vec4{0, 0, 0, 1.0f});
    auto plane_mat = vsg::PhongMaterialValue::create();
    plane_mat->value().ambient.set(0, 0, 0, 1.0f);
    plane_mat->value().diffuse.set(0.55, 0.55, 0.55, 1.0f);
    plane_mat->value().specular.set(0.7, 0.7, 0.7, 1.0f);
    plane_mat->value().shininess = 25;

#elif (0) //纯色
    auto plane_colors = vsg::vec4Value::create(vsg::vec4{1, 1, 1, 1.0f});
    auto plane_mat = vsg::PhongMaterialValue::create();
    plane_mat->value().ambient.set(1, 0, 0, 1.0f);
    plane_mat->value().diffuse.set(1, 0, 0, 1.0f);
    plane_mat->value().specular.set(1, 0, 0, 1.0f);
    plane_mat->value().shininess = 0.25;
#endif

    //    //3. （手动）设置顶点、法向、材质坐标等信息，传入着色器
    float plane_length = 5000.0f;
    auto plane_vertices = vsg::vec3Array::create(
        {{-plane_length, -plane_length / 4, -plane_length},
         {plane_length, -plane_length / 4, -plane_length},
         {plane_length, -plane_length / 4, plane_length},
         {-plane_length, -plane_length / 4, plane_length}});

    auto plane_normals = vsg::vec3Array::create(
        {{0.0f, 1.0f, 0.0f},
         {0.0f, 1.0f, 0.0f},
         {0.0f, 1.0f, 0.0f},
         {0.0f, 1.0f, 0.0f}});

    auto plane_texcoords = vsg::vec2Array::create(
        {{0.0f, 0.0f},
         {1.0f, 0.0f},
         {1.0f, 1.0f},
         {0.0f, 1.0f}});

    auto plane_indices = vsg::ushortArray::create(
        {
            0,
            1,
            2,
            2,
            3,
            0,
        });

    //---------------------------------------阴影模拟------------------------------------------//
    //传入手写立方体的参数
    vsg::DataList plane_vertexArrays;
    auto plane_graphicsPipelineConfig = vsg::GraphicsPipelineConfigurator::create(shader); //渲染管线创建
    plane_graphicsPipelineConfig->assignArray(plane_vertexArrays, "vsg_Vertex", VK_VERTEX_INPUT_RATE_VERTEX, plane_vertices);
    plane_graphicsPipelineConfig->assignArray(plane_vertexArrays, "vsg_Normal", VK_VERTEX_INPUT_RATE_VERTEX, plane_normals);
    plane_graphicsPipelineConfig->assignArray(plane_vertexArrays, "vsg_TexCoord0", VK_VERTEX_INPUT_RATE_VERTEX, plane_texcoords);
    plane_graphicsPipelineConfig->assignArray(plane_vertexArrays, "vsg_Color", VK_VERTEX_INPUT_RATE_INSTANCE, plane_colors);
    plane_graphicsPipelineConfig->assignDescriptor("material", plane_mat);
    // 绑定索引
    auto plane_drawCommands = vsg::Commands::create();
    plane_drawCommands->addChild(vsg::BindVertexBuffers::create(plane_graphicsPipelineConfig->baseAttributeBinding, plane_vertexArrays));
    plane_drawCommands->addChild(vsg::BindIndexBuffer::create(plane_indices));
    //cout << mesh.indexes->size() << endl;
    plane_drawCommands->addChild(vsg::DrawIndexed::create(6, 1, 0, 0, 0));

    //双面显示
    auto rs = vsg::RasterizationState::create();
    rs->cullMode = VK_CULL_MODE_NONE;
    plane_graphicsPipelineConfig->pipelineStates.push_back(rs);

    plane_graphicsPipelineConfig->init();

    // create StateGroup as the root of the scene/command graph to hold the GraphicsPipeline, and binding of Descriptors to decorate the whole graph
    auto plane_stateGroup = vsg::StateGroup::create();
    plane_graphicsPipelineConfig->copyTo(plane_stateGroup);
    // set up model transformation node
    auto plane_transform = vsg::MatrixTransform::create(); //用于位置变换
    //plane_transform->matrix = vsg::translate(centre + vsg::dvec3(radius * (0.75 + scale * 0.5), 0.0, 0.0)) * vsg::rotate(vsg::radians(90.0), 1.0, 0.0, 0.0) * vsg::translate(-centre);
    //plane_transform->matrix = vsg::translate(1000.0, 0.0, 0.0);
    //transform->subgraphRequiresLocalFrustum = false;
    // add drawCommands to StateGroup
    plane_stateGroup->addChild(plane_drawCommands);
    plane_transform->addChild(plane_stateGroup);
    scene->addChild(plane_transform); //*******************************************
}

void CADMesh::buildIntgNode(vsg::ref_ptr<vsg::Group> scene, vsg::ref_ptr<vsg::ShaderSet> shader, vsg::ref_ptr<vsg::ImageInfo>* imageInfos)
{
#if (1) //金属
    auto colors = vsg::vec4Value::create(vsg::vec4{1.0, 1.0, 1.0, 1.0f});
    auto mat = vsg::PhongMaterialValue::create();
    mat->value().ambient.set(0.24725, 0.1995, 0.0745, 1.0f);
    mat->value().diffuse.set(0.75164, 0.60648, 0.22648, 1.0f);
    mat->value().specular.set(0.628281, 0.555802, 0.366065, 1.0f);
    //std::cout << mat->value().shininess;//默认值为100
    mat->value().shininess = 100;

#elif (0) //塑料
    auto colors = vsg::vec4Value::create(vsg::vec4{0, 0, 0, 1.0f});
    auto mat = vsg::PhongMaterialValue::create();
    mat->value().ambient.set(0, 0, 0, 1.0f);
    mat->value().diffuse.set(0.55, 0.55, 0.55, 1.0f);
    mat->value().specular.set(0.7, 0.7, 0.7, 1.0f);
    mat->value().shininess = 25;

#elif (0) //纯色
    auto colors = vsg::vec4Value::create(vsg::vec4{1, 1, 1, 1.0f});
    auto mat = vsg::PhongMaterialValue::create();
    mat->value().ambient.set(1, 0, 0, 1.0f);
    mat->value().diffuse.set(1, 0, 0, 1.0f);
    mat->value().specular.set(1, 0, 0, 1.0f);
    mat->value().shininess = 0.25;
#endif

    //    //3. （手动）设置顶点、法向、材质坐标等信息，传入着色器
    float length = 5000.0f;
    auto vertices = vsg::vec3Array::create(
        {{-length, -length, -length / 10},
         {length, -length, -length / 10},
         {length, length, -length / 10},
         {-length, length, -length / 10}});

    auto normals = vsg::vec3Array::create(
        {{0.0f, 0.0f, 1.0f},
         {0.0f, 0.0f, 1.0f},
         {0.0f, 0.0f, 1.0f},
         {0.0f, 0.0f, 1.0f}});

    auto texcoords = vsg::vec2Array::create(
        {{0.0f, 0.0f},
         {1.0f, 0.0f},
         {1.0f, 1.0f},
         {0.0f, 1.0f}});

    auto indices = vsg::ushortArray::create(
        {
            0,
            1,
            2,
            2,
            3,
            0,
        });

    vsg::DataList Env_vertexArrays;
    auto Env_graphicsPipelineConfig = vsg::GraphicsPipelineConfigurator::create(shader); //渲染管线创建
    vsg::ImageInfoList cadColor = {imageInfos[0]};
    vsg::ImageInfoList cadDepth = {imageInfos[1]};
    vsg::ImageInfoList planeColor = {imageInfos[2]};
    vsg::ImageInfoList planeDepth = {imageInfos[3]};
    vsg::ImageInfoList shadowColor = {imageInfos[4]};
    vsg::ImageInfoList shadowDepth = {imageInfos[5]};

    Env_graphicsPipelineConfig->assignTexture("cadColor", cadColor);
    Env_graphicsPipelineConfig->assignTexture("cadDepth", cadDepth);
    Env_graphicsPipelineConfig->assignTexture("planeColor", planeColor);
    Env_graphicsPipelineConfig->assignTexture("planeDepth", planeDepth);
    Env_graphicsPipelineConfig->assignTexture("shadowColor", shadowColor);
    Env_graphicsPipelineConfig->assignTexture("shadowDepth", shadowDepth);

    Env_graphicsPipelineConfig->assignArray(Env_vertexArrays, "vsg_Vertex", VK_VERTEX_INPUT_RATE_VERTEX, vertices);
    Env_graphicsPipelineConfig->assignArray(Env_vertexArrays, "vsg_Normal", VK_VERTEX_INPUT_RATE_VERTEX, normals);
    Env_graphicsPipelineConfig->assignArray(Env_vertexArrays, "vsg_TexCoord0", VK_VERTEX_INPUT_RATE_VERTEX, texcoords);
    Env_graphicsPipelineConfig->assignArray(Env_vertexArrays, "vsg_Color", VK_VERTEX_INPUT_RATE_INSTANCE, colors);
    Env_graphicsPipelineConfig->assignDescriptor("material", mat);

    //Env_graphicsPipelineConfig->assignTexture("", images[1]->data);
    //Env_graphicsPipelineConfig->assignTexture("", images[2]->data);
    //Env_graphicsPipelineConfig->assignTexture("", images[3]->data);
    //Env_graphicsPipelineConfig->assignTexture("", images[4]->data);
    // 绑定索引
    auto Env_drawCommands = vsg::Commands::create();
    Env_drawCommands->addChild(vsg::BindVertexBuffers::create(Env_graphicsPipelineConfig->baseAttributeBinding, Env_vertexArrays));
    Env_drawCommands->addChild(vsg::BindIndexBuffer::create(indices));
    //cout << mesh.indexes->size() << endl;
    Env_drawCommands->addChild(vsg::DrawIndexed::create(6, 1, 0, 0, 0));

    //双面显示
    auto Env_rs = vsg::RasterizationState::create();
    Env_rs->cullMode = VK_CULL_MODE_NONE;
    Env_graphicsPipelineConfig->pipelineStates.push_back(Env_rs);

    Env_graphicsPipelineConfig->init();

    // create StateGroup as the root of the scene/command graph to hold the GraphicsPipeline, and binding of Descriptors to decorate the whole graph
    auto Env_stateGroup = vsg::StateGroup::create();
    Env_graphicsPipelineConfig->copyTo(Env_stateGroup);
    // set up model transformation node
    auto Env_transform = vsg::MatrixTransform::create(); //用于位置变换

    //transform->subgraphRequiresLocalFrustum = false;
    // add drawCommands to StateGroup
    Env_stateGroup->addChild(Env_drawCommands);
    Env_transform->addChild(Env_stateGroup);
    scene->addChild(Env_transform);
}

void CADMesh::buildCloudIntgNode(vsg::ref_ptr<vsg::Group> scene, vsg::ref_ptr<vsg::ShaderSet> shader, vsg::ref_ptr<vsg::Data> image)
{
#if (1) //金属
    auto colors = vsg::vec4Value::create(vsg::vec4{1.0, 1.0, 1.0, 1.0f});
    auto mat = vsg::PhongMaterialValue::create();
    mat->value().ambient.set(0.24725, 0.1995, 0.0745, 1.0f);
    mat->value().diffuse.set(0.75164, 0.60648, 0.22648, 1.0f);
    mat->value().specular.set(0.628281, 0.555802, 0.366065, 1.0f);
    //std::cout << mat->value().shininess;//默认值为100
    mat->value().shininess = 100;

#elif (0) //塑料
    auto colors = vsg::vec4Value::create(vsg::vec4{0, 0, 0, 1.0f});
    auto mat = vsg::PhongMaterialValue::create();
    mat->value().ambient.set(0, 0, 0, 1.0f);
    mat->value().diffuse.set(0.55, 0.55, 0.55, 1.0f);
    mat->value().specular.set(0.7, 0.7, 0.7, 1.0f);
    mat->value().shininess = 25;

#elif (0) //纯色
    auto colors = vsg::vec4Value::create(vsg::vec4{1, 1, 1, 1.0f});
    auto mat = vsg::PhongMaterialValue::create();
    mat->value().ambient.set(1, 0, 0, 1.0f);
    mat->value().diffuse.set(1, 0, 0, 1.0f);
    mat->value().specular.set(1, 0, 0, 1.0f);
    mat->value().shininess = 0.25;
#endif

    //    //3. （手动）设置顶点、法向、材质坐标等信息，传入着色器
    float length = 5000.0f;
    auto vertices = vsg::vec3Array::create(
        {{-length, -length, -length / 10},
         {length, -length, -length / 10},
         {length, length, -length / 10},
         {-length, length, -length / 10}});

    auto normals = vsg::vec3Array::create(
        {{0.0f, 0.0f, 1.0f},
         {0.0f, 0.0f, 1.0f},
         {0.0f, 0.0f, 1.0f},
         {0.0f, 0.0f, 1.0f}});

    auto texcoords = vsg::vec2Array::create(
        {{0.0f, 0.0f},
         {1.0f, 0.0f},
         {1.0f, 1.0f},
         {0.0f, 1.0f}});

    auto indices = vsg::ushortArray::create(
        {
            0,
            1,
            2,
            2,
            3,
            0,
        });

    vsg::DataList Env_vertexArrays;
    auto Env_graphicsPipelineConfig = vsg::GraphicsPipelineConfigurator::create(shader); //渲染管线创建
    auto sampler = vsg::Sampler::create();
    Env_graphicsPipelineConfig->assignTexture("cadColor", image, sampler);
    //Env_graphicsPipelineConfig->assignTexture("cadDepth", cadDepth);
    //Env_graphicsPipelineConfig->assignTexture("planeColor", planeColor);
    //Env_graphicsPipelineConfig->assignTexture("planeDepth", planeDepth);
    //Env_graphicsPipelineConfig->assignTexture("shadowColor", shadowColor);
    //Env_graphicsPipelineConfig->assignTexture("shadowDepth", shadowDepth);

    Env_graphicsPipelineConfig->assignArray(Env_vertexArrays, "vsg_Vertex", VK_VERTEX_INPUT_RATE_VERTEX, vertices);
    Env_graphicsPipelineConfig->assignArray(Env_vertexArrays, "vsg_Normal", VK_VERTEX_INPUT_RATE_VERTEX, normals);
    Env_graphicsPipelineConfig->assignArray(Env_vertexArrays, "vsg_TexCoord0", VK_VERTEX_INPUT_RATE_VERTEX, texcoords);
    Env_graphicsPipelineConfig->assignArray(Env_vertexArrays, "vsg_Color", VK_VERTEX_INPUT_RATE_INSTANCE, colors);
    Env_graphicsPipelineConfig->assignDescriptor("material", mat);

    //Env_graphicsPipelineConfig->assignTexture("", images[1]->data);
    //Env_graphicsPipelineConfig->assignTexture("", images[2]->data);
    //Env_graphicsPipelineConfig->assignTexture("", images[3]->data);
    //Env_graphicsPipelineConfig->assignTexture("", images[4]->data);
    // 绑定索引
    auto Env_drawCommands = vsg::Commands::create();
    Env_drawCommands->addChild(vsg::BindVertexBuffers::create(Env_graphicsPipelineConfig->baseAttributeBinding, Env_vertexArrays));
    Env_drawCommands->addChild(vsg::BindIndexBuffer::create(indices));
    //cout << mesh.indexes->size() << endl;
    Env_drawCommands->addChild(vsg::DrawIndexed::create(6, 1, 0, 0, 0));

    //双面显示
    auto Env_rs = vsg::RasterizationState::create();
    Env_rs->cullMode = VK_CULL_MODE_NONE;
    Env_graphicsPipelineConfig->pipelineStates.push_back(Env_rs);

    Env_graphicsPipelineConfig->init();

    // create StateGroup as the root of the scene/command graph to hold the GraphicsPipeline, and binding of Descriptors to decorate the whole graph
    auto Env_stateGroup = vsg::StateGroup::create();
    Env_graphicsPipelineConfig->copyTo(Env_stateGroup);
    // set up model transformation node
    auto Env_transform = vsg::MatrixTransform::create(); //用于位置变换

    //transform->subgraphRequiresLocalFrustum = false;
    // add drawCommands to StateGroup
    Env_stateGroup->addChild(Env_drawCommands);
    Env_transform->addChild(Env_stateGroup);
    scene->addChild(Env_transform);
}

void CADMesh::buildnode(const std::string& path, bool fullNormal, vsg::ref_ptr<vsg::Group> scene, vsg::ref_ptr<vsg::ShaderSet> shader)
{
    //设置材质参数
    default_color = vsg::vec4Value::create(vsg::vec4{0.9, 0.9, 0.9, 1.0});
    default_material = vsg::PhongMaterialValue::create();
    default_material->value().ambient.set(0.9, 0.9, 0.9, 1.0);
    default_material->value().diffuse.set(0.55, 0.55, 0.55, 1.0);
    default_material->value().specular.set(0.7, 0.7, 0.7, 1.0);
    //std::cout << mat->value().shininess;//默认值为100
    default_material->value().shininess = 25;

    highlighted_color = vsg::vec4Value::create(vsg::vec4{1.0, 0.0, 0.0, 1.0});
    highlighted_material = vsg::PhongMaterialValue::create();
    highlighted_material->value().ambient.set(1.0, 0.0, 0.0, 1.0);
    highlighted_material->value().diffuse.set(1.0, 0.0, 0.0, 1.0);
    highlighted_material->value().specular.set(1.0, 0.0, 0.0, 1.0);
    //std::cout << mat->value().shininess;//默认值为100
    highlighted_material->value().shininess = 25;

    uint8_t* buffer_data;
    int buffer_size;
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Failed to open the file." << std::endl;
    }
    file.seekg(0, std::ios::end);
    std::streampos fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<uint8_t> buffer(fileSize);
    file.read(reinterpret_cast<char*>(buffer.data()), fileSize);
    file.close();
    buffer_data = buffer.data();
    buffer_size = fileSize;

    builder_out.PushFlatBuffer(buffer_data, buffer_size);
    auto renderFlatBuffer = RenderFlatBuffer::GetRenderFlatBufferDoc(builder_out.GetBufferPointer());
    int emptyProtoNum = 0;

    for (int p = 0; p < renderFlatBuffer->ProtoData()->size(); p++)
    {
        //std::cout << "proto" << p << ":" << std::endl;
        const RenderFlatBuffer::Proto* protofbs = renderFlatBuffer->ProtoData()->Get(p);
        if (protofbs->Models()->size() == 0)
        {
            emptyProtoNum++;
            continue;
        }
        protoIndex[protofbs->ProtoID()->str()] = static_cast<uint32_t>(p - emptyProtoNum);
        std::unordered_map<TinyModelVertex, uint32_t> uniqueVertices;
        std::vector<TinyModelVertex> mVertices{};
        std::vector<uint32_t> mIndices{};

        for (int m = 0; m < protofbs->Models()->size(); m++)
        {
            const RenderFlatBuffer::Model* modelfbs = protofbs->Models()->Get(m);
            auto modelGeo = modelfbs->Geo();
            //std::cout << "modelfbs->Type()->str()" << modelfbs->Type()->str() << std::endl;
            if (modelfbs->Type()->str() == "mesh")
            {
                if (fullNormal)
                {
                    for (int i = 0; i < modelGeo->Index()->size(); i += 1)
                    {
                        TinyModelVertex vertex;
                        int index = modelGeo->Index()->Get(i);
                        vertex.pos = toVec3(modelGeo->Position(), index * 3);
                        vertex.normal = toVec3(modelGeo->Normal(), index * 3);
                        // vertex.uv = toVec2(modelGeo->UV(), index * 2);

                        if (uniqueVertices.count(vertex) == 0) //if unique
                        {
                            uniqueVertices[vertex] = static_cast<uint32_t>(mVertices.size());
                            mVertices.push_back(vertex);
                        }
                        mIndices.push_back(uniqueVertices[vertex]);
                    }
                }
                else
                {
                    for (int i = 0; i < modelGeo->Index()->size(); i += 3)
                    {
                        std::vector<TinyModelVertex> vertex = {};
                        for (int j = 0; j < 3; j++)
                        {
                            TinyModelVertex v;
                            int index = modelGeo->Index()->Get(i + j);
                            // std::cout <<index << "\t";
                            v.pos = toVec3(modelGeo->Position(), index * 3);
                            // vertex.uv = toVec2(modelGeo->UV(), index * 2);
                            vertex.push_back(v);
                        }
                        vsg::vec3 n = vsg::normalize(vsg::cross((vertex[1].pos - vertex[0].pos), (vertex[2].pos - vertex[0].pos)));
                        vertex[0].normal = n;
                        vertex[1].normal = n;
                        vertex[2].normal = n;

                        for (int j = 0; j < 3; j++)
                        {
                            if (uniqueVertices.count(vertex[j]) == 0) //if unique
                            {
                                uniqueVertices[vertex[j]] = static_cast<uint32_t>(mVertices.size());
                                mVertices.push_back(vertex[j]);
                            }
                            mIndices.push_back(uniqueVertices[vertex[j]]);
                        }
                    }
                }
            }
        }
        int Nodenumber = mVertices.size();   //顶点、法向个数
        int Indicesnumber = mIndices.size(); //索引个数
        protoTriangleNum[protofbs->ProtoID()->str()] = static_cast<uint32_t>(mIndices.size() / 3);

        vsg::ref_ptr<vsg::vec3Array> vertices = vsg::vec3Array::create(Nodenumber);
        vsg::ref_ptr<vsg::vec3Array> normals = vsg::vec3Array::create(Nodenumber);
        //coordinates = vsg::vec2Array::create(Nodenumber);
        vsg::ref_ptr<vsg::uintArray> indices = vsg::uintArray::create(Indicesnumber);

        //读取顶点
        for (int i = 0; i < Nodenumber; i++)
        {
            vertices->at(i) = vsg::vec3(mVertices[i].pos);
            normals->at(i) = vsg::vec3(mVertices[i].normal);
        }
        //读取索引
        for (int i = 0; i < Indicesnumber; i++)
        {
            indices->at(i) = mIndices[i];
        }
        verticesVector.push_back(vertices);
        normalsVector.push_back(normals);
        indicesVector.push_back(indices);
    }

    for (int i = 0; i < renderFlatBuffer->Bom()->size(); i++)
    {
        auto instance = renderFlatBuffer->Bom()->Get(i);
        instanceIndex[instance->InstanceID()->str()] = i;
    }

    vsg::mat4 matrix = vsg::mat4(1.0f);
    treeNode top;
    top.transform = vsg::MatrixTransform::create();
    top.transform->matrix = matrix;
    top.originalMatrix = matrix;
    top.id = "";
    nodePtr[""] = top;
    idPtr[top.transform] = "";
    nodenumber++;

    //创建树的所有节点
    for (int i = 0; i < renderFlatBuffer->Bom()->size(); i++)
    {

        auto instance = renderFlatBuffer->Bom()->Get(i);
        auto protoId = instance->ProtoID()->str();
        auto currentId = instance->InstanceID()->str();
        if (protoIndex.find(protoId) != protoIndex.end())
        {
            auto graphicsPipelineConfig = vsg::GraphicsPipelineConfigurator::create(shader);
            vsg::DataList vertexArrays;
            auto drawCommands = vsg::Commands::create();

            //创建纹理或遮罩
            bool addTexture = 0;
            auto options = vsg::Options::create();
            vsg::Path textureFile("../data/textures/lz.vsgb");
            if (textureFile && addTexture)
            {
                auto textureData = vsg::read_cast<vsg::Data>(textureFile, options);
                if (!textureData)
                {
                    std::cout << "Could not read texture file : " << textureFile << std::endl;
                }
                graphicsPipelineConfig->assignTexture("diffuseMap", textureData);
            }

            //传入模型几何参数
            graphicsPipelineConfig->assignArray(vertexArrays, "vsg_Vertex", VK_VERTEX_INPUT_RATE_VERTEX, verticesVector[protoIndex[instance->ProtoID()->str()]]);
            graphicsPipelineConfig->assignArray(vertexArrays, "vsg_Normal", VK_VERTEX_INPUT_RATE_VERTEX, normalsVector[protoIndex[instance->ProtoID()->str()]]);
            //graphicsPipelineConfig->assignArray(vertexArrays, "vsg_TexCoord0", VK_VERTEX_INPUT_RATE_VERTEX, texcoords);

            graphicsPipelineConfig->assignArray(vertexArrays, "vsg_Color", VK_VERTEX_INPUT_RATE_INSTANCE, default_color);
            graphicsPipelineConfig->assignDescriptor("material", default_material);
            //绑定索引
            drawCommands->addChild(vsg::BindVertexBuffers::create(graphicsPipelineConfig->baseAttributeBinding, vertexArrays));
            drawCommands->addChild(vsg::BindIndexBuffer::create(indicesVector[protoIndex[instance->ProtoID()->str()]]));
            drawCommands->addChild(vsg::DrawIndexed::create(indicesVector[protoIndex[instance->ProtoID()->str()]]->size(), 1, 0, 0, 0));

            graphicsPipelineConfig->init();
            auto stateGroup = vsg::StateGroup::create();
            graphicsPipelineConfig->copyTo(stateGroup);
            stateGroup->addChild(drawCommands);

            treeNode node;
            node.transform = vsg::MatrixTransform::create();
            for (int m = 0; m < 4; m++)
                for (int n = 0; n < 3; n++)
                {
                    node.transform->matrix[m][n] = instance->Matrix()->Get(3 * m + n);
                    node.originalMatrix[m][n] = node.transform->matrix[m][n];
                }
            node.instance = instance;
            node.pipeline_1 = graphicsPipelineConfig;
            node.stateGroup = stateGroup;
            


            node.transform->addChild(node.stateGroup);
            node.id = currentId;


            //highlighted pipeline
            auto graphicsPipelineConfig2 = vsg::GraphicsPipelineConfigurator::create(shader);
            vsg::DataList vertexArrays2;
            //传入模型几何参数
            graphicsPipelineConfig2->assignArray(vertexArrays, "vsg_Vertex", VK_VERTEX_INPUT_RATE_VERTEX, verticesVector[protoIndex[instance->ProtoID()->str()]]);
            graphicsPipelineConfig2->assignArray(vertexArrays, "vsg_Normal", VK_VERTEX_INPUT_RATE_VERTEX, normalsVector[protoIndex[instance->ProtoID()->str()]]);
            //graphicsPipelineConfig2->assignArray(vertexArrays, "vsg_TexCoord0", VK_VERTEX_INPUT_RATE_VERTEX, texcoords);
            graphicsPipelineConfig2->assignArray(vertexArrays, "vsg_Color", VK_VERTEX_INPUT_RATE_INSTANCE, highlighted_color);
            graphicsPipelineConfig2->assignDescriptor("material", highlighted_material);

            graphicsPipelineConfig2->init();

            node.pipeline_2 = graphicsPipelineConfig2;



            //set up reflection
            nodePtr[currentId] = node;
            
            idPtr[node.transform] = currentId;
            std::cout << "渲染id: " << idPtr[node.transform] << std::endl;
            nodenumber++;

            //nodePtr[currentId].bounds = vsg::visit<vsg::ComputeBounds>(nodePtr[currentId].transform).bounds;
            //nodePtr[currentId].transform->bounds = nodePtr[currentId].bounds;
        }
        else
        {
            vsg::mat4 matrix = vsg::mat4(1.0f);
            for (int m = 0; m < 4; m++)
            {
                for (int n = 0; n < 3; n++)
                {
                    matrix[m][n] = instance->Matrix()->Get(3 * m + n);
                    // std::cout << matrix[m][n] << " ";
                }
                // std::cout << std::endl;
            }
                
            treeNode node;
            node.transform = vsg::MatrixTransform::create();
            node.transform->matrix = matrix;
            node.originalMatrix = matrix;
            node.id = currentId;

            nodePtr[currentId] = node;
            
            //std::cout << "node.transform: " << node.transform << "   ";
            idPtr[node.transform] = currentId;
            //std::cout << "id: " << idPtr[node.transform] << std::endl;


            nodenumber++;
        }
    }

    //处理树中节点的关系
    uint32_t totalTriangleNum = 0;
    for (int i = 0; i < renderFlatBuffer->Bom()->size(); i++)
    {
        auto instance = renderFlatBuffer->Bom()->Get(i);
        auto currentId = instance->InstanceID()->str();
        auto parentId = instance->ParentID()->str();
        auto protoId = instance->ProtoID()->str();
        if (protoIndex.find(protoId) != protoIndex.end())
        {
            totalTriangleNum += protoTriangleNum[protoId];
            while (true)
            {
                if (added.find(currentId) == added.end())
                {
                    nodePtr[parentId].kids.push_back(currentId);
                    nodePtr[parentId].transform->addChild(nodePtr[currentId].transform);
                    added.insert(currentId);
                }
                if (parentId == "")
                    break;
                currentId = parentId;
                parentId = renderFlatBuffer->Bom()->Get(instanceIndex[currentId])->ParentID()->str();
            }
        }
    }

    std::cout << "totalTriangleNum = " << totalTriangleNum << std::endl;
    scene->addChild(nodePtr[""].transform);

    //更新bounding box
    for (int i = 0; i < renderFlatBuffer->Bom()->size(); i++)
    {
        auto instance = renderFlatBuffer->Bom()->Get(i);
        auto currentId = instance->InstanceID()->str();
        nodePtr[currentId].bounds = vsg::visit<vsg::ComputeBounds>(nodePtr[currentId].transform).bounds;
        nodePtr[currentId].transform->bounds = nodePtr[currentId].bounds;

    }
    nodePtr[""].bounds = vsg::visit<vsg::ComputeBounds>(nodePtr[""].transform).bounds;
    nodePtr[""].transform->bounds = nodePtr[""].bounds;
    positionCalculate(nodePtr[""], nodePtr[""].transform->matrix);
}

void CADMesh::repaintAll(treeNode& node, MaterialObj& mat)
{
    if (node.kids.size() == 0) // leaf node
    {
        auto material = vsg::PhongMaterialValue::create();
        if (mat.name != "")
        {
            material->value().ambient = mat.ambient;
            material->value().diffuse = mat.diffuse;
            material->value().specular = mat.specular;
            material->value().shininess = mat.shininess;
        }
        else
        {
            material = default_material;
            mat.color = default_color;
        }

        ////重新设置管线
        auto graphicsPipelineConfig = vsg::GraphicsPipelineConfigurator::create(shader);
        vsg::DataList vertexArrays;
        //传入模型几何参数
        graphicsPipelineConfig->assignArray(vertexArrays, "vsg_Vertex", VK_VERTEX_INPUT_RATE_VERTEX, verticesVector[protoIndex[node.instance->ProtoID()->str()]]);
        graphicsPipelineConfig->assignArray(vertexArrays, "vsg_Normal", VK_VERTEX_INPUT_RATE_VERTEX, normalsVector[protoIndex[node.instance->ProtoID()->str()]]);
        graphicsPipelineConfig->assignArray(vertexArrays, "vsg_Color", VK_VERTEX_INPUT_RATE_INSTANCE, mat.color);
        graphicsPipelineConfig->assignDescriptor("material", material);
        graphicsPipelineConfig->init();

        graphicsPipelineConfig->copyTo(node.stateGroup);
        node.pipeline_1 = graphicsPipelineConfig;
    }
    else
    {
        for (auto& child_id : node.kids)
        {
            repaintAll(nodePtr[child_id], mat);
        }
    }
}
