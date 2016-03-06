#include "StackBasedFsm.h"
#include "../hal/hal_inputdevice.h"
#include "../hal/hal_misc.h"
#include "InputDevice.h"

static bool anyDirectionIsPressed(InputDeviceStates_t state) {
  return state.South || state.North || state.West || state.East;
}

static bool isLastDirectionEqual(InputDeviceStates_t currentState, InputDeviceStates_t lastState) {
  uint16_t currentDirectionCode = currentState.South | currentState.North | currentState.West | 
    currentState.East;

  uint16_t lastDirectionCode = lastState.South | lastState.North | lastState.West | lastState.East;

  return currentDirectionCode == lastDirectionCode;
}

static void processCursorButtons(StackBasedFsm_t* pFsm, FsmState* pState, InputDeviceStates_t* pButtonStates, 
    InputDeviceStates_t* pLastButtonStates, uint32_t* pTimeOnLastButtonPress) {

  static bool isInRepetitionMode = false;
  static uint32_t timeOnLastDirectionPress = 0;
  static uint32_t timeOnLastRepetition = 0;

  // TODO: simplify
  // Cursor delay and repetition, when holding direction button
  if (anyDirectionIsPressed(*pButtonStates)) {
    if (isLastDirectionEqual(*pButtonStates, *pLastButtonStates)) {
      if (isInRepetitionMode) {
        if (hal_clock() > timeOnLastDirectionPress + CURSOR_DELAY_MS_BEFORE_REPEAT) {
          if (hal_clock() > timeOnLastRepetition + 1000 / CURSOR_SPEED_ITEMS_PER_SECOND) {
            if (pState->onDirection)
              pState->onDirection(pFsm, pButtonStates->South, pButtonStates->North, pButtonStates->West, 
                  pButtonStates->East);

            // hal_printf("timeOnLastRepetition: %d, repeat on: %d", timeOnLastRepetition, timeOnLastRepetition + 1000 / CURSOR_SPEED_ITEMS_PER_SECOND);

            timeOnLastRepetition = hal_clock();
          }
        }
      }
      else {
        *pTimeOnLastButtonPress = hal_clock();
        timeOnLastDirectionPress = hal_clock();
        timeOnLastRepetition = hal_clock();
        isInRepetitionMode = true;

        // hal_printf("In repetition mode");
        if (pState->onDirection)
          pState->onDirection(pFsm, pButtonStates->South, pButtonStates->North, pButtonStates->West,
              pButtonStates->East);
      }
    }
  }
  else {
    if (isInRepetitionMode) {
      isInRepetitionMode = false;
      // hal_printf("End of repetition mode");
    }
  }
}

// static void checkButtonPressCondition(uint16_t currentButtonState, uint16_t lastButtonState) {
// 
//   if (pButtonStates->Action && !pLastButtonStates->Action) {
//     if (pState->onActionPress)
//       pState->onActionPress(pFsm);
// 
//     *pTimeOnLastButtonPress = hal_clock();
//   }
// }

static void processActionButtons(StackBasedFsm_t* pFsm, FsmState* pState, InputDeviceStates_t* pButtonStates,
    InputDeviceStates_t* pLastButtonStates, uint32_t* pTimeOnLastButtonPress) {

  // Force user to press button again after entering a menu
  // TODO: DRY!

  // Action button
  if (pButtonStates->Action && !pLastButtonStates->Action) {
    if (pState->onActionPress)
      pState->onActionPress(pFsm);
  }

  if (!pButtonStates->Action && pLastButtonStates->Action) {
    if (pState->onActionRelease)
      pState->onActionRelease(pFsm);

    *pTimeOnLastButtonPress = hal_clock(); // CHECKME: Is this also needed on release for debouce?
  }

  // Back button
  if (pButtonStates->Back && !pLastButtonStates->Back) {
    if (pState->onBackPress)
      pState->onBackPress(pFsm);

    *pTimeOnLastButtonPress = hal_clock();
  }

  if (!pButtonStates->Back && pLastButtonStates->Back) {
    if (pState->onBackRelease)
      pState->onBackRelease(pFsm);

    *pTimeOnLastButtonPress = hal_clock(); // CHECKME: Is this also needed on release for debouce?
  }

  // Start button
  if (pButtonStates->Start && !pLastButtonStates->Start) {
    if (pState->onStartPress)
      pState->onStartPress(pFsm);

    *pTimeOnLastButtonPress = hal_clock();
  }

  if (!pButtonStates->Start && pLastButtonStates->Start) {
    if (pState->onStartRelease)
      pState->onStartRelease(pFsm);

    *pTimeOnLastButtonPress = hal_clock(); // CHECKME: Is this also needed on release for debouce?
  }

  // Select button
  if (pButtonStates->Select && !pLastButtonStates->Select) {
    if (pState->onSelectPress)
      pState->onSelectPress(pFsm);

    *pTimeOnLastButtonPress = hal_clock();
  }

  if (!pButtonStates->Select && pLastButtonStates->Select) {
    if (pState->onSelectRelease)
      pState->onSelectRelease(pFsm);

    *pTimeOnLastButtonPress = hal_clock(); // CHECKME: Is this also needed on release for debouce?
  }
}

static bool isDebouncing(uint32_t timeOnLastButtonPress) {
  return hal_clock() > timeOnLastButtonPress + INPUT_DEVICE_DEBOUNCE_MS ? false : true;
}

void processInputDevice(StackBasedFsm_t* pFsm) {
  FsmState* pState = fsmGetCurrentState(pFsm);
  InputDeviceStates_t buttonStates = getInputDeviceState();
  static InputDeviceStates_t lastButtonStates = { 0 };
  static uint32_t timeOnLastButtonPress = 0;

  // TODO: do deboucing here!
  if (!isDebouncing(timeOnLastButtonPress)) { // CHECKME: it may be neccesarry to debounce each button individually
    processCursorButtons(pFsm, pState, &buttonStates, &lastButtonStates, &timeOnLastButtonPress);
    processActionButtons(pFsm, pState, &buttonStates, &lastButtonStates, &timeOnLastButtonPress);

    lastButtonStates = buttonStates;
  }
}