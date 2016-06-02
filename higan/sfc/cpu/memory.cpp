auto CPU::portRead(uint2 port) const -> uint8 {
  return status.port[port];
}

auto CPU::portWrite(uint2 port, uint8 data) -> void {
  status.port[port] = data;
}

auto CPU::io() -> void {
  status.clock_count = 6;
  dmaEdge();
  addClocks(6);
  aluEdge();
}

auto CPU::read(uint24 addr) -> uint8 {
  status.clock_count = speed(addr);
  dmaEdge();
  addClocks(status.clock_count - 4);
  regs.mdr = bus.read(addr, regs.mdr);
  addClocks(4);
  aluEdge();
  debugger.op_read(addr, regs.mdr);
  return regs.mdr;
}

auto CPU::write(uint24 addr, uint8 data) -> void {
  aluEdge();
  status.clock_count = speed(addr);
  dmaEdge();
  addClocks(status.clock_count);
  bus.write(addr, regs.mdr = data);
  debugger.op_write(addr, regs.mdr);
}

auto CPU::speed(uint24 addr) const -> uint {
  if(addr & 0x408000) {
    if(addr & 0x800000) return status.rom_speed;
    return 8;
  }
  if((addr + 0x6000) & 0x4000) return 8;
  if((addr - 0x4000) & 0x7e00) return 6;
  return 12;
}

auto CPU::disassemblerRead(uint24 addr) -> uint8 {
  return bus.read(addr, regs.mdr);
}
