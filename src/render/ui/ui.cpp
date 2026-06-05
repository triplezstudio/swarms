
#include <Eigen/Dense>
#include <immediate_commands.hh>
#include <render_helpers.hh>
#include <ui.hh>

tz::UISystem::UISystem(UIHost host) : host(host)
{
  masterPipelineLayout = new tz::MasterPipelineLayout(*host.renderer);
  auto textureAssetManager = new tz::TextureAssetManager(*host.renderer, masterPipelineLayout->getDiffuseTextureDescriptorSet());
  textRenderer = new tz::render::TextRenderer(*host.renderer, *textureAssetManager);
  immediateCommandProcessor = new tz::ImmediateCommandProcessor(*host.renderer,
                                                                *textRenderer,
                                                                *masterPipelineLayout);

  font = textRenderer->createFont("assets/consola.ttf", 18);

}
tz::render::vulkan::CommandBuffer &tz::UISystem::recordFrameCommandBuffer()
{
  immediateCommandProcessor->activateUICamera({0, 0, 4});

  for (auto& widget : topLevelWidgets)
  {
    Eigen::Vector2f offset = {widget->getSize().x()/2, widget->getSize().y()/2};

    auto pos = Eigen::Vector3f{widget->getPosition().x() + offset.x(), widget->getPosition().y() + offset.y() , 0};
    auto size = Eigen::Vector3f(widget->getSize().x(), widget->getSize().y(), 1);
    immediateCommandProcessor->renderQuad({pos, size});
    auto textRect = textRenderer->measureText("Click Me", *font);
    Eigen::Vector2f textRectOffset = {(textRect.right - textRect.left)/2, (textRect.top - textRect.bottom)/2};
    Eigen::Vector3f textDebugQuadPos = {widget->getPosition().x() + textRectOffset.x(),
                                         widget->getPosition().y() + textRectOffset.y(), -0.6};
    Eigen::Vector3f textDebugQuadSize = {textRect.right - textRect.left, textRect.top - textRect.bottom, 1};
    RenderHints textDebugQuadRenderHints;
    textDebugQuadRenderHints.color = {0, 0, 1, 1};
    immediateCommandProcessor->renderQuad({textDebugQuadPos, textDebugQuadSize},
                                          textDebugQuadRenderHints);
    immediateCommandProcessor->renderText("Click Me", *font,
                                          {{widget->getPosition().x() + offset.x() - textRectOffset.x(),
                                                               widget->getPosition().y() + offset.y() - textRectOffset.y(), -0.5}},
                                          {1, 0, 0, 1});

  }

  auto& frameCommandBuffer = immediateCommandProcessor->recordFrameCommandBuffer();
  return frameCommandBuffer;

}


tz::UIWidget &tz::UISystem::createWidget(tz::UIWidget *parent)
{
  auto widget = new UIWidget(parent);
  topLevelWidgets.push_back(widget);
  return *widget;
}
tz::UIButton &tz::UISystem::createButton(tz::UIWidget *parent)
{

  auto button = new UIButton(parent);
  topLevelWidgets.push_back(button);
  return *button;
}

/**
 * The position is influenced by a layout.
 * If we do not belong to a layout, our position is just
 * directly our position value.
 * If we belong to a layout, we delegate this question to the layout.
 *
 * @return the actual position of this Widget within parent-space.
 */
Eigen::Vector2f tz::UIWidget::getPosition()
{
  return layout ? layout->getPositionForWidget(*this) : position;

}


/**
 * We delegate to a layout to determine our size, if we have one.
 * @return the size of this widget.
 */
Eigen::Vector2f tz::UIWidget::getSize()
{
  return layout ? layout->getSizeForWidget(*this) : size;
}
void tz::UIWidget::init()
{
  if (auto maybeLayout = dynamic_cast<UILayout*>(parent))
  {
    layout = maybeLayout;
  }

}
void tz::UIWidget::move(int x, int y)
{
  position = {x,y};
}

void tz::UIWidget::resize(int weight, int height)
{
  size = {weight, height};
}

Eigen::Vector2f tz::UILayout::getPositionForWidget(tz::UIWidget &widget)
{
  // TODO implement
  return Eigen::Vector2f();
}
Eigen::Vector2f tz::UILayout::getSizeForWidget(tz::UIWidget& widget)
{
  // TODO implement
  return Eigen::Vector2f();
}
