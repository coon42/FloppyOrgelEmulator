#include "../StackBasedFsm.h"
#include "../../hal/hal_misc.h"

#include "_example.h"

//--------------------------------------------------------------------------------------------------------------
// Example state implementation
//
// This example shows, how to implement a state.
// - A state has always its own file, which consists of a collection of callbacks.
// - At the end of the file there is always an initialization function, which activates the state by setting
//   the callbacks.
// - The context struct can be used to store local variables. If this state gets called multiple times in a
//   state chain, an array of contexts can be used, to get an own context for each state.
//--------------------------------------------------------------------------------------------------------------

static struct {
  StackBasedFsm_t* pFsm;

} context;

static void onAction() {
  hal_printf("example::onAction()");

  // This function is called, when the player presses the action button on the game pad.
  // On the NES game pad this is the 'A' button.
}

static void onBack() {
  hal_printf("example::onBack()");

  // This function is called, when the player presses the back button on the Gamepad.
  // On the NES game pad this is the 'B' button.

  // In most cases you want to go to the previous screen. This is done by executing 'fsmPop(context.pFsm);'
  fsmPop(context.pFsm);
}

static void onEnter(void* pArgs) {
  hal_printf("example::onEnter()");

  // This function is called, when the state is entered for the first time.
  context.pFsm = pArgs;
}

static void onReenter() {
  hal_printf("example::onReenter()");

  // This function is called, when the user pressed the back button on a higher state and lands back in this 
  // state again.
}

static void onLeaveState() {
  hal_printf("example::onLeaveState()");

  // This function is called, when the user leaves the current state. Either by going to a next state or by 
  // going back to the previous state.
}

static void onTick() {
  hal_printf("example::onTick()");

  // This function is called frequently as fast as possible, as long as this state is active. This state can be
  // used for time critical applications like animations, audio playback or advanced user interaction.
}

static void onDirection(bool south, bool north, bool west, bool east) {
  hal_printf("example::onDirection()");

  // This function is called, if the user presses one of the direction buttons on the game pad.
}

// Do not change the following implementation! Just copy it unchanged and always as last function of the state.
void example(FsmState* state, void* pArgs) {
  state->onAction = onAction;
  state->onBack = onBack;
  state->onDirection = onDirection;
  state->onEnterState = onEnter;
  state->onReenterState = onReenter;
  state->onLeaveState = onLeaveState;
  state->onTick = onTick;

  state->onEnterState(pArgs);
}
