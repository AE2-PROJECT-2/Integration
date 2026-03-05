#ifndef CUSTOMFUNCTIONS_H
#define CUSTOMFUNCTIONS_H

//---------------------------------------------------------------------------------------------------------
// FUNCTIONS
// Here the custom functions are defined
//---------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------
// Function that prints an error to the serial port and makes the RGBLED red
//---------------------------------------------------------------------------------------------------------
void Error(String errorMessage) {
  Serial.println(errorMessage);
  myLed.SetColor(RED);
}

//---------------------------------------------------------------------------------------------------------
// Function that gives the state of a tristate switch: LOSTATE (-1), MIDSTATE (0), HIGHSTATE (1)
// Note that the PPM Signals are such that they produce low values when the switch is physically up/high
//---------------------------------------------------------------------------------------------------------
triStateSwitch GetChannelTriState(int channelValue){
  if (channelValue < PPMTHRESHLOW) {
    return HISTATE;
  } else if (channelValue < PPMTHRESHHIGH) {
    return MIDSTATE;
  } else {
    return LOSTATE;
  }
}

//---------------------------------------------------------------------------------------------------------
// Function that controls the leading gear and hatch, based on the switch state
//---------------------------------------------------------------------------------------------------------

void HandleLandingGearSwitch(int channelValue){
  // landingGearSwitchState can have the state LOSTATE (-1), MIDSTATE (0), or HISTATE (1)
  // the function GetChannelTriState returns one of these three states depending on the value of the channel
  landingGearSwitchState =  GetChannelTriState(channelValue);

  if (landingGearSwitchState < landingGearSwitchStatePrev) {
    // towards rectracted state
    if (landingGearSwitchState == MIDSTATE) {
      // So it was HISTATE, now pull in the gear
      servoTargetPos[SERVOLANDINGGEAR] = SERVOLANDINGGEARPOSRETRACTED;
      Serial.println("Retract gear");
    } else {
      // what shall we do here?

    }
  }
  if (landingGearSwitchState > landingGearSwitchStatePrev) {
    // towards extended state
    // build another if statement like above, do distinguish between a hatch action and a gear action.

  }
  landingGearSwitchStatePrev = landingGearSwitchState; //update the previous state for the next loop

}


//---------------------------------------------------------------------------------------------------------
// Function that controls the servos to their setpoints with limited speed
//---------------------------------------------------------------------------------------------------------
void UpdateServos(){
  for (int i = 0; i < NUMBEROFSERVOS; i++) {
    // Calculate the differrence between target and current position
    float posError = servoTargetPos[i] - servoActualPos[i];
    float maxStep = MAXSERVOSPEEDDEGS[i]*LOOPTIMESERVOMS/1000.0; //maximum step a servo can make in a loopcycle based on maximum speed
    if (posError > maxStep) {
      // if the difference is larger than the maximum step size, limit the step size
      posError = maxStep;
    }
    if (posError < -maxStep) {
      // same, but for negative values
      posError = -maxStep;
    }
    servoActualPos[i] = servoActualPos[i]+posError;

    // Write the rounded off setpoint to the servo motor
    myServo[i].write(int(servoActualPos[i]));
  }
}

#endif  //CUSTOMFUNCTIONS_H