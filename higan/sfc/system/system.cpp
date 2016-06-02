#include <sfc/sfc.hpp>

namespace SuperFamicom {

System system;

#include "device.cpp"
#include "random.cpp"
#include "serialization.cpp"

auto System::loaded() const -> bool { return _loaded; }
auto System::region() const -> Region { return _region; }
auto System::expansionPort() const -> Device::ID { return _expansionPort; }
auto System::cpuFrequency() const -> uint { return _cpuFrequency; }
auto System::apuFrequency() const -> uint { return _apuFrequency; }

auto System::run() -> void {
  scheduler.enter();
}

auto System::runToSave() -> void {
  if(CPU::Threaded) scheduler.synchronize(cpu.thread);
  if(SMP::Threaded) scheduler.synchronize(smp.thread);
  if(PPU::Threaded) scheduler.synchronize(ppu.thread);
  if(DSP::Threaded) scheduler.synchronize(dsp.thread);
  for(auto chip : cpu.coprocessors) {
    scheduler.synchronize(chip->thread);
  }
}

auto System::init() -> void {
  assert(interface != nullptr);

  satellaview.init();
  superdisc.init();
  s21fx.init();

  icd2.init();
  mcc.init();
  nss.init();
  event.init();
  sa1.init();
  superfx.init();
  armdsp.init();
  hitachidsp.init();
  necdsp.init();
  epsonrtc.init();
  sharprtc.init();
  spc7110.init();
  sdd1.init();
  obc1.init();
  msu1.init();

  bsmemory.init();

  device.connect(0, (Device::ID)settings.controllerPort1);
  device.connect(1, (Device::ID)settings.controllerPort2);
  device.connect(2, (Device::ID)settings.expansionPort);
}

auto System::term() -> void {
}

auto System::load() -> void {
  interface->loadRequest(ID::SystemManifest, "manifest.bml", true);
  auto document = BML::unserialize(information.manifest);

  if(auto iplrom = document["system/smp/rom/name"].text()) {
    interface->loadRequest(ID::IPLROM, iplrom, true);
  }

  cartridge.load();
  _region = cartridge.region() == Cartridge::Region::NTSC ? Region::NTSC : Region::PAL;
  _expansionPort = (Device::ID)settings.expansionPort;
  _cpuFrequency = region() == Region::NTSC ? 21'477'272 : 21'281'370;
  _apuFrequency = 24'606'720;

  bus.reset();
  bus.map();

  cpu.enable();
  ppu.enable();

  if(expansionPort() == Device::ID::Satellaview) satellaview.load();
  if(expansionPort() == Device::ID::SuperDisc) superdisc.load();
  if(expansionPort() == Device::ID::S21FX) s21fx.load();

  if(cartridge.hasICD2()) icd2.load();
  if(cartridge.hasMCC()) mcc.load();
  if(cartridge.hasNSSDIP()) nss.load();
  if(cartridge.hasEvent()) event.load();
  if(cartridge.hasSA1()) sa1.load();
  if(cartridge.hasSuperFX()) superfx.load();
  if(cartridge.hasARMDSP()) armdsp.load();
  if(cartridge.hasHitachiDSP()) hitachidsp.load();
  if(cartridge.hasNECDSP()) necdsp.load();
  if(cartridge.hasEpsonRTC()) epsonrtc.load();
  if(cartridge.hasSharpRTC()) sharprtc.load();
  if(cartridge.hasSPC7110()) spc7110.load();
  if(cartridge.hasSDD1()) sdd1.load();
  if(cartridge.hasOBC1()) obc1.load();
  if(cartridge.hasMSU1()) msu1.load();

  if(cartridge.hasBSMemorySlot()) bsmemory.load();
  if(cartridge.hasSufamiTurboSlots()) sufamiturboA.load(), sufamiturboB.load();

  serializeInit();
  _loaded = true;
}

auto System::unload() -> void {
  if(!loaded()) return;
  if(expansionPort() == Device::ID::Satellaview) satellaview.unload();
  if(expansionPort() == Device::ID::SuperDisc) superdisc.unload();
  if(expansionPort() == Device::ID::S21FX) s21fx.unload();

  if(cartridge.hasICD2()) icd2.unload();
  if(cartridge.hasMCC()) mcc.unload();
  if(cartridge.hasNSSDIP()) nss.unload();
  if(cartridge.hasEvent()) event.unload();
  if(cartridge.hasSA1()) sa1.unload();
  if(cartridge.hasSuperFX()) superfx.unload();
  if(cartridge.hasARMDSP()) armdsp.unload();
  if(cartridge.hasHitachiDSP()) hitachidsp.unload();
  if(cartridge.hasNECDSP()) necdsp.unload();
  if(cartridge.hasEpsonRTC()) epsonrtc.unload();
  if(cartridge.hasSharpRTC()) sharprtc.unload();
  if(cartridge.hasSPC7110()) spc7110.unload();
  if(cartridge.hasSDD1()) sdd1.unload();
  if(cartridge.hasOBC1()) obc1.unload();
  if(cartridge.hasMSU1()) msu1.unload();

  if(cartridge.hasBSMemorySlot()) bsmemory.unload();
  if(cartridge.hasSufamiTurboSlots()) sufamiturboA.unload(), sufamiturboB.unload();

  cartridge.unload();
  _loaded = false;
}

auto System::power() -> void {
  random.seed((uint)time(0));

  cpu.power();
  smp.power();
  dsp.power();
  ppu.power();

  if(expansionPort() == Device::ID::Satellaview) satellaview.power();
  if(expansionPort() == Device::ID::SuperDisc) superdisc.power();
  if(expansionPort() == Device::ID::S21FX) s21fx.power();

  if(cartridge.hasICD2()) icd2.power();
  if(cartridge.hasMCC()) mcc.power();
  if(cartridge.hasNSSDIP()) nss.power();
  if(cartridge.hasEvent()) event.power();
  if(cartridge.hasSA1()) sa1.power();
  if(cartridge.hasSuperFX()) superfx.power();
  if(cartridge.hasARMDSP()) armdsp.power();
  if(cartridge.hasHitachiDSP()) hitachidsp.power();
  if(cartridge.hasNECDSP()) necdsp.power();
  if(cartridge.hasEpsonRTC()) epsonrtc.power();
  if(cartridge.hasSharpRTC()) sharprtc.power();
  if(cartridge.hasSPC7110()) spc7110.power();
  if(cartridge.hasSDD1()) sdd1.power();
  if(cartridge.hasOBC1()) obc1.power();
  if(cartridge.hasMSU1()) msu1.power();

  if(cartridge.hasBSMemorySlot()) bsmemory.power();

  reset();
}

auto System::reset() -> void {
  cpu.reset();
  smp.reset();
  dsp.reset();
  ppu.reset();

  if(expansionPort() == Device::ID::Satellaview) satellaview.reset();
  if(expansionPort() == Device::ID::SuperDisc) superdisc.reset();
  if(expansionPort() == Device::ID::S21FX) s21fx.reset();

  if(cartridge.hasICD2()) icd2.reset();
  if(cartridge.hasMCC()) mcc.reset();
  if(cartridge.hasNSSDIP()) nss.reset();
  if(cartridge.hasEvent()) event.reset();
  if(cartridge.hasSA1()) sa1.reset();
  if(cartridge.hasSuperFX()) superfx.reset();
  if(cartridge.hasARMDSP()) armdsp.reset();
  if(cartridge.hasHitachiDSP()) hitachidsp.reset();
  if(cartridge.hasNECDSP()) necdsp.reset();
  if(cartridge.hasEpsonRTC()) epsonrtc.reset();
  if(cartridge.hasSharpRTC()) sharprtc.reset();
  if(cartridge.hasSPC7110()) spc7110.reset();
  if(cartridge.hasSDD1()) sdd1.reset();
  if(cartridge.hasOBC1()) obc1.reset();
  if(cartridge.hasMSU1()) msu1.reset();

  if(cartridge.hasBSMemorySlot()) bsmemory.reset();

  if(cartridge.hasICD2()) cpu.coprocessors.append(&icd2);
  if(cartridge.hasEvent()) cpu.coprocessors.append(&event);
  if(cartridge.hasSA1()) cpu.coprocessors.append(&sa1);
  if(cartridge.hasSuperFX()) cpu.coprocessors.append(&superfx);
  if(cartridge.hasARMDSP()) cpu.coprocessors.append(&armdsp);
  if(cartridge.hasHitachiDSP()) cpu.coprocessors.append(&hitachidsp);
  if(cartridge.hasNECDSP()) cpu.coprocessors.append(&necdsp);
  if(cartridge.hasEpsonRTC()) cpu.coprocessors.append(&epsonrtc);
  if(cartridge.hasSharpRTC()) cpu.coprocessors.append(&sharprtc);
  if(cartridge.hasSPC7110()) cpu.coprocessors.append(&spc7110);
  if(cartridge.hasMSU1()) cpu.coprocessors.append(&msu1);
  if(expansionPort() == Device::ID::SuperDisc) cpu.coprocessors.append(&superdisc);
  if(expansionPort() == Device::ID::S21FX) cpu.coprocessors.append(&s21fx);

  scheduler.reset();
  device.connect(0, (Device::ID)settings.controllerPort1);
  device.connect(1, (Device::ID)settings.controllerPort2);
  device.connect(2, (Device::ID)settings.expansionPort);
}

}
