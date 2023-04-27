/*This example is to detect the JVC signal recieved and Covert it to the hexcode*/

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

/*Duration is the calculation for timer in microseconds*/

#define 	DURATION 	(k_cycle_get_32() - timer)/168
#define   	IR1_NODE        DT_ALIAS(ir1)

/* Nec Code to be recieved*/

uint32_t jvc_code;

static const struct gpio_dt_spec ir = GPIO_DT_SPEC_GET_OR( IR1_NODE, gpios, 0);

static struct gpio_callback ir_cb_data;


/*Prototype of GPIO interrupt handler*/

void ir_recieved(const struct device *, struct gpio_callback *, uint32_t );

void main(void)
{	
	int ret;
	if (!gpio_is_ready_dt(&ir)) {
		
		printk("Gpio is not ready..\n");
		
		return;
	}

	/*Configure the interrupt*/

	ret = gpio_pin_interrupt_configure_dt(&ir, GPIO_INT_EDGE_BOTH);
	if (ret != 0) {

		printk("Couldn't Configure GPIO as Interrupt..\n");

		return;
	}
	/*Call back initilize and add*/

	gpio_init_callback(&ir_cb_data, ir_recieved, BIT(ir.pin));
	gpio_add_callback(ir.port, &ir_cb_data);

}

void ir_recieved(const struct device *dev, struct gpio_callback *cb,
		uint32_t pins)
{	
	uint32_t  timer_value, ret;

	static uint32_t timer, i, jvc_state = 0;

	/*Check the state and change the timer value*/

	if(jvc_state != 0) {

		timer_value = DURATION;
		timer = k_cycle_get_32();
	}

	switch(jvc_state) {

		case 0 :			/*beginning of 8.4ms pulse*/
			timer = k_cycle_get_32();  
			jvc_state = 1;
			i = 0;

			return ;

		case 1 :			
			if( (timer_value > 8900) || (timer_value < 7900) ) 

				jvc_state = 0;
			else 
				jvc_state = 2; /*beginning of 4.2ms space*/

			return;

		case 2 :		
			if( (timer_value > 4700) || (timer_value < 3700) ) 

				jvc_state = 0;
			else 
				jvc_state = 3;/*start to recieve */

			return;	

		case 3 :
			if( (timer_value  > 700) || (timer_value < 400) ) 

				jvc_state = 0;
			else 
				jvc_state = 4;

			return;

		case 4 :
			if( (timer_value > 1800) || (timer_value < 400)) {

				jvc_state = 0;

				return;

			}

			/*set and clear the bit with respect to the time value*/

			if(timer_value > 1000) 

				WRITE_BIT(jvc_code, (15 - i), 1);
			else 
				WRITE_BIT(jvc_code, (15 - i), 0);
			i++;

			if(i > 15) {

				printk("The JVC-Code recieved is - 0x%X\n",jvc_code);

				/*Disable the interrupt */

				ret = gpio_pin_interrupt_configure_dt(&ir, GPIO_INT_DISABLE);
				if( ret != 0){

					printk("Error: Cannot Disabel interrupt..\n");

					//return;
				}
			}
			jvc_state = 3;   /*goes to state 3 when the next arrival of space */

			return;
	}

}


