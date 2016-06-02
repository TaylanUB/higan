struct Sprite {
  struct Object {
    uint9 x;
    uint8 y;
    uint8 character;
    uint1 nameselect;
    uint1 vflip;
    uint1 hflip;
    uint2 priority;
    uint3 palette;
    uint1 size;
    alwaysinline auto width() const -> uint;
    alwaysinline auto height() const -> uint;
  } list[128];

  struct Item {
    bool  valid;
    uint7 index;
  };

  struct Tile {
    bool  valid;
    uint9 x;
    uint2 priority;
    uint8 palette;
    uint1 hflip;
    uint8 d0, d1, d2, d3;
  };

  struct State {
    uint x;
    uint y;

    uint item_count;
    uint tile_count;

    bool active;
    Item item[2][32];
    Tile tile[2][34];
  } t;

  struct Regs {
    bool main_enable;
    bool sub_enable;
    bool interlace;

    uint3  base_size;
    uint2  nameselect;
    uint16 tiledata_addr;
    uint7  first_sprite;

    uint priority0;
    uint priority1;
    uint priority2;
    uint priority3;

    bool time_over;
    bool range_over;
  } regs;

  struct Output {
    struct Pixel {
      uint  priority;  //0 = none (transparent)
      uint8 palette;
    } main, sub;
  } output;

  Sprite(PPU& self);

  //list.cpp
  auto update(uint10 addr, uint8 data) -> void;
  auto synchronize() -> void;

  //sprite.cpp
  alwaysinline auto address_reset() -> void;
  alwaysinline auto set_first_sprite() -> void;
  auto frame() -> void;
  auto scanline() -> void;
  auto run() -> void;
  auto tilefetch() -> void;
  auto reset() -> void;

  auto on_scanline(Object&) -> bool;

  auto serialize(serializer&) -> void;

  PPU& self;
  friend class PPU;
};
