#ifndef LitterboxPinConfig
#define LitterboxPinConfig

using GpioNums = gpio_num_t;

// todo change
const GpioNums buttonPin = GPIO_NUM_32;

const GpioNums weightSensorDataPin1 = GPIO_NUM_15;
const GpioNums weightSensorClockPin1 = GPIO_NUM_2;

const GpioNums weightSensorDataPin2 = GPIO_NUM_4;
const GpioNums weightSensorClockPin2 = GPIO_NUM_16;

const GpioNums weightSensorDataPin3 = GPIO_NUM_13;
const GpioNums weightSensorClockPin3 = GPIO_NUM_12;

const GpioNums weightSensorDataPin4 = GPIO_NUM_14;
const GpioNums weightSensorClockPin4 = GPIO_NUM_27;

const GpioNums rgbPin = GPIO_NUM_26;

#endif
