# nRF52840 SuperMini / ProMicro clone --- história da investigação

## 1. Objetivo

O objetivo era pôr a funcionar uma placa chinesa barata baseada no
**nRF52840**, no formato **ProMicro / SuperMini / nice!nano**, para
posteriormente a usar com um módulo LoRa **DX-LR30 / SX1262** e, em
particular, com o **MeshCore**.

O problema acabou por revelar várias camadas independentes que à
primeira vista pareciam ser uma só:

-   identificação real da placa;
-   bootloader instalado;
-   versão do SoftDevice existente na flash;
-   definição da placa no core Arduino;
-   mapa de memória usado pelo linker;
-   método de upload;
-   geração de UF2;
-   definição dos GPIOs;
-   LEDs da placa e circuito de carga.

Este documento regista o caminho seguido, incluindo os becos sem saída
e, sobretudo, aquilo que ficou efetivamente confirmado.

------------------------------------------------------------------------

## 2. A placa física

A placa é uma clone chinesa no formato **ProMicro / SuperMini
nRF52840**, fisicamente compatível em grande parte com o ecossistema
nice!nano.

Não é uma nice!nano genuína, embora o bootloader instalado de fábrica se
identifique dessa forma.

O mapa de GPIO usado pelo `variant.cpp` do pacote Arduino instalado é:

  Arduino             nRF52840
  ------------------- ----------
  D0                  P0.06
  D1                  P0.08
  D2                  P0.17
  D3                  P0.20
  D4                  P0.22
  D5                  P0.24
  D6                  P1.00
  D7                  P0.11
  D8                  P1.04
  D9                  P1.06
  D10                 P0.09
  D11                 P0.10
  D12                 P1.11
  D13                 P1.13
  D14                 P1.15
  D15                 P0.02
  D16                 P0.29
  D17                 P0.31
  D18                 P1.01
  D19                 P1.02
  D20                 P1.07
  D21 / EXT_VCC       P0.13
  D22 / LED_BUILTIN   P0.15

O `P0.13` está associado a `EXT_VCC` e, pelo esquema da placa, controla
o **enable de um LDO**.

O `P0.15` corresponde ao LED controlável pelo microcontrolador.

------------------------------------------------------------------------

## 3. Estado original do bootloader

Ao ligar inicialmente a placa por USB, aparecia uma unidade UF2 chamada:

``` text
NICENANO
```

e uma porta série USB.

O `INFO_UF2.TXT` original indicava:

``` text
UF2 Bootloader 0.6.0
lib/nrfx (v2.0.0)
lib/tinyusb (0.10.1-41-gdf0cda2d)
lib/uf2 (remotes/origin/configupdate-9-gadbb8c7)

Model: nice!nano
Board-ID: nRF52840-nicenano
SoftDevice: S140 version 6.1.1
Date: Jun 19 2021
```

Isto estabeleceu duas coisas importantes:

1.  o bootloader identificava-se como **nice!nano**;
2.  existia na placa o **SoftDevice S140 6.1.1**.

A identificação `nice!nano` pertence ao bootloader e **não prova que o
hardware seja uma nice!nano genuína**.

A porta COM do bootloader apresentava:

``` text
VID_239A
PID_00B3
```

que corresponde à configuração USB do bootloader nice!nano.

------------------------------------------------------------------------

## 4. Atualização do bootloader

O bootloader foi atualizado para a versão oficial Adafruit 0.9.2 usando:

``` text
update-nice_nano_bootloader-0.9.2_nosd.uf2
```

Foi deliberadamente usada a variante `_nosd`, para atualizar o
bootloader **sem substituir o SoftDevice existente**.

Depois da atualização:

``` text
UF2 Bootloader 0.9.2
lib/nrfx (v2.0.0)
lib/tinyusb (0.12.0-145-g9775e7691)
lib/uf2 (remotes/origin/configupdate-9-gadbb8c7)

Model: nice!nano
Board-ID: nRF52840-nicenano
Date: Jul 19 2024
SoftDevice: S140 6.1.1
```

Portanto:

-   bootloader: **0.9.2**
-   SoftDevice: **S140 6.1.1**

A atualização do bootloader **não resolveu** o problema de upload série
via `adafruit-nrfutil`.

------------------------------------------------------------------------

## 5. O problema do upload Arduino

Foi instalado o pacote:

``` text
nRFMicro-like-Boards
```

versão:

``` text
1.0.2
```

em:

``` text
C:\Users\Luis\AppData\Local\Arduino15\packages\nRFMicro-like-Boards\hardware\nrf52\1.0.2\
```

Foi selecionada a placa:

``` text
SuperMini nRF52840
```

A compilação funcionava, mas o upload série falhava.

O processo tentava primeiro fazer o conhecido toque a 1200 baud:

``` text
Performing 1200-bps touch reset on serial port COM29
Waiting for upload port...
No upload port found, using COM29 as fallback
```

e depois executava `adafruit-nrfutil`.

O resultado era:

``` text
Opened serial port COM29
Starting DFU upgrade of type 4...
Sending DFU start packet
Sending DFU init packet

Failed to upgrade target.

ClearCommError failed
(PermissionError(13, 'The device does not recognize the command.', None, 22))
```

O mesmo comportamento continuou depois da atualização do bootloader para
0.9.2.

Conclusão prática: **o caminho de upload série/nrfutil não ficou
resolvido**, mas deixou de ser necessário quando se estabeleceu o método
UF2.

------------------------------------------------------------------------

## 6. A discrepância escondida no `boards.txt`

Aqui apareceu uma das partes mais importantes da investigação.

A definição `SuperMini nRF52840` do pacote Arduino continha:

``` ini
supermini.build.ldscript=nrf52840_s140_v7.ld

supermini.menu.softdevice.s140v6=S140 7.3.0
supermini.menu.softdevice.s140v6.build.sd_name=s140
supermini.menu.softdevice.s140v6.build.sd_version=7.3.0
supermini.menu.softdevice.s140v6.build.sd_fwid=0x0123
```

Apesar do nome estranho do menu (`s140v6`), a configuração efetiva
estava preparada para **S140 7.3.0**.

Mas a placa tinha efetivamente:

``` text
S140 6.1.1
```

Ou seja, escolher simplesmente **SuperMini nRF52840** no Arduino IDE não
descrevia corretamente o estado real desta placa.

Isto era particularmente importante porque a versão do SoftDevice
determina o **mapa de memória disponível para a aplicação**.

------------------------------------------------------------------------

## 7. Correção para S140 6.1.1

A definição da SuperMini foi alterada no ficheiro:

``` text
C:\Users\Luis\AppData\Local\Arduino15\packages\nRFMicro-like-Boards\hardware\nrf52\1.0.2\boards.txt
```

As alterações efetivas foram:

``` text
nrf52840_s140_v7.ld
        ↓
nrf52840_s140_v6.ld
```

``` text
S140 7.3.0
        ↓
S140 6.1.1
```

e:

``` text
0x0123
   ↓
0x00B6
```

Ficando:

``` ini
supermini.build.ldscript=nrf52840_s140_v6.ld

supermini.menu.softdevice.s140v6=S140 6.1.1
supermini.menu.softdevice.s140v6.build.sd_name=s140
supermini.menu.softdevice.s140v6.build.sd_version=6.1.1
supermini.menu.softdevice.s140v6.build.sd_fwid=0x00B6
```

O linker para S140 v6 coloca o início da aplicação em:

``` text
0x26000
```

Isto passou a alinhar a aplicação Arduino com o SoftDevice que realmente
estava instalado na placa.

Depois de editar `boards.txt`, foi necessário **fechar completamente e
reabrir o Arduino IDE**, porque a configuração da plataforma é carregada
no arranque.

------------------------------------------------------------------------

## 8. Confirmação pelo log de compilação

Depois das alterações, o log mostrou explicitamente:

``` text
-Tnrf52840_s140_v6.ld
```

e a geração do pacote DFU passou a usar:

``` text
--sd-req 0x00B6
```

Isto confirmou que o Arduino IDE estava efetivamente a usar as novas
definições.

------------------------------------------------------------------------

## 9. Conversão manual HEX → UF2

O pacote Arduino gerava originalmente:

``` text
.elf
.hex
.zip
```

mas não gerava automaticamente um `.uf2`.

Foi localizado o conversor incluído no próprio pacote:

``` text
C:\Users\Luis\AppData\Local\Arduino15\packages\nRFMicro-like-Boards\hardware\nrf52\1.0.2\tools\uf2conv\uf2conv.py
```

Para nRF52840, foi usado o Family ID:

``` text
0xADA52840
```

A conversão do Intel HEX não necessita de indicar manualmente o endereço
base, porque o próprio HEX contém os endereços.

O conversor indicou:

``` text
Converting to bin, output size: 54528, start address: 0x26000
```

O `uf2conv.py` antigo incluído no pacote apresentou também um problema
de Python/Windows na enumeração das drives:

``` text
TypeError: a bytes-like object is required, not 'str'
```

Esse erro não significava que o UF2 estivesse corrompido; era um
problema separado no código que enumera volumes no Windows.

------------------------------------------------------------------------

## 10. Validação independente do primeiro UF2

O UF2 produzido foi analisado bloco a bloco.

Resultado:

``` text
Blocos: 213
Início: 0x00026000
Fim:    0x00033500
Family IDs: ['0xADA52840']
```

Isto confirmou:

-   endereço inicial correto: `0x26000`;
-   Family ID correto para nRF52840;
-   ficheiro estruturalmente coerente;
-   aplicação situada na zona esperada da flash.

O UF2 foi então copiado para:

``` text
NICENANO
```

e arrancou corretamente.

------------------------------------------------------------------------

## 11. Primeira aplicação Arduino funcional

Depois do carregamento UF2, a aplicação apareceu por USB numa nova porta
COM e produziu:

``` text
--------------------------------------------------
ProMicro / SuperMini nRF52840 hardware information
--------------------------------------------------

Device ID: B142FA2B9A89950F
Part: 00052840
Variant: AAD0
Package: 00002004 (QIxx - 7x7 73-pin aQFN)
RAM: 256 KB (262144 bytes)
Flash: 1024 KB (1.0 MB)
BLE MAC address: 0F:0D:E8:50:1B:AC
Internal temperature: 33.50°C
Bootloader version: s140 6.1.1
```

Os campos de hardware lidos do FICR confirmaram um nRF52840 com:

``` text
Flash: 1 MB
RAM:   256 KB
```

Há uma ressalva quanto à linha:

``` text
Bootloader version: s140 6.1.1
```

O texto é semanticamente suspeito: `S140 6.1.1` é a identificação do
**SoftDevice**, não a versão do bootloader, que sabemos pelo
`INFO_UF2.TXT` ser **0.9.2**.

A implementação concreta de `getBootloaderVersion()` ainda não foi
auditada nesta investigação, pelo que não convém tirar mais conclusões
dessa linha sem consultar o código da função.

------------------------------------------------------------------------

## 12. Recuperação do bootloader

Depois de a aplicação Arduino estar instalada, foi testado o duplo
reset.

O volume:

``` text
NICENANO
```

voltou a aparecer.

Portanto ficaram confirmados simultaneamente:

-   aplicação Arduino funcional;
-   SoftDevice S140 6.1.1 preservado;
-   bootloader UF2 0.9.2 funcional;
-   duplo reset funcional;
-   possibilidade de voltar ao bootloader mesmo depois de gravar uma
    aplicação.

Isto tornou o UF2 um método de desenvolvimento perfeitamente utilizável,
mesmo sem resolver o `nrfutil` por série.

------------------------------------------------------------------------

## 13. Automatização da geração do UF2

Fazer a conversão manual a cada compilação seria incómodo.

No ficheiro:

``` text
C:\Users\Luis\AppData\Local\Arduino15\packages\nRFMicro-like-Boards\hardware\nrf52\1.0.2\platform.txt
```

já existia a receita:

``` ini
## Create uf2 file
#recipe.objcopy.uf2.pattern=python "{runtime.platform.path}/tools/uf2conv/uf2conv.py" -f {build.uf2_family} -c -o "{build.path}/{build.project_name}.uf2" "{build.path}/{build.project_name}.hex"
```

Estava simplesmente comentada.

Foi ativada removendo `#`:

``` ini
recipe.objcopy.uf2.pattern=python "{runtime.platform.path}/tools/uf2conv/uf2conv.py" -f {build.uf2_family} -c -o "{build.path}/{build.project_name}.uf2" "{build.path}/{build.project_name}.hex"
```

No `boards.txt` foi também necessário garantir:

``` ini
supermini.build.uf2_family=0xADA52840
```

Depois de reiniciar o Arduino IDE, a compilação passou a produzir
automaticamente:

``` text
sketch_sep7a.ino.hex
sketch_sep7a.ino.uf2
sketch_sep7a.ino.zip
```

O log confirmou:

``` text
python ".../uf2conv.py" -f 0xADA52840 -c ...
```

e:

``` text
Converting to uf2, output size: 109056, start address: 0x26000
Wrote 109056 bytes to ...\sketch_sep7a.ino.uf2
```

Portanto o fluxo atual é simplesmente:

``` text
Arduino IDE
    ↓
compilar
    ↓
HEX
    ↓
UF2 automático
    ↓
duplo reset
    ↓
NICENANO
    ↓
copiar UF2
```

------------------------------------------------------------------------

## 14. A investigação dos LEDs

Depois de a aplicação funcionar apareceu outro comportamento curioso:

-   um LED vermelho permanecia aceso;
-   um LED azul continuava a piscar.

Inicialmente parecia possível que o bootloader estivesse envolvido, mas
isso não fazia sentido enquanto a aplicação estava efetivamente em
execução.

Foi então consultado o `variant.cpp`.

A definição contém:

``` cpp
13, // 'D21' is P0.13 (EXT_VCC) [active high]
15, // 'D22' is P0.15 (LED)
```

e:

``` cpp
void initVariant()
{
    // enable EXT_VCC by default
    pinMode(EXT_VCC, OUTPUT);
    digitalWrite(EXT_VCC, HIGH);

    // enable LED by default
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
}
```

Portanto, antes sequer de entrar no `setup()`, o core:

1.  coloca `P0.13 / EXT_VCC` como saída HIGH;
2.  coloca `P0.15 / LED_BUILTIN` como saída HIGH.

------------------------------------------------------------------------

## 15. Um teste que inicialmente pareceu falhar --- mas não tinha corrido

Foi preparado um teste simples:

``` cpp
void loop() {
    digitalWrite(LED_BUILTIN, LOW);
    delay(3000);

    digitalWrite(LED_BUILTIN, HIGH);
    delay(3000);
}
```

Depois de compilar e carregar, aparentemente nenhum LED mudou.

A primeira interpretação seria que nenhum dos LEDs estava ligado a
`P0.15`.

Mas o sketch original tinha no `setup()`:

``` cpp
Serial.begin(115200);
while (!Serial);
```

Como a porta série não tinha sido aberta, o programa estava parado em:

``` cpp
while (!Serial);
```

e **nunca tinha chegado ao `loop()`**.

Este detalhe explicou completamente o falso resultado.

Depois de abrir a porta série, o teste passou efetivamente a executar.

Resultado:

**o LED vermelho é o LED controlado por P0.15 / LED_BUILTIN.**

------------------------------------------------------------------------

## 16. O misterioso LED azul

Pelo esquema consultado, o segundo LED está associado ao circuito de
carga, através do pino `CHRG` de um **TP4054**.

O circuito é, conceptualmente:

``` text
USB+
 |
 LED
 |
CHRG
 |
TP4054
```

O pino `CHRG` do TP4054 é uma saída open-drain de indicação de carga.

Isto explica por que esse LED:

-   não responde ao `LED_BUILTIN`;
-   não é controlado diretamente por um GPIO do nRF52840;
-   depende do estado do carregador e da presença de USB/bateria.

A placa estava ligada por USB **sem bateria instalada**, precisamente a
situação que tornou o piscar do LED de carga inicialmente estranho.

O comportamento do carregador sem bateria pode produzir indicação
intermitente no pino de estado, explicando o piscar observado.

Assim, a interpretação física atual é:

``` text
LED vermelho
    ↕
P0.15
    ↕
nRF52840
```

enquanto:

``` text
LED azul
    ↕
CHRG
    ↕
TP4054
    ↕
USB / bateria
```

As cores reais são mais importantes do que nomes como `BLED` encontrados
em esquemas, porque esses nomes podem significar outra coisa ou
corresponder a outra revisão da placa.

------------------------------------------------------------------------

## 17. O P0.13 e o corte de alimentação

Também surgiu a memória de documentação sobre desligar alimentação para
reduzir consumo.

O `variant.cpp` chama ao P0.13:

``` text
EXT_VCC
```

e coloca-o automaticamente em HIGH:

``` cpp
pinMode(EXT_VCC, OUTPUT);
digitalWrite(EXT_VCC, HIGH);
```

Pelo esquema desta placa, o P0.13 controla o **enable de um LDO**.

Portanto não deve ser interpretado simplesmente como um "pino para
apagar LEDs".

A sua função relevante é controlar uma alimentação externa/regulada.

Isto poderá ser particularmente importante quando o DX-LR30 for ligado à
placa, porque será necessário decidir:

-   de que rail será alimentado;
-   se esse rail passa pelo LDO controlado pelo P0.13;
-   qual a corrente disponível;
-   se interessa desligar o LoRa por hardware para poupança energética.

Essa parte ainda deve ser verificada antes de ligar o DX-LR30
definitivamente.

------------------------------------------------------------------------

## 18. Porque foi necessário mexer em definições tão escondidas?

Esta é provavelmente a conclusão mais importante de toda a história.

A opção visível no Arduino IDE:

``` text
SuperMini nRF52840
```

parece à primeira vista definir a placa.

Na realidade, por trás desse nome existem várias decisões independentes:

``` text
placa selecionada
      │
      ├── variant.cpp
      │      └── mapa dos GPIOs
      │
      ├── boards.txt
      │      ├── MCU
      │      ├── SoftDevice assumido
      │      ├── FWID do SoftDevice
      │      ├── linker script
      │      └── parâmetros de upload
      │
      ├── platform.txt
      │      ├── compilação
      │      ├── objcopy
      │      ├── criação do pacote DFU
      │      └── criação UF2
      │
      └── bootloader existente fisicamente na placa
```

A clone chinesa veio de fábrica com uma combinação que **não coincidia
exatamente com a combinação assumida pelo pacote Arduino**.

Fisicamente tínhamos:

``` text
nRF52840
+
bootloader nice!nano
+
SoftDevice S140 6.1.1
+
hardware SuperMini/ProMicro clone
```

Mas a definição Arduino `SuperMini` assumia:

``` text
nRF52840
+
S140 7.3.0
+
linker s140_v7
+
FWID 0x0123
```

Daí a necessidade de alterar parâmetros aparentemente escondidos.

Não se tratou de "afinar" arbitrariamente o core: foi necessário fazer
com que o ambiente de compilação descrevesse **o que realmente estava
gravado no chip**.

------------------------------------------------------------------------

## 19. Estado atual confirmado

Neste momento está confirmado:

### Hardware

``` text
MCU:      nRF52840
Flash:    1 MB
RAM:      256 KB
Package:  QIxx / 7x7 aQFN
```

### Bootloader

``` text
Adafruit UF2 Bootloader 0.9.2
Model: nice!nano
Board-ID: nRF52840-nicenano
```

### SoftDevice

``` text
S140 6.1.1
FWID: 0x00B6
```

### Aplicação

``` text
Linker: nrf52840_s140_v6.ld
Application start: 0x26000
```

### UF2

``` text
Family ID: 0xADA52840
```

A geração de UF2 está agora integrada automaticamente no Arduino IDE.

### Upload

Funciona:

``` text
duplo reset
→ NICENANO
→ copiar .uf2
→ aplicação arranca
```

Não está resolvido:

``` text
upload série via adafruit-nrfutil
```

mas deixou de ser necessário para trabalhar.

### GPIO especiais

``` text
P0.13 → EXT_VCC / enable do LDO
P0.15 → LED_BUILTIN → LED vermelho físico
```

### LEDs

``` text
Vermelho → controlado pelo nRF52840 / P0.15
Azul     → associado ao TP4054 / circuito de carga
```

------------------------------------------------------------------------

## 20. O que ainda não está completamente fechado

Apesar de a placa já estar operacional, ainda há pontos que merecem
confirmação antes de considerar o hardware totalmente caracterizado:

1.  confirmar completamente o esquema elétrico desta revisão concreta da
    clone;
2.  perceber em detalhe o comportamento do TP4054 e do LED azul sem
    bateria;
3.  confirmar exatamente que rail é comutado pelo LDO controlado pelo
    P0.13;
4.  medir/verificar a capacidade desse rail antes de alimentar o
    DX-LR30;
5.  decidir se o DX-LR30 deverá ser alimentado permanentemente ou
    através de alimentação controlável;
6.  mapear os nove GPIO necessários ao SX1262:
    -   SCK
    -   MOSI
    -   MISO
    -   NSS/CS
    -   BUSY
    -   DIO1
    -   RESET
    -   TXEN
    -   RXEN
7.  adaptar posteriormente o MeshCore para esta combinação específica de
    nRF52840 + DX-LR30.

Há ainda uma questão de software que pode valer a pena esclarecer por
curiosidade: a implementação de:

``` cpp
getBootloaderVersion()
```

porque o texto devolvido (`s140 6.1.1`) parece identificar o SoftDevice
e não o bootloader UF2 0.9.2.

------------------------------------------------------------------------

## 21. Configuração que convém guardar

Como `boards.txt` e `platform.txt` pertencem ao pacote instalado pelo
Arduino, uma atualização ou reinstalação do pacote pode apagar as
alterações.

Convém guardar uma cópia das alterações.

### `boards.txt`

Configuração essencial:

``` ini
supermini.build.ldscript=nrf52840_s140_v6.ld
supermini.build.uf2_family=0xADA52840

supermini.menu.softdevice.s140v6=S140 6.1.1
supermini.menu.softdevice.s140v6.build.sd_name=s140
supermini.menu.softdevice.s140v6.build.sd_version=6.1.1
supermini.menu.softdevice.s140v6.build.sd_fwid=0x00B6
```

### `platform.txt`

Ativação da criação automática do UF2:

``` ini
recipe.objcopy.uf2.pattern=python "{runtime.platform.path}/tools/uf2conv/uf2conv.py" -f {build.uf2_family} -c -o "{build.path}/{build.project_name}.uf2" "{build.path}/{build.project_name}.hex"
```

------------------------------------------------------------------------

## 22. Resumo em uma frase

A placa não estava avariada: **o hardware, o bootloader, o SoftDevice e
a definição `SuperMini` do core Arduino não estavam todos a falar
exatamente da mesma configuração**.

Foi necessário identificar cada camada separadamente e alinhá-las:

``` text
clone nRF52840 real
        +
bootloader nice!nano 0.9.2
        +
SoftDevice S140 6.1.1
        +
linker s140_v6
        +
FWID 0x00B6
        +
UF2 family 0xADA52840
        =
aplicação Arduino funcional
```

A partir daqui já existe uma base conhecida e reproduzível para avançar
para o objetivo seguinte: **ligar o DX-LR30/SX1262 e adaptar o MeshCore
sem voltar a misturar problemas de bootloader, SoftDevice, mapa de
memória e hardware da placa.**
