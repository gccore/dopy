cmake_minimum_required(VERSION 3.12)

project(ftxui
  LANGUAGES CXX
  VERSION 5.0.0
  DESCRIPTION "C++ Functional Terminal User Interface."
)

set(FTXUI_MICROSOFT_TERMINAL_FALLBACK_HELP_TEXT "On windows, assume the \
terminal used will be one of Microsoft and use a set of reasonnable fallback \
to counteract its implementations problems.")
if (WIN32)
  option(FTXUI_MICROSOFT_TERMINAL_FALLBACK
    ${FTXUI_MICROSOFT_TERMINAL_FALLBACK_HELP_TEXT} ON)
else()
  option(FTXUI_MICROSOFT_TERMINAL_FALLBACK
    ${FTXUI_MICROSOFT_TERMINAL_FALLBACK_HELP_TEXT} OFF)
endif()

add_library(screen STATIC
  include/ftxui/screen/box.hpp
  include/ftxui/screen/color.hpp
  include/ftxui/screen/color_info.hpp
  include/ftxui/screen/image.hpp
  include/ftxui/screen/pixel.hpp
  include/ftxui/screen/screen.hpp
  include/ftxui/screen/string.hpp
  src/ftxui/screen/box.cpp
  src/ftxui/screen/color.cpp
  src/ftxui/screen/color_info.cpp
  src/ftxui/screen/image.cpp
  src/ftxui/screen/screen.cpp
  src/ftxui/screen/string.cpp
  src/ftxui/screen/terminal.cpp
  src/ftxui/screen/util.hpp
)

add_library(dom STATIC
  include/ftxui/dom/canvas.hpp
  include/ftxui/dom/direction.hpp
  include/ftxui/dom/elements.hpp
  include/ftxui/dom/flexbox_config.hpp
  include/ftxui/dom/node.hpp
  include/ftxui/dom/requirement.hpp
  include/ftxui/dom/take_any_args.hpp
  src/ftxui/dom/automerge.cpp
  src/ftxui/dom/blink.cpp
  src/ftxui/dom/bold.cpp
  src/ftxui/dom/hyperlink.cpp
  src/ftxui/dom/border.cpp
  src/ftxui/dom/box_helper.cpp
  src/ftxui/dom/box_helper.hpp
  src/ftxui/dom/canvas.cpp
  src/ftxui/dom/clear_under.cpp
  src/ftxui/dom/color.cpp
  src/ftxui/dom/composite_decorator.cpp
  src/ftxui/dom/dbox.cpp
  src/ftxui/dom/dim.cpp
  src/ftxui/dom/flex.cpp
  src/ftxui/dom/flexbox.cpp
  src/ftxui/dom/flexbox_config.cpp
  src/ftxui/dom/flexbox_helper.cpp
  src/ftxui/dom/flexbox_helper.hpp
  src/ftxui/dom/focus.cpp
  src/ftxui/dom/frame.cpp
  src/ftxui/dom/gauge.cpp
  src/ftxui/dom/graph.cpp
  src/ftxui/dom/gridbox.cpp
  src/ftxui/dom/hbox.cpp
  src/ftxui/dom/inverted.cpp
  src/ftxui/dom/linear_gradient.cpp
  src/ftxui/dom/node.cpp
  src/ftxui/dom/node_decorator.cpp
  src/ftxui/dom/paragraph.cpp
  src/ftxui/dom/reflect.cpp
  src/ftxui/dom/scroll_indicator.cpp
  src/ftxui/dom/separator.cpp
  src/ftxui/dom/size.cpp
  src/ftxui/dom/spinner.cpp
  src/ftxui/dom/strikethrough.cpp
  src/ftxui/dom/table.cpp
  src/ftxui/dom/text.cpp
  src/ftxui/dom/underlined.cpp
  src/ftxui/dom/underlined_double.cpp
  src/ftxui/dom/util.cpp
  src/ftxui/dom/vbox.cpp
)

add_library(component STATIC
  include/ftxui/component/animation.hpp
  include/ftxui/component/captured_mouse.hpp
  include/ftxui/component/component.hpp
  include/ftxui/component/component_base.hpp
  include/ftxui/component/component_options.hpp
  include/ftxui/component/event.hpp
  include/ftxui/component/loop.hpp
  include/ftxui/component/mouse.hpp
  include/ftxui/component/receiver.hpp
  include/ftxui/component/screen_interactive.hpp
  include/ftxui/component/task.hpp
  src/ftxui/component/animation.cpp
  src/ftxui/component/button.cpp
  src/ftxui/component/catch_event.cpp
  src/ftxui/component/checkbox.cpp
  src/ftxui/component/collapsible.cpp
  src/ftxui/component/component.cpp
  src/ftxui/component/component_options.cpp
  src/ftxui/component/container.cpp
  src/ftxui/component/dropdown.cpp
  src/ftxui/component/event.cpp
  src/ftxui/component/hoverable.cpp
  src/ftxui/component/input.cpp
  src/ftxui/component/loop.cpp
  src/ftxui/component/maybe.cpp
  src/ftxui/component/menu.cpp
  src/ftxui/component/modal.cpp
  src/ftxui/component/radiobox.cpp
  src/ftxui/component/radiobox.cpp
  src/ftxui/component/renderer.cpp
  src/ftxui/component/resizable_split.cpp
  src/ftxui/component/screen_interactive.cpp
  src/ftxui/component/slider.cpp
  src/ftxui/component/terminal_input_parser.cpp
  src/ftxui/component/terminal_input_parser.hpp
  src/ftxui/component/util.cpp
  src/ftxui/component/window.cpp
)

target_link_libraries(dom
  PUBLIC screen
)

target_link_libraries(component
  PUBLIC dom
)

if (NOT EMSCRIPTEN)
  find_package(Threads)
  target_link_libraries(component
    PUBLIC Threads::Threads
  )
endif()

include(cmake/ftxui_set_options.cmake)
ftxui_set_options(screen)
ftxui_set_options(dom)
ftxui_set_options(component)

add_library(ftxui::screen ALIAS screen)
add_library(ftxui::dom ALIAS dom)
add_library(ftxui::component ALIAS component)
export(
  TARGETS screen dom component
  NAMESPACE ftxui::
  FILE "${PROJECT_BINARY_DIR}/ftxui-targets.cmake"
)
