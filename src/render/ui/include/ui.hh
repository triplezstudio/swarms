#ifndef SWARMS_UI_HH
#define SWARMS_UI_HH

#include <Eigen/Dense>
#include <immediate_commands.hh>
#include <input.hh>
#include <render_helpers.hh>
#include <text_render.hh>
#include <vulkan_renderer.hh>
#include <window.hh>

namespace tz {

/**
 * This is where the UISystem lives.
 * It can not exist on its own and
 * must be associated to a toplevel os window.
 * Inside this toplevel window, it gets assigned to a
 * viewport.
 * All its ui widgets will only be layed out inside this viewport.
 *
 * This allows the user to reserve different dedicated areas in the toplevel
 * window for different UISystems.
 *
 */
struct UIHost
{
  Window *window                     = nullptr;
  render::vulkan::Renderer *renderer = nullptr;
  input::SDLInputSystem *inputSystem = nullptr;

  // The viewport rectangle within the window.
  Eigen::Vector2f viewPortPosition;
  Eigen::Vector2f viewPortSize;
};

struct UIRenderContext
{
  UIHost uiHost;
  ImmediateCommandProcessor& immediateCommandProcessor;
  render::TextRenderer& textRenderer;
  render::Font& font;
};

class UILayout;
/**
 * A Widget is the root class for every ui element.
 * Subclasses implement typical behaviors for
 * buttons, combo-boxes, textboxes etc.
 *
 * Widgets can be positioned to absolute coordinates
 * or may be parented to Layouts, which automatically
 * position and resize Widgets.
 *
 */
class TZ_API UIWidget
{
  public:
  explicit UIWidget(UIWidget *parent)
    : parent(parent)
  {
    init();
  };
  virtual ~UIWidget() = default;

  Eigen::Vector2f getPosition();
  Eigen::Vector2f getSize();

  virtual void render(UIRenderContext& uiRenderContext);

  /**
   * This function walks up the parent hierarchy and
   * adds up the parent-space positions along the way.
   * So in the end we arrive at the overall global position of the starting widget.
   * @param target The global position.
   */
  void getGlobalPosition(Eigen::Vector2f *target);

  /**
       * Move this widget to a position in parent space.
       * Note, if this widget belongs to a Layout, this movement may
       * be ignored.
       * @param x
       * @param y
       */
  void move(int x, int y);

  void resize(int weight, int height);

  /**
       *
       * @return a list of children, normally a copy, so no manipulation possible
       *         of the children from outside via this function.
       */
  [[nodiscard]] std::vector<UIWidget *> getChildren() const
  {
    return children;
  }

  protected:
  UIWidget *parent = nullptr;
  std::vector<UIWidget *> children;

  Eigen::Vector2f position;
  Eigen::Vector2f size;

  UILayout *layout = nullptr;

  void init();

};

/**
 * A UILayout handles the position and size of its child widgets.
 * Typical subclasses include layouts such as
 * VerticalBox, HorizontalBox, FlowLayout etc.
 */
class UILayout : public UIWidget
{
  public:
  UILayout()  = default;
  ~UILayout() = default;

  Eigen::Vector2f getPositionForWidget(UIWidget &widget);
  Eigen::Vector2f getSizeForWidget(UIWidget &widget);
};

/**
 * A clickable button with text.
 * Can change appearance on hovering.
 */
class UIButton : public UIWidget
{
  public:
  explicit UIButton(UIWidget *parent)
    : UIWidget(parent)
  {}
  ~UIButton() = default;

  void setText(const std::string& newText)
  {
    this->text = newText;
  }

  virtual void render(UIRenderContext& rc) override;

  protected:
      std::string text;

};

/**
 * The UISystem manages the overall state for a group of related widgets.
 * It is allowed to have more than UISystem in an application.
 * Every UISystem gets assigned a viewport inside the host window, which may also overlap.
 * In this case, there is an implicit z-order dictated by the sequence of the creation of the
 * respective UISystems.
 * Subsequent UISystems sit on top of earlier ones.
 * So different UISystems may be used to have "meta"-layers:
 * First created UISystem1 sits on implicit layer 1.
 * UISystem2 sits on implicit layer 2, therefore on top.
 * Every widget in system-layer2, will always be on top of every widget in system-layer1.
 *
 * Widgets in the same layer have their own separate z-order.
 *
 */
class TZ_API UISystem
{
  public:
  explicit UISystem(UIHost host);

  /**
   * This method prcesses the latest frame inputs and
   * decides on the overall state of this UISystem:
   * - current focused Widget
   * - current hovered Widget
   * - drag&drop state
   */
  void update();

  /**
   * Iterates through all its widgets and delegates rendering to them.
   * Every widget is told its current position and size,
   * so it knows where to position itself.
   */
  render::vulkan::CommandBuffer &recordFrameCommandBuffer();

  UIWidget &createWidget(UIWidget *parent);
  UIButton &createButton(UIWidget *parent);

  private:
  UIHost host;
  std::vector<UIWidget *> topLevelWidgets;
  MasterPipelineLayout *masterPipelineLayout           = nullptr;
  render::TextRenderer *textRenderer                   = nullptr;
  ImmediateCommandProcessor *immediateCommandProcessor = nullptr;
  tz::render::Font *font                               = nullptr;
};


} // namespace tz

#endif //SWARMS_UI_HH
