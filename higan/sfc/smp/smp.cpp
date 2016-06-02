#include <sfc/sfc.hpp>

namespace SuperFamicom {

SMP smp;

#include "memory.cpp"
#include "timing.cpp"
#include "serialization.cpp"

auto SMP::step(uint clocks) -> void {
  clock += clocks * (uint64)cpu.frequency;
  dsp.clock -= clocks;
}

auto SMP::synchronizeCPU() -> void {
  if(CPU::Threaded) {
    if(clock >= 0 && !scheduler.synchronizing()) co_switch(cpu.thread);
  } else {
    while(clock >= 0) cpu.main();
  }
}

auto SMP::synchronizeDSP() -> void {
  if(DSP::Threaded) {
    if(dsp.clock < 0 && !scheduler.synchronizing()) co_switch(dsp.thread);
  } else {
    while(dsp.clock < 0) dsp.main();
  }
}

auto SMP::Enter() -> void {
  while(true) scheduler.synchronize(), smp.main();
}

auto SMP::main() -> void {
  debugger.op_exec(regs.pc);
  op_step();
}

auto SMP::power() -> void {
  //targets not initialized/changed upon reset
  timer0.target = 0;
  timer1.target = 0;
  timer2.target = 0;
}

auto SMP::reset() -> void {
  create(Enter, system.apuFrequency());

  regs.pc.l = iplrom[62];
  regs.pc.h = iplrom[63];
  regs.a = 0x00;
  regs.x = 0x00;
  regs.y = 0x00;
  regs.s = 0xef;
  regs.p = 0x02;

  for(auto& byte : apuram) byte = random(0x00);
  apuram[0x00f4] = 0x00;
  apuram[0x00f5] = 0x00;
  apuram[0x00f6] = 0x00;
  apuram[0x00f7] = 0x00;

  status.clockCounter = 0;
  status.dspCounter = 0;
  status.timerStep = 3;

  //$00f0
  status.clockSpeed = 0;
  status.timerSpeed = 0;
  status.timersEnable = true;
  status.ramDisable = false;
  status.ramWritable = true;
  status.timersDisable = false;

  //$00f1
  status.iplromEnable = true;

  //$00f2
  status.dspAddr = 0x00;

  //$00f8,$00f9
  status.ram00f8 = 0x00;
  status.ram00f9 = 0x00;

  timer0.stage0 = 0;
  timer1.stage0 = 0;
  timer2.stage0 = 0;

  timer0.stage1 = 0;
  timer1.stage1 = 0;
  timer2.stage1 = 0;

  timer0.stage2 = 0;
  timer1.stage2 = 0;
  timer2.stage2 = 0;

  timer0.stage3 = 0;
  timer1.stage3 = 0;
  timer2.stage3 = 0;

  timer0.line = 0;
  timer1.line = 0;
  timer2.line = 0;

  timer0.enable = false;
  timer1.enable = false;
  timer2.enable = false;
}

}
