Controle de Servo e LED com PWM no Raspberry Pi Pico
Este projeto utiliza o Raspberry Pi Pico para controlar um servo motor e um LED via PWM (Pulse Width Modulation).

OBJETIVO:

Controlar a posição de um servo motor variando a largura do pulso PWM.
Ajustar o brilho de um LED com base no ciclo de trabalho do PWM.
Usar interrupções para atualização automática do sinal PWM.

COMPOSIÇÃO:

Servo conectado ao GPIO 22.
LED conectado ao GPIO 12.
Frequência do PWM definida em 50 Hz.


-Compile e carregue o código no Raspberry Pi Pico.
-O servo alternará automaticamente entre posições mínimas e máximas.
-O LED ajustará seu brilho com base no controle PWM do servo.
