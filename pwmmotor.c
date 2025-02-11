#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/irq.h"
#include "hardware/clocks.h"
#include <stdio.h>

#define SERVO_GPIO 22
#define LED_GPIO 12
#define PWM_FREQUENCIA 50 // Frequência do PWM em Hz
#define PWM_PERIODO_US 20000 // Período de 20ms
#define INTERVALO_ATUALIZACAO_MS 10 // Intervalo entre mudanças de posição (ms)



// Configura o sinal PWM
void configurar_pwm(uint pino, uint16_t largura_pulso_us) {
    uint canal_pwm = pwm_gpio_to_slice_num(pino); // Obtém o slice PWM do GPIO
    uint valor_maximo = clock_get_hz(clk_sys) / (PWM_FREQUENCIA * 64); // Calcula o "wrap"
    pwm_set_wrap(canal_pwm, valor_maximo); // Define o limite do PWM
    pwm_set_chan_level(canal_pwm, PWM_CHAN_A, (valor_maximo * largura_pulso_us) / PWM_PERIODO_US); // Ajusta o ciclo de trabalho
    pwm_set_enabled(canal_pwm, true); // Ativa o PWM
}

// Manipulador de interrupção para controle do servo e LED
void manipulador_interrupcao_pwm() {
    static uint16_t largura_pulso_atual = 500;
    static bool aumentando = true;
    static bool inicializado = false;
    uint canal_servo = pwm_gpio_to_slice_num(SERVO_GPIO);
    uint canal_led = pwm_gpio_to_slice_num(LED_GPIO);
    
    pwm_clear_irq(canal_servo); // Limpa a interrupção do servo
    pwm_clear_irq(canal_led); // Limpa a interrupção do LED
    
    if (!inicializado) {
        largura_pulso_atual = 500; // Inicia oscilação em 0 graus
        inicializado = true;
    }
    
    configurar_pwm(SERVO_GPIO, largura_pulso_atual); // Atualiza PWM do servo
    configurar_pwm(LED_GPIO, largura_pulso_atual * largura_pulso_atual); // Ajusta PWM do LED
    
    // Alterna entre aumentar e diminuir o ciclo de trabalho
    if (aumentando) {
        largura_pulso_atual += 5;
        if (largura_pulso_atual >= 2400) aumentando = false;
    } else {
        largura_pulso_atual -= 5;
        if (largura_pulso_atual <= 500) aumentando = true;
    }
}

// Configura PWM com interrupção
void configurar_pwm_com_interrupcao(uint pino) {
    uint canal_pwm = pwm_gpio_to_slice_num(pino);
    gpio_set_function(pino, GPIO_FUNC_PWM); // Define função PWM para o pino
    pwm_clear_irq(canal_pwm); // Limpa interrupções pendentes
    pwm_set_irq_enabled(canal_pwm, true); // Habilita interrupção PWM
    irq_set_exclusive_handler(PWM_IRQ_WRAP, manipulador_interrupcao_pwm);
    irq_set_enabled(PWM_IRQ_WRAP, true); // Ativa interrupção global
    
    pwm_config config_pwm = pwm_get_default_config();
    pwm_config_set_clkdiv(&config_pwm, 64.0);
    pwm_init(canal_pwm, &config_pwm, true);
}

int main() {
    stdio_init_all();
    
    gpio_set_function(SERVO_GPIO, GPIO_FUNC_PWM);
    gpio_set_function(LED_GPIO, GPIO_FUNC_PWM);
    uint canal_servo = pwm_gpio_to_slice_num(SERVO_GPIO);
    uint canal_led = pwm_gpio_to_slice_num(LED_GPIO);
    pwm_set_clkdiv(canal_servo, 64.0);
    pwm_set_clkdiv(canal_led, 64.0);
    
    // Define posições iniciais do servo e LED
    configurar_pwm(SERVO_GPIO, 2400);
    configurar_pwm(LED_GPIO, 2400);
    sleep_ms(5000);
    
    configurar_pwm(SERVO_GPIO, 1470);
    configurar_pwm(LED_GPIO, 1470);
    sleep_ms(5000);
    
    configurar_pwm(SERVO_GPIO, 500);
    configurar_pwm(LED_GPIO, 500);
    sleep_ms(5000);
    
    // Configura e ativa interrupções do PWM
    configurar_pwm_com_interrupcao(SERVO_GPIO);
    configurar_pwm_com_interrupcao(LED_GPIO);
    
    while (true) {
        tight_loop_contents();
    }
}