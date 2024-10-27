///
/// C++ FTXUI Audio Vizualizer Project
///

#include <cmath> // for sin, cos
#include <iostream>
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

#include <pulse/error.h>
#include <pulse/simple.h>

using namespace ftxui;

int pulse_sound_detect() {
    // Define the PulseAudio stream parameters
    pa_sample_spec ss;
    ss.format = PA_SAMPLE_S16LE; // 16-bit signed integer, little-endian
    ss.rate = 44100;             // 44.1 kHz
    ss.channels = 2;             // Stereo

    // Create a PulseAudio simple stream
    pa_simple* s = nullptr;
    int error;

    s = pa_simple_new(nullptr,                  // Use the default server
                      "Audio Detection",        // Application name
                      PA_STREAM_RECORD,         // We are recording (listening)
                      nullptr,                  // Use the default device
                      "Audio Detection Stream", // Stream description
                      &ss,                      // Sample format
                      nullptr,                  // Default channel map
                      nullptr,                  // Default buffering attributes
                      &error                    // Error code
    );

    if (!s) {
        std::cerr << "Failed to create PulseAudio stream: "
                  << pa_strerror(error) << std::endl;
        return 1;
    }

    // Buffer to hold the audio data
    const int buffer_size = 1024;
    char buffer[buffer_size];

    // Read audio data from the stream
    if (pa_simple_read(s, buffer, buffer_size, &error) < 0) {
        std::cerr << "Failed to read from PulseAudio stream: "
                  << pa_strerror(error) << std::endl;
        pa_simple_free(s);
        return 1;
    }

    // Check if the buffer contains non-zero data
    bool audio_playing = false;
    for (int i = 0; i < buffer_size; ++i) {
        if (buffer[i] != 0) {
            audio_playing = true;
            break;
        }
    }

    std::cout << (audio_playing ? "Audio is playing." : "No audio detected.")
              << std::endl;

    // Clean up
    pa_simple_free(s);

    return 1;
}

int main() {
    // pulse_sound_detect();

    // Some variables for mouse tracking
    int mouse_x = 0;
    int mouse_y = 0;

    // Some constants
    const int CANVAS_SIZE_X = 500;
    const int CANVAS_SIZE_Y = 100;
    const Color COLOR_EQI = Color::Wheat1;
    const std::string TITLE = "Audio Visualizer";

    // Plot rendering
    auto renderer_plot_2 = Renderer([&] {
        auto c = Canvas(CANVAS_SIZE_X, CANVAS_SIZE_Y);
        c.DrawText(CANVAS_SIZE_X / 3.5, 0, TITLE);

        std::vector<int> ys(CANVAS_SIZE_X);
        for (int x = 0; x < CANVAS_SIZE_X; x++) {
            ys[x] = int(30 +                                //
                        10 * cos(x * 0.2 + mouse_x * 0.1) + //
                        5 * -sin(x * 0.4) +                 //
                        5 * cos(x * 0.3 + mouse_y * 0.1));  //
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
            renderer_plot_2,
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

    return 0;
}
