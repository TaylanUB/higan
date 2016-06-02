struct MBC5 : MMIO {
  auto mmio_read(uint16 addr) -> uint8;
  auto mmio_write(uint16 addr, uint8 data) -> void;
  auto power() -> void;

  bool ram_enable;    //$0000-1fff
  uint16 rom_select;  //$2000-2fff + $3000-3fff
  uint8 ram_select;   //$4000-5fff
} mbc5;
