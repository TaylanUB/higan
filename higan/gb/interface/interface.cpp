#include <gb/gb.hpp>

namespace GameBoy {

Interface* interface = nullptr;
Settings settings;

Interface::Interface() {
  interface = this;
  hook = nullptr;

  information.manufacturer = "Nintendo";
  information.name         = "Game Boy";
  information.width        = 160;
  information.height       = 144;
  information.overscan     = false;
  information.aspectRatio  = 1.0;
  information.resettable   = false;

  information.capability.states = true;
  information.capability.cheats = true;

  media.append({ID::GameBoy,      "Game Boy",       "gb" , true});
  media.append({ID::GameBoyColor, "Game Boy Color", "gbc", true});

  {
    Device device{0, ID::Device, "Controller"};
    device.input.append({0, 0, "Up"    });
    device.input.append({1, 0, "Down"  });
    device.input.append({2, 0, "Left"  });
    device.input.append({3, 0, "Right" });
    device.input.append({4, 0, "B"     });
    device.input.append({5, 0, "A"     });
    device.input.append({6, 0, "Select"});
    device.input.append({7, 0, "Start" });
    device.order = {0, 1, 2, 3, 4, 5, 6, 7};
    this->device.append(device);
  }

  port.append({0, "Device", {device[0]}});
}

auto Interface::manifest() -> string {
  return cartridge.manifest();
}

auto Interface::title() -> string {
  return cartridge.title();
}

auto Interface::videoFrequency() -> double {
  return 4194304.0 / (154.0 * 456.0);
}

auto Interface::audioFrequency() -> double {
  return 4194304.0 / 2.0;
}

auto Interface::loaded() -> bool {
  return system.loaded();
}

auto Interface::sha256() -> string {
  return cartridge.sha256();
}

auto Interface::group(uint id) -> uint {
  switch(id) {
  case ID::SystemManifest:
  case ID::GameBoyBootROM:
  case ID::SuperGameBoyBootROM:
  case ID::GameBoyColorBootROM:
    return 0;
  case ID::Manifest:
  case ID::ROM:
  case ID::RAM:
    switch(system.revision()) {
    case System::Revision::GameBoy: return ID::GameBoy;
    case System::Revision::SuperGameBoy: return ID::SuperGameBoy;
    case System::Revision::GameBoyColor: return ID::GameBoyColor;
    }
    throw;
  }
  throw;
}

auto Interface::load(uint id) -> void {
  if(id == ID::GameBoy) system.load(System::Revision::GameBoy);
  if(id == ID::SuperGameBoy) system.load(System::Revision::SuperGameBoy);
  if(id == ID::GameBoyColor) system.load(System::Revision::GameBoyColor);
}

auto Interface::save() -> void {
  for(auto& memory : cartridge.memory) {
    interface->saveRequest(memory.id, memory.name);
  }
}

auto Interface::load(uint id, const stream& stream) -> void {
  if(id == ID::SystemManifest) {
    system.information.manifest = stream.text();
  }

  if(id == ID::GameBoyBootROM) {
    stream.read((uint8_t*)system.bootROM.dmg, min( 256u, stream.size()));
  }

  if(id == ID::SuperGameBoyBootROM) {
    stream.read((uint8_t*)system.bootROM.sgb, min( 256u, stream.size()));
  }

  if(id == ID::GameBoyColorBootROM) {
    stream.read((uint8_t*)system.bootROM.cgb, min(2048u, stream.size()));
  }

  if(id == ID::Manifest) {
    cartridge.information.markup = stream.text();
  }

  if(id == ID::ROM) {
    stream.read((uint8_t*)cartridge.romdata, min(cartridge.romsize, stream.size()));
  }

  if(id == ID::RAM) {
    stream.read((uint8_t*)cartridge.ramdata, min(stream.size(), cartridge.ramsize));
  }
}

auto Interface::save(uint id, const stream& stream) -> void {
  if(id == ID::RAM) {
    stream.write((uint8_t*)cartridge.ramdata, cartridge.ramsize);
  }
}

auto Interface::unload() -> void {
  save();
  system.unload();
}

auto Interface::power() -> void {
  system.power();
}

auto Interface::reset() -> void {
  system.power();
}

auto Interface::run() -> void {
  system.run();
}

auto Interface::serialize() -> serializer {
  system.runToSave();
  return system.serialize();
}

auto Interface::unserialize(serializer& s) -> bool {
  return system.unserialize(s);
}

auto Interface::cheatSet(const lstring& list) -> void {
  cheat.reset();
  for(auto& codeset : list) {
    lstring codes = codeset.split("+");
    for(auto& code : codes) {
      lstring part = code.split("/");
      if(part.size() == 2) cheat.append(hex(part[0]), hex(part[1]));
      if(part.size() == 3) cheat.append(hex(part[0]), hex(part[1]), hex(part[2]));
    }
  }
}

auto Interface::lcdScanline() -> void {
  if(hook) hook->lcdScanline();
}

auto Interface::lcdOutput(uint2 color) -> void {
  if(hook) hook->lcdOutput(color);
}

auto Interface::joypWrite(bool p15, bool p14) -> void {
  if(hook) hook->joypWrite(p15, p14);
}

auto Interface::cap(const string& name) -> bool {
  if(name == "Blur Emulation") return true;
  if(name == "Color Emulation") return true;
  return false;
}

auto Interface::get(const string& name) -> any {
  if(name == "Blur Emulation") return settings.blurEmulation;
  if(name == "Color Emulation") return settings.colorEmulation;
  return {};
}

auto Interface::set(const string& name, const any& value) -> bool {
  if(name == "Blur Emulation" && value.is<bool>()) return settings.blurEmulation = value.get<bool>(), true;
  if(name == "Color Emulation" && value.is<bool>()) return settings.colorEmulation = value.get<bool>(), true;
  return false;
}

}
