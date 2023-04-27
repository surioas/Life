#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

/*This Sample is to set a Given Hex to the RC-5 Transmitter***/
#define 	LOW     	800
#define 	HIGH		889 					
#define 	IR1_NODE 	DT_ALIAS(ir1)				/**NODE by Alias name**/

/*Code to Transmit*/

uint32_t rc5_code = 0x1667;


static const struct gpio_dt_spec ir1 = GPIO_DT_SPEC_GET(IR1_NODE, gpios);

/*Generates a Carrier Signal, for the given time in microseconds*/

static void IRcarrier( uint32_t TimeForCarrier) {

	int ret;
	for (int i = 0; i <(TimeForCarrier / 27); i++) {

		ret  = gpio_pin_set_dt(&ir1, 1);
		if (ret > 0) {
			return;
		}

		k_busy_wait(13);	
		
		ret = gpio_pin_set_dt(&ir1, 0);
		if (ret > 0) {
			return;	
		}

		k_busy_wait(13);
	}

}
/** Sends the given RC5-code **/

void IRsend(uint32_t code ){

	
	int i = 0;
	while (i <= 2) {
		if (i < 2)
			k_usleep(LOW);	/* initial High and Toggle bit */
	
		IRcarrier(HIGH);
	
		i++;
	}
	k_usleep(LOW);
	
	for (int i = 0; i <11; i++) {   /* Low High for True and High Low for False*/

		if (code & 0x400) {
			
			k_usleep(LOW);
			IRcarrier(HIGH);		
		}
		else {
			
			IRcarrier(HIGH);
			k_usleep(LOW);
		}
		code <<= 1;


	}
}




void main(void)
{
	int ret;

	if (!gpio_is_ready_dt(&ir1)) {
		return;
	}

	ret = gpio_pin_configure_dt(&ir1, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return;
	}

	ret = gpio_pin_set_dt(&ir1, 0);
	if (ret > 0) {
		return;
	}

	IRsend(rc5_code);
	 
}
