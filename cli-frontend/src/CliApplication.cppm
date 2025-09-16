export module via.cli;

import std;
import ftxui;

using namespace ftxui;

export {
struct SourceApplication {
  std::string title;
  bool active = false;
  float volume = 0.0;
};

struct InputChannel {
  std::string title;
  float volume = 0.0;
  bool mute = false;
  std::vector<SourceApplication> sources = {};
};

struct OutputChannel {
  std::string title;
  std::string device;
  float volume = 0.0;
  bool mute = false;
  std::vector<InputChannel> sources = {};
};

struct ApplicationData {
  std::vector<SourceApplication> applications;
  std::vector<InputChannel>      inputs;
  std::vector<OutputChannel>     outputs;
};

struct CliApplication {
  CliApplication (int argc, char** argv);

  auto exec () noexcept -> int;

  ApplicationData data;
  Component view;
};
}

module :private;

// Based on RadioboxOption::transform (defined in RadioboxOption::Simple)
constexpr auto radiobox(bool value) noexcept -> Element {
#if defined(FTXUI_MICROSOFT_TERMINAL_FALLBACK)
  // Microsoft terminal do not use fonts able to render properly the default
  // radiobox glyph.
  return text(value ? "(*)" : "( )");  // NOLINT
#else
  return text(value ? "◉" : "○");  // NOLINT
#endif
}
// Based on CheckboxOption::transform (defined in CheckboxOption::Simple)
constexpr auto checkbox(bool value) noexcept -> Element {
#if defined(FTXUI_MICROSOFT_TERMINAL_FALLBACK)
  // Microsoft terminal do not use fonts able to render properly the default
  // radiobox glyph.
  return text(value ? "[X]" : "[ ]");  // NOLINT
#else
  return text(value ? "▣" : "☐");  // NOLINT
#endif
}

CliApplication::CliApplication(int argc, char** argv) {
// TODO replace with real values
  data = {
    .applications = {
      { "Zen Browser"    , true  },
      { "Steam"          , true  },
      { "Mozilla Firefox", false },
      { "Roboquest"      , false },
      { "Sea of Thieves" , false },
    },
    .inputs = {
      { "Game" , 0.8, false, {
        { "Hollow Knight" , true, 1.0 },
        { "Slay the Spire", true, 0.9 },
      } },
      { "Chat" , 1.0, false, {
        { "Discord"  , true , 1.0 },
        { "TeamSpeak", true , 0.8 },
        { "Mumble"   , false, 0.4 },
      } },
      { "Music", 0.0, true , {
        { "VLC"   , true, 1.0 },
        { "Winamp", true, 0.9 },
      } },
    },
    .outputs = {
      { "Headset" , "Razer Barracuda X 2.4", 0.8, false, {
        { "Game" , 0.8 },
        { "Chat" , 1.0 },
        { "Music", 0.5 },
      } },
      { "Speakers", "DJM-750-K"            , 0.5, false, {
        { "Music", 0.3 },
      } },
      { "Stream 1", "Virtual"              , 1.0, false, {
        { "Game" , 0.8 },
        { "Chat" , 1.0 },
        { "Music", 0.5 },
      } },
      { "Stream 2", "Virtual"              , 0.0, true , {
        { "Music", 1.0 },
      } },
    }
  };

  auto applications_items = Components {};
  for (auto& app : data.applications) {
    applications_items.push_back(
      Container::Horizontal({
        Renderer([&active = app.active] {
          return radiobox(active);
        })
          | Renderer(vcenter),
        Renderer([] { return text(" "); }),
        Renderer([&title = app.title] {
          return text(title);
        })
          | Renderer(vcenter),
      })
    );
  }

  auto inputs_items = Components {};
  for (auto& input : data.inputs) {
    auto sources_items = Components {};
    for (const auto& source : input.sources) {
      sources_items.push_back(
        Container::Horizontal({
          Renderer([&active = source.active] {
            return radiobox(active);
          })
            | Renderer(vcenter),
          Renderer([] { return text(" "); }),
          Container::Vertical({
            Renderer([&title = source.title] {
              return text(title);
            }),
            Renderer([&volume = source.volume] {
              return gaugeRight(volume);
            }),
          })
           | Renderer(xflex_grow),
        })
      );
    }

    inputs_items.push_back(
      Container::Vertical({
        Slider<decltype(input.volume)>({
          .value = &input.volume,
          .direction = Direction::Up
        })
          | Renderer(border)
          | Renderer(hcenter)
          | Renderer(yflex_grow),
        Container::Horizontal({
          Checkbox({.checked = &input.mute, .transform = [](auto state) { return checkbox(state.state) | center; }}),
          Button("opts", [] {}),
        })
          | Renderer(hcenter),
        Container::Vertical(sources_items)
          | Renderer(size(HEIGHT, EQUAL, 6)),
      })
        | Renderer([&title = input.title](Element inner) {
            return window(text(title), inner)
              | size(WIDTH, EQUAL, 16);
        })
    );
  }

  auto outputs_items = Components {};
  for (auto& output : data.outputs) {
    auto sources_items = Components {};
    for (const auto& source : output.sources) {
      sources_items.push_back(
        Container::Vertical({
          Renderer([&title = source.title] {
            return text(title);
          }),
          Renderer([&volume = source.volume] {
            return gaugeRight(volume);
          }),
        })
      );
    }

    outputs_items.push_back(
      Container::Vertical({
        Renderer([&device = output.device] {
          return hbox({ text("<"), text(device) | xflex_shrink, text(">") }) | hcenter;
        }),
        Slider<decltype(output.volume)>({
          .value = &output.volume,
          .direction = Direction::Up
        })
          | Renderer(border)
          | Renderer(hcenter)
          | Renderer(yflex_grow),
        Container::Horizontal({
          Checkbox("mute", &output.mute),
          Button("opts", [] {}),
        })
          | Renderer(hcenter),
        Container::Vertical(sources_items)
          | Renderer(size(HEIGHT, EQUAL, 6)),
      })
        | Renderer([&title = output.title](Element inner) {
            return window(text(title), inner)
              | size(WIDTH, EQUAL, 16);
        })
    );
  }

  view = Container::Horizontal({
    Container::Vertical(applications_items)
      | Renderer([](Element inner) {
          return window(text("Applications"), inner);
      }),
    Container::Horizontal(inputs_items)
      | Renderer([](Element inner) {
          return window(text("Input Channels"), inner)
            | flex_grow;
      }),
    Container::Horizontal(outputs_items)
      | Renderer([](Element inner) {
          return window(text("Output Channels"), inner)
            | flex_grow;
      }),
  })
    | Renderer([](Element inner) {
        return window(text("Via Vocis"), inner);
    });
}

auto CliApplication::exec() noexcept -> int {
  auto screen = ScreenInteractive::Fullscreen();
  // auto main_loop = Loop{&screen, view};

  using clk = std::chrono::high_resolution_clock;
  using namespace std::chrono_literals;

  auto program_thread = std::jthread { [&](std::stop_token stop) mutable noexcept {

    auto genBool = std::bind(std::uniform_int_distribution { 0, 1 }, std::default_random_engine {});
    auto vary = [] (clk::time_point timestamp, auto i) {
      return (1.0 + std::sin(2.0 * std::numbers::pi * ( timestamp.time_since_epoch().count() + (i * 100) ) / 1000000000 + i)) / 2.0;
    };

    constexpr auto tickrate = 15;
    constexpr auto tickperiod = std::chrono::duration_cast<clk::duration>( 1000ms / tickrate );
    auto last_time = clk::now();

    while (not stop.stop_requested()) {

      const auto timestamp = last_time + tickperiod;

      auto i = 0;
      for (auto& app : data.applications) {
        app.active = genBool();
        app.volume = vary(timestamp, i++);
      }
      for (auto& input : data.inputs) {
        for (auto& source : input.sources) {
          source.active = genBool();
          source.volume = vary(timestamp, i++);
        }
      }
      for (auto& output : data.outputs) {
        for (auto& source : output.sources) {
          source.volume = vary(timestamp, i++);
        }
      }

      auto elapsed = clk::now() - last_time;
      auto missing = tickperiod - std::min(elapsed, tickperiod);
      std::this_thread::sleep_for(missing);

      screen.RequestAnimationFrame();

      last_time = clk::now();
    }
  } };

  screen.Loop(view);
  return 0;
}
