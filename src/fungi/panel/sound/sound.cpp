#include <alsa/asoundlib.h>
#include <cmath>
#include <ftxui/component/container.hpp>
#include <ftxui/screen/string.hpp>
#include "fungi/panel/panel.hpp"

using namespace ftxui;

struct SoundMixerSelemID {
 public:
  SoundMixerSelemID() { snd_mixer_selem_id_malloc(&id_); }
  SoundMixerSelemID(snd_mixer_elem_t* elem) : SoundMixerSelemID() {
    snd_mixer_selem_get_id(elem, id_);
  }
  ~SoundMixerSelemID() {
    if (id_) {
      snd_mixer_selem_id_free(id_);
    }
  }

  SoundMixerSelemID& operator=(SoundMixerSelemID&& moved) {
    std::swap(id_, moved.id_);
    return *this;
  }
  SoundMixerSelemID& operator=(const SoundMixerSelemID&) = delete;
  SoundMixerSelemID(SoundMixerSelemID&& moved) { std::swap(id_, moved.id_); }
  SoundMixerSelemID(const SoundMixerSelemID&) = delete;

  void SetIndex(int index) { snd_mixer_selem_id_set_index(id_, index); }

  std::string GetName() { return snd_mixer_selem_id_get_name(id_); }
  void SetName(std::string name) {
    snd_mixer_selem_id_set_name(id_, name.c_str());
  }

  snd_mixer_elem_t* FindElement(snd_mixer_t* handle) {
    return snd_mixer_find_selem(handle, id_);
  }

  snd_mixer_selem_id_t* id() { return id_; }

 private:
  snd_mixer_selem_id_t* id_ = nullptr;
};

class TitleComponent : public ComponentBase {
 public:
  Element Render() final {
    auto style = Focused() ? inverted : nothing;
    return vbox({
        text(L"Sound") | style,
        separator(),
    });
  }
};

struct SoundInterface : public ComponentBase {
 public:
  SoundInterface(snd_mixer_elem_t* elem): elem_(elem) {
    snd_mixer_selem_get_playback_volume_range(elem_, &min_, &max_);
    snd_mixer_selem_get_playback_volume(elem_, SND_MIXER_SCHN_FRONT_LEFT,
                                        &playback_left_);
    snd_mixer_selem_get_playback_volume(elem_, SND_MIXER_SCHN_FRONT_RIGHT,
                                        &playback_right_);
    snd_mixer_selem_get_capture_volume(elem_, SND_MIXER_SCHN_FRONT_LEFT,
                                       &capture_left_);
    snd_mixer_selem_get_capture_volume(elem_, SND_MIXER_SCHN_FRONT_RIGHT,
                                       &capture_right_);
    selem_id_ = SoundMixerSelemID(elem_);
    name_ = selem_id_.GetName();

    if (min_ == max_)
      max_ = min_ + 1;

    playback_left_ = std::min(max_, std::max(min_, playback_left_));
    playback_right_ = std::min(max_, std::max(min_, playback_right_));
    capture_left_ = std::min(max_, std::max(min_, capture_left_));
    capture_right_ = std::min(max_, std::max(min_, capture_right_));

    long inc = (max_ - min_) / 100;
    slider_playback_left_ = Slider("Left :", &playback_left_, min_, max_, inc);
    slider_playback_right_ = Slider("Right:", &playback_right_, min_, max_, inc);
    slider_capture_left_ = Slider("Left :", &capture_left_, min_, max_, inc);
    slider_capture_right_ = Slider("Right:", &capture_right_, min_, max_, inc);

    Add(Container::Vertical({
        slider_playback_left_,
        slider_playback_right_,
        slider_capture_left_,
        slider_capture_right_,
    }));
  }

  Element Render() {
    return vbox({
        text(to_wstring(name_)) | bold,
        hbox({
            text(L"Playback:") | color(Color::Blue),
            vbox({
                slider_playback_left_->Render(),
                slider_playback_right_->Render(),
            }) | xflex,
        }),
        hbox({
            text(L"Capture :") | color(Color::Blue),
            vbox({
                slider_capture_left_->Render(),
                slider_capture_right_->Render(),
            }) | xflex,
        }),
        separator(),
    });
  }

  bool OnEvent(Event event) {
    long playback_left = playback_left_;
    long playback_right = playback_right_;
    long capture_left = capture_left_;
    long capture_right = capture_right_;

    bool ret = ComponentBase::OnEvent(event);

    if (playback_left != playback_left_) {
      snd_mixer_selem_set_playback_volume(elem_, SND_MIXER_SCHN_FRONT_LEFT,
                                          playback_left_);
    } else {
      snd_mixer_selem_get_playback_volume(elem_, SND_MIXER_SCHN_FRONT_LEFT,
                                          &playback_left_);
    }

    if (playback_right != playback_right_) {
      snd_mixer_selem_set_playback_volume(elem_, SND_MIXER_SCHN_FRONT_RIGHT,
                                          playback_right_);
    } else {
      snd_mixer_selem_get_playback_volume(elem_, SND_MIXER_SCHN_FRONT_RIGHT,
                                          &playback_right_);
    }

    if (capture_left != capture_left_) {
      snd_mixer_selem_set_capture_volume(elem_, SND_MIXER_SCHN_FRONT_LEFT,
                                          capture_left_);
    } else {
      snd_mixer_selem_get_capture_volume(elem_, SND_MIXER_SCHN_FRONT_LEFT,
                                          &capture_left_);
    }

    if (capture_right != capture_right_) {
      snd_mixer_selem_set_capture_volume(elem_, SND_MIXER_SCHN_FRONT_RIGHT,
                                          capture_right_);
    } else {
      snd_mixer_selem_get_capture_volume(elem_, SND_MIXER_SCHN_FRONT_RIGHT,
                                          &capture_right_);
    }

    playback_left_ = std::min(max_, std::max(min_, playback_left_));
    playback_right_ = std::min(max_, std::max(min_, playback_right_));
    capture_left_ = std::min(max_, std::max(min_, capture_left_));
    capture_right_ = std::min(max_, std::max(min_, capture_right_));

    return ret;
  }

 private:
  std::string name_;
  snd_mixer_elem_t* elem_;
  SoundMixerSelemID selem_id_;
  long min_;
  long max_;
  long playback_left_;
  long playback_right_;
  long capture_left_;
  long capture_right_;
  Component slider_playback_left_;
  Component slider_playback_right_;
  Component slider_capture_left_;
  Component slider_capture_right_;
};

std::vector<std::shared_ptr<SoundInterface>> ListInterfaces(
    snd_mixer_t* handle) {
  std::vector<std::shared_ptr<SoundInterface>> out;

  for (snd_mixer_elem_t* elem = snd_mixer_first_elem(handle); elem;
       elem = snd_mixer_elem_next(elem)) {
    out.push_back(Make<SoundInterface>(elem));
  }
  return out;
}

class SoundBase : public PanelBase {
 public:
  std::wstring Title() { return L"Sound"; }
  SoundBase() {
    Init();
  }
  virtual ~SoundBase() {}

 private:

  void Init() {
    Add(container_);
    container_->Add(Make<TitleComponent>());
    if (handle_)
      snd_mixer_close(handle_);
    snd_mixer_open(&handle_, 0);
    snd_mixer_attach(handle_, "pulse");

    snd_mixer_selem_register(handle_, NULL, NULL);
    snd_mixer_load(handle_);

    // SoundMixerSelemID selem_id;
    // selem_id.SetIndex(0);
    // selem_id.SetName("Master");
    snd_mixer_selem_id_t* sid = nullptr;
    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, "Master");
    elem_ = snd_mixer_find_selem(handle_, sid);
    snd_mixer_selem_get_playback_volume_range(elem_, &min_, &max_);
    // snd_mixer_selem_id_free(sid);
    // elem_ = selem_id.FindElement(handle_);
    // snd_mixer_selem_get_playback_volume_range(elem_, &min_, &max_);

    for (auto& it : ListInterfaces(handle_))
      container_->Add(it);
  }

  std::vector<std::string> find_mixer_elem_by_name() {
    std::vector<std::string> ret;
    snd_mixer_elem_t* elem;
    snd_mixer_selem_id_t* sid = NULL;

    snd_mixer_selem_id_malloc(&sid);

    for (elem = snd_mixer_first_elem(handle_); elem;
         elem = snd_mixer_elem_next(elem)) {
      const char* name;
      snd_mixer_selem_get_id(elem, sid);
      name = snd_mixer_selem_id_get_name(sid);
      ret.push_back(name);
    }

    snd_mixer_selem_id_free(sid);
    return ret;
  }

  snd_mixer_t* handle_ = nullptr;
  snd_mixer_selem_id_t* sid_ = nullptr;
  snd_mixer_elem_t* elem_ = nullptr;
  long min_, max_;

  Component title_ = Make<TitleComponent>();
  Component container_ = Container::Vertical({});

  float sound_mean_smooth_ = 0.f;
  float sound_direction_smooth_ = 0.f;
};

namespace panel {
Panel Sound() {
  return std::make_unique<SoundBase>();
}
}  // namespace panel
