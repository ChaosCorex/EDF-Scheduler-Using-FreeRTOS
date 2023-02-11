# EDF-Scheduler-Using-FreeRTOS
Utilizing FreeRTOS to implement Earliest Deadline First Scheduler

After downloading FreeRTOS from the official website https://www.freertos.org/
I used the FreeRTOS Demo ARM7_LPC2129_Keil_RVDS with keil microvision to edit
the tasks, main and FreeRTOSConfig files to implement EDF Scheduler functionalities

These changes included Cloning the xTaskCreate function to make a modified version
xTaskPeriodicCreate, a function that receives the periodicity of the task as a parameter,
Using a completely different ReadyList for periodic functions,
and accounting for some differences in behaviour relative to context switching
