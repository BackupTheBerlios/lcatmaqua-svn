#ifndef F_CPU
#define F_CPU 		9830400
#endif

#define DSI_PORT 	PORTD
#define DSI_DDR 	DDRD
#define DSI_PIN 	7
#define T_VALUE		(256 - 32)			// 1/1200 / (1/(9.8304MHz / 256))=32
/* dsi seting time = 5ms */

void init_dsi(void);
void dsi_send(void);

