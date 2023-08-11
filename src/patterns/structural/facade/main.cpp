#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <utility>

class CPU {
 public:
  void freeze() {
    m_is_frozen = true;
    std::cout << "CPU is frozen." << std::endl;
  }
  void jump(int64_t position) {
    m_position = position;
    std::cout << "CPU jumped to position: " << position << "." << std::endl;
  }
  void execute() {
    m_is_executing = true;
    std::cout << "CPU execution started." << std::endl;
  }

 private:
  int64_t m_position{};
  bool m_is_frozen{false};
  bool m_is_executing{false};
};

class Memory {
 public:
  void load(int64_t position, std::string data) {
    m_position = position;
    m_data = std::move(data);
    std::cout << "Memory loaded data: " << m_data
              << " at position: " << m_position << "." << std::endl;
  }

 private:
  int64_t m_position{};
  std::string m_data{};
};

class HardDrive {
 public:
  [[nodiscard]] std::string read(int64_t lba, int64_t size) const {
    std::cout << "HardDrive read data: " << m_data << " at lba: " << lba
              << " with size: " << size << "." << std::endl;
    return m_data;
  }

 private:
  std::string m_data{"Some data"};
};

class ComputerFacade {
 public:
  ComputerFacade() = default;
  void start() {
    processor.freeze();
    ram.load(BOOT_ADDRESS, hd.read(BOOT_SECTOR, SECTOR_SIZE));
    processor.jump(BOOT_ADDRESS);
    processor.execute();
  }

  static const int64_t BOOT_ADDRESS = 0;
  static const int64_t BOOT_SECTOR = 0;
  static const int SECTOR_SIZE = 512;

 private:
  [[no_unique_address]] CPU processor{};
  [[no_unique_address]] Memory ram{};
  [[no_unique_address]] HardDrive hd{};
};

int main() {
  ComputerFacade computer;
  computer.start();
  return EXIT_SUCCESS;
}