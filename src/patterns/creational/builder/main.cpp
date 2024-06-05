#include <cstdlib>
#include <iostream>
#include <string>
#include <string_view>

// Product
class Computer {
 public:
  void setCPU(const std::string_view& cpu) { m_cpu = cpu; }
  void setRAM(const std::string_view& ram) { m_ram = ram; }
  void setStorage(const std::string_view& storage) { m_storage = storage; }

  void show() const {
    std::cout << "CPU: " << m_cpu << "\n"
              << "RAM: " << m_ram << "\n"
              << "Storage: " << m_storage << "\n";
  }

 private:
  std::string m_cpu;
  std::string m_ram;
  std::string m_storage;
};

// Abstract Builder
class ComputerBuilder {
 public:
  ComputerBuilder() = default;
  virtual ~ComputerBuilder() = default;
  ComputerBuilder(const ComputerBuilder&) = default;
  ComputerBuilder& operator=(const ComputerBuilder&) = default;
  ComputerBuilder(ComputerBuilder&&) = default;
  ComputerBuilder& operator=(ComputerBuilder&&) = default;

  virtual void buildCPU() = 0;
  virtual void buildRAM() = 0;
  virtual void buildStorage() = 0;
  virtual Computer getComputer() = 0;
};

// Concrete Builder
class GamingComputerBuilder : public ComputerBuilder {
 public:
  void buildCPU() override { m_computer.setCPU("Gaming CPU"); }
  void buildRAM() override { m_computer.setRAM("16GB DDR4"); }
  void buildStorage() override { m_computer.setStorage("1TB SSD"); }

  Computer getComputer() override { return m_computer; }

 private:
  Computer m_computer;
};

// Concrete Builder
class LaptopComputerBuilder : public ComputerBuilder {
 public:
  void buildCPU() override { m_computer.setCPU("Laptop CPU"); }
  void buildRAM() override { m_computer.setRAM("8GB DDR3"); }
  void buildStorage() override { m_computer.setStorage("256GB SSD"); }

  Computer getComputer() override { return m_computer; }

 private:
  Computer m_computer;
};

// Director
class Engineer {
 public:
  Engineer() = default;
  void setBuilder(ComputerBuilder* builder) { m_builder = builder; }
  void constructComputer() {
    m_builder->buildCPU();
    m_builder->buildRAM();
    m_builder->buildStorage();
  }

 private:
  ComputerBuilder* m_builder{nullptr};
};

int main() {
  Engineer engineer;
  GamingComputerBuilder gaming_builder;

  engineer.setBuilder(&gaming_builder);
  engineer.constructComputer();
  Computer gaming_computer = gaming_builder.getComputer();
  gaming_computer.show();

  std::cout << "\n";

  LaptopComputerBuilder laptop_builder;
  engineer.setBuilder(&laptop_builder);
  engineer.constructComputer();
  Computer laptop_computer = laptop_builder.getComputer();
  laptop_computer.show();

  return EXIT_SUCCESS;
}
