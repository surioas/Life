#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

/*This Sample is to set a Given Hex to the IR Transmitter*(JVC)**/

#define 	BITtime		526 					
#define 	IR1_NODE 	DT_ALIAS(ir1)				/**NODE by Alias name**/

/*Code to Transmit MSB-first*/

uint32_t IRcode = 0x523;


static const struct gpio_dt_spec ir1 = GPIO_DT_SPEC_GET(IR1_NODE, gpios);

/*Generates a Carrier Signal, for the given time in microseconds*/

static void IRcarrier( uint32_t TimeForCarrier) {

	int ret;
	for (int i = 0; i <(TimeForCarrier / 26); i++) {

		ret  = gpio_pin_set_dt(&ir1, 1);
		if (ret > 0) {
			return;
		}

		k_busy_wait(12);	
		
		ret = gpio_pin_set_dt(&ir1, 0);
		if (ret > 0) {
			return;	
		}

		k_busy_wait(12);
	}

}
/** Sends the given IR code **/

void IRsend(uint32_t code ){

	IRcarrier(8400);
	k_usleep(4200);

	for (int i = 0; i <16; i++) {

		IRcarrier(BITtime);
		if (code & 0x8000)
			k_usleep(3 * BITtime);
		else
			k_usleep(BITtime);
		code <<= 1;


	}
	IRcarrier(BITtime);	
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

	IRsend(IRcode);
	 
}
