# FreeRTOS-USING-ESP32
ISA VESIT SUMMER BOOTCAMP 26-27
## Fundamentals
`TaskHandle_t` is a variable that points a FreeRTOS task , allowing to control.

`void setup()` is a function pre-defined use to tell the microcontroller what the the things that will be used from microcontroller like **GPIO Pins,Communication Protocols**

`void loop()` is a function pre-defined for the execution ***Superloop***

`pinMode`(GPIO_NUMBER,MODE[**INPUT/OUTPUT** ]) is a function used in `void setup()` for initialization of the **GPIO** Pin of the board

`digitalWrite` (GPIO_NUMBER, VALUE( **HIGH/LOW** )) is a function used to tell the gpio what it needs to do

`analogRead( GPIO_NUMBER)` is used to read analog values from the pin 

` ledcAttach(LED_PIN, PWM_FREQ, PWM_RESOLUTION)` use for initialization of pwm in esp32 boards

`ledcWrite(LED_PIN, brightness)` used to write. similar to digitalWrite

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

## Heap Memory

There is a function that you can call inside your task to determine the stack usage: the `uxTaskGetStackHighWaterMark()` function. That function determines the allocated stack size that is not being used.

The heap is a shared memory pool in the ESP32’s SRAM, used for dynamic memory allocation, including task stacks, buffers, and other runtime data allocated by FreeRTOS or the Arduino core. We can call the `xPortGetFreeHeapSize()` function in our code to determine the free heap.

## Queue
Use the `xQueueCreate(size, item_size)` to create a queue—`size` corresponds to the number of items that can be on the queue and `item_size` is the bytes size of heap allocated for each item on the queue.

To send data to a queue use the `xQueueSend()` to add data to the queue. Or use `xQueueSendFromISR()` if sending the data from an ISR (interrupt service routine).

The `xQueueReceive()` function reads data from the queue, if available.

## Semaphores Basic Functions
To create a binary semaphore, use the` xSemaphoreCreateBinary()` function. It returns a `SemaphoreHandle_t` handle if successful, or NULL if the creation fails.

To create a counting semaphore, use the `xSemaphoreCreateCounting()` function. It returns a `SemaphoreHandle_t` handle if successful, or NULL if the creation fails. Pass as an argument the maximum count.

Use the `xSemaphoreTake(semaphore, timeout)` function in a task to wait for or take a semaphore. For a binary semaphore, it blocks until the semaphore is available (state 1), setting it to 0 when taken.

For a counting semaphore, it decrements the count if greater than 0, or blocks if the count is 0. The timeout parameter specifies how long to wait (in ticks); `portMAX_DELAY` means wait indefinitely. This means the task will be blocked until there’s a semaphore value to take.

To give a semaphore use the `xSemaphoreGive()` function if inside a task, or `xSemaphoreGiveFromISR()` if used in ISRs (interrupt service routine functions).

## Timer
To create a timer, you use the `xTimerCreate()` function and pass the following parameters as arguments in this order:
```
timer name
period
autoReload (pdTRUE for periodic timer, or pdFALSE for one-shot timer)
timerID (a user-defined value passed to the callback)
callback function

xTimerCreate(
    "BlinkTimer",                   // Timer name
    1000 / portTICK_PERIOD_MS,      // 1s period
    pdTRUE,                         // Auto-reload (periodic timer)
    NULL,                           // Timer ID
    BlinkCallback                   // Callback function
);
```

To start a timer use `xTimerStart(timer, blockTime)`. The first argument is the timer handler, and the second argument is the number of seconds to wait before starting the timer.

To stop a running timer, you just need to call `xTimerStop(timer, blockTime)`. The arguments are the same as the previous function.

o reset a timer, which means restarting a timer’s countdown, even if it is already running, call `xTimerReset(timer, blockTime)`. The arguments are the same as the previous functions.

If you want to start a timer from an ISR (interrupt service routine function), you should call `xTimerStartFromISR(timer, &higherPriorityTaskWoken).`

`higherPriorityTaskWoken` can be `pdTRUE` or `pdFALSE`. Set to `pdTRUE` if the task is of higher priority and we should switch to it immediately. In this case, we should call `portYIELD_FROM_ISR()` from the ISR to switch to that task immediately for real-time responsiveness.
