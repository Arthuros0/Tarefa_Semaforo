#include "matrix_leds.h"


#define NUM_PIXELS 25

#define OUT_PIN 7


PIO pio=pio0;
uint sm=0;

uint8_t obter_index(uint8_t i) {
    uint8_t x = i % 5;  // Coluna
    uint8_t y = i / 5;  // Linha
    return (y % 2 == 0) ? 24-(y * 5 + x) : 24-(y * 5 + (4 - x));
}

void desenha_frame(const uint32_t matriz[][25],uint8_t frame){

    for (uint8_t i = 0; i < NUM_PIXELS; i++)
    {
        uint8_t pos=obter_index(i);         
        pio_sm_put_blocking(pio0,sm,matriz[frame][pos]);
    }
    
}

void apaga_matriz(){
    for (uint8_t i = 0; i < NUM_PIXELS; i++)
    {
        uint8_t pos=obter_index(i);         
        pio_sm_put_blocking(pio0,sm,0x00000000);
    }
}

void setup_led_matrix() {
    bool ok;
    // Configura o clock para 133 MHz
    ok = set_sys_clock_khz(133000, false);
    stdio_init_all();

    printf("Iniciando a transmissão PIO\n");
    if (ok) printf("Clock configurado para %ld Hz\n", clock_get_hz(clk_sys));

    // Configuração do PIO
    uint offset = pio_add_program(pio, &pio_matrix_program);
    uint sm = pio_claim_unused_sm(pio, true);
    pio_matrix_program_init(pio, sm, offset, OUT_PIN);
}


const uint32_t estados[4][25] = {
    {   //VERDE
        0x19000000, 0x19000000, 0x19000000, 0x19000000, 0x19000000, 
        0x19000000, 0x19000000, 0x19000000, 0x19000000, 0x19000000, 
        0x19000000, 0x19000000, 0x19000000, 0x19000000, 0x19000000, 
        0x19000000, 0x19000000, 0x19000000, 0x19000000, 0x19000000, 
        0x19000000, 0x19000000, 0x19000000, 0x19000000, 0x19000000
    },
    {   //AMARELO
        0x19190000, 0x19190000, 0x19190000, 0x19190000, 0x19190000, 
        0x19190000, 0x19190000, 0x19190000, 0x19190000, 0x19190000, 
        0x19190000, 0x19190000, 0x19190000, 0x19190000, 0x19190000, 
        0x19190000, 0x19190000, 0x19190000, 0x19190000, 0x19190000, 
        0x19190000, 0x19190000, 0x19190000, 0x19190000, 0x19190000
    },
    {   //VERMELHO
        0x00190000, 0x00190000, 0x00190000, 0x00190000, 0x00190000, 
        0x00190000, 0x00190000, 0x00190000, 0x00190000, 0x00190000, 
        0x00190000, 0x00190000, 0x00190000, 0x00190000, 0x00190000, 
        0x00190000, 0x00190000, 0x00190000, 0x00190000, 0x00190000, 
        0x00190000, 0x00190000, 0x00190000, 0x00190000, 0x00190000
    },
    {   //DESLIGADO
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
    },
};