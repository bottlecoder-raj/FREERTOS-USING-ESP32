# FreeRTOS-USING-ESP32
ISA VESIT SUMMER BOOTCAMP 26-27

`TaskHandle_t` is a variable that points a FreeRTOS task , allowing to control.

`void setup()` is a function pre-defined use to tell the microcontroller what the the things that will be used from microcontroller like **GPIO Pins,Communication Protocols**

`void loop()` is a function pre-defined for the execution ***Superloop***

`pinMode`(GPIO_NUMBER,MODE[**INPUT/OUTPUT** ]) is a function used in `void setup()` for initialization of the **GPIO** Pin of the board

`digitalWrite` (GPIO_NUMBER, VALUE( **HIGH/LOW** )) is a function used to tell the gpio what it needs to do

`Serial.begin`(*baud_rate*) is pre-defined function to initialze communication between host and slave(PC AND ESP32)

`Serial.println()` is used to print data on the ***Serial Monitor*** in the Computer.

`vTaskDelay(1000/portTICK_PERIOD_MS)` is a function similar to ***delay(1000)*** but for FreeRTOS **It does not block the code but allowing other task to run during that time**

`xPortGetCoreID()` returns which core the task is running

` xTaskCreatePinnedToCore()` function  is used  to actually create a FreeRTOS task and assign it to a specific core

``` 
xTaskCreatePinnedToCore(
             BlinkTask,      // Task function
             "BlinkTask",   // Task name
             10000,           // Stack size (bytes)
             NULL,            // Parameters
             1,                   // Priority
             &BlinkTaskHandle,  // Task handle
             1                  // Core 1
) 
```
`Stack Size` The task stack size is the amount of memory allocated for the task to store its variables, function calls, and temporary data while it runs, ensuring it has enough space to operate without crashing the ESP32.

*The loop() is empty because the FreeRTOS scheduler will run the task. However, it is possible to add code to the loop() to run any other commands you want.*

`vTaskSuspend` and `vTaskResume` are self-explanatory