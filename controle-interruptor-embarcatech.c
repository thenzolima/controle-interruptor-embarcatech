#include <stdio.h>
#include "pico/stdlib.h"
#include "ws2812.pio.h"
#include "hardware/pio.h"
#include "hardware/timer.h"
#include "algarismos.h"

#define IS_RGBW false
#define WS2812_PIN 7

#define RED_LED_PIN 13
#define BUTTON_A_PIN 5
#define BUTTON_B_PIN 6

static int count = 0;

bool led_buffer[NUM_PIXELS];
bool button_pressed = false;

void gpio_irq_handler(uint gpio, uint32_t events);  
void copy_array(bool *dest, const bool *src);  

static inline void put_pixel(uint32_t pixel_grb);  
static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b);  
void set_one_led(uint8_t r, uint8_t g, uint8_t b);  

int main()
{
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);

    stdio_init_all();

    gpio_init(RED_LED_PIN);
    gpio_set_dir(RED_LED_PIN, GPIO_OUT);

    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);

    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_B_PIN);

    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);

    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(BUTTON_B_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    copy_array(led_buffer, zero);  
    set_one_led(6, 214, 160);  

    while (true)
    {
        if (button_pressed)
        {
            set_one_led(6, 214, 160);  
        }
        else
        {
            button_pressed = false;
        }

        for (int i = 0; i < 5; i++)
        {
            gpio_put(RED_LED_PIN, true);  
            sleep_ms(50);
            gpio_put(RED_LED_PIN, false);  
            sleep_ms(150);
        }
    }
}

// Interrupção dos botões A e B para alterar o número exibido
void gpio_irq_handler(uint gpio, uint32_t events)
{
    volatile static uint32_t last_time = 0;
    volatile uint32_t current_time = to_ms_since_boot(get_absolute_time());

    if (current_time - last_time < 400)
    {
        return;  
    }

    last_time = current_time;

    if (gpio == BUTTON_A_PIN)
    {
        count++;  
        if (count > 9)
            count = 0;  
    }
    else if (gpio == BUTTON_B_PIN)
    {
        count--;  
        if (count < 0)
            count = 9;  
    }

    switch (count)
    {
    case 0:
        copy_array(led_buffer, zero);
        break;
    case 1:
        copy_array(led_buffer, one);
        break;
    case 2:
        copy_array(led_buffer, two);
        break;
    case 3:
        copy_array(led_buffer, three);
        break;
    case 4:
        copy_array(led_buffer, four);
        break;
    case 5:
        copy_array(led_buffer, five);
        break;
    case 6:
        copy_array(led_buffer, six);
        break;
    case 7:
        copy_array(led_buffer, seven);
        break;
    case 8:
        copy_array(led_buffer, eight);
        break;
    case 9:
        copy_array(led_buffer, nine);
        break;
    }

    button_pressed = true;  
}

// Copia o padrão de LEDs para o buffer
void copy_array(bool *dest, const bool *src)
{
    for (int i = 0; i < NUM_PIXELS; i++)
    {
        dest[i] = src[i];  
    }
}

// Envia o valor de cor de um LED para o WS2812
static inline void put_pixel(uint32_t pixel_grb)
{
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);  
}

// Converte valores RGB para formato GRB de 32 bits
static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b)
{
    return ((uint32_t)(r) << 8) | ((uint32_t)(g) << 16) | (uint32_t)(b);  
}

// Define a cor de um LED com base no buffer de LEDs
void set_one_led(uint8_t r, uint8_t g, uint8_t b)
{
    uint32_t color = urgb_u32(r, g, b);  

    for (int i = 0; i < NUM_PIXELS; i++)
    {
        if (led_buffer[i])
        {
            put_pixel(color);  
        }
        else
        {
            put_pixel(0);  
        }
    }
}
