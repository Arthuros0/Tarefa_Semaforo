#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "lib/ssd1306.h"
#include "lib/font.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include <stdio.h>
#include "lib/matrix_leds.h"
#include "lib/buzzer.h"

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C

#define VERDE 11
#define VERMELHO 13

#define BUTTOM_A 5

volatile bool modo_noturno=false;       //Usada para alternar entre os modos
volatile uint8_t state_traffic_light=0; //Variável usada para salvar estado do semaforo
uint slice_buzzer;

//Função de inicialização do botão
void init_buttom(uint8_t pin){
    gpio_init(pin);
    gpio_set_dir(pin,GPIO_IN);
    gpio_pull_up(pin);
}

//Função de inicialização do LED
void led_init(uint8_t pin){
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);
}

//Função de desligamento dos LEDs
void turn_off_leds(){
    gpio_put(VERMELHO,0);
    gpio_put(VERDE,0);
}

//Tarefa de altenância entre os modos de operação
void vModeTask(){
    init_buttom(BUTTOM_A);

    absolute_time_t last_time=0;
    absolute_time_t current_time=0;

    while (1)
    {
        current_time=to_ms_since_boot(get_absolute_time());

        //Verifica se o botão foi pressionado e se passou o tempo do debounce
        if (!gpio_get(BUTTOM_A) && current_time-last_time >= 250)
        {
            modo_noturno=!modo_noturno;
            last_time=current_time;
        }
        vTaskDelay(pdMS_TO_TICKS(100)); //Suspende a tarefa por 100ms
    }
    
}

//Tarefa de gerenciamento do LED RGB
void vTrafficLightsTask(){

    led_init(VERDE);
    led_init(VERMELHO);

    while(1){
    
        if (modo_noturno) //Se o modo noturno estiver ativo alterna a luz amarela(LED verde e vermelho) de um em um segundo
        {
            state_traffic_light=3;

            gpio_put(VERMELHO,1);
            gpio_put(VERDE,1);

            vTaskDelay(pdMS_TO_TICKS(1000));

            turn_off_leds();

            vTaskDelay(pdMS_TO_TICKS(1000));

        }else{ //Caso contrário o semaforo funciona normalmente na ordem verde, amarelo e vermelho, alternando de 4 em 4 segundos

            state_traffic_light=0;
            gpio_put(VERDE,1);          //Liga led verde
            vTaskDelay(pdMS_TO_TICKS(4000));
            
            if(!modo_noturno){
                state_traffic_light=1;
                gpio_put(VERMELHO,1);   //Liga led vermelho que juntamente com o led verde emite a luz amarela
                vTaskDelay(pdMS_TO_TICKS(4000));
            }
            if(!modo_noturno){
                state_traffic_light=2;
                gpio_put(VERDE,0);      //Desliga led verde, mantendo apenas led vermelho
                vTaskDelay(pdMS_TO_TICKS(4000));
            }
            turn_off_leds();            //Desliga todos os leds
        }
    }
}

void vMatrixTask(){
    setup_led_matrix(); //Configura matriz de LEDs

    while (1)
    {
        if (modo_noturno)   //Se o modo noturno estiver ativado verifica se o LED esta emitindo a luz amarela e emite na matriz a luz amarela
        {                   //Caso contrário desliga matriz de LEDs
            if(gpio_get(VERMELHO) && gpio_get(VERDE)){
                desenha_frame(estados,1);
            }else{
                apaga_matriz();
            }
        }else{ //Emite a cor na matriz de LEDs de acordo com o estado do semaforo
            switch (state_traffic_light)
            {
            case 0:
                desenha_frame(estados,0);
                break;
            case 1:
                desenha_frame(estados,1);
                break;
            case 2:
                desenha_frame(estados,2);
                break;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100)); //Suspende tarefa por 100ms
        
    }
    
}


void vBuzzerTask(){
    slice_buzzer=buzzer_init(BUZZER_A_PIN); //Inicializar o buzzer e obtem o slice

    while (1)
    {
        if (modo_noturno) //Se o modo noturno estiver ativo, emite um beep lento a cada 2 segundos
        {
            buzzer_turn_on(BUZZER_A_PIN,slice_buzzer,700);
            vTaskDelay(pdMS_TO_TICKS(300));
            buzzer_turn_off(BUZZER_A_PIN);
            vTaskDelay(pdMS_TO_TICKS(1800));
        }else{
            switch (state_traffic_light) //No modo normal emite um beep diferente para cada cor
            {
                case 0: //Um beep curto por segundo para o sinal verde
                    buzzer_turn_on(BUZZER_A_PIN,slice_buzzer,1000);
                    vTaskDelay(pdMS_TO_TICKS(200));
                    buzzer_turn_off(BUZZER_A_PIN);
                    vTaskDelay(pdMS_TO_TICKS(800));
                        break;
                case 1: //Um beep rápido e intermitente (4 beeps por segundo)
                    buzzer_turn_on(BUZZER_A_PIN,slice_buzzer,2000);
                    vTaskDelay(pdMS_TO_TICKS(50));
                    buzzer_turn_off(BUZZER_A_PIN);
                    vTaskDelay(pdMS_TO_TICKS(200));
                        break;
                case 2: //Um beep de meio segundo a cada um segundo e meio
                    buzzer_turn_on(BUZZER_A_PIN,slice_buzzer,3000);
                    vTaskDelay(pdMS_TO_TICKS(500));
                    buzzer_turn_off(BUZZER_A_PIN);
                    vTaskDelay(pdMS_TO_TICKS(1500));
                        break;
            }
        }
        
    }
    

}

void vDisplayTask(){
    i2c_init(I2C_PORT, 400 * 1000);

    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);                    // Set the GPIO pin function to I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);                    // Set the GPIO pin function to I2C
    gpio_pull_up(I2C_SDA);                                        // Pull up the data line
    gpio_pull_up(I2C_SCL);                                        // Pull up the clock line
    ssd1306_t ssd;                                                // Inicializa a estrutura do display
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
    ssd1306_config(&ssd);                                         // Configura o display
    ssd1306_send_data(&ssd);                                      // Envia os dados para o display
    // Limpa o display. O display inicia com todos os pixels apagados.
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    bool cor = true;
    while (1)
    {

        ssd1306_fill(&ssd, !cor);                          // Limpa o display
        ssd1306_rect(&ssd, 3, 3, 122, 60, cor, !cor);      // Desenha um retângulo
        ssd1306_line(&ssd, 3, 25, 123, 25, cor);           // Desenha uma linha

        if(!modo_noturno){
            ssd1306_draw_string(&ssd, "Sinal:", 8, 6); // Desenha uma string
            ssd1306_draw_string(&ssd, (state_traffic_light==0)?"Verde":(state_traffic_light==1)?"Amarelo":"Vermelho", 20, 16);
            ssd1306_draw_string(&ssd, (state_traffic_light == 0)?"Atravesse":(state_traffic_light==1)?"Atencao":"Pare", 18, 38);
        }else{
            ssd1306_draw_string(&ssd, "Modo Noturno", 10, 6); 
            ssd1306_draw_string(&ssd, "Sinal: Amarelo", 8, 16);
            ssd1306_draw_string(&ssd, "Tenha atencao", 8, 38);
        }
        ssd1306_send_data(&ssd);    // Atualiza o display
        vTaskDelay(pdMS_TO_TICKS(50));
    }

}

int main()
{
    stdio_init_all();

    xTaskCreate(vModeTask,"Mode Task",configMINIMAL_STACK_SIZE,NULL,tskIDLE_PRIORITY+1,NULL);

    xTaskCreate(vTrafficLightsTask, "Traffic Task",configMINIMAL_STACK_SIZE,NULL,tskIDLE_PRIORITY,NULL);

    xTaskCreate(vMatrixTask, "Matrix Task",configMINIMAL_STACK_SIZE,NULL,tskIDLE_PRIORITY,NULL);

    xTaskCreate(vBuzzerTask, "Buzzer Task",configMINIMAL_STACK_SIZE,NULL,tskIDLE_PRIORITY,NULL);

    xTaskCreate(vDisplayTask, "Display Task",configMINIMAL_STACK_SIZE,NULL,tskIDLE_PRIORITY,NULL);

    vTaskStartScheduler(); //Inicia o agendador do FreeRTOS
    panic_unsupported();
}
