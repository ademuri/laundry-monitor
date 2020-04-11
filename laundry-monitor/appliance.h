#ifndef APPLIANCE
#define APPLIANCE

#include "median-filter.h"

class Appliance {
 public:
  template <uint32_t pin>
  static Appliance *Create(uint32_t on_to_off_threshold);

  // Read from sensors and do the logic. Call this periodically.
  void Run();

  // Whether the appliance turned on or off in the last call to Run.
  bool TurnedOn();
  bool TurnedOff();
  bool State();

 private:
  Appliance(MedianFilter<uint32_t, uint32_t, 5> *const easy_filter,
            MedianFilter<uint32_t, uint32_t, 201> *const hard_filter,
            uint32_t on_to_off_threshold);

  MedianFilter<uint32_t, uint32_t, 5> *const easy_filter_;
  MedianFilter<uint32_t, uint32_t, 201> *const hard_filter_;

  // Don't regard the appliance as on for this long after power up, to avoid noise from filling up the hard filter.
  static const uint32_t power_on_delay_ = 5 * 1000;

  const uint32_t on_to_off_threshold_;

  uint32_t change_at_ = 0;
  bool state_ = false;
  bool prev_state_ = false;
};

template <uint32_t pin>
Appliance *Appliance::Create(uint32_t on_to_off_threshold) {
  pinMode(pin, INPUT);
  auto easy_filter = new MedianFilter<uint32_t, uint32_t, 5>(
                           filter_functions::ForAnalogRead<pin>());
  easy_filter->SetMinRunInterval(5);

  auto hard_filter = new MedianFilter<uint32_t, uint32_t, 201>(
                           filter_functions::ForAnalogRead<pin>());
  hard_filter->SetMinRunInterval(100);

  return new Appliance(easy_filter, hard_filter, on_to_off_threshold);
}

#endif
