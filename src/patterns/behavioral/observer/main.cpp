#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

// Forward declaration
class Observer;

// Subject
class WeatherData {
 public:
  void registerObserver(Observer *o) { m_observers.push_back(o); }
  void removeObserver(Observer *o) {
    m_observers.erase(std::remove(m_observers.begin(), m_observers.end(), o),
                      m_observers.end());
  }
  void notifyObservers();
  void measurementsChanged() { notifyObservers(); }
  void setMeasurements(float t, float h, float p) {
    m_temperature = t;
    m_humidity = h;
    m_pressure = p;
    measurementsChanged();
  }

 private:
  std::vector<Observer *> m_observers;
  float m_temperature = 0.0;
  float m_humidity = 0.0;
  float m_pressure = 0.0;
};

// Observer Interface
class Observer {
 public:
  Observer() = default;
  virtual ~Observer() = default;
  Observer(const Observer &) = default;
  Observer &operator=(const Observer &) = default;
  Observer(Observer &&) = default;
  Observer &operator=(Observer &&) = default;
  virtual void update(float temperature, float humidity, float pressure) = 0;
};

// Concrete Observer
class CurrentConditionsDisplay : public Observer {
 public:
  explicit CurrentConditionsDisplay(std::shared_ptr<WeatherData> weather_data)
      : m_weather_data(std::move(weather_data)) {
    m_weather_data->registerObserver(this);
  }

  void update(float temperature, float humidity, float /*pressure*/) override {
    m_temperature = temperature;
    m_humidity = humidity;
    display();
  }

  void display() const {
    std::cout << "Current conditions: " << m_temperature << "F degrees and "
              << m_humidity << "% humidity" << std::endl;
  }

 private:
  std::shared_ptr<WeatherData> m_weather_data;
  float m_temperature = 0.0;
  float m_humidity = 0.0;
};

void WeatherData::notifyObservers() {
  for (Observer *observer : m_observers) {
    observer->update(m_temperature, m_humidity, m_pressure);
  }
}

int main() {
  auto weather_data = std::make_shared<WeatherData>();

  CurrentConditionsDisplay current_display(weather_data);

  weather_data->setMeasurements(80, 65, 30.4f);
  weather_data->setMeasurements(82, 70, 29.2f);
  weather_data->setMeasurements(78, 90, 29.2f);

  current_display.display();

  return EXIT_SUCCESS;
}
