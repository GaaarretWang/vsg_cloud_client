#include "matWindow.h"

matWindow::matWindow(vsg::ref_ptr<vsg::WindowTraits> in_traits, CADMesh& in_mesh, vsg::ref_ptr<vsgQt::Viewer>& in_viewer, QWidget* parent) :
    QMainWindow(parent)
{
    traits = in_traits;
    mesh = &in_mesh;
    viewer_mainWindow = in_viewer;

    ui.setupUi(this);
    setWindowIcon(QIcon("../data/images/myappiteam.png"));
    this->setWindowTitle(QString::fromLocal8Bit("CAD材质库"));
    ui.label->setStyleSheet("color:#2F4F2F;");

    //set up materials
    materialInit();

    //initialize material list
    QTreeWidgetItem* root_phong = new QTreeWidgetItem();
    root_phong->setText(0, QString::fromLocal8Bit("材质集合1"));

    //QTreeWidgetItem* root_pbr = new QTreeWidgetItem();
    //QTreeWidgetItem* root_image = new QTreeWidgetItem();
    //root_pbr->setText(0, QString::fromLocal8Bit("材质集合2"));
    //root_image->setText(0, QString::fromLocal8Bit("材质贴图"));
    //ui.treeWidget->addTopLevelItem(root_pbr);
    //ui.treeWidget->addTopLevelItem(root_image);

    for (int i = 0; i < index; i++)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem();
        std::string str = namePtr_phong[i];
        item->setText(0, QString::fromLocal8Bit(str.c_str())); //set up a new node
        root_phong->addChild(item);                            //add the new node to his father
    }

    ui.treeWidget->addTopLevelItem(root_phong);

    connect(ui.treeWidget, &QTreeWidget::itemDoubleClicked, this, &matWindow::doMaterialUpdate);
    connect(ui.applyButton, SIGNAL(clicked()), this, SLOT(on_Action_apply()));

    //initialize default shaderset (phong)
    auto options = vsg::Options::create();
    auto phong_vertexShader = vsg::read_cast<vsg::ShaderStage>("../data/shaders/standard.vert", options);
    auto phong_fragShader = vsg::read_cast<vsg::ShaderStage>("../data/shaders/standard_phong.frag", options);
    shaderSet_phong = vsg::createPhongShaderSet();
    if (shaderSet_phong && phong_vertexShader && phong_fragShader)
    {
        // replace shaders
        shaderSet_phong->stages.clear();
        shaderSet_phong->stages.push_back(phong_vertexShader);
        shaderSet_phong->stages.push_back(phong_fragShader);

        // clear prebuilt variants
        shaderSet_phong->variants.clear();
        std::cout << "Replaced phong shader." << std::endl;
    }
    else
    {
        std::cout << "Could not create shaders." << std::endl;
    }

    //initialize default graphicsPipelineConfig
    graphicsPipelineConfig = vsg::GraphicsPipelineConfigurator::create(shaderSet_phong);

    //initialize geometry of a sphere
    auto dx = vsg::vec3(1.0, 0.0, 0.0);
    auto dy = vsg::vec3(0.0, 1.0, 0.0);
    auto dz = vsg::vec3(0.0, 0.0, 1.0);     //大小
    auto origin = vsg::vec3(0.0, 0.0, 0.0); //圆心

    unsigned int num_columns = 80; //细分
    unsigned int num_rows = 80;
    unsigned int num_vertices = num_columns * num_rows;
    unsigned int num_indices = (num_columns - 1) * (num_rows - 1) * 6;

    vertices = vsg::vec3Array::create(num_vertices);
    normals = vsg::vec3Array::create(num_vertices);
    texcoords = vsg::vec2Array::create(num_vertices);
    indices = vsg::ushortArray::create(num_indices);

    for (unsigned int r = 0; r < num_rows; ++r)
    {
        float beta = ((float(r) / float(num_rows - 1)) - 0.5f) * vsg::PIf;
        float ty = 0.0 + 1.0 * float(r) / float(num_rows - 1);
        float cos_beta = cosf(beta);
        vsg::vec3 dz_sin_beta = dz * sinf(beta);

        vsg::vec3 v = dy * cos_beta + dz_sin_beta;
        vsg::vec3 n = normalize(v);

        unsigned int left_i = r * num_columns;
        vertices->set(left_i, v + origin);
        normals->set(left_i, n);
        texcoords->set(left_i, vsg::vec2(0.0f, ty));

        unsigned int right_i = left_i + num_columns - 1;
        vertices->set(right_i, v + origin);
        normals->set(right_i, n);
        texcoords->set(right_i, vsg::vec2(1.0f, ty));

        for (unsigned int c = 1; c < num_columns - 1; ++c)
        {
            unsigned int vi = left_i + c;
            float alpha = (float(c) / float(num_columns - 1)) * 2.0f * vsg::PIf;
            v = dx * (-sinf(alpha) * cos_beta) + dy * (cosf(alpha) * cos_beta) + dz_sin_beta;
            n = normalize(v);
            vertices->set(vi, origin + v);
            normals->set(vi, n);
            texcoords->set(vi, vsg::vec2(float(c) / float(num_columns - 1), ty));
        }
    }

    unsigned int i = 0;
    for (unsigned int r = 0; r < num_rows - 1; ++r)
    {
        for (unsigned int c = 0; c < num_columns - 1; ++c)
        {
            unsigned lower = num_columns * r + c;
            unsigned upper = lower + num_columns;

            indices->set(i++, lower);
            indices->set(i++, lower + 1);
            indices->set(i++, upper);

            indices->set(i++, upper);
            indices->set(i++, lower + 1);
            indices->set(i++, upper + 1);
        }
    }

    auto default_color = vsg::vec4Value::create(vsg::vec4{1.0, 1.0, 1.0, 1.0f});
    auto default_material = vsg::PhongMaterialValue::create();
    default_material->value().ambient.set(1.0, 1.0, 1.0, 1.0);
    default_material->value().diffuse.set(1.0, 1.0, 1.0, 1.0);
    default_material->value().specular.set(1.0, 1.0, 1.0, 1.0);

    //send geometry to pipeline
    graphicsPipelineConfig->assignArray(vertexArrays, "vsg_Vertex", VK_VERTEX_INPUT_RATE_VERTEX, vertices);
    graphicsPipelineConfig->assignArray(vertexArrays, "vsg_Normal", VK_VERTEX_INPUT_RATE_VERTEX, normals);
    //graphicsPipelineConfig->assignArray(vertexArrays, "vsg_TexCoord0", VK_VERTEX_INPUT_RATE_VERTEX, texcoords);
    graphicsPipelineConfig->assignArray(vertexArrays, "vsg_Color", VK_VERTEX_INPUT_RATE_INSTANCE, default_color);
    graphicsPipelineConfig->assignDescriptor("material", default_material);

    // setup geometry
    auto drawCommands = vsg::Commands::create();
    drawCommands->addChild(vsg::BindVertexBuffers::create(graphicsPipelineConfig->baseAttributeBinding, vertexArrays));
    drawCommands->addChild(vsg::BindIndexBuffer::create(indices));
    drawCommands->addChild(vsg::DrawIndexed::create(num_indices, 1, 0, 0, 0));

    graphicsPipelineConfig->init();

    stateGroup = vsg::StateGroup::create();
    graphicsPipelineConfig->copyTo(stateGroup);
    stateGroup->addChild(drawCommands);
    scene->addChild(stateGroup); // add sphere to scene

    //initialize vsg window
    createVsgMatWindow(nullptr);
}

matWindow::~matWindow()
{
}

void matWindow::materialInit()
{
    materials_phong[index].name = "黑色橡胶";
    materials_phong[index].color = vsg::vec4Value::create(vsg::vec4{0.0, 0.0, 0.0, 1.0});
    materials_phong[index].ambient = vsg::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    materials_phong[index].diffuse = vsg::vec4(0.01f, 0.01f, 0.01f, 1.0f);
    materials_phong[index].specular = vsg::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    materials_phong[index].shininess = 7.8;
    indexPtr_phong[materials_phong[index].name] = index;
    namePtr_phong[index] = materials_phong[index].name;
    index++;

    materials_phong[index].name = "白色橡胶";
    materials_phong[index].color = vsg::vec4Value::create(vsg::vec4{1.0, 1.0, 1.0, 1.0});
    materials_phong[index].ambient = vsg::vec4(0.8, 0.8, 0.8, 1.0f);
    materials_phong[index].diffuse = vsg::vec4(0.55, 0.55, 0.55, 1.0f);
    materials_phong[index].specular = vsg::vec4(0.7, 0.7, 0.7, 1.0f);
    materials_phong[index].shininess = 0.0;
    indexPtr_phong[materials_phong[index].name] = index;
    namePtr_phong[index] = materials_phong[index].name;
    index++;

    materials_phong[index].name = "青色橡胶";
    materials_phong[index].color = vsg::vec4Value::create(vsg::vec4{0.0, 0.51, 0.50, 1.0});
    materials_phong[index].ambient = vsg::vec4(0.0f, 1.0f, 1.0f, 1.0f);
    //materials_phong[index].ambient = vsg::vec4(0.0f, 0.1f, 0.06f, 1.0f);
    materials_phong[index].diffuse = vsg::vec4(0.0f, 0.51f, 0.51f, 1.0f);
    materials_phong[index].specular = vsg::vec4(0.50196f, 0.50196f, 0.50196f, 1.0f);
    materials_phong[index].shininess = 0.0;
    indexPtr_phong[materials_phong[index].name] = index;
    namePtr_phong[index] = materials_phong[index].name;
    index++;

    materials_phong[index].name = "红色橡胶";
    materials_phong[index].color = vsg::vec4Value::create(vsg::vec4{1.0, 0.0, 0.0, 1.0});
    materials_phong[index].ambient = vsg::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    materials_phong[index].diffuse = vsg::vec4(0.5f, 0.0f, 0.0f, 1.0f);
    materials_phong[index].specular = vsg::vec4(0.7f, 0.6f, 0.6f, 1.0f);
    materials_phong[index].shininess = 0.0;
    indexPtr_phong[materials_phong[index].name] = index;
    namePtr_phong[index] = materials_phong[index].name;
    index++;

    materials_phong[index].name = "蓝色橡胶";
    materials_phong[index].color = vsg::vec4Value::create(vsg::vec4{0.0, 0.0, 1.0, 1.0});
    materials_phong[index].ambient = vsg::vec4(0.0f, 0.0f, 1.0f, 1.0f);
    //materials_phong[index].ambient = vsg::vec4(0.0f, 0.1f, 0.06f, 1.0f);
    materials_phong[index].diffuse = vsg::vec4(0.0f, 0.51f, 0.51f, 1.0f);
    materials_phong[index].specular = vsg::vec4(0.50196f, 0.50196f, 0.50196f, 1.0f);
    materials_phong[index].shininess = 0.0;
    indexPtr_phong[materials_phong[index].name] = index;
    namePtr_phong[index] = materials_phong[index].name;
    index++;

    materials_phong[index].name = "黑色塑料";
    materials_phong[index].color = vsg::vec4Value::create(vsg::vec4{0.0, 0.0, 0.0, 1.0});
    materials_phong[index].ambient = vsg::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    materials_phong[index].diffuse = vsg::vec4(0.01f, 0.01f, 0.01f, 1.0f);
    materials_phong[index].specular = vsg::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    materials_phong[index].shininess = 25;
    indexPtr_phong[materials_phong[index].name] = index;
    namePtr_phong[index] = materials_phong[index].name;
    index++;

    materials_phong[index].name = "白色塑料";
    materials_phong[index].color = vsg::vec4Value::create(vsg::vec4{1.0, 1.0, 1.0, 1.0});
    materials_phong[index].ambient = vsg::vec4(0.8, 0.8, 0.8, 1.0f);
    materials_phong[index].diffuse = vsg::vec4(0.55, 0.55, 0.55, 1.0f);
    materials_phong[index].specular = vsg::vec4(0.7, 0.7, 0.7, 1.0f);
    materials_phong[index].shininess = 25;
    indexPtr_phong[materials_phong[index].name] = index;
    namePtr_phong[index] = materials_phong[index].name;
    index++;

    materials_phong[index].name = "青色塑料";
    materials_phong[index].color = vsg::vec4Value::create(vsg::vec4{0.0, 0.51, 0.50, 1.0});
    materials_phong[index].ambient = vsg::vec4(0.0f, 1.0f, 1.0f, 1.0f);
    //materials_phong[index].ambient = vsg::vec4(0.0f, 0.1f, 0.06f, 1.0f);
    materials_phong[index].diffuse = vsg::vec4(0.0f, 0.51f, 0.51f, 1.0f);
    materials_phong[index].specular = vsg::vec4(0.50196f, 0.50196f, 0.50196f, 1.0f);
    materials_phong[index].shininess = 25;
    indexPtr_phong[materials_phong[index].name] = index;
    namePtr_phong[index] = materials_phong[index].name;
    index++;

    materials_phong[index].name = "红色塑料";
    materials_phong[index].color = vsg::vec4Value::create(vsg::vec4{1.0, 0.0, 0.0, 1.0});
    materials_phong[index].ambient = vsg::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    materials_phong[index].diffuse = vsg::vec4(0.5f, 0.0f, 0.0f, 1.0f);
    materials_phong[index].specular = vsg::vec4(0.7f, 0.6f, 0.6f, 1.0f);
    materials_phong[index].shininess = 25;
    indexPtr_phong[materials_phong[index].name] = index;
    namePtr_phong[index] = materials_phong[index].name;
    index++;

    materials_phong[index].name = "蓝色塑料";
    materials_phong[index].color = vsg::vec4Value::create(vsg::vec4{0.0, 0.0, 1.0, 1.0});
    materials_phong[index].ambient = vsg::vec4(0.0f, 0.0f, 1.0f, 1.0f);
    //materials_phong[index].ambient = vsg::vec4(0.0f, 0.1f, 0.06f, 1.0f);
    materials_phong[index].diffuse = vsg::vec4(0.0f, 0.51f, 0.51f, 1.0f);
    materials_phong[index].specular = vsg::vec4(0.50196f, 0.50196f, 0.50196f, 1.0f);
    materials_phong[index].shininess = 25;
    indexPtr_phong[materials_phong[index].name] = index;
    namePtr_phong[index] = materials_phong[index].name;
    index++;

    materials_phong[index].name = "合金";
    materials_phong[index].color = vsg::vec4Value::create(vsg::vec4{0.94, 0.84, 0.18, 1.0});
    materials_phong[index].ambient = vsg::vec4(0.24725f, 0.1995f, 0.0745f, 1.0f);
    materials_phong[index].diffuse = vsg::vec4(0.75164f, 0.60648f, 0.22648f, 1.0f);
    materials_phong[index].specular = vsg::vec4(0.628281f, 0.555802f, 0.366065, 1.0f);
    materials_phong[index].shininess = 60.0;
    indexPtr_phong[materials_phong[index].name] = index;
    namePtr_phong[index] = materials_phong[index].name;
    index++;

    materials_phong[index].name = "银（默认）";
    materials_phong[index].color = vsg::vec4Value::create(vsg::vec4{0.8, 0.8, 0.8, 1.0});
    materials_phong[index].ambient = vsg::vec4(0.8, 0.8, 0.8, 1.0);
    materials_phong[index].diffuse = vsg::vec4(0.55, 0.55, 0.55, 1.0);
    materials_phong[index].specular = vsg::vec4(0.7, 0.7, 0.7, 1.0);
    materials_phong[index].shininess = 60.0;
    indexPtr_phong[materials_phong[index].name] = index;
    namePtr_phong[index] = materials_phong[index].name;
    index++;

    materials_phong[index].name = "铁";
    materials_phong[index].color = vsg::vec4Value::create(vsg::vec4{0.4, 0.4, 0.38, 1.0});
    materials_phong[index].ambient = vsg::vec4(0.4, 0.4, 0.38, 1.0);
    materials_phong[index].diffuse = vsg::vec4(0.55, 0.55, 0.55, 1.0);
    materials_phong[index].specular = vsg::vec4(0.7, 0.7, 0.7, 1.0);
    materials_phong[index].shininess = 40.0;
    indexPtr_phong[materials_phong[index].name] = index;
    namePtr_phong[index] = materials_phong[index].name;
    index++;

    materials_phong[index].name = "黄铜";
    materials_phong[index].color = vsg::vec4Value::create(vsg::vec4{0.44, 0.39, 0.22, 1.0});
    materials_phong[index].ambient = vsg::vec4(0.329412f, 0.223529f, 0.027451f, 1.0f);
    materials_phong[index].diffuse = vsg::vec4(0.78, 0.5686, 0.1137, 1.0f);
    materials_phong[index].specular = vsg::vec4(0.992, 0.941, 0.807, 1.0f);
    materials_phong[index].shininess = 10.0;
    indexPtr_phong[materials_phong[index].name] = index;
    namePtr_phong[index] = materials_phong[index].name;
    index++;

    materials_phong[index].name = "玉石";
    materials_phong[index].color = vsg::vec4Value::create(vsg::vec4{0.97, 0.96, 1.0, 1.0});
    materials_phong[index].ambient = vsg::vec4(0.87, 0.86, 0.9, 1.0f);
    materials_phong[index].diffuse = vsg::vec4(0.54, 0.89, 0.63, 1.0f);
    materials_phong[index].specular = vsg::vec4(0.316228, 0.316228, 0.316228, 1.0f);
    materials_phong[index].shininess = 20.0;
    indexPtr_phong[materials_phong[index].name] = index;
    namePtr_phong[index] = materials_phong[index].name;
    index++;
}

void matWindow::createVsgMatWindow(QWindow* parent)
{
    //main window
    //QWindow* parent = nullptr;
    window = new vsgQt::Window(viewer, traits, parent);
    //auto window = new vsgQt::Window(viewer_main, traits, parent);
    window->initializeWindow();

    // if this is the first window to be created, use its device for future window creation.
    //if (!traits->device) {
    //    traits->device = window->windowAdapter->getOrCreateDevice();
    //}

    // compute the bounds of the scene graph to help position camera
    uint32_t width = window->traits->width;
    //std::cout << width << std::endl;
    uint32_t height = window->traits->height;
    //std::cout << height << std::endl;
    vsg::ComputeBounds computeBounds;
    scene->accept(computeBounds);
    vsg::dvec3 centre = (computeBounds.bounds.min + computeBounds.bounds.max) * 0.5;
    double radius = vsg::length(computeBounds.bounds.max - computeBounds.bounds.min) * 0.6;
    auto span = vsg::length(computeBounds.bounds.max - computeBounds.bounds.min);
    double nearFarRatio = 0.001;
    {
        auto lookAt = vsg::LookAt::create(centre + vsg::dvec3(0.0, -radius * 3.5, 0.0), centre, vsg::dvec3(0.0, 0.0, 1.0));
        vsg::ref_ptr<vsg::ProjectionMatrix> perspective;

        perspective = vsg::Perspective::create(
            30.0,
            static_cast<double>(width) /
                static_cast<double>(height),
            nearFarRatio * radius, radius * 45);

        camera = vsg::Camera::create(perspective, lookAt, vsg::ViewportState::create(VkExtent2D{width, height}));
    }

    //ambinet
    auto group = vsg::Group::create();
    group->addChild(scene);

    for (int i = 0; i < 1; i++)
    {
        vsg::ref_ptr<vsg::AmbientLight> ambientLight;
        ambientLight = vsg::AmbientLight::create();
        ambientLight->name = "ambient";
        ambientLight->color.set(1.0, 1.0, 1.0);
        ambientLight->intensity = 0.95;
        group->addChild(ambientLight);
    }

    //方向光
    for (int i = 0; i < 1; i++)
    {
        auto directionalLight = vsg::DirectionalLight::create();
        directionalLight->name = "directional";
        directionalLight->color.set(1.0, 1.0, 1.0);
        directionalLight->intensity = 0.7;
        directionalLight->direction = vsg::normalize(vsg::vec3(0.9, 1.0, -1.0));
        group->addChild(directionalLight);
    }

    for (int i = 0; i < 1; i++)
    {
        auto directionalLight2 = vsg::DirectionalLight::create();
        directionalLight2->name = "2nd directional";
        directionalLight2->color.set(1.0, 1.0, 1.0);
        directionalLight2->intensity = 0.1;
        directionalLight2->direction = vsg::normalize(vsg::vec3(-0.9, -1.0, 1.0));
        group->addChild(directionalLight2);
    }

    scene = group;

    view = vsg::View::create(camera, scene);
    auto renderGraph = vsg::RenderGraph::create(*window, view);
    renderGraph->clearValues[0].color = {{0.9f, 0.9f, 0.9f, 1.0f}};
    auto commandGraph = vsg::CommandGraph::create(*window, renderGraph);
    commandGraph->addChild(renderGraph);

    //auto commandGraph = vsg::createCommandGraphForView(*window, camera, scene);

    viewer->addRecordAndSubmitTaskAndPresentation({commandGraph});

    auto trackball = vsg::Trackball::create(camera);
    trackball->addWindow(*window);
    viewer->addEventHandler(trackball);

    viewer->compile();

    //from vsg scenes to qt widgets
    ui.viewLayout->addWidget(createWindowContainer(window));
}

void matWindow::doMaterialUpdate(QTreeWidgetItem* item)
{
    if (item->childCount() == 0) // leave node
    {
        std::string str = item->text(0).toStdString();

        int curindex = indexPtr_phong[str];
        currentMat = materials_phong[curindex];
        materialUpdate(currentMat, "phong");
    }
}

void matWindow::materialUpdate(MaterialObj mat, std::string mode)
{
    if (mode == "phong")
    {
        std::cout << "更换管线" << std::endl;
        auto material = vsg::PhongMaterialValue::create();

        material->value().ambient = mat.ambient;
        material->value().diffuse = mat.diffuse;
        material->value().specular = mat.specular;
        material->value().shininess = mat.shininess;

        //参数传递到着色器
        vsg::ref_ptr<vsg::GraphicsPipelineConfigurator> gpc_temp;
        vsg::DataList vertexArrays_temp;

        std::cout << "change phong" << std::endl;
        gpc_temp = vsg::GraphicsPipelineConfigurator::create(shaderSet_phong);

        gpc_temp->assignArray(vertexArrays_temp, "vsg_Vertex", VK_VERTEX_INPUT_RATE_VERTEX, vertices);
        gpc_temp->assignArray(vertexArrays_temp, "vsg_Normal", VK_VERTEX_INPUT_RATE_VERTEX, normals);
        //gpc_temp->assignArray(vertexArrays_temp, "vsg_TexCoord0", VK_VERTEX_INPUT_RATE_VERTEX, texcoords);
        gpc_temp->assignArray(vertexArrays_temp, "vsg_Color", VK_VERTEX_INPUT_RATE_INSTANCE, mat.color);
        gpc_temp->assignDescriptor("material", material);
        gpc_temp->init();
        gpc_temp->copyTo(stateGroup);
    }

    viewer->compile();
}

void matWindow::on_Action_apply()
{
    if (currentMat.name != "")
    {
        treeNode node = mesh->getNode("");
        mesh->repaintAll(node, currentMat);
        viewer_mainWindow->compile();
    }
}
