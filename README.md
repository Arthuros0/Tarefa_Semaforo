# Semáforo Acessível Inteligente com Modo Noturno

## Objetivo do Projeto

Desenvolver um sistema embarcado utilizando a placa BitDogLab (RP2040) capaz de:

- Simular o funcionamento de um semáforo urbano com acessibilidade.  
- Operar em dois modos distintos: **normal** e **noturno**.  
- Fornecer sinalização **visual** (LED RGB, matriz de LEDs e Display Oled) e **sonora** (buzzer).  
- Permitir a troca de modos via botão físico (Botão A).  
- Implementar toda a lógica utilizando **apenas tarefas do FreeRTOS**.  

## Funcionamento

O sistema possui dois modos principais:

### Modo Normal
- **Verde**: Indica “pode atravessar”.  
  - LED RGB e matriz em verde.  
  - Buzzer emite 1 beep curto por segundo.
- **Amarelo**: Indica “atenção”.  
  - LED RGB e matriz em amarelo.  
  - Buzzer emite beeps intermitentes curtos e rápidos.
- **Vermelho**: Indica “pare”.  
  - LED RGB e matriz em vermelho.  
  - Buzzer emite um som contínuo (500ms ligado e 1.5s desligado).

### Modo Noturno
- Apenas o **amarelo pisca lentamente** a cada 2 segundos.  
- O buzzer acompanha o piscar, emitindo beeps sincronizados.  
- Simula o comportamento de um semáforo noturno de baixa circulação.

A mudança de modo ocorre ao pressionar o **botão A**, que altera uma flag monitorada pelas tarefas.

## Hardware Utilizado

- Placa **BitDogLab (RP2040)**  
- **LED RGB** embutido  
- **Matriz de LEDs RGB 5x5**
- **Display SSD1306**  
- **Buzzer ativo**  
- **Botão A** (interruptor físico)  

## Bibliotecas Utilizadas

- [pico-sdk](https://github.com/raspberrypi/pico-sdk)  
- FreeRTOS portado para RP2040  
- Biblioteca de controle da matriz de LEDs da BitDogLab
- Biblioteca de controle do Display SSD1306
- Biblioteca PWM/GPIO para controle do buzzer  

## Organização do Código

- `void vTrafficLightTask()`: Controla a mudança de cores do semáforo.  
- `void vBuzzerTask()`: Emite os sons correspondentes a cada estado do semáforo.  
- `void vModeTask()`: Monitora o botão A e alterna entre os modos.
- `void vMatrixTask()`: Inicializa e controla a mudança de cores da matriz de LEDs.
- `void vDisplayTask()`: Inicializa e controla o Display SSD1306.
- `main()`: Cria as tarefas do FreeRTOS.  

## Arquivos no Repositório

- `main.c`: Código principal com a lógica do semáforo.  
- `CMakeLists.txt`: Arquivo de build do projeto.  
- `lib/matrix_leds.c` e `matrix_leds.h`: Controle da matriz de LEDs RGB.
- `lib/ssd1306.c` e `ssd1306.h`: Controle do display oled.  
- `lib/buzzer.c` e `lib/buzzer.h`: Controle do buzzer por PWM.  

## Observações

- A lógica do sistema foi desenvolvida **exclusivamente com tarefas do FreeRTOS**. 
