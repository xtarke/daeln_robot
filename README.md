# Repositótio de código do robô InMoov - Instituto Federal de Santa Catarina -- Florianópolis

Descrição do protocolo de comunicação:

O protocolo de comunicação é baseado no protocolo que o X_Bee usa.  O pacote de comunicação é composto de um byteinicializador, um byte de tamanho, um byte identificador de qual comando está sendo dado, os bytes de dados do comando e por fim um byte de checksum. Sendo que o checksum é sempre 0xFF menos os bytes do frame, descartando inicializador e tamanho. A resposta do pacote é sempre um eco do pacote enviado porém com o identificador somado ao valor 0x7E.

Pacote de comunicação é formado por:

| PKG_START | PKG_SIZE | < PAYLOAD > | CHECKSUM  |
| :-------: | :-------: | :-------: | :-------: |
|  0x7e     | < size > |             | 0xff-data | 

Assume-se que PKG_START e PKG_SIZE não fazem parte dos dados:

| PKG_CMD | < PWM_DATA > or < ADC_DATA> |
| -------- | ---------------------------|

Pacotes típicos:

Servo motores:

| PKG_START | PKG_SIZE | < PKG_CMD > |  < SRV_ID > | < SRV_POS > | CHECKSUM  |
| :-------: | :------: | :---------: | :---------: | :---------: | :-------: |  
|  0x7e     |   0x03   |   0x01      |  0 -> 6     |  0% a 100%  | 0xff-data | 
| :-------: | :------: | :---------: | :---------: | :---------: | :-------: |  
| Resposta  |
|  0x7e     |   0x03   |   0x08      |  0 -> 6     |  0% a 100%  | 0xff-data | 

Leitura de corrente para apenas um servo:

| PKG_START | PKG_SIZE | < PKG_CMD > |  < SRV_ID > | CHECKSUM  |
| :-------: | :------: | :---------: | :---------: | :-------: |  
|  0x7e     |   0x02   |   0x11      |  0 -> 6     | 0xff-data | 

Resposta:

| PKG_START | PKG_SIZE | < PKG_CMD > |  < SRV_ID > | I_ma(h)  |  I_ma(l) | CHECKSUM  |
| :-------: | :------: | :---------: | :---------: | :------: | :------: | :------:  |   
|  0x7e     |   0x04   |   0x90      |  0 -> 6     |  0-0xff  |  0-0xff  | 0xff-data | 

Leitura de corrente para todos os servos:

| PKG_START | PKG_SIZE | < PKG_CMD > |  CHECKSUM  |
| :-------: | :------: | :---------: |  :-------: |  
|  0x7e     |   0x01   |   0x13      |  0xff-data | 


Resposta:

| PKG_START | PKG_SIZE | < PKG_CMD > | I_ma(h) 0 |  I_ma(l) 0 | I_ma(h) n |  I_ma(l) n | CHECKSUM  |
| :-------: | :------: | :---------: | :-------: | :------: | :---------: | :-------:  | :-------: |     
|  0x7e     |   0x0f   |   0x90      |   0-0xff  |  0-0xff  |  0-0xff     |  0-0xff    | 0xff-data | 
