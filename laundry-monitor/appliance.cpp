#include "appliance.h"

Appliance::Appliance(MedianFilter<uint32_t, uint32_t, 5> *const easy_filter,
                     MedianFilter<uint32_t, uint32_t, 201> *const hard_filter,
                     uint32_t on_to_off_threshold)
    : easy_filter_(easy_filter),
      hard_filter_(hard_filter),
      on_to_off_threshold_(on_to_off_threshold) {}

void Appliance::Run() {
  prev_state_ = state_;

  easy_filter_->Run();
  hard_filter_->Run();

  if (state_) {
    if (easy_filter_->GetFilteredValue() > 0) {
      change_at_ = millis() + on_to_off_threshold_;
    } else if (millis() > change_at_) {
      state_ = false;
    }
  } else {
    // state_ == false
    if (hard_filter_->GetFilteredValue() > 0) {
      state_ = true;
      change_at_ = millis() + on_to_off_threshold_;
    }
  }
}

bool Appliance::TurnedOn() { return (state_ == true && prev_state_ == false); }

bool Appliance::TurnedOff() { return (state_ == false && prev_state_ == true); }

bool Appliance::State() { return state_; }
