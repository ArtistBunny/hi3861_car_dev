#include <stdio.h>
#include <hi_io.h>
#include <hi_pwm.h>
#include <stdlib.h>
#include <memory.h>
#include <hi_gpio.h>
#include <hi_task.h>
#include <hi_time.h>

hi_void gpio_control(hi_io_name gpio, hi_u8 func, hi_gpio_idx idx, hi_gpio_dir dir, hi_gpio_value value){
    hi_io_set_func(gpio, func);
    hi_gpio_set_dir(idx, dir);
    hi_gpio_set_ouput_val(idx, value);
}

hi_void pwm_control(hi_io_name name, hi_u8 func, hi_pwm_port port, hi_u16 duty) { 
    hi_io_set_func(name, func);
    hi_pwm_init(port);
    hi_pwm_set_clock(PWM_CLK_160M);
    hi_pwm_start(port. duty, 40000);

}

hi_void car_forward_6000(hi_void) {
    gpio_control(HI_IO_NAME_GPIO_0, HI_IO_FUNC_GPIO_0_GPIO, HI_GPIO_IDX_0, HI_GPIO_DIR_OUT, HI_GPIO_VALUE1);
    pwm_control(HI_IO_NAME_GPIO_1, HI_IO_FUNC_GPIO_1_PWM4_OUT, HI_PWM_PORT_PWM4, 100);
    gpio_control(HI_IO_NAME_GPIO_9, HI_IO_FUNC_GPIO_9_GPIO, HI_GPIO_IDX_9, HI_GPIO_DIR_OUT, HI_GPIO_VALUE1);
    pwm_control(HI_IO_NAME_GPIO_10, HI_IO_FUNC_GPIO_1_PWM4_OUT, HI_PWM_PORT_PWM4, 100);
}

hi_void car_forward(hi_void) {
    gpio_control(HI_IO_NAME_GPIO_0, HI_IO_FUNC_GPIO_0_GPIO, HI_GPIO_IDX_0, HI_GPIO_DIR_OUT, HI_GPIO_VALUE1);
    pwm_control(HI_IO_NAME_GPIO_1, HI_IO_FUNC_GPIO_1_PWM4_OUT, HI_PWM_PORT_PWM4, 6000);
    gpio_control(HI_IO_NAME_GPIO_9, HI_IO_FUNC_GPIO_9_GPIO, HI_GPIO_IDX_9, HI_GPIO_DIR_OUT, HI_GPIO_VALUE1);
    pwm_control(HI_IO_NAME_GPIO_10, HI_IO_FUNC_GPIO_1_PWM4_OUT, HI_PWM_PORT_PWM4, 6000);
}

hi_void car_turn(hi_void) {
    gpio_control(HI_IO_NAME_GPIO_0, HI_IO_FUNC_GPIO_0_GPIO, HI_GPIO_IDX_0, HI_GPIO_DIR_OUT, HI_GPIO_VALUE1);
    pwm_control(HI_IO_NAME_GPIO_1, HI_IO_FUNC_GPIO_1_PWM4_OUT, HI_PWM_PORT_PWM4, 100);
    gpio_control(HI_IO_NAME_GPIO_9, HI_IO_FUNC_GPIO_9_GPIO, HI_GPIO_IDX_9, HI_GPIO_DIR_OUT, HI_GPIO_VALUE1);
    pwm_control(HI_IO_NAME_GPIO_10, HI_IO_FUNC_GPIO_1_PWM4_OUT, HI_PWM_PORT_PWM4, 40000);
}   

hi_void car_stop(hi_void) {
    // 延时执行，value全1，刹车
    gpio_control(HI_IO_NAME_GPIO_0, HI_IO_FUNC_GPIO_0_GPIO, HI_GPIO_IDX_0, HI_GPIO_DIR_OUT, HI_GPIO_VALUE1);
    gpio_control(HI_IO_NAME_GPIO_1, HI_IO_FUNC_GPIO_1_GPIO, HI_GPIO_IDX_1, HI_GPIO_DIR_OUT, HI_GPIO_VALUE1);
    gpio_control(HI_IO_NAME_GPIO_9, HI_IO_FUNC_GPIO_9_GPIO, HI_GPIO_IDX_9, HI_GPIO_DIR_OUT, HI_GPIO_VALUE1);
    gpio_control(HI_IO_NAME_GPIO_10, HI_IO_FUNC_GPIO_10_GPIO, HI_GPIO_IDX_10, HI_GPIO_DIR_OUT, HI_GPIO_VALUE1);
}

hi_void robot_l9110s_demo(hi_void* param){
    printf("start test l9110s\r\n");
    // // 轮子一
    // gpio_control(HI_IO_NAME_GPIO_0, HI_IO_FUNC_GPIO_0_GPIO, HI_GPIO_IDX_0, HI_GPIO_DIR_OUT, HI_GPIO_VALUE1);
    // gpio_control(HI_IO_NAME_GPIO_1, HI_IO_FUNC_GPIO_1_GPIO, HI_GPIO_IDX_1, HI_GPIO_DIR_OUT, HI_GPIO_VALUE0);
    // // 轮子二
    // gpio_control(HI_IO_NAME_GPIO_9, HI_IO_FUNC_GPIO_9_GPIO, HI_GPIO_IDX_9, HI_GPIO_DIR_OUT, HI_GPIO_VALUE1);
    // gpio_control(HI_IO_NAME_GPIO_10, HI_IO_FUNC_GPIO_10_GPIO, HI_GPIO_IDX_10, HI_GPIO_DIR_OUT, HI_GPIO_VALUE0);
    car_forward();
    hi_sleep(2000);
    car_forward_6000();
    car_turn();
    hi_sleep(2000);
    car_stop();
} 

hi_u32 g_car_control_demo_task_id= 0;
hi_void app_l9110s_task(hi_void)
{
    hi_u32 ret = 0;
    hi_task_attr attr = {0};
    
    hi_task_lock();
    attr.stack_size = 1024;
    attr.task_prio = 25;
    attr.task_name = (hi_char*)"car_l9110s_demo";
    
    ret = hi_task_create(&g_car_control_demo_task_id, &attr, robot_l9110s_demo, HI_NULL);
    if (ret != HI_ERR_SUCCESS) {
        printf("Falied to create car control demo task!\n");
    }
    hi_task_unlock();
}