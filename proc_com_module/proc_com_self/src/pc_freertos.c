#include <rmw_microros/rmw_microros.h>
#include <rmw_microxrcedds_c/config.h>
#include <uxr/client/transport.h>
#include <std_msgs/msg/string.h>
#include <rcl/error_handling.h>
#include <rclc/executor.h>
#include <rclc/rclc.h>
#include <FreeRTOS.h>
#include <cmsis_os.h>
#include <stdbool.h>
#include <rcl/rcl.h>
#include <unistd.h>
#include <usart.h>
#include <stdio.h>
#include <main.h>
#include <stdint.h>
#include <string.h>

/******************************* Aliases **************************************/
#define BUF_SIZE 64

/****************************** Global Variables ******************************/
uint8_t rxbuff[BUF_SIZE] = {0};


/********************************* Prototypes ********************************/
extern bool cubemx_transport_open(struct uxrCustomTransport *transport);
extern bool cubemx_transport_close(struct uxrCustomTransport *transport);
extern size_t cubemx_transport_write(struct uxrCustomTransport *transport, const uint8_t *buf, size_t len, uint8_t *err);
extern size_t cubemx_transport_read(struct uxrCustomTransport *transport, uint8_t *buf, size_t len, int timeout, uint8_t *err);
extern void microros_deallocate(void *pointer, void *state);
extern void *microros_allocate(size_t size, void *state);
extern void *microros_reallocate(void *pointer, size_t size, void *state);
extern void *microros_zero_allocate(size_t number_of_elements, size_t size_of_element, void *state);

/* This task creates and handles the execution of ros node */
void start_uros_task(void *argument)
{
    rmw_uros_set_custom_transport(true, (void *)&huart1, cubemx_transport_open, cubemx_transport_close, cubemx_transport_write, cubemx_transport_read);

    rcl_allocator_t freeRTOS_allocator = rcutils_get_zero_initialized_allocator();
    freeRTOS_allocator.allocate = microros_allocate;
    freeRTOS_allocator.deallocate = microros_deallocate;
    freeRTOS_allocator.reallocate = microros_reallocate;
    freeRTOS_allocator.zero_allocate = microros_zero_allocate;

    if (!rcutils_set_default_allocator(&freeRTOS_allocator))
    {
        printf("Error on default allocators (line %d)\n", __LINE__);
    }

    // micro-ROS app
    std_msgs__msg__String msg;

    rcl_allocator_t allocator;
    allocator = rcl_get_default_allocator();

    rclc_support_t support;
    rclc_support_init(&support, 0, NULL, &allocator);

    rcl_node_t node;
    rclc_node_init_default(&node, "rms_node", "", &support);

    rcl_publisher_t publisher;
    rclc_publisher_init_default(&publisher, &node, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String), "test_node");

    std_msgs__msg__String__init(&msg);
    msg.data.data = malloc(BUF_SIZE);
    msg.data.capacity = BUF_SIZE;

    while (1)
    {
        HAL_UART_Receive_DMA(&huart2, rxbuff, BUF_SIZE);
        memcpy(msg.data.data, rxbuff, BUF_SIZE);
        msg.data.size = strlen(msg.data.data);
        rcl_publish(&publisher, &msg, NULL);
    }
}