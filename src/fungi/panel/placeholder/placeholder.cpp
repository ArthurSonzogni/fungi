#include "fungi/panel/panel.hpp"

using namespace ftxui;

class PlaceHolderImpl : public PanelBase {
 public:
  PlaceHolderImpl(const std::wstring& title) : title_(title) {}
  std::wstring Title() final { return title_; }
  Element Render() final {
    auto style = Focused() ? inverted : nothing;
    return vbox({
        text(title_) | style,
        separator(),
        text(L"Placeholder"),
    });
  }

 private:
  std::wstring title_;
};

namespace panel {
Panel PlaceHolder(const std::wstring& title) {
  return std::make_unique<PlaceHolderImpl>(title);
}
}  // namespace panel
