struct GPR {
  inline operator uint32_t() const { return data; }
  inline auto operator=(uint32_t n) { data = n; if(modify) modify(); return *this; }
  inline auto operator=(const GPR& source) { return operator=(source.data); }

  inline auto operator &=(uint32_t n) { return operator=(data  & n); }
  inline auto operator |=(uint32_t n) { return operator=(data  | n); }
  inline auto operator ^=(uint32_t n) { return operator=(data  ^ n); }
  inline auto operator +=(uint32_t n) { return operator=(data  + n); }
  inline auto operator -=(uint32_t n) { return operator=(data  - n); }
  inline auto operator *=(uint32_t n) { return operator=(data  * n); }
  inline auto operator /=(uint32_t n) { return operator=(data  / n); }
  inline auto operator %=(uint32_t n) { return operator=(data  % n); }
  inline auto operator<<=(uint32_t n) { return operator=(data << n); }
  inline auto operator>>=(uint32_t n) { return operator=(data >> n); }

  uint32_t data = 0;
  function<auto () -> void> modify;
};

struct PSR {
  inline operator uint32_t() const {
    return (n << 31) + (z << 30) + (c << 29) + (v << 28)
         + (i <<  7) + (f <<  6) + (t <<  5) + (m <<  0);
  }

  inline auto operator=(uint32_t d) {
    n = d & (1 << 31);
    z = d & (1 << 30);
    c = d & (1 << 29);
    v = d & (1 << 28);
    i = d & (1 <<  7);
    f = d & (1 <<  6);
    t = d & (1 <<  5);
    m = d & 31;
    return *this;
  }

  auto serialize(serializer&) -> void;

  bool n = false;  //negative
  bool z = false;  //zero
  bool c = false;  //carry
  bool v = false;  //overflow
  bool i = false;  //irq
  bool f = false;  //fiq
  bool t = false;  //thumb
  uint m = 0;      //mode
};

struct Pipeline {
  bool reload = false;
  bool nonsequential = false;

  struct Instruction {
    uint32 address = 0;
    uint32 instruction = 0;
  };

  Instruction execute;
  Instruction decode;
  Instruction fetch;
};

struct Processor {
  enum class Mode : unsigned {
    USR = 0x10,  //user
    FIQ = 0x11,  //fast interrupt request
    IRQ = 0x12,  //interrupt request
    SVC = 0x13,  //supervisor (software interrupt)
    ABT = 0x17,  //abort
    UND = 0x1b,  //undefined
    SYS = 0x1f,  //system
  };

  GPR r0, r1, r2, r3, r4, r5, r6, r7;

  struct USR {
    GPR r8, r9, r10, r11, r12, sp, lr;
  } usr;

  struct FIQ {
    GPR r8, r9, r10, r11, r12, sp, lr;
    PSR spsr;
  } fiq;

  struct IRQ {
    GPR sp, lr;
    PSR spsr;
  } irq;

  struct SVC {
    GPR sp, lr;
    PSR spsr;
  } svc;

  struct ABT {
    GPR sp, lr;
    PSR spsr;
  } abt;

  struct UND {
    GPR sp, lr;
    PSR spsr;
  } und;

  GPR pc;
  PSR cpsr;
  bool carryout = false;
  bool irqline = false;

  GPR* r[16] = {nullptr};
  PSR* spsr = nullptr;

  auto power() -> void;
  auto setMode(Mode) -> void;
};

Processor processor;
Pipeline pipeline;
bool crash = false;

alwaysinline auto r(uint n) -> GPR& { return *processor.r[n]; }
alwaysinline auto cpsr() -> PSR& { return processor.cpsr; }
alwaysinline auto spsr() -> PSR& { return *processor.spsr; }
alwaysinline auto carryout() -> bool& { return processor.carryout; }
alwaysinline auto instruction() -> uint32 { return pipeline.execute.instruction; }
alwaysinline auto mode() -> Processor::Mode { return (Processor::Mode)processor.cpsr.m; }
alwaysinline auto privilegedMode() const -> bool { return (Processor::Mode)processor.cpsr.m != Processor::Mode::USR; }
alwaysinline auto exceptionMode() const -> bool { return privilegedMode() && (Processor::Mode)processor.cpsr.m != Processor::Mode::SYS; }
