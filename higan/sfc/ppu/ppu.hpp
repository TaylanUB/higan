#include "video.hpp"

struct PPU : Thread, public PPUcounter {
  enum : bool { Threaded = true };

  PPU();
  ~PPU();

  alwaysinline auto step(uint clocks) -> void;
  alwaysinline auto synchronizeCPU() -> void;

  auto latch_counters() -> void;
  auto interlace() const -> bool;
  auto overscan() const -> bool;

  auto main() -> void;
  auto enable() -> void;
  auto power() -> void;
  auto reset() -> void;

  auto serialize(serializer&) -> void;

  uint8 vram[64 * 1024];
  uint8 oam[544];
  uint8 cgram[512];

privileged:
  uint ppu1_version = 1;  //allowed: 1
  uint ppu2_version = 3;  //allowed: 1, 2, 3

  uint32* output = nullptr;

  struct {
    bool interlace;
    bool overscan;
  } display;

  #include "background/background.hpp"
  #include "mmio/mmio.hpp"
  #include "screen/screen.hpp"
  #include "sprite/sprite.hpp"
  #include "window/window.hpp"

  Background bg1;
  Background bg2;
  Background bg3;
  Background bg4;
  Sprite sprite;
  Window window;
  Screen screen;

  static auto Enter() -> void;
  alwaysinline auto add_clocks(uint) -> void;

  auto scanline() -> void;
  auto frame() -> void;

  friend class PPU::Background;
  friend class PPU::Sprite;
  friend class PPU::Window;
  friend class PPU::Screen;
  friend class Video;

  struct Debugger {
    hook<auto (uint16, uint8) -> void> vram_read;
    hook<auto (uint16, uint8) -> void> oam_read;
    hook<auto (uint16, uint8) -> void> cgram_read;
    hook<auto (uint16, uint8) -> void> vram_write;
    hook<auto (uint16, uint8) -> void> oam_write;
    hook<auto (uint16, uint8) -> void> cgram_write;
  } debugger;
};

extern PPU ppu;
