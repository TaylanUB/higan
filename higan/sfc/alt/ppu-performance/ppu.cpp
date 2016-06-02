#include <sfc/sfc.hpp>

namespace SuperFamicom {

#include <sfc/ppu/video.cpp>
PPU ppu;

#include "mmio/mmio.cpp"
#include "window/window.cpp"
#include "cache/cache.cpp"
#include "background/background.cpp"
#include "sprite/sprite.cpp"
#include "screen/screen.cpp"
#include "serialization.cpp"

PPU::PPU() :
cache(*this),
bg1(*this, Background::ID::BG1),
bg2(*this, Background::ID::BG2),
bg3(*this, Background::ID::BG3),
bg4(*this, Background::ID::BG4),
sprite(*this),
screen(*this) {
  output = new uint32[512 * 512]();
  output += 16 * 512;  //overscan offset
  display.width = 256;
  display.height = 224;
  display.frameskip = 0;
  display.framecounter = 0;
}

PPU::~PPU() {
  output -= 16 * 512;
  delete[] output;
}

auto PPU::step(uint clocks) -> void {
  clock += clocks;
}

auto PPU::synchronizeCPU() -> void {
  if(CPU::Threaded == true) {
    if(clock >= 0 && !scheduler.synchronizing()) co_switch(cpu.thread);
  } else {
    while(clock >= 0) cpu.main();
  }
}

auto PPU::Enter() -> void {
  while(true) scheduler.synchronize(), ppu.main();
}

auto PPU::main() -> void {
  scanline();
  if(vcounter() < display.height && vcounter()) {
    add_clocks(512);
    render_scanline();
    add_clocks(lineclocks() - 512);
  } else {
    add_clocks(lineclocks());
  }
}

auto PPU::add_clocks(uint clocks) -> void {
  tick(clocks);
  step(clocks);
  synchronizeCPU();
}

auto PPU::render_scanline() -> void {
  if(display.framecounter) return;  //skip this frame?
  bg1.scanline();
  bg2.scanline();
  bg3.scanline();
  bg4.scanline();
  if(regs.display_disable) return screen.render_black();
  screen.scanline();
  bg1.render();
  bg2.render();
  bg3.render();
  bg4.render();
  sprite.render();
  screen.render();
}

auto PPU::scanline() -> void {
  display.width = 512;
  display.height = !overscan() ? 225 : 240;
  if(vcounter() == 0) frame();
  if(vcounter() == display.height && regs.display_disable == false) sprite.address_reset();

  if(vcounter() == 241) {
    video.refresh();
    scheduler.exit(Scheduler::Event::Frame);
  }
}

auto PPU::frame() -> void {
  sprite.frame();
  display.interlace = regs.interlace;
  display.overscan = regs.overscan;
  display.framecounter = display.frameskip == 0 ? 0 : (display.framecounter + 1) % display.frameskip;
}

auto PPU::enable() -> void {
  function<auto (uint, uint8) -> uint8> reader{&PPU::mmio_read, (PPU*)&ppu};
  function<auto (uint, uint8) -> void> writer{&PPU::mmio_write, (PPU*)&ppu};

  bus.map(reader, writer, 0x00, 0x3f, 0x2100, 0x213f);
  bus.map(reader, writer, 0x80, 0xbf, 0x2100, 0x213f);
}

auto PPU::power() -> void {
  for(auto& n : vram) n = 0;
  for(auto& n : oam) n = 0;
  for(auto& n : cgram) n = 0;
  reset();
}

auto PPU::reset() -> void {
  create(Enter, system.cpuFrequency());
  PPUcounter::reset();
  memset(output, 0, 512 * 480 * sizeof(uint32));
  mmio_reset();
  display.interlace = false;
  display.overscan = false;
  video.reset();
}

auto PPU::layer_enable(uint layer, uint priority, bool enable) -> void {
  switch(layer * 4 + priority) {
  case  0: bg1.priority0_enable = enable; break;
  case  1: bg1.priority1_enable = enable; break;
  case  4: bg2.priority0_enable = enable; break;
  case  5: bg2.priority1_enable = enable; break;
  case  8: bg3.priority0_enable = enable; break;
  case  9: bg3.priority1_enable = enable; break;
  case 12: bg4.priority0_enable = enable; break;
  case 13: bg4.priority1_enable = enable; break;
  case 16: sprite.priority0_enable = enable; break;
  case 17: sprite.priority1_enable = enable; break;
  case 18: sprite.priority2_enable = enable; break;
  case 19: sprite.priority3_enable = enable; break;
  }
}

auto PPU::set_frameskip(uint frameskip) -> void {
  display.frameskip = frameskip;
  display.framecounter = 0;
}

}
