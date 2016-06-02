struct Video {
  Video();

  auto reset() -> void;
  auto refresh() -> void;

private:
  auto drawCursor(uint32 color, int x, int y) -> void;
  auto drawCursors() -> void;

  unique_pointer<uint32[]> output;
  unique_pointer<uint32[]> paletteLiteral;
  unique_pointer<uint32[]> paletteStandard;
  unique_pointer<uint32[]> paletteEmulation;
};

extern Video video;
