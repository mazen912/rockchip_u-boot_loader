#ifndef __GPIO_OPER_H__
#define __GPIO_OPER_H__

typedef enum{
    KEY_GPIO = 0,   // IO����
    KEY_AD,      // AD����
}KEY_TYPE;

typedef struct
{
    int     group;
    int     index;
    int     valid;

    // IO������ַ
	uint32	io_read;
	uint32  io_write;
	uint32	io_dir_conf;
	uint32	io_debounce;
}gpio_conf;

typedef struct
{
    uint32  index;
    uint32  keyValueLow;
    uint32  keyValueHigh;
	uint32	data;
	uint32  stas;
	uint32	ctrl;
}adc_conf;

typedef struct {
    KEY_TYPE type;
    union{
        gpio_conf   gpio;   // IO����
        adc_conf    adc;    // AD����
    }key;
}key_config;

void boot_gpio_set(void);
int GetPortState(key_config* gpio);
#endif

