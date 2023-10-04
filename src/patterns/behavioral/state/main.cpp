#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>

class MusicPlayer;

// State
class PlayerState {
 public:
  PlayerState() = default;
  virtual ~PlayerState() = default;
  PlayerState(const PlayerState &) = default;
  PlayerState &operator=(const PlayerState &) = default;
  PlayerState(PlayerState &&) = default;
  PlayerState &operator=(PlayerState &&) = default;

  virtual void play(MusicPlayer &player) = 0;
  virtual void pause(MusicPlayer &player) = 0;
  virtual void stop(MusicPlayer &player) = 0;
};

// Context
class MusicPlayer {
 public:
  MusicPlayer();
  void setState(std::unique_ptr<PlayerState> new_state) {
    m_state = std::move(new_state);
  }
  void play() { m_state->play(*this); }
  void pause() { m_state->pause(*this); }
  void stop() { m_state->stop(*this); }

 private:
  std::unique_ptr<PlayerState> m_state;
};

// ConcreteState
class PlayingState : public PlayerState {
 public:
  void play(MusicPlayer &player) override;
  void pause(MusicPlayer &player) override;
  void stop(MusicPlayer &player) override;
};

// ConcreteState
class PausedState : public PlayerState {
 public:
  void play(MusicPlayer &player) override;
  void pause(MusicPlayer &player) override;
  void stop(MusicPlayer &player) override;
};

// ConcreteState
class StoppedState : public PlayerState {
 public:
  void play(MusicPlayer &player) override;
  void pause(MusicPlayer &player) override;
  void stop(MusicPlayer &player) override;
};

MusicPlayer::MusicPlayer() : m_state(std::make_unique<StoppedState>()) {}

void PlayingState::play(MusicPlayer & /*player*/) {
  std::cout << "Already playing." << std::endl;
}

void PlayingState::pause(MusicPlayer &player) {
  std::cout << "Music paused." << std::endl;
  player.setState(std::make_unique<PausedState>());
}

void PlayingState::stop(MusicPlayer &player) {
  std::cout << "Music stopped." << std::endl;
  player.setState(std::make_unique<StoppedState>());
}

void PausedState::play(MusicPlayer &player) {
  std::cout << "Resuming music." << std::endl;
  player.setState(std::make_unique<PlayingState>());
}

void PausedState::pause(MusicPlayer & /*player*/) {
  std::cout << "Already paused." << std::endl;
}

void PausedState::stop(MusicPlayer &player) {
  std::cout << "Music stopped." << std::endl;
  player.setState(std::make_unique<StoppedState>());
}

void StoppedState::play(MusicPlayer &player) {
  std::cout << "Playing music." << std::endl;
  player.setState(std::make_unique<PlayingState>());
}

void StoppedState::pause(MusicPlayer & /*player*/) {
  std::cout << "Can't pause when stopped." << std::endl;
}

void StoppedState::stop(MusicPlayer & /*player*/) {
  std::cout << "Already stopped." << std::endl;
}

int main() {
  MusicPlayer player;
  player.play();
  player.pause();
  player.stop();

  return EXIT_SUCCESS;
}
