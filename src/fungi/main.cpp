// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

#include <iostream>
#include <thread>
#include <vector>

#include "ftxui/component/container.hpp"
#include "ftxui/component/menu.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/component/toggle.hpp"
#include "ftxui/screen/string.hpp"

#include "fungi/panel/panel.hpp"

using namespace ftxui;

class MainMenu : public ComponentBase {
 public:
  MainMenu(std::vector<Panel> panels) {
    Add(Container::Horizontal({
        menu_,
        tab_,
    }));

    for (auto& panel : panels) {
      menu_entries_.push_back(panel->Title());
      tab_->Add(panel);
    }
  }

  Element Render() override {
    return hbox({
               vbox({
                   text(L"  Fungi"),
                   separator(),
                   menu_->Render() | yframe,
               }),
               separator(),
               tab_->Render() | flex,
           }) |
           border;
  }

 private:
  int menu_index_ = 0;
  std::vector<std::wstring> menu_entries_;
  Component menu_ = Menu(&menu_entries_, &menu_index_);
  Component tab_ = Container::Tab({}, &menu_index_);
};

int main(int argc, const char* argv[]) {
  auto screen = ScreenInteractive::Fullscreen();
  std::vector<Panel> panels = {
      panel::Sound(),
      panel::PlaceHolder(L"Wi-Fi"),
      panel::PlaceHolder(L"Network"),
      panel::PlaceHolder(L"Bluetooth"),
      panel::PlaceHolder(L"Background"),
      panel::PlaceHolder(L"Notification"),
      panel::PlaceHolder(L"Search"),
      panel::PlaceHolder(L"Application"),
      panel::PlaceHolder(L"Confidentiality"),
      panel::PlaceHolder(L"Online account"),
      panel::PlaceHolder(L"Sharing"),
      panel::PlaceHolder(L"Energy"),
      panel::PlaceHolder(L"Screen"),
      panel::PlaceHolder(L"Mouse & Trackpad"),
      panel::PlaceHolder(L"Printer"),
      panel::PlaceHolder(L"Amovible device"),
      panel::PlaceHolder(L"Printer"),
      panel::PlaceHolder(L"Colors"),
      panel::PlaceHolder(L"State and language"),
      panel::PlaceHolder(L"Universal access"),
      panel::PlaceHolder(L"Users"),
      panel::PlaceHolder(L"Default application"),
      panel::PlaceHolder(L"Date and time"),
      panel::PlaceHolder(L"About"),
  };
  Component main_menu = Make<MainMenu>(std::move(panels));

  std::thread update([&screen]() {
    for (;;) {
      using namespace std::chrono_literals;
      std::this_thread::sleep_for(0.05s);
      screen.PostEvent(Event::Custom);
    }
  });

  screen.Loop(main_menu);
}
