## Motivation
This is a simple Arduino library written to allow Lux values to be read from the Vishay VEML7700 Ambient Light Sensor with appropriate gain and integration time values being algorithmically selected. These values are selected according to an interpretation of the "Typical Software Flow Chart With Correction Formula" found on page 22 of the "Designing the VEML7700 Into an Application" application note (availabe at https://www.vishay.com/docs/84323/designingveml7700.pdf [accessed 23-05-22])
If you are able to, I would appreciate if you could compare the lux value output by this library with the lux value output by a lux meter.

## Important
This library has not been heavily tested! As such, it may return inaccurate values, may contain bugs such as infinite loops.
I have no affiliation with Vishay or the VEML7700 Ambient Light Sensor.

## Adding the Library to Your Sketch
In the Arduino editor, simply select "Sketch" -> "Include Library" -> "Add .ZIP Library" and select the .zip file included with this repo to allow Arduino to access the library.
Be sure to include `#include<VEML7700.h>` at the top of your program!

## Using the Library
Before using any of the functions included with this library, you will need to create a VEML7700 object such as:

VEML7700 myVEML(0x10)

0x10 being the default I2C address of the VEML7700 (it's the address of the one in front of me anyway). All functions will be called as a method of this object.
### Easy Functions
The only two functions you should need for simple use cases are:
- double getAmbient()
    - This function adjusts the gain and integration time in the sensor's configuration register according to values read from the ALS register, reads the value stored in the ALS register once the sensor is adjusted, calculates the approximate lux value and then applies the non-linear correction formula if necessary.
    - Returns the calculated lux value based on the ALS register
- double getWhite()
    - This function adjusts the gain and integration time in the sensor's configuration register according to the values read from the WHITE register, reads the value stored in the WHITE register once the sensor is adjusted, calculates the approximate lux value and then applies the non-linear correction formula if necessary
    - Returns the calculated lux value based on the WHITE register

### Other Public Functions
- void initialise()
    - Sets the config register to an initial state of 0x1000
    - SD=0, ALS_INT_EN=0, ALS_PERS=0, ALS_IT=0000 (100ms), ALS_GAIN=10 = 1/8
- int ALScalibrate()
    - Consists of permanent loop which:
        - Reads the value from the ALS register
        - Increases gain and then integration time until the value read from the ALS register is greater than 100
    - Returns 0 if integration time is set to max to signal that only LUX_VEML (approximation) is to be calculated
    - Returns ALS_ITNegativeAdjust() if the ALS value is greater than 100. 
- int WHITEcalibrate()
    - Behaves similarly to ALScalibrate, but uses the WHITE register of the VEML7700 as opposed to the ALS register.

### Private Functions
- int ALS_ITNegativeAdjust()
    - Once the gain has been adjusted to a suitable value, this function decreases IT to the lowest suitable value
    - If the value read from the ALS is greater than 10,000, the integration time is decremented. If the integration time is minimum, then **2 is returned** to indicate that the non-linear correction formula should be applied to the read value of the ALS
    - If the value of the ALS is less than or equal to 10,000, the function **returns 1** to indicate that the non-linear correction formula should be applied to the read value of the ALS.
- int WHITE_ITNegativeAdjust()
    - Behaves similarly to ALS_ITNegativeAdjust, but uses the WHITE register of the VEML7700 as opposed to the ALS register
- bool increment_IT()
    - If the VEML7700_IT counter is less than or equal to 2, it is incremented
        - Note that this variable is only tracked locally and is not read from the VEML7700. This may be the cause of future bugs.
    - getITBits is called with the new value of VEML7700_IT to change the value of the ALS_IT variable in the host, pending the next update to the VEML7700's config register.
    - updateConfigRegister is called to write this change to the VEML7700.
    - Returns true if VEML7700_IT counter is greater than 4, otherwise returns false
- bool decrement_IT()
    - If the VEML7700_IT counter is greater than or equal to -1, it is decremented
    - getITBits is called with the new value of VEML7700_IT to change the value of the ALS_IT variable in the host, pending the next update to the VEML7700's config register.
    - updateConfigRegister is called to write this change to the VEML7700
    - Returns true if the VEML7700_IT variable is less than or equal to -2, otherwise returns false.
- bool incrementGain()
    - If the VEML7700_gain variable is <= 3, it is incremented
    - getGainBits is called to update the ALS_gain variable
    - updateConfigRegister is called to write this change to the VEML7700
    - Returns true if VEML7700_gain >=4, returns false otherwise
- void getITBits(int ITNumber, uint8_t &ITBits)
    - Uses a switch statement with ITNumber as the input and sets the value of ITBits to the appropriate 4-bit sequence from a pool of pre-defined values.
    - This function is usually called with ALS_IT as the &ITBits argument, which is written to the config register on each call to updateConfigRegister()
- void getGainBits(int gainNumber, uint8_t &gainBits)
    - Uses a switch statement with gainNumber as the input and sets the value of gainBits to the appropriate 2-bit sequence from a pool of pre-defined values
    - This function is usually called with ALS_GAIN as the &gainBits argument, which is written to the config register on each call to updateConfigRegister()
- void sendData(uint8_t register_address, uint16_t data)
    - Uses the Wire library to write to the high and low bytes of the data variable to the address specified by register_address
    - Note: This uses the I2C address defined by VEML7700_ADDRESS in VEML7700.h (0x10). This should be changed so that it is set during the definition of the VEML7700 object.
- void readData(uint8_t register_address, uint16_t &data)
    - Uses the Wire library to read from the address defined by register_address.
    - During one function call, two bytes are read from the register and are stored in the 16-bit uint16_t &data variable.
- void updateConfigRegister()
    - Inserts ALS_GAIN, ALS_IT, ALS_PERS, ALS_INT_EN and ALS_SD variables into the uint16_t cmd and uses sendData to write cmd to the config register, 0x00.
- double getResolution()
    - Uses the current values of the ALS_GAIN and ALS_IT variables and a lookup table to obtain the current resolution of the sensor
    - This table is given on page 5 of the "Designing the VEML7700 Into an Application" application note, available at https://www.vishay.com/docs/84323/designingveml7700.pdf
    - Returns the current resolution of the sensor
- int getIT_ms()
    - Uses the value of the ALS_IT variable to return the integration time in milliseconds.
