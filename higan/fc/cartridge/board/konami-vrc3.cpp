struct KonamiVRC3 : Board {
  KonamiVRC3(Markup::Node& document) : Board(document), vrc3(*this) {
    settings.mirror = document["board/mirror/mode"].text() == "vertical" ? 1 : 0;
  }

  auto main() -> void {
    vrc3.main();
  }

  auto prg_read(uint addr) -> uint8 {
    if((addr & 0xe000) == 0x6000) return prgram.read(addr & 0x1fff);
    if(addr & 0x8000) return prgrom.read(vrc3.prg_addr(addr));
    return cpu.mdr();
  }

  auto prg_write(uint addr, uint8 data) -> void {
    if((addr & 0xe000) == 0x6000) return prgram.write(addr & 0x1fff, data);
    if(addr & 0x8000) return vrc3.reg_write(addr, data);
  }

  auto chr_read(uint addr) -> uint8 {
    if(addr & 0x2000) {
      if(settings.mirror == 0) addr = ((addr & 0x0800) >> 1) | (addr & 0x03ff);
      return ppu.ciram_read(addr & 0x07ff);
    }
    return chrram.read(addr);
  }

  auto chr_write(uint addr, uint8 data) -> void {
    if(addr & 0x2000) {
      if(settings.mirror == 0) addr = ((addr & 0x0800) >> 1) | (addr & 0x03ff);
      return ppu.ciram_write(addr & 0x07ff, data);
    }
    return chrram.write(addr, data);
  }

  auto power() -> void {
    vrc3.power();
  }

  auto reset() -> void {
    vrc3.reset();
  }

  auto serialize(serializer& s) -> void {
    Board::serialize(s);
    vrc3.serialize(s);
  }

  struct Settings {
    bool mirror;  //0 = horizontal, 1 = vertical
  } settings;

  VRC3 vrc3;
};
