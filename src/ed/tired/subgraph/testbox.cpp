
#include <print>

#include "testbox.h"

namespace tire {

Testbox::Testbox( vsg::Viewer* viewer, QObject* parent )
    : QObject{ parent }
    , _testbox{ new TestboxSubgraph{ viewer } } {
    //
    _testbox->initPipeline();
    _testbox->initDrawCommand();
}

auto Testbox::testbox() const -> vsg::ref_ptr<TestboxSubgraph> {
    return _testbox;
}

void Testbox::setSize( float value ) {
    _testbox->_size = value;
}

float Testbox::gridSize() {
    return _testbox->_size;
}

// ===============================================================================

TestboxSubgraph::TestboxSubgraph( vsg::Viewer* viewer )
    : Subgraph{ viewer } {
}

auto TestboxSubgraph::initPipeline() -> void {
    // load shaders
    vsg::Paths searchPaths = std::vector<vsg::Path>{ std::format( "{}{}", PREFIX_PATH_ONE, "/shaders/spirv" ),
                                                     std::format( "{}{}", PREFIX_PATH_ONE, "/assets" ) };

    vsg::ref_ptr<vsg::ShaderStage> vertexShader =
        vsg::ShaderStage::read( VK_SHADER_STAGE_VERTEX_BIT, "main", vsg::findFile( "vert_testbox.spv", searchPaths ) );
    vsg::ref_ptr<vsg::ShaderStage> fragmentShader = vsg::ShaderStage::read(
        VK_SHADER_STAGE_FRAGMENT_BIT, "main", vsg::findFile( "frag_testbox.spv", searchPaths ) );
    if ( !vertexShader || !fragmentShader ) {
        std::println( "Could not create shaders." );
        std::terminate();
    }

    // set up graphics pipeline
    vsg::DescriptorSetLayoutBindings descriptorBindings{
        { 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT,
          nullptr }  // { binding, descriptorType, descriptorCount, stageFlags, pImmutableSamplers}
    };

    auto descriptorSetLayout = vsg::DescriptorSetLayout::create( descriptorBindings );

    vsg::PushConstantRanges pushConstantRanges{
        { VK_SHADER_STAGE_VERTEX_BIT, 0,
          128 }  // projection, view, and model matrices, actual push constant calls automatically provided by the VSG's RecordTraversal
    };

    vsg::VertexInputState::Bindings vertexBindingsDescriptions{};

    vsg::VertexInputState::Attributes vertexAttributeDescriptions{};

    vsg::GraphicsPipelineStates pipelineStates{
        vsg::VertexInputState::create( vertexBindingsDescriptions, vertexAttributeDescriptions ),
        vsg::InputAssemblyState::create(),
        vsg::RasterizationState::create(),
        vsg::MultisampleState::create(),
        vsg::ColorBlendState::create(),
        vsg::DepthStencilState::create() };

    auto pipelineLayout =
        vsg::PipelineLayout::create( vsg::DescriptorSetLayouts{ descriptorSetLayout }, pushConstantRanges );
    auto graphicsPipeline = vsg::GraphicsPipeline::create(
        pipelineLayout, vsg::ShaderStages{ vertexShader, fragmentShader }, pipelineStates );
    auto bindGraphicsPipeline = vsg::BindGraphicsPipeline::create( graphicsPipeline );

    _stateGroup->add( bindGraphicsPipeline );
}

auto TestboxSubgraph::initDrawCommand() -> void {
    // Add draw command.
    auto drawCommands = vsg::Commands::create();
    drawCommands->addChild( vsg::Draw::create( 36, 1, 0, 0 ) );

    auto tr = vsg::MatrixTransform::create();
    tr->addChild( drawCommands );

    _stateGroup->addChild( tr );
}

}  // namespace tire
