cmake_minimum_required(VERSION 3.10)
project(zelda3 VERSION 1)

find_package(SDL2 REQUIRED)

set(IMGUI_PATH  "lib/imgui")
file(GLOB IMGUI_SOURCES ${IMGUI_PATH}/*.cpp)
add_library("ImGui" STATIC ${IMGUI_SOURCES})
target_include_directories("ImGui" PUBLIC ${IMGUI_PATH})
target_include_directories(ImGui PUBLIC ${SDL2_INCLUDE_DIR})
target_compile_definitions(ImGui PUBLIC 
  IMGUI_IMPL_OPENGL_LOADER_CUSTOM=<SDL2/SDL_opengl.h>  GL_GLEXT_PROTOTYPES=1)

  set(
  IMGUI_SRC
  ${IMGUI_PATH}/imgui.cpp 
  ${IMGUI_PATH}/imgui_demo.cpp
  ${IMGUI_PATH}/imgui_draw.cpp 
  ${IMGUI_PATH}/imgui_widgets.cpp
  ${IMGUI_PATH}/backends/imgui_impl_sdl.cpp
  ${IMGUI_PATH}/backends/imgui_impl_sdlrenderer.cpp 
  ${IMGUI_PATH}/misc/cpp/imgui_stdlib.cpp
)

add_executable(
  zelda3
  ancilla.cpp 
  attract.cpp 
  dungeon.cpp 
  ending.cpp 
  hud.cpp 
  load_gfx.cpp 
  main.cpp 
  messaging.cpp 
  misc.cpp 
  nmi.cpp 
  other_modules.cpp 
  overlord.cpp 
  overworld.cpp 
  player.cpp 
  player_oam.cpp 
  poly.cpp 
  select_file.cpp 
  spc_player.cpp 
  sprite.cpp 
  sprite_main.cpp 
  tagalong.cpp 
  tile_detect.cpp 
  tracing.cpp 
  zelda_cpu_infra.cpp 
  zelda_rtl.cpp 
  snes/apu.cpp 
  snes/cart.cpp 
  snes/cpu.cpp 
  snes/dma.cpp 
  snes/dsp.cpp 
  snes/input.cpp 
  snes/ppu.cpp 
  snes/snes.cpp 
  snes/snes_other.cpp 
  snes/spc.cpp
  ${IMGUI_SRC}
)

target_include_directories(
  zelda3 PUBLIC
  ./
  lib/
  ${SDL2_INCLUDE_DIR}
)

target_link_libraries(zelda3
  SDL2::SDL2
  ImGui
)