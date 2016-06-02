struct Icarus {
  //core.cpp
  Icarus();

  auto error() const -> string;
  auto success(string location) -> string;
  auto failure(string message) -> string;

  auto manifest(string location) -> string;
  auto import(string location) -> string;

  auto concatenate(vector<uint8_t>& output, string location) -> void;

  //famicom.cpp
  auto famicomManifest(string location) -> string;
  auto famicomManifest(vector<uint8_t>& buffer, string location, uint* prgrom = nullptr, uint* chrrom = nullptr) -> string;
  auto famicomImport(vector<uint8_t>& buffer, string location) -> string;

  //super-famicom.cpp
  auto superFamicomManifest(string location) -> string;
  auto superFamicomManifest(vector<uint8_t>& buffer, string location, bool* firmwareAppended = nullptr) -> string;
  auto superFamicomManifestScan(vector<Markup::Node>& roms, Markup::Node node) -> void;
  auto superFamicomImport(vector<uint8_t>& buffer, string location) -> string;

  //game-boy.cpp
  auto gameBoyManifest(string location) -> string;
  auto gameBoyManifest(vector<uint8_t>& buffer, string location) -> string;
  auto gameBoyImport(vector<uint8_t>& buffer, string location) -> string;

  //game-boy-color.cpp
  auto gameBoyColorManifest(string location) -> string;
  auto gameBoyColorManifest(vector<uint8_t>& buffer, string location) -> string;
  auto gameBoyColorImport(vector<uint8_t>& buffer, string location) -> string;

  //game-boy-advance.cpp
  auto gameBoyAdvanceManifest(string location) -> string;
  auto gameBoyAdvanceManifest(vector<uint8_t>& buffer, string location) -> string;
  auto gameBoyAdvanceImport(vector<uint8_t>& buffer, string location) -> string;

  //bs-memory.cpp
  auto bsMemoryManifest(string location) -> string;
  auto bsMemoryManifest(vector<uint8_t>& buffer, string location) -> string;
  auto bsMemoryImport(vector<uint8_t>& buffer, string location) -> string;

  //sufami-turbo.cpp
  auto sufamiTurboManifest(string location) -> string;
  auto sufamiTurboManifest(vector<uint8_t>& buffer, string location) -> string;
  auto sufamiTurboImport(vector<uint8_t>& buffer, string location) -> string;

  //wonderswan.cpp
  auto wonderSwanManifest(string location) -> string;
  auto wonderSwanManifest(vector<uint8_t>& buffer, string location) -> string;
  auto wonderSwanImport(vector<uint8_t>& buffer, string location) -> string;

  //wonderswan-color.cpp
  auto wonderSwanColorManifest(string location) -> string;
  auto wonderSwanColorManifest(vector<uint8_t>& buffer, string location) -> string;
  auto wonderSwanColorImport(vector<uint8_t>& buffer, string location) -> string;

private:
  string errorMessage;

  struct {
    Markup::Node famicom;
    Markup::Node superFamicom;
    Markup::Node gameBoy;
    Markup::Node gameBoyColor;
    Markup::Node gameBoyAdvance;
    Markup::Node wonderSwan;
    Markup::Node wonderSwanColor;
    Markup::Node bsMemory;
    Markup::Node sufamiTurbo;
  } database;
};
