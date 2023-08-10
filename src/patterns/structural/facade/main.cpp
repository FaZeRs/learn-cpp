#include <iostream>

class CPU {
 public:
  static void freeze() { std::cout << "CPU is frozen.\n"; }
  static void jump(int64_t position) {
    std::cout << "CPU jumped to position: " << position << ".\n";
  }
  static void execute() { std::cout << "CPU execution started.\n"; }
};

class Memory {
 public:
  static void load(int64_t position, const std::string& data) {
    std::cout << "Memory loaded from position " << position
              << " with data: " << data << ".\n";
  }
};

class HardDrive {
 public:
  static std::string read(int64_t /*lba*/, int /*size*/) {
    return "data from sector";
  }
};

class ComputerFacade {
 private:
  CPU processor;
  Memory ram;
  HardDrive hd;

 public:
  ComputerFacade() : processor(), ram(), hd() {}

  void start() {
    processor.freeze();
    ram.load(BOOT_ADDRESS, hd.read(BOOT_SECTOR, SECTOR_SIZE));
    processor.jump(BOOT_ADDRESS);
    processor.execute();
  }

  static const int64_t BOOT_ADDRESS = 0;
  static const int64_t BOOT_SECTOR = 0;
  static const int SECTOR_SIZE = 512;
};

int main() {
  ComputerFacade computer;
  computer.start();
  return EXIT_SUCCESS;
}