#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

/*This Sample is to set a Given Hex to the RC-6 Transmitter with Toggle***/
#define 	LOW     	333
#define 	HIGH		444 					
#define 	IR1_NODE 	DT_ALIAS(ir1)				/**NODE by Alias name**/

/*Code to Transmit*/

uint32_t rc6_code = 0xA37A;


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
/** Sends the given RC6-code **/

void IRsend(uint32_t code ){



	static int a = 0;
	auto int i = 0;

	IRcarrier(2666);  		/*Start bit*/
	k_usleep(800);



	while( i<= 2){			/*Field bits*/

		if ( i == 0 ) {
			IRcarrier(HIGH);
			k_usleep(LOW);

		}
		k_usleep(LOW);
		IRcarrier(HIGH);

		i ++;		
	}

	if( a == 0){
		k_usleep(800);		/*Toggle bit*/
		IRcarrier(889);
		a +=1;
	}
	else {


		IRcarrier(889);
		k_usleep(800);
		a -= 1;
	}


	for (int i = 0; i <16; i++) {   /* Low High for True and High Low for False*/

		if (code & 0x8000) {

			IRcarrier(HIGH);
			k_usleep(LOW);
		}
		else {

			k_usleep(LOW);
			IRcarrier(HIGH);		
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

	IRsend(rc6_code);	//Toggle bit 0 
	k_msleep(2000); 
	IRsend(rc6_code);	 //Toggle bit 1
}
