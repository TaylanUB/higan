Icarus::Icarus() {
  database.famicom = BML::unserialize(string::read(locate("Database/Famicom.bml")));
  database.superFamicom = BML::unserialize(string::read(locate("Database/Super Famicom.bml")));
  database.gameBoy = BML::unserialize(string::read(locate("Database/Game Boy.bml")));
  database.gameBoyColor = BML::unserialize(string::read(locate("Database/Game Boy Color.bml")));
  database.gameBoyAdvance = BML::unserialize(string::read(locate("Database/Game Boy Advance.bml")));
  database.wonderSwan = BML::unserialize(string::read(locate("Database/WonderSwan.bml")));
  database.wonderSwanColor = BML::unserialize(string::read(locate("Database/WonderSwan Color.bml")));
  database.bsMemory = BML::unserialize(string::read(locate("Database/BS Memory.bml")));
  database.sufamiTurbo = BML::unserialize(string::read(locate("Database/Sufami Turbo.bml")));
}

auto Icarus::error() const -> string {
  return errorMessage;
}

auto Icarus::success(string location) -> string {
  errorMessage = "";
  return location;
}

auto Icarus::failure(string message) -> string {
  errorMessage = message;
  return {};
}

auto Icarus::manifest(string location) -> string {
  location.transform("\\", "/").rtrim("/").append("/");
  if(!directory::exists(location)) return "";

  auto type = suffixname(location).downcase();
  if(type == ".fc") return famicomManifest(location);
  if(type == ".sfc") return superFamicomManifest(location);
  if(type == ".gb") return gameBoyManifest(location);
  if(type == ".gbc") return gameBoyColorManifest(location);
  if(type == ".gba") return gameBoyAdvanceManifest(location);
  if(type == ".ws") return wonderSwanManifest(location);
  if(type == ".wsc") return wonderSwanColorManifest(location);
  if(type == ".bs") return bsMemoryManifest(location);
  if(type == ".st") return sufamiTurboManifest(location);

  return "";
}

auto Icarus::import(string location) -> string {
  location.transform("\\", "/").rtrim("/");
  if(!file::exists(location)) return failure("file does not exist");
  if(!file::readable(location)) return failure("file is unreadable");

  auto name = prefixname(location);
  auto type = suffixname(location).downcase();
  if(!name || !type) return failure("invalid file name");

  auto buffer = file::read(location);
  if(!buffer) return failure("file is empty");

  if(type == ".zip") {
    Decode::ZIP zip;
    if(!zip.open(location)) return failure("ZIP archive is invalid");
    if(!zip.file) return failure("ZIP archive is empty");

    name = prefixname(zip.file[0].name);
    type = suffixname(zip.file[0].name).downcase();
    buffer = zip.extract(zip.file[0]);
  }

  if(type == ".fc" || type == ".nes") return famicomImport(buffer, location);
  if(type == ".sfc" || type == ".smc") return superFamicomImport(buffer, location);
  if(type == ".gb") return gameBoyImport(buffer, location);
  if(type == ".gbc") return gameBoyColorImport(buffer, location);
  if(type == ".gba") return gameBoyAdvanceImport(buffer, location);
  if(type == ".ws") return wonderSwanImport(buffer, location);
  if(type == ".wsc") return wonderSwanColorImport(buffer, location);
  if(type == ".bs") return bsMemoryImport(buffer, location);
  if(type == ".st") return sufamiTurboImport(buffer, location);

  return failure("unrecognized file extension");
}

auto Icarus::concatenate(vector<uint8_t>& output, string location) -> void {
  if(auto input = file::read(location)) {
    auto size = output.size();
    output.resize(size + input.size());
    memory::copy(output.data() + size, input.data(), input.size());
  }
}
