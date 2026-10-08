#pragma once

#include <Arduino.h>

namespace chrono_paper {

enum class ButtonAction : uint8_t { None, Previous, Next, CycleView, ToggleWeekStart };

class InputManager {
public:
  void begin();
  ButtonAction poll(uint32_t nowMs);
private:
  struct ButtonState { uint8_t pin; bool stableHigh; bool lastHigh; uint32_t changedAt; uint32_t pressedAt; bool longSent; };
  ButtonState buttons_[3]{{D2, true, true, 0, 0, false}, {D3, true, true, 0, 0, false}, {D4, true, true, 0, 0, false}};
  ButtonAction update(ButtonState &button, uint32_t nowMs, ButtonAction shortAction, ButtonAction longAction);
};

} // namespace chrono_paper
