cmake_minimum_required(VERSION 3.10)
project(zelda3 VERSION 1)
find_package(SDL2 REQUIRED)
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
)

target_include_directories(
  zelda3 PUBLIC
  ./
  ${SDL2_INCLUDE_DIR}
)

target_link_libraries(zelda3
  SDL2::SDL2
)