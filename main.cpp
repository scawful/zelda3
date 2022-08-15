
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include <SDL.h>

#include <imgui/backends/imgui_impl_sdl.h>
#include <imgui/backends/imgui_impl_sdlrenderer.h>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <string>

#include "snes/snes.h"
#include "tracing.h"

#include "types.h"
#include "variables.h"

#include "zelda_rtl.h"

extern uint8 g_emulated_ram[0x20000];
bool g_run_without_emu = false;

void PatchRom(uint8_t *rom);
void SetSnes(Snes *snes);
void RunAudioPlayer();
void CopyStateAfterSnapshotRestore(bool is_reset);
void SaveLoadSlot(int cmd, int which);
void PatchCommand(char cmd);
bool RunOneFrame(Snes *snes, int input_state, bool turbo);

static uint8_t* readFile(char* name, size_t* length);
static bool loadRom(char* name, Snes* snes);
static bool checkExtention(const char* name, bool forZip);
static void playAudio(Snes *snes, SDL_AudioDeviceID device, int16_t* audioBuffer);
static void renderScreen(SDL_Renderer* renderer, SDL_Texture* texture);
static void handleInput(int keyCode, int modCode, bool pressed);

int input1_current_state;

void setButtonState(int button, bool pressed) {
  // set key in constroller
  if (pressed) {
    input1_current_state |= 1 << button;
  } else {
    input1_current_state &= ~(1 << button);
  }
}

void InitializeKeymap() {
  ImGuiIO &io = ImGui::GetIO();
  io.KeyMap[ImGuiKey_Backspace] = SDL_GetScancodeFromKey(SDLK_BACKSPACE);
  io.KeyMap[ImGuiKey_Enter] = SDL_GetScancodeFromKey(SDLK_RETURN);
  io.KeyMap[ImGuiKey_UpArrow] = SDL_GetScancodeFromKey(SDLK_UP);
  io.KeyMap[ImGuiKey_DownArrow] = SDL_GetScancodeFromKey(SDLK_DOWN);
  io.KeyMap[ImGuiKey_Tab] = SDL_GetScancodeFromKey(SDLK_TAB);
  io.KeyMap[ImGuiKey_LeftCtrl] = SDL_GetScancodeFromKey(SDLK_LCTRL);
}

void HandleKeyDown(SDL_Event &event) {
  ImGuiIO &io = ImGui::GetIO();
  switch (event.key.keysym.sym) {
    case SDLK_UP:
    case SDLK_DOWN:
    case SDLK_RETURN:
    case SDLK_BACKSPACE:
    case SDLK_TAB:
      io.KeysDown[event.key.keysym.scancode] = (event.type == SDL_KEYDOWN);
      break;
    default:
      break;
  }
}

void HandleKeyUp(SDL_Event &event) {
  ImGuiIO &io = ImGui::GetIO();
  int key = event.key.keysym.scancode;
  IM_ASSERT(key >= 0 && key < IM_ARRAYSIZE(io.KeysDown));
  io.KeysDown[key] = (event.type == SDL_KEYDOWN);
  io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
  io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
  io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
  io.KeySuper = ((SDL_GetModState() & KMOD_GUI) != 0);
}

void ChangeWindowSizeEvent(SDL_Event &event) {
  ImGuiIO &io = ImGui::GetIO();
  io.DisplaySize.x = static_cast<float>(event.window.data1);
  io.DisplaySize.y = static_cast<float>(event.window.data2);
}

void HandleMouseMovement(int &wheel) {
  ImGuiIO &io = ImGui::GetIO();
  int mouseX;
  int mouseY;
  const int buttons = SDL_GetMouseState(&mouseX, &mouseY);

  io.DeltaTime = 1.0f / 60.0f;
  io.MousePos = ImVec2(static_cast<float>(mouseX), static_cast<float>(mouseY));
  io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
  io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);
  io.MouseWheel = static_cast<float>(wheel);
}


#undef main
int main(int argc, char** argv) {
  // set up SDL
  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
    printf("Failed to init SDL: %s\n", SDL_GetError());
    return 1;
  }
  SDL_Window* window = SDL_CreateWindow("Zelda3", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 512, 500, SDL_WINDOW_RESIZABLE);
  if(window == NULL) {
    printf("Failed to create window: %s\n", SDL_GetError());
    return 1;
  }
  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if(renderer == NULL) {
    printf("Failed to create renderer: %s\n", SDL_GetError());
    return 1;
  }
  // create imgui context 
  ImGui::CreateContext();
  ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
  ImGui_ImplSDLRenderer_Init(renderer);
  ImGui_ImplSDLRenderer_NewFrame();
  ImGui_ImplSDL2_NewFrame(window);
  SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBX8888, SDL_TEXTUREACCESS_STREAMING, 512, 480);
  if(texture == NULL) {
    printf("Failed to create texture: %s\n", SDL_GetError());
    return 1;
  }
  SDL_AudioSpec want, have;
  SDL_AudioDeviceID device;
  SDL_memset(&want, 0, sizeof(want));
  want.freq = 44100;
  want.format = AUDIO_S16;
  want.channels = 2;
  want.samples = 2048;
  want.callback = NULL; // use queue
  device = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
  if(device == 0) {
    printf("Failed to open audio device: %s\n", SDL_GetError());
    return 1;
  }
  int16_t* audioBuffer = (int16_t * )malloc(735 * 4); // *2 for stereo, *2 for sizeof(int16)
  SDL_PauseAudioDevice(device, 0);

  Snes *snes = snes_init(g_emulated_ram), *snes_run = NULL;
  if (argc >= 2 && !g_run_without_emu) {
    // init snes, load rom
    bool loaded = loadRom(argv[1], snes);
    if (!loaded) {
      puts("No rom loaded");
      return 1;
    }
    snes_run = snes;
  } else {
    snes_reset(snes, true);
  }
  SetSnes(snes);
  ZeldaInitialize();
  bool hooks = true;
  // sdl loop
  bool running = true;
  SDL_Event event;
  uint32_t lastTick = SDL_GetTicks();
  uint32_t curTick = 0;
  uint32_t delta = 0;
  int numFrames = 0;
  bool cpuNext = false;
  bool spcNext = false;
  int counter = 0;
  bool paused = false;
  bool turbo = true;
  uint32_t frameCtr = 0;
  int wheel = 0;
  printf("%d\n", *(int *)snes->cart->ram);

  while(running) {
    while(SDL_PollEvent(&event)) {
      switch(event.type) {
        case SDL_KEYDOWN: {
          switch(event.key.keysym.sym) {
            case SDLK_e:
              if (snes) {
                snes_reset(snes, event.key.keysym.sym == SDLK_e);
                CopyStateAfterSnapshotRestore(true);
              }
              break;
            case SDLK_p: paused ^= true; break;
            case SDLK_w:
              PatchCommand('w');
              break;
            case SDLK_o:
              PatchCommand('o');
              break;
            case SDLK_k:
              PatchCommand('k');
              break;
            case SDLK_t:
              turbo = !turbo;
              break;
          }
          handleInput(event.key.keysym.sym, event.key.keysym.mod, true);
          HandleKeyDown(event);
          break;
        }
        case SDL_WINDOWEVENT:
        switch (event.window.event) {
          case SDL_WINDOWEVENT_SIZE_CHANGED:
            ChangeWindowSizeEvent(event);
            break;
          default:
            break;
        }
        break;
        case SDL_KEYUP: {
          handleInput(event.key.keysym.sym, event.key.keysym.mod, false);
          HandleKeyUp(event);
          break;
        }
        case SDL_QUIT: {
          running = false;
          break;
        }
      }
      HandleMouseMovement(wheel);
    }

    if (!paused) {
      bool is_turbo = RunOneFrame(snes_run, input1_current_state, (counter++ & 0x7f) != 0 && turbo);

      if (is_turbo)
        continue;
    }

    ZeldaDrawPpuFrame();

    if (!paused)
      playAudio(snes_run, device, audioBuffer);

    renderScreen(renderer, texture);

    // draw imgui overlay
    const ImGuiIO &io = ImGui::GetIO();
    ImGui::NewFrame();
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImVec2 dimensions(io.DisplaySize.x, 20);
    ImGui::SetNextWindowSize(dimensions, ImGuiCond_Always);

    if (!ImGui::Begin("##zelda3_overlay", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse |
    ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_MenuBar |
    ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoTitleBar)) {
      ImGui::End();
    }
        
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
          if (ImGui::MenuItem("Import ROM")) {

          }
          ImGui::Separator();
          if (ImGui::MenuItem("Pause")) {
            paused ^= true;
          }
          if (ImGui::MenuItem("Reset")) {
            if (snes) {
              snes_reset(snes, event.key.keysym.sym == SDLK_e);
              CopyStateAfterSnapshotRestore(true);
            }
          }
          
          ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Snapshot"))
        {
            if (ImGui::BeginMenu("Save Snapshot"))
            {
              ImGui::MenuItem("#1", "F1");
              ImGui::MenuItem("#2", "F2");
              ImGui::MenuItem("#3", "F3");
              ImGui::MenuItem("#4", "F4");
              ImGui::MenuItem("#5", "F5");
              ImGui::MenuItem("#6", "F6");
              ImGui::MenuItem("#7", "F7");
              ImGui::MenuItem("#8", "F8");
              ImGui::MenuItem("#9", "F9");
              ImGui::MenuItem("#10", "F10");
              ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Load Snapshot")) {
              ImGui::MenuItem("#1", "Shift + F1");
              ImGui::MenuItem("#2", "Shift + F2");
              ImGui::MenuItem("#3", "Shift + F3");
              ImGui::MenuItem("#4", "Shift + F4");
              ImGui::MenuItem("#5", "Shift + F5");
              ImGui::MenuItem("#6", "Shift + F6");
              ImGui::MenuItem("#7", "Shift + F7");
              ImGui::MenuItem("#8", "Shift + F8");
              ImGui::MenuItem("#9", "Shift + F9");
              ImGui::MenuItem("#10", "Shift + F10");
              ImGui::EndMenu();
            }

            ImGui::MenuItem("Replay");
            ImGui::Separator();
            ImGui::MenuItem("Clear History");
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    ImGui::End();

    // render the imgui 
    ImGui::Render();
    ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());

    SDL_RenderPresent(renderer); // vsyncs to 60 FPS
    // if vsync isn't working, delay manually
    curTick = SDL_GetTicks();

    static const uint8 delays[3] = { 17, 17, 16 }; // 60 fps
#if 1
    lastTick += delays[frameCtr++ % 3];

    if (lastTick > curTick) {
      delta = lastTick - curTick;
      if (delta > 500) {
        lastTick = curTick - 500;
        delta = 500;
      }
      SDL_Delay(delta);
    } else if (curTick - lastTick > 500) {
      lastTick = curTick;
    }
#endif
  }
  // clean snes
  snes_free(snes);
  // clean imgui 
  ImGui_ImplSDLRenderer_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();
  // clean sdl
  SDL_PauseAudioDevice(device, 1);
  SDL_CloseAudioDevice(device);
  free(audioBuffer);
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}

extern struct Ppu *GetPpuForRendering();
extern struct Dsp *GetDspForRendering();

static void playAudio(Snes *snes, SDL_AudioDeviceID device, int16_t* audioBuffer) {
  // generate enough samples
  if (!kIsOrigEmu && snes) {
    while (snes->apu->dsp->sampleOffset < 534)
      apu_cycle(snes->apu);
    snes->apu->dsp->sampleOffset = 0;
  }

  dsp_getSamples(GetDspForRendering(), audioBuffer, 735);
  if(SDL_GetQueuedAudioSize(device) <= 735 * 4 * 6) {
    // don't queue audio if buffer is still filled
    SDL_QueueAudio(device, audioBuffer, 735 * 4);
  } else {
    printf("Skipping audio!\n");
  }
}

static void renderScreen(SDL_Renderer* renderer, SDL_Texture* texture) {
  void* pixels = NULL;
  int pitch = 0;
  if(SDL_LockTexture(texture, NULL, &pixels, &pitch) != 0) {
    printf("Failed to lock texture: %s\n", SDL_GetError());
    return;
  }

  ppu_putPixels(GetPpuForRendering(), (uint8_t*) pixels);
  SDL_UnlockTexture(texture);
  SDL_Rect textue_offset_rect;
  textue_offset_rect.x = 0;
  textue_offset_rect.y = 20;
  textue_offset_rect.w = 512;
  textue_offset_rect.h = 480;
  SDL_RenderCopy(renderer, texture, NULL, &textue_offset_rect);
}


static void handleInput(int keyCode, int keyMod, bool pressed) {
  switch(keyCode) {
    case SDLK_z: setButtonState(0, pressed); break;
    case SDLK_a: setButtonState(1, pressed); break;
    case SDLK_RSHIFT: setButtonState(2, pressed); break;
    case SDLK_RETURN: setButtonState(3, pressed); break;
    case SDLK_UP: setButtonState(4, pressed); break;
    case SDLK_DOWN: setButtonState(5, pressed); break;
    case SDLK_LEFT: setButtonState(6, pressed); break;
    case SDLK_RIGHT: setButtonState(7, pressed); break;
    case SDLK_x: setButtonState(8, pressed); break;
    case SDLK_s: setButtonState(9, pressed); break;
    case SDLK_d: setButtonState(10, pressed); break;
    case SDLK_c: setButtonState(11, pressed); break;
    case SDLK_BACKSPACE:
    case SDLK_1:
    case SDLK_2:
    case SDLK_3:
    case SDLK_4:
    case SDLK_5:
    case SDLK_6:
    case SDLK_7:
    case SDLK_8:
    case SDLK_9:
    case SDLK_0:
    case SDLK_MINUS:
    case SDLK_EQUALS:
      if (pressed) {
        SaveLoadSlot(
          (keyMod & KMOD_CTRL) != 0 ? kSaveLoad_Replay : kSaveLoad_Load,
          256 + (keyCode == SDLK_0 ? 9 : 
                 keyCode == SDLK_MINUS ? 10 : 
                 keyCode == SDLK_EQUALS ? 11 :
                 keyCode == SDLK_BACKSPACE ? 12 :
                 keyCode - SDLK_1));
      }
      break;

    case SDLK_F1: 
    case SDLK_F2: 
    case SDLK_F3: 
    case SDLK_F4: 
    case SDLK_F5: 
    case SDLK_F6: 
    case SDLK_F7: 
    case SDLK_F8: 
    case SDLK_F9: 
    case SDLK_F10: 
      if (pressed) {
        SaveLoadSlot(
          (keyMod & KMOD_CTRL) != 0 ? kSaveLoad_Replay : 
          (keyMod & KMOD_SHIFT) != 0 ? kSaveLoad_Save : kSaveLoad_Load,
          keyCode - SDLK_F1);
      }
      break;
  }
}

static bool checkExtention(const char* name, bool forZip) {
  if(name == NULL) return false;
  int length = strlen(name);
  if(length < 4) return false;
  if(forZip) {
    if(strcmp(name + length - 4, ".zip") == 0) return true;
    if(strcmp(name + length - 4, ".ZIP") == 0) return true;
  } else {
    if(strcmp(name + length - 4, ".smc") == 0) return true;
    if(strcmp(name + length - 4, ".SMC") == 0) return true;
    if(strcmp(name + length - 4, ".sfc") == 0) return true;
    if(strcmp(name + length - 4, ".SFC") == 0) return true;
  }
  return false;
}

static bool loadRom(char* name, Snes* snes) {
  // zip library from https://github.com/kuba--/zip
  size_t length = 0;
  uint8_t* file = NULL;
  file = readFile(name, &length);
  if(file == NULL) {
    puts("Failed to read file");
    return false;
  }

  PatchRom(file);

  bool result = snes_loadRom(snes, file, length);
  free(file);
  return result;
}

static uint8_t* readFile(char* name, size_t* length) {
  FILE* f = fopen(name, "rb");
  if(f == NULL) {
    return NULL;
  }
  fseek(f, 0, SEEK_END);
  int size = ftell(f);
  rewind(f);
  uint8_t* buffer = (uint8_t *)malloc(size);
  fread(buffer, size, 1, f);
  fclose(f);
  *length = size;
  return buffer;
}
