//Sony CXP1100Q-1

struct SMP : Processor::SPC700, Thread {
  enum : bool { Threaded = true };

  alwaysinline auto step(uint clocks) -> void;
  alwaysinline auto synchronizeCPU() -> void;
  alwaysinline auto synchronizeDSP() -> void;

  auto portRead(uint2 port) const -> uint8;
  auto portWrite(uint2 port, uint8 data) -> void;

  auto main() -> void;
  auto power() -> void;
  auto reset() -> void;

  auto serialize(serializer&) -> void;

  uint8 iplrom[64];
  uint8 apuram[64 * 1024];

privileged:
  struct {
    //timing
    uint clockCounter;
    uint dspCounter;
    uint timerStep;

    //$00f0
    uint8 clockSpeed;
    uint8 timerSpeed;
    bool timersEnable;
    bool ramDisable;
    bool ramWritable;
    bool timersDisable;

    //$00f1
    bool iplromEnable;

    //$00f2
    uint8 dspAddr;

    //$00f8,$00f9
    uint8 ram00f8;
    uint8 ram00f9;
  } status;

  static auto Enter() -> void;

  struct Debugger {
    hook<void (uint16)> op_exec;
    hook<void (uint16, uint8)> op_read;
    hook<void (uint16, uint8)> op_write;
  } debugger;

  //memory.cpp
  auto ramRead(uint16 addr) -> uint8;
  auto ramWrite(uint16 addr, uint8 data) -> void;

  auto busRead(uint16 addr) -> uint8;
  auto busWrite(uint16 addr, uint8 data) -> void;

  auto op_io() -> void;
  auto op_read(uint16 addr) -> uint8;
  auto op_write(uint16 addr, uint8 data) -> void;

  auto disassembler_read(uint16 addr) -> uint8;

  //timing.cpp
  template<unsigned Frequency>
  struct Timer {
    uint8 stage0;
    uint8 stage1;
    uint8 stage2;
    uint4 stage3;
    bool line;
    bool enable;
    uint8 target;

    auto tick() -> void;
    auto synchronizeStage1() -> void;
  };

  Timer<192> timer0;
  Timer<192> timer1;
  Timer< 24> timer2;

  alwaysinline auto addClocks(uint clocks) -> void;
  alwaysinline auto cycleEdge() -> void;
};

extern SMP smp;
