# GigantesdeMDF_2025
Repositório dedicado para o projeto Gigantes de MDF 2/2025 do curso de Engenharia da Computação da UNISANTA, proposto e avaliado pelo professor Sérgio Schina de Andrade.

Este repositório é de propriedade do grupo TanqueLaser composto pelos alunos Cayo Vinycius Oliveira de Almeida, Felipe Silone de Campos, Hélio Cordeiro de Andrade Junior e Leonardo Henrique Rulim Nogueira.

A proposta deste projeto é desenvolver um carrinho de combate controlado por um ATmega328p para a disciplina Programação de Hardware.

Os materiais utilizados para o desenvolvimento do projeto foram:

- 1 ATmega328p;
- 2 motores DC 3V-6V com caixa de redução de torque;
- 3 LEDs laranjas;
- 1 laser vermelho 5V;
- 1 LDR de 20mm;
- 1 regulador de tensão L7805CV;
- 1 capacitor poliéster de 330nF (0.33uF);
- 2 capacitores poliésters de 100nF (0.1uF);
- 2 capacitores eletrolíticos de 10uF;
- 2 capacitores de cerâmica 22pF;
- 1 cristal oscilado de 16MHz;
- 1 CI L293D;
- 1 push button;
- 2 resistências de 10K ohms;
- 3 resistências de 150 ohms;
- 1 módulo bluetooh HC-05;
- 1 resistência de 1k ohms;
- 1 resistência de 2k ohms;
- 9 bornes de 2 vias 5.08mm kf301-3t;
- 2 esteiras de borracha;
- 2 placas de MDF 80x80cm;

Na execução deste projeto, foi decidido fazer um chassi de mini tanque. Seu canhão foi o laser e seu alvo (para que os adversários pudessem antigí-lo) foi LDR. O laser e o LDR ficaram posicionados à 10cm de altura e ambos um do lado do outro.

Cada vez que o LDR detectar um laser adversário, um LED deve ser apagado, o mini tanque deve realizar um giro de 180º e permanecer inativo por 5 segundos.

Para a movimentação, foi decidido usar o CI L293D, pois ele permite trocar o sentido de rotação dos motores sem muitas complicações.
