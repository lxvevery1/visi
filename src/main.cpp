///
/// C++ FTXUI Audio Vizualizer Project
///

#include <cmath> // for sin, cos
#include <sdbus-c++/IProxy.h>
#include <string>  // for string, basic_string
#include <utility> // for move
#include <vector>  // for vector, __alloc_traits<>::value_type

#include <ftxui/component/component.hpp> // for Renderer, CatchEvent, Horizontal, Menu, Tab
#include <ftxui/component/component_base.hpp>     // for ComponentBase
#include <ftxui/component/event.hpp>              // for Event
#include <ftxui/component/mouse.hpp>              // for Mouse
#include <ftxui/component/screen_interactive.hpp> // for ScreenInteractive
#include <ftxui/dom/canvas.hpp>                   // for Canvas
#include <ftxui/dom/elements.hpp> // for canvas, Element, separator, hbox, operator|, border
#include <ftxui/screen/color.hpp> // for Color, Color::Red, Color::Blue, Color::Green, ftxui
#include <ftxui/screen/screen.hpp> // for Pixel

#include "./dbus/dbus_metadata_picker.h"

using namespace ftxui;

void ui() {

    // Some variables for mouse tracking
    int mouse_x = 0;
    int mouse_y = 0;

    // Some constants
    const int CANVAS_SIZE_X = 500;
    const int CANVAS_SIZE_Y = 100;
    const Color COLOR_EQI = Color::Wheat1;
    const std::string TITLE = "Audio Visualizer";

    // Plot rendering
    auto renderer_plot = Renderer([&] {
        auto c = Canvas(CANVAS_SIZE_X, CANVAS_SIZE_Y);
        c.DrawText(CANVAS_SIZE_X / 3.5, 0, TITLE);

        std::vector<int> ys(CANVAS_SIZE_X);
        for (int x = 0; x < CANVAS_SIZE_X; x++) {
            ys[x] = int(0.12 * 100 * sin(x));
        }

        for (int x = 0; x < CANVAS_SIZE_X; x++) {
            c.DrawPointLine(x, CANVAS_SIZE_Y / 2 + ys[x], x,
                            CANVAS_SIZE_Y / 2 - ys[x], COLOR_EQI);
        }

        return canvas(std::move(c));
    });

    // Tab rendering
    int selected_tab = 0;
    auto tab = Container::Tab(
        {
            renderer_plot,
        },
        &selected_tab);

    // This capture the last mouse position.
    auto tab_with_mouse = CatchEvent(tab, [&](Event e) {
        if (e.is_mouse()) {
            mouse_x = (e.mouse().x - 1);
            mouse_y = (e.mouse().y - 1);
        }
        return false;
    });

    auto component = Container::Horizontal({
        tab_with_mouse,
    });

    // Add some separator to decorate the whole component:
    auto component_renderer = Renderer(component, [&] {
        return vbox({
                   tab_with_mouse->Render(),
               }) |
               border;
    });

    auto screen = ScreenInteractive::FitComponent();
    screen.Loop(component_renderer);
}

void get_audio_name() {
    DBusMetadataPicker dbus_metadata_picker;
    dbus_metadata_picker.dbus_get_audio_name();
}

int main() {
    get_audio_name();
    // pulse_sound_detect();
    // ui();
    return 0;
}
