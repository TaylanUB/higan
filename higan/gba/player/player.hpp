struct Player {
  struct Status {
    bool enable;
    bool rumble;

    bool logoDetected;
    uint logoCounter;

    uint packet;
    uint32 send;
    uint32 recv;
  } status;

  auto power() -> void;
  auto frame() -> void;

  auto keyinput() -> maybe<uint16>;
  auto read() -> maybe<uint32>;
  auto write(uint2 addr, uint8 byte) -> void;

  auto serialize(serializer& s) -> void;
};

extern Player player;
