
//Atmega328p


#define F_CPU 16000000UL // 16 MHz


#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#define I2C_ADDR 0x27 // Адрес PCF8574, если A0, A1 и A2 не подключены, то адрес 0x27, если подключены к Vcc, то 0x4E
#define BL 3 // Предполагаем, что подсветка управляется четвертым битом


// Инициализация I2C
void i2c_init(void) {
	TWSR = 0x00; // Установка предделителя
	TWBR = ((F_CPU / 100000UL) - 16) / 2; // Установка скорости передачи 100kHz
}

// Отправка условия START
void i2c_start(void) {
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
}

// Отправка условия STOP
void i2c_stop(void) {
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

// Отправка байта данных
void i2c_write(uint8_t data) {
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
}

// Функция для отправки команды на LCD через PCF8574
void lcd_send_cmd(uint8_t cmd) {
	uint8_t data_u, data_l;
	uint8_t data = cmd;
	data_u = (data & 0xf0) | 0x04 | (1 << BL); // en=1, rs=0, bl=1
	data_l = ((data << 4) & 0xf0) | 0x04 | (1 << BL); // en=1, rs=0, bl=1

	i2c_start();
	i2c_write(I2C_ADDR << 1);
	i2c_write(data_u);
	i2c_write(data_u & 0xFB); // en=0
	i2c_write(data_l);
	i2c_write(data_l & 0xFB); // en=0
	i2c_stop();
	_delay_us(50);
}

// Функция для отправки данных (символов) на LCD через PCF8574
void lcd_send_data(uint8_t data) {
	uint8_t data_u, data_l;
	uint8_t data_t = data;
	data_u = (data_t & 0xf0) | 0x05 | (1 << BL); // en=1, rs=1, bl=1
	data_l = ((data_t << 4) & 0xf0) | 0x05 | (1 << BL); // en=1, rs=1, bl=1

	i2c_start();
	i2c_write(I2C_ADDR << 1);
	i2c_write(data_u);
	i2c_write(data_u & 0xFB); // en=0
	i2c_write(data_l);
	i2c_write(data_l & 0xFB); // en=0
	i2c_stop();
	_delay_us(50);
}


// Функция для очистки экрана LCD
void lcd_clear(void) {
	lcd_send_cmd(0x01); // Команда очистки дисплея
	_delay_ms(2);
}



// Функция для установки курсора на LCD
void lcd_set_cursor(uint8_t row, uint8_t col) {
	uint8_t offset[] = {0x00, 0x40}; // Смещения для начала каждой строки
	lcd_send_cmd(0x80 | (offset[row] + col)); // Команда установки курсора
}

// Функция для вывода строки на LCD
void lcd_print(const char *str) {
	while (*str) {
		lcd_send_data(*str++);
	}
}


// Функция для инициализации LCD
void lcd_init(void) {
	// Инициализация дисплея в 4-битном режиме
	lcd_send_cmd(0x02);
	lcd_send_cmd(0x28); // 4-битный режим, 2 строки, 5x8 точек
	lcd_send_cmd(0x0c); // Включение дисплея, курсор выключен, мигание выключено
	lcd_send_cmd(0x01); // Очистка дисплея
	_delay_ms(2);
	lcd_send_cmd(0x06); // Инкремент курсора
	
}



int main(void) {
	i2c_init(); // Инициализация I2C
	lcd_init(); // Инициализация LCD
	
	
	uint8_t mmm = 114; // Пример числа для первой строки
	float fff=134.782;
	char znak;
	

	char buffer[16]; // Буфер для форматирования строки

	while (1) {
		mmm++;
		
		
		
		lcd_clear(); // Очистка экрана перед выводом новой информации
		
		lcd_set_cursor(0, 1); // Установка курсора в начало первой строки со второго символа
		snprintf(buffer, sizeof(buffer), "ADF=%u", mmm); // Форматирование строки с числом mmm
		lcd_print(buffer); // Вывод строки на LCD



		//float fff=-134.782;
		//char znak;

		// вывод float +/- с плавающей точкой
		uint16_t hh = (uint16_t)fff;// до запятой
		uint16_t ll = (uint16_t)((fff - hh) * 1000);//1000 сколько знаков после запятой
		if (fff>=0)
		{znak='+'; hh=hh; ll=ll;} // если float>=0, то "+"
		else
		{znak='-'; hh=-hh; ll=-ll;}// если float<0, то "-" и hh и ll переворачиваем
				
				
		lcd_set_cursor(1, 0); // Установка курсора в начало второй строки
		snprintf(buffer, sizeof(buffer), "Floa %c%u.%u",znak, hh, ll); // Форматирование строки с числом fff
		lcd_print(buffer); // Вывод строки на LCD
		
		
		
		
		if (fff>=0)
		{fff=-134.782;} 
		else
		{fff=134.782;}
		
		
		
		
		_delay_ms(500);
	}
}


