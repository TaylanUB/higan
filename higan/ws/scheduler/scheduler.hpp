struct Scheduler {
  enum class Mode : uint {
    Run,
    SynchronizeCPU,
    SynchronizeAll,
  };

  enum class Event : uint {
    Unknown,
    Frame,
    Synchronize,
  };

  auto power() -> void;
  auto enter(Mode = Mode::Run) -> Event;
  auto exit(Event) -> void;
  auto synchronize(cothread_t) -> void;
  auto synchronize() -> void;
  auto synchronizing() const -> bool;

private:
  cothread_t host = nullptr;
  cothread_t resume = nullptr;
  Mode mode = Mode::Run;
  Event event = Event::Unknown;
};

extern Scheduler scheduler;
