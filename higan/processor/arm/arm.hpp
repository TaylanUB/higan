#pragma once

namespace Processor {

//Supported Models:
//* ARMv3  (ARM60)
//* ARMv4T (ARM7TDMI)

struct ARM {
  enum : unsigned {       //mode flags for bus_read, bus_write:
    Nonsequential =   1,  //N cycle
    Sequential    =   2,  //S cycle
    Prefetch      =   4,  //instruction fetch (eligible for prefetch)
    Byte          =   8,  //8-bit access
    Half          =  16,  //16-bit access
    Word          =  32,  //32-bit access
    Load          =  64,  //load operation
    Store         = 128,  //store operation
    Signed        = 256,  //sign extend
  };

  #include "registers.hpp"
  #include "instructions-arm.hpp"
  #include "instructions-thumb.hpp"
  #include "disassembler.hpp"

  virtual auto step(unsigned clocks) -> void = 0;
  virtual auto bus_idle() -> void = 0;
  virtual auto bus_read(unsigned mode, uint32 addr) -> uint32 = 0;
  virtual auto bus_write(unsigned mode, uint32 addr, uint32 word) -> void = 0;

  //arm.cpp
  auto power() -> void;
  auto exec() -> void;
  auto idle() -> void;
  auto read(unsigned mode, uint32 addr) -> uint32;
  auto load(unsigned mode, uint32 addr) -> uint32;
  auto write(unsigned mode, uint32 addr, uint32 word) -> void;
  auto store(unsigned mode, uint32 addr, uint32 word) -> void;
  auto vector(uint32 addr, Processor::Mode mode) -> void;

  //algorithms.cpp
  auto condition(uint4 condition) -> bool;
  auto bit(uint32 result) -> uint32;
  auto add(uint32 source, uint32 modify, bool carry) -> uint32;
  auto sub(uint32 source, uint32 modify, bool carry) -> uint32;
  auto mul(uint32 product, uint32 multiplicand, uint32 multiplier) -> uint32;
  auto lsl(uint32 source, uint8 shift) -> uint32;
  auto lsr(uint32 source, uint8 shift) -> uint32;
  auto asr(uint32 source, uint8 shift) -> uint32;
  auto ror(uint32 source, uint8 shift) -> uint32;
  auto rrx(uint32 source) -> uint32;

  //step.cpp
  auto pipeline_step() -> void;
  auto arm_step() -> void;
  auto thumb_step() -> void;

  //serialization.cpp
  auto serialize(serializer&) -> void;

  bool trace = false;
  uintmax_t instructions = 0;
};

}
