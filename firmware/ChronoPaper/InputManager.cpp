#include "InputManager.h"

namespace chrono_paper {

void InputManager::begin() {
  for (auto &button : buttons_) {
    pinMode(button.pin, INPUT_PULLUP);
    button.stableHigh = button.lastHigh = digitalRead(button.pin);
  }
}

ButtonAction InputManager::update(ButtonState &button, uint32_t nowMs,
                                  ButtonAction shortAction, ButtonAction longAction) {
  const bool rawHigh = digitalRead(button.pin);
  if (rawHigh != button.lastHigh) {
    button.lastHigh = rawHigh;
    button.changedAt = nowMs;
  }
  if (rawHigh != button.stableHigh && nowMs - button.changedAt >= 30) {
    button.stableHigh = rawHigh;
    if (!rawHigh) {
      button.pressedAt = nowMs;
      button.longSent = false;
    } else if (!button.longSent) {
      return shortAction;
    }
  }
  if (!button.stableHigh && !button.longSent && nowMs - button.pressedAt >= 700) {
    button.longSent = true;
    return longAction;
  }
  return ButtonAction::None;
}

ButtonAction InputManager::poll(uint32_t nowMs) {
  const ButtonAction actions[] = {
      update(buttons_[0], nowMs, ButtonAction::Previous, ButtonAction::Previous),
      update(buttons_[1], nowMs, ButtonAction::Next, ButtonAction::Next),
      update(buttons_[2], nowMs, ButtonAction::CycleView, ButtonAction::ToggleWeekStart)};
  for (ButtonAction action : actions) if (action != ButtonAction::None) return action;
  return ButtonAction::None;
}

} // namespace chrono_paper
