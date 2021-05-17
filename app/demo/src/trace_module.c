#include <trace_module.h>
#include <hi_timer.h>
extern hi_u8   g_car_control_mode;
extern hi_u8   g_led_control_module;
extern hi_u16  g_car_modular_control_module;
extern hi_u16  g_car_direction_control_module;

extern hi_void regress_middle(hi_void);
extern hi_void  car_go_forward(hi_void);
extern hi_void  car_stop(hi_void);
extern hi_void  car_turn_left(hi_void);
extern hi_void  car_turn_right(hi_void);


/*set gpio input mode*/
hi_void set_gpio_input_mode(hi_io_name id, hi_u8 val, hi_gpio_idx idx, hi_gpio_dir dir)
{
    hi_io_set_func(id, val);
    hi_gpio_set_dir(idx, dir);
}

/*
init gpio11/12 as a input io
GPIO 11 connects the left tracking module
GPIO 11 connects the right tracking module
*/
hi_void trace_module_init(hi_void)
{
//    set_gpio_input_mode(HI_IO_NAME_GPIO_11,HI_IO_FUNC_GPIO_11_GPIO,HI_GPIO_IDX_11,HI_GPIO_DIR_IN);
//    set_gpio_input_mode(HI_IO_NAME_GPIO_12,HI_IO_FUNC_GPIO_12_GPIO,HI_GPIO_IDX_12,HI_GPIO_DIR_IN);
}

/*get do value*/
// hi_gpio_value get_do_value(hi_gpio_idx idx)
// {
//     hi_gpio_value val = HI_GPIO_VALUE0;
//     hi_u32 ret = hi_gpio_get_input_val(idx, &val);
//     if (ret != HI_ERR_SUCCESS) {
//         return HI_ERR_FAILURE;
//     }
//     return val;
// }

hi_gpio_value get_do_value(hi_adc_channel_index idx)
{
    // hi_gpio_value val = HI_GPIO_VALUE0;
    // hi_u32 ret = hi_gpio_get_input_val(idx, &val);
    // if (ret != HI_ERR_SUCCESS) {
    //     return HI_ERR_FAILURE;
    // }
    // return val;
    hi_u16 data = 0;
    hi_u32 ret = 0;

    for (int i = 0; i < ADC_TEST_LENGTH; i++) {
        //printf("________zyh(%d  %d)____________\n", ADC_TEST_LENGTH, __LINE__);
        ret = hi_adc_read(idx, &data, HI_ADC_EQU_MODEL_4, HI_ADC_CUR_BAIS_DEFAULT, 0xF0);//ADC_Channal_6  自动识别模式  CNcomment:4次平均算法模式 CNend */
        //printf("________zyh(%d)____________\n", data);
        if (ret != HI_ERR_SUCCESS) 
        {
            printf("hi_adc_read failed\n");
            // return  HI_NULL;
        }
    }

    if (idx == HI_ADC_CHANNEL_5) {
        printf("gpio11 m_left_value is %d \n",data);
    } else if (idx == HI_ADC_CHANNEL_0) {
        printf("gpio12 m_right_value is %d \n",data);
    }

    if((data > 1700) && (data < 1900)) {
        return HI_GPIO_VALUE0;
    } else if((data > 100) && (data < 200)){
        return HI_GPIO_VALUE1;
    }
}

/*
According to the data received by the infrared tracking module, 
send the corresponding instructions to the car
*/
#define car_speed_left 100
#define car_speed_right 100
hi_u32 g_car_speed_left = car_speed_left;
hi_u32 g_car_speed_right = car_speed_right;
hi_u8 count = 0;
hi_gpio_value io_status_left;
hi_gpio_value io_status_right;
hi_void timer1_callback(hi_u32 arg)
{
    hi_gpio_value io_status;
    if(g_car_speed_left != car_speed_left)   
    {
        count++;
        if(count >=2)
        {
            hi_gpio_get_input_val(HI_GPIO_IDX_11,&io_status);
            if(io_status != HI_GPIO_VALUE0){
                g_car_speed_left = car_speed_left;
                printf("left speed change \r\n");
                count = 0;
            }
        }

    }

    if(g_car_speed_right != car_speed_right)   
    {
        count++;
        if(count >=2)
        {
            hi_gpio_get_input_val(HI_GPIO_IDX_12,&io_status);
            if(io_status != HI_GPIO_VALUE0){
                g_car_speed_right = car_speed_right;
                printf("right speed change \r\n");
                count = 0;
            }
        }
    }
    if(g_car_speed_left != car_speed_left && g_car_speed_right != car_speed_right)
    {
        g_car_speed_left = car_speed_left;
        g_car_speed_right = car_speed_right;
    }
    hi_gpio_get_input_val(HI_GPIO_IDX_11,&io_status_left);
    hi_gpio_get_input_val(HI_GPIO_IDX_12,&io_status_right);
    if(io_status_right == HI_GPIO_VALUE0 && io_status_left != HI_GPIO_VALUE0)
    {
        g_car_speed_left = car_speed_left;
        g_car_speed_right = 60000;
    } 
    if(io_status_right != HI_GPIO_VALUE0 && io_status_left == HI_GPIO_VALUE0)
    {
        g_car_speed_left = 60000;
        g_car_speed_right = car_speed_right;
    }       
}

hi_void  trace_module(hi_void)
{

    hi_u8 current_car_modular_control_module = g_car_modular_control_module;
    hi_u8 current_car_control_mode = g_car_control_mode;
    hi_gpio_value m_left_value  = HI_GPIO_VALUE0;
    hi_gpio_value m_right_value = HI_GPIO_VALUE0;
    
    trace_module_init();
    hi_u32 timer_id1;
    hi_timer_create(&timer_id1);
    hi_timer_start(timer_id1, HI_TIMER_TYPE_PERIOD, 1, timer1_callback, 0);
    gpio_control(HI_IO_NAME_GPIO_0, HI_GPIO_IDX_0, HI_GPIO_DIR_OUT, HI_GPIO_VALUE1, HI_IO_FUNC_GPIO_0_GPIO);
    pwm_control(HI_IO_NAME_GPIO_1,HI_IO_FUNC_GPIO_1_PWM4_OUT,HI_PWM_PORT_PWM4, g_car_speed_left);
    gpio_control(HI_IO_NAME_GPIO_9, HI_GPIO_IDX_9, HI_GPIO_DIR_OUT, HI_GPIO_VALUE1, HI_IO_FUNC_GPIO_9_GPIO);
    pwm_control(HI_IO_NAME_GPIO_10,HI_IO_FUNC_GPIO_10_PWM1_OUT,HI_PWM_PORT_PWM1, g_car_speed_right);
    while (1) {
        
        hi_pwm_start(HI_PWM_PORT_PWM4, g_car_speed_left, 60000);
        hi_pwm_start(HI_PWM_PORT_PWM1, g_car_speed_right, 60000);
        //m_left_value  = get_do_value(HI_ADC_CHANNEL_5); //gpio11 ==>ADC5
        //m_right_value = get_do_value(HI_ADC_CHANNEL_0); //gpio12 ==>ADC0
        // m_left_value  = get_do_value(HI_GPIO_IDX_11);
        // m_right_value = get_do_value(HI_GPIO_IDX_12);
        /*if ((m_left_value == HI_GPIO_VALUE0) && (m_right_value == HI_GPIO_VALUE0)) {
            //car_go_forward();
            //car_trace_back();
            car_go_forward();
            /*if (flag == 1) {
                car_turn_right();
                flag = 2;
            } else if (flag == 2) {
                car_turn_left();
                flag = 1;
            }
            printf("\n\nforward\n\n");
        } else if ((m_left_value == HI_GPIO_VALUE0) && (m_right_value == HI_GPIO_VALUE1)) {   //左偏，向右转
            //car_turn_right();
            //car_trace_left();
            //hi_sleep(500);
            car_turn_right();
            //car_trace_right();
            //hi_sleep(800);
            //flag = 1;
        } else if ((m_left_value == HI_GPIO_VALUE1) && (m_right_value == HI_GPIO_VALUE0)) {   //右偏，向左转
            //car_turn_left();
            //car_trace_right();
            //hi_sleep(500);
            car_turn_left();
            //car_trace_left();
            //hi_sleep(800);
            //flag = 2;
        } else if ((m_left_value == HI_GPIO_VALUE1) && (m_right_value == HI_GPIO_VALUE1)) {   //脱离轨道
            //car_stop();
            //printf("\n\nforward\n\n");
            //car_go_forward();
            car_stop();
            //flag = 0;
        }*/
        //hi_sleep(1);
        hi_udelay(1);

        if ((current_car_modular_control_module != g_car_modular_control_module)
             || (current_car_control_mode != g_car_control_mode)) {
            break;
        }
    }
    hi_timer_delete(timer_id1);
}