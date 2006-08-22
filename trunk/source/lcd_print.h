#define LCD_DPORT 	PORTB
#define LCD_PIN 	PINB
#define LCD_DDR 	DDRB
#define LCD_SPORT 	PORTA
#define LCD_SDDR 	DDRA
#define LCD_RS	 	(2)
#define LCD_WR	 	(1)
#define LCD_E	 	(0)

#define LCD_BUSY_FLAG	 	(7)

#define LCD_SET_FUNC	 	(5)
#define LCD_8_BIT	 	(4)
#define LCD_2_LINE	 	(3)
#define LCD_5X10	 	(2)

#define LCD_FUNC_ON_OFF 	(3)
#define LCD_DISPLAY_ON	 	(2)
#define LCD_CURSOR_ON	 	(1)
#define LCD_BLINKING_ON	 	(0)

#define LCD_ENTRY_MODE	 	(2)
#define LCD_INCREASE	 	(1)
#define LCD_SHIFTED	 	(0)

#define LCD_CLEAR	 	(0)
#define LCD_INIT	 	(0x30)

void lcd_print(char *buffer, uint8_t buf_size);
void init_lcd(void);
void command_lcd(uint8_t);
void read_busy_lcd(void);
void put_char_lcd(int8_t);

