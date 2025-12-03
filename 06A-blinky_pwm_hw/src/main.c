#include <zephyr/kernel.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/drivers/gpio.h>

#define PWM_LED_NODE DT_ALIAS(pwm_led0)
#define PWM_LED_EXT_RED_NODE DT_ALIAS(pwm_ext_led_red)
#define PWM_LED_EXT_GREEN_NODE DT_ALIAS(pwm_led_ext_green)
#define BUTTON_NODE DT_ALIAS(sw0)
#define SW_NODE DT_ALIAS(sw1)

static const struct pwm_dt_spec pwm_led = PWM_DT_SPEC_GET(PWM_LED_NODE);
static const struct pwm_dt_spec pwm_red = PWM_DT_SPEC_GET(PWM_LED_EXT_RED_NODE);
static const struct pwm_dt_spec pwm_green = PWM_DT_SPEC_GET(PWM_LED_EXT_GREEN_NODE);
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(BUTTON_NODE, gpios);
static const struct gpio_dt_spec sw = GPIO_DT_SPEC_GET(SW_NODE, gpios);

static struct gpio_callback button_cb_data;

 void button_pressed_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
 {
    pwm_set_dt(&pwm_red, pwm_red.period, pwm_red.period / 4);
    printk("Button pressed!\n");
 }

int main(void)
{
    int ret;

    if (!device_is_ready(pwm_led.dev) || !device_is_ready(pwm_led.dev)
            || !device_is_ready(pwm_led.dev)) {
        printk("pwm problem!\n");
        return -1;
    }

    if (!device_is_ready(button.port)) {
        printk("switchies problem!\n");
        return -1;
    }

    ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
    if (ret < 0) {
        printk("button problem: %d!\n", ret);
        return -1;
    }

    ret = gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_TO_ACTIVE);
    if (ret < 0) {
        printk("button int problem: %d!\n", ret);
        return -1;
    }

    gpio_init_callback(&button_cb_data, button_pressed_isr, BIT(button.pin));
    gpio_add_callback(button.port, &button_cb_data);

    if (!device_is_ready(sw.port)) {
        printk("switch port problem!\n");
        return -1;
    }

     ret = gpio_pin_configure_dt(&sw, GPIO_INPUT);
     if (ret < 0) {
        printk("switch  problem: %d!\n", ret);
        return -1;
     }

	printf("PWM LED device is ready\n");

    while (1) {
        // 100% brilho
        pwm_set_dt(&pwm_led, pwm_led.period, pwm_led.period);
        //pwm_set_dt(&pwm_green, pwm_green.period, pwm_green.period);
        k_msleep(1000);

		// 75% brilho
		pwm_set_dt(&pwm_led, pwm_led.period, pwm_led.period * 3 / 4);
        pwm_set_dt(&pwm_red, pwm_red.period, pwm_red.period * 3 / 4);
		k_msleep(1000);

        // 50% brilho
        pwm_set_dt(&pwm_led, pwm_led.period, pwm_led.period / 2);
        //pwm_set_dt(&pwm_green, pwm_green.period, pwm_green.period /2);
        k_msleep(1000);

		// 25% brilho
		pwm_set_dt(&pwm_led, pwm_led.period, pwm_led.period / 4);
		k_msleep(1000);

        // 0% brilho
        pwm_set_dt(&pwm_led, pwm_led.period, 0);
        //pwm_set_dt(&pwm_green, pwm_green.period, 0);
        k_msleep(1000);

         int state = gpio_pin_get_dt(&sw);
         if (state == 1) {
            pwm_set_dt(&pwm_green, pwm_green.period, pwm_green.period /4);
            printk("Switch on!\n");
         } else {
            pwm_set_dt(&pwm_green, pwm_green.period, pwm_green.period /2);
         }
 
    }
	return 0;
}
