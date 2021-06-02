#ifndef FUNGICORE_PANEL_PANEL_HPP
#define FUNGICORE_PANEL_PANEL_HPP

#include <memory>
#include "ftxui/component/component.hpp"

class PanelBase : public ftxui::ComponentBase {
 public:
  virtual std::wstring Title() { return L"title undefined"; }
  virtual ~PanelBase() {}
};
using Panel = std::shared_ptr<PanelBase>;

namespace panel {
Panel PlaceHolder(const std::wstring& title);
Panel Sound();
}  // namespace panel

#endif /* end of include guard: FUNGICORE_PANEL_PANEL_HPP */
