#include "TUM_Draw.h"
#include "TUM_Event.h"
#include "TUM_Ball.h"
#include "TUM_Sound.h"
#include "TUM_Utils.h"
#include "TUM_Print.h"

#include "defines.h"
#include "main.h"
#include "demo_tasks.h"
#include "draw.h"
#include "async_message_queues.h"
#include "async_sockets.h"
#include "buttons.h"
#include "state_machine.h"

TaskHandle_t DemoTask1 = NULL;
TaskHandle_t DemoTask2 = NULL;
TaskHandle_t DemoTask3 = NULL;
TaskHandle_t DemoSendTask = NULL;

TaskHandle_t TaskDrawCircle = NULL;
TaskHandle_t TaskToggleFlag1 = NULL;
TaskHandle_t TaskToggleFlag2 = NULL;

int xCircle1 = 100;
int yCircle1 = 100;
int radiusCircle1 = 50;

int xCircle2 = 300;
int yCircle2 = 100;
int radiusCircle2 = 70;

SDL_bool flagDrawCircle1 = SDL_FALSE;
SDL_bool flagDrawCircle2 = SDL_FALSE;

void vTaskDrawCircle (void *pvParameters)
{
    TickType_t xLastWakeTime, prevWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    prevWakeTime = xLastWakeTime;

    

    while (1) {
        if (DrawSignal)
            if (xSemaphoreTake(DrawSignal, portMAX_DELAY) ==
                pdTRUE) {
                xLastWakeTime = xTaskGetTickCount();

                xGetButtonInput(); // Update global button data
                
                vDrawClearScreen();

                if(flagDrawCircle1)
                {
                    tumDrawCircle(xCircle1,yCircle1,radiusCircle1,Red);
                }
                if(!flagDrawCircle1)
                {
                    tumDrawCircle(xCircle1,yCircle1,radiusCircle1,White);
                }
                if(flagDrawCircle2)
                {
                    tumDrawCircle(xCircle2,yCircle2,radiusCircle2,Blue);
                }
                if(!flagDrawCircle2)
                {
                    tumDrawCircle(xCircle2,yCircle2,radiusCircle2,White);
                }
                
                // Check for state change
                vCheckStateInput();

                // Keep track of when task last ran so that you know how many ticks
                //(in our case miliseconds) have passed so that the balls position
                // can be updated appropriatley
                prevWakeTime = xLastWakeTime;
            }
    }
}

void vTaskToggleFlag1 (void *pvParameters)
{
    const TickType_t xDelayFlag1 = 250 / portTICK_PERIOD_MS;
    TickType_t xLastWakeTime, prevWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    prevWakeTime = xLastWakeTime;

    while (1) {
        if (DrawSignal)
            if (xSemaphoreTake(DrawSignal, portMAX_DELAY) ==
                pdTRUE) {
                xLastWakeTime = xTaskGetTickCount();

                xGetButtonInput(); // Update global button data

                vTaskDelay(xDelayFlag1);
                flagDrawCircle1 = !flagDrawCircle1;


                // Check for state change
                vCheckStateInput();

                // Keep track of when task last ran so that you know how many ticks
                //(in our case miliseconds) have passed so that the balls position
                // can be updated appropriatley
                prevWakeTime = xLastWakeTime;
            }
    }
    
}

void vTaskToggleFlag2 (void *pvParameters)
{
    const TickType_t xDelayFlag2 = 500 / portTICK_PERIOD_MS;
    TickType_t xLastWakeTime, prevWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    prevWakeTime = xLastWakeTime;

    while (1) {
        if (DrawSignal)
            if (xSemaphoreTake(DrawSignal, portMAX_DELAY) ==
                pdTRUE) {
                xLastWakeTime = xTaskGetTickCount();

                xGetButtonInput(); // Update global button data

                vTaskDelay(xDelayFlag2);
                flagDrawCircle2 = !flagDrawCircle2;


                // Check for state change
                vCheckStateInput();

                // Keep track of when task last ran so that you know how many ticks
                //(in our case miliseconds) have passed so that the balls position
                // can be updated appropriatley
                prevWakeTime = xLastWakeTime;
            }
    }
    
}


void vDemoTask1(void *pvParameters)
{
    vDrawInitAnnimations();

    TickType_t xLastFrameTime = xTaskGetTickCount();

    while (1) {
        if (DrawSignal)
            if (xSemaphoreTake(DrawSignal, portMAX_DELAY) ==
                pdTRUE) {
                tumEventFetchEvents(FETCH_EVENT_BLOCK |
                                    FETCH_EVENT_NO_GL_CHECK);
                xGetButtonInput(); // Update global input

                vDrawClearScreen();

                vSolutionAssignmentTwo();
            
                xLastFrameTime = xTaskGetTickCount();

                // Draw FPS in lower right corner
                vDrawFPS();

                // Get input and check for state change
                vCheckStateInput();
            }
    }
}

void playBallSound(void *args)
{
    tumSoundPlaySample(a3);
}

void vDemoTask2(void *pvParameters)
{
    TickType_t xLastWakeTime, prevWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    prevWakeTime = xLastWakeTime;

    while (1) {
        if (DrawSignal)
            if (xSemaphoreTake(DrawSignal, portMAX_DELAY) ==
                pdTRUE) {
                xLastWakeTime = xTaskGetTickCount();

                xGetButtonInput(); // Update global button data

                vDrawClearScreen();


                // Check for state change
                vCheckStateInput();

                // Keep track of when task last ran so that you know how many ticks
                //(in our case miliseconds) have passed so that the balls position
                // can be updated appropriatley
                prevWakeTime = xLastWakeTime;
            }
    }
}

void vDemoTask3(void *pvParameters)
{
    TickType_t xLastWakeTime, prevWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    prevWakeTime = xLastWakeTime;

    ball_t *my_ball = createBall(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, Blue,
                                 20, 1000, &playBallSound, NULL);
    setBallSpeed(my_ball, 250, 250, 0, SET_BALL_SPEED_AXES);

    wall_t *left_wall = NULL, *right_wall = NULL, *top_wall = NULL, *bottom_wall = NULL;
    vCreateWalls(&left_wall, &right_wall, &top_wall, &bottom_wall);

    unsigned char collisions = 0;

    prints("Task 1 init'd\n");

    while (1) {
        if (DrawSignal)
            if (xSemaphoreTake(DrawSignal, portMAX_DELAY) ==
                pdTRUE) {
                xLastWakeTime = xTaskGetTickCount();

                xGetButtonInput(); // Update global button data

                vDrawClearScreen();
                vDrawStaticItems();
                vDrawWalls(left_wall, right_wall, top_wall, bottom_wall);

                // Check if ball has made a collision
                collisions = checkBallCollisions(my_ball, NULL,
                                                 NULL);
                if (collisions) {
                    prints("Collision\n");
                }

                // Update the balls position now that possible collisions have
                // updated its speeds
                updateBallPosition(
                    my_ball, xLastWakeTime - prevWakeTime);

                vDrawBall(my_ball);

                // Draw FPS in lower right corner
                vDrawFPS();

                // Check for state change
                vCheckStateInput();

                // Keep track of when task last ran so that you know how many ticks
                //(in our case miliseconds) have passed so that the balls position
                // can be updated appropriatley
                prevWakeTime = xLastWakeTime;
            }
    }
}

void vDemoSendTask(void *pvParameters)
{
    static char *test_str_1 = "UDP test 1";
    static char *test_str_2 = "UDP test 2";
    static char *test_str_3 = "TCP test";

    while (1) {
        prints("*****TICK******\n");
        if (mq_one) {
            aIOMessageQueuePut(mq_one_name, "Hello MQ one");
        }
        if (mq_two) {
            aIOMessageQueuePut(mq_two_name, "Hello MQ two");
        }

        if (udp_soc_one)
            aIOSocketPut(UDP, NULL, UDP_TEST_PORT_1, test_str_1,
                         strlen(test_str_1));
        if (udp_soc_two)
            aIOSocketPut(UDP, NULL, UDP_TEST_PORT_2, test_str_2,
                         strlen(test_str_2));
        if (tcp_soc)
            aIOSocketPut(TCP, NULL, TCP_TEST_PORT, test_str_3,
                         strlen(test_str_3));

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

int createDemoTasks(void)
{
    if (xTaskCreate(vDemoTask1, "DemoTask1", mainGENERIC_STACK_SIZE * 2,
                    NULL, mainGENERIC_PRIORITY + 1, &DemoTask1) != pdPASS) {
        PRINT_TASK_ERROR("DemoTask1");
        goto err_task1;
    }
    if (xTaskCreate(vDemoTask2, "DemoTask2", mainGENERIC_STACK_SIZE * 2,
                    NULL, mainGENERIC_PRIORITY + 1, &DemoTask2) != pdPASS) {
        PRINT_TASK_ERROR("DemoTask2");
        goto err_task2;
    }
    if (xTaskCreate(vTaskDrawCircle, "TaskDrawCircle", mainGENERIC_STACK_SIZE * 2,
                    NULL, mainGENERIC_PRIORITY + 1, &TaskDrawCircle) != pdPASS) {
        PRINT_TASK_ERROR("TaskDrawCircle");
        goto err_taskDrawCircle;
    }
    if (xTaskCreate(vTaskToggleFlag1, "TaskToggleFlag1", mainGENERIC_STACK_SIZE * 2,
                    NULL, mainGENERIC_PRIORITY + 2, &TaskToggleFlag1) != pdPASS) {
        PRINT_TASK_ERROR("TaskToggleFlag1");
        goto err_taskToggleFlag1;
    }
    if (xTaskCreate(vTaskToggleFlag2, "TaskToggleFlag2", mainGENERIC_STACK_SIZE * 2,
                    NULL, mainGENERIC_PRIORITY + 2, &TaskToggleFlag2) != pdPASS) {
        PRINT_TASK_ERROR("TaskToggleFlag2");
        goto err_taskToggleFlag2;
    }
    if (xTaskCreate(vDemoTask3, "DemoTask3", mainGENERIC_STACK_SIZE * 2,
                    NULL, mainGENERIC_PRIORITY + 1, &DemoTask3) != pdPASS) {
        PRINT_TASK_ERROR("DemoTask3");
        goto err_task3;
    }

    if (xTaskCreate(vDemoSendTask, "DemoSendTask", mainGENERIC_STACK_SIZE * 2, NULL,
                    configMAX_PRIORITIES - 1, &DemoSendTask) != pdPASS) {
        PRINT_TASK_ERROR("DemoSendTask");
        goto err_send_task;
    }

    vTaskSuspend(DemoTask1);
    vTaskSuspend(DemoTask2);
    vTaskSuspend(TaskDrawCircle);
    vTaskSuspend(TaskToggleFlag1);
    vTaskSuspend(TaskToggleFlag2);
    vTaskSuspend(DemoTask3);

    return 0;

err_send_task:
    vTaskDelete(DemoTask3);
err_task3:
    vTaskDelete(TaskToggleFlag2);
err_taskToggleFlag2:
    vTaskDelete(TaskToggleFlag1);
err_taskToggleFlag1:
    vTaskDelete(TaskDrawCircle);
err_taskDrawCircle:
    vTaskDelete(DemoTask2);
err_task2:
    vTaskDelete(DemoTask1);
err_task1:
    return -1;
}

void deleteDemoTasks(void)
{
    if (DemoTask1) {
        vTaskDelete(DemoTask1);
    }
    if (DemoTask2) {
        vTaskDelete(DemoTask2);
    }
    if(TaskDrawCircle){
        vTaskDelete(TaskDrawCircle);
    }
    if(TaskToggleFlag1){
        vTaskDelete(TaskToggleFlag1);
    }
    if(TaskToggleFlag2){
        vTaskDelete(TaskToggleFlag2);
    }
    if (DemoTask3) {
        vTaskDelete(DemoTask3);
    }
    if (DemoSendTask) {
        vTaskDelete(DemoSendTask);
    }
}