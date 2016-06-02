//NES-GNROM
//NES-MHROM

struct NES_GxROM : Board {
  NES_GxROM(Markup::Node& document) : Board(document) {
    settings.mirror = document["board/mirror/mode"].text() == "vertical" ? 1 : 0;
  }

  auto prg_read(uint addr) -> uint8 {
    if(addr & 0x8000) return prgrom.read((prg_bank << 15) | (addr & 0x7fff));
    return cpu.mdr();
  }

  auto prg_write(uint addr, uint8 data) -> void {
    if(addr & 0x8000) {
      prg_bank = (data & 0x30) >> 4;
      chr_bank = (data & 0x03) >> 0;
    }
  }

  auto chr_read(uint addr) -> uint8 {
    if(addr & 0x2000) {
      if(settings.mirror == 0) addr = ((addr & 0x0800) >> 1) | (addr & 0x03ff);
      return ppu.ciram_read(addr & 0x07ff);
    }
    addr = (chr_bank * 0x2000) + (addr & 0x1fff);
    return Board::chr_read(addr);
  }

  auto chr_write(uint addr, uint8 data) -> void {
    if(addr & 0x2000) {
      if(settings.mirror == 0) addr = ((addr & 0x0800) >> 1) | (addr & 0x03ff);
      return ppu.ciram_write(addr & 0x07ff, data);
    }
    addr = (chr_bank * 0x2000) + (addr & 0x1fff);
    Board::chr_write(addr, data);
  }

  auto power() -> void {
  }

  auto reset() -> void {
    prg_bank = 0;
    chr_bank = 0;
  }

  auto serialize(serializer& s) -> void {
    Board::serialize(s);
    s.integer(prg_bank);
    s.integer(chr_bank);
  }

  struct Settings {
    bool mirror;  //0 = horizontal, 1 = vertical
  } settings;

  uint2 prg_bank;
  uint2 chr_bank;
};
