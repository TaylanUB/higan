#include <gb/gb.hpp>

namespace GameBoy {

#include "sequencer/sequencer.cpp"
#include "square1/square1.cpp"
#include "square2/square2.cpp"
#include "wave/wave.cpp"
#include "noise/noise.cpp"
#include "serialization.cpp"
APU apu;

auto APU::Enter() -> void {
  while(true) scheduler.synchronize(), apu.main();
}

auto APU::main() -> void {
  square1.run();
  square2.run();
  wave.run();
  noise.run();
  sequencer.run();

  hipass(sequencer.center, sequencer.centerBias);
  hipass(sequencer.left, sequencer.leftBias);
  hipass(sequencer.right, sequencer.rightBias);

  interface->audioSample(sequencer.left, sequencer.right);

  if(cycle == 0) {  //512hz
    if(phase == 0 || phase == 2 || phase == 4 || phase == 6) {  //256hz
      square1.clockLength();
      square2.clockLength();
      wave.clockLength();
      noise.clockLength();
    }
    if(phase == 2 || phase == 6) {  //128hz
      square1.clockSweep();
    }
    if(phase == 7) {  //64hz
      square1.clockEnvelope();
      square2.clockEnvelope();
      noise.clockEnvelope();
    }
    phase++;
  }
  cycle++;

  clock += cpu.frequency;
  if(clock >= 0 && !scheduler.synchronizing()) co_switch(cpu.thread);
}

//filter to remove DC bias
auto APU::hipass(int16& sample, int64& bias) -> void {
  bias += ((((int64)sample << 16) - (bias >> 16)) * 57593) >> 16;
  sample = sclamp<16>(sample - (bias >> 32));
}

auto APU::power() -> void {
  create(Enter, 2 * 1024 * 1024);
  for(uint n = 0xff10; n <= 0xff3f; n++) bus.mmio[n] = this;

  square1.power();
  square2.power();
  wave.power();
  noise.power();
  sequencer.power();
  phase = 0;
  cycle = 0;

  LinearFeedbackShiftRegisterGenerator r;
  for(auto& n : wave.pattern) n = r();
}

auto APU::mmio_read(uint16 addr) -> uint8 {
  if(addr >= 0xff10 && addr <= 0xff14) return square1.read(addr);
  if(addr >= 0xff15 && addr <= 0xff19) return square2.read(addr);
  if(addr >= 0xff1a && addr <= 0xff1e) return wave.read(addr);
  if(addr >= 0xff1f && addr <= 0xff23) return noise.read(addr);
  if(addr >= 0xff24 && addr <= 0xff26) return sequencer.read(addr);
  if(addr >= 0xff30 && addr <= 0xff3f) return wave.read(addr);
  return 0xff;
}

auto APU::mmio_write(uint16 addr, uint8 data) -> void {
  if(!sequencer.enable) {
    bool valid = addr == 0xff26;  //NR52
    if(!system.cgb()) {
      //NRx1 length is writable only on DMG,SGB; not on CGB
      if(addr == 0xff11) valid = true, data &= 0x3f;  //NR11; duty is not writable (remains 0)
      if(addr == 0xff16) valid = true, data &= 0x3f;  //NR21; duty is not writable (remains 0)
      if(addr == 0xff1b) valid = true;  //NR31
      if(addr == 0xff20) valid = true;  //NR41
    }
    if(!valid) return;
  }

  if(addr >= 0xff10 && addr <= 0xff14) return square1.write(addr, data);
  if(addr >= 0xff15 && addr <= 0xff19) return square2.write(addr, data);
  if(addr >= 0xff1a && addr <= 0xff1e) return wave.write(addr, data);
  if(addr >= 0xff1f && addr <= 0xff23) return noise.write(addr, data);
  if(addr >= 0xff24 && addr <= 0xff26) return sequencer.write(addr, data);
  if(addr >= 0xff30 && addr <= 0xff3f) return wave.write(addr, data);
}

}
