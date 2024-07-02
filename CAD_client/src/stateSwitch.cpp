#include <vsg/all.h>

#include <chrono>
#include <iostream>

#ifdef vsgXchange_FOUND
#    include <vsgXchange/all.h>
#endif

class InsertStateSwitch : public vsg::Visitor
{
public:
    std::vector<vsg::Object*> parents;
    std::set<vsg::Object*> visited;
    std::map<vsg::BindGraphicsPipeline*, vsg::ref_ptr<vsg::StateSwitch>> pipelineMap;
    vsg::Mask mask_1 = 0x1;
    vsg::Mask mask_2 = 0x2;

    void traverse(vsg::Object& object)
    {
        parents.push_back(&object);
        object.traverse(*this);
        parents.pop_back();
    }

    void apply(vsg::Object& object) override
    {
        traverse(object);
    }

    vsg::ref_ptr<vsg::GraphicsPipeline> createAlternate(vsg::GraphicsPipeline& pipeline)
    {
        auto alternative_pipeline = vsg::GraphicsPipeline::create();

        *alternative_pipeline = pipeline;

        for (auto& pipelineState : alternative_pipeline->pipelineStates)
        {
            if (auto rasterizationState = pipelineState.cast<vsg::RasterizationState>())
            {
                auto alternate_rasterizationState = vsg::RasterizationState::create(*rasterizationState);

                alternate_rasterizationState->polygonMode = VK_POLYGON_MODE_LINE;
                pipelineState = alternate_rasterizationState;
            }
        }
        return alternative_pipeline;
    }

    void apply(vsg::StateGroup& sg) override
    {
        if (visited.count(&sg) > 0) return;
        visited.insert(&sg);

        for (auto& sc : sg.stateCommands)
        {
            if (auto bgp = sc->cast<vsg::BindGraphicsPipeline>())
            {
                auto& stateSwitch = pipelineMap[bgp];

                if (!stateSwitch)
                {
                    stateSwitch = vsg::StateSwitch::create();
                    stateSwitch->slot = bgp->slot;
                    stateSwitch->add(mask_1, sc);

                    auto alternate_gp = createAlternate(*(bgp->pipeline));
                    auto alternate_bgp = vsg::BindGraphicsPipeline::create(alternate_gp);

                    stateSwitch->add(mask_2, alternate_bgp);
                }
                sc = stateSwitch;
            }
        }

        traverse(sg);
    }
};