# Histórico de configuração — repetidor LusoFW nRF52840 + DX-LR30

Registo iniciado na sessão de configuração de 8 de setembro de 2026,
com atualizações posteriores identificadas nas respetivas secções.
Descreve as decisões, alterações, falhas e verificações efetuadas.
Os valores abaixo são os do código no momento deste registo; as preferências
persistentes da placa podem sobrepor-se aos valores por defeito.

## 1. Objetivo e origem

Criar uma variante de repetidor a partir do projeto
[Meshcore-Portugal/lusofw](https://github.com/Meshcore-Portugal/lusofw), para uma
placa TENSTAR ROBOT ProMicro nRF52840 e um módulo DX-LR30 com SX1262.

Pasta de trabalho:

```text
C:\Users\Luis\Documents\PlatformIO\Projects\luso
```

O projeto já tinha estrutura PlatformIO. O `platformio.ini` principal inclui
`variants/*/platformio.ini` e `platformio.local.ini`.
Na inspeção inicial, a pasta não era reconhecida como repositório Git.
Não foi criado um repositório nem configurada sincronização com o upstream.

O utilizador pediu confirmação antes das ações e que qualquer interrupção fosse
respeitada. As alterações foram autorizadas por etapas. A gravação na placa foi
efetuada pelo utilizador; o assistente compilou e gerou os ficheiros.

## 2. Identificação do hardware

### Microcontrolador e bootloader

A fotografia identifica a placa vermelha como TENSTAR ROBOT ProMicro NRF52840.
Após a primeira atualização, o bootloader apresentava-se como nice!nano 0.9.2.
O original de fábrica era nice!nano 0.6.0; a sequência completa está registada
na secção de atualização do bootloader mais abaixo.

```text
UF2 Bootloader 0.9.2
lib/nrfx (v2.0.0)
lib/tinyusb (0.12.0-145-g9775e7691)
lib/uf2 (remotes/origin/configupdate-9-gadbb8c7)
Model: nice!nano
Board-ID: nRF52840-nicenano
Date: Jul 19 2024
SoftDevice: S140 6.1.1
```

A identificação do bootloader não foi usada para presumir um pinout físico:
as ligações foram definidas a partir da fotografia e do esquema fornecidos.

Referências locais fornecidas:

```text
C:\Users\Luis\Documents\PlatformIO\Projects\top_bottom_red.jpg
C:\Users\Luis\Documents\PlatformIO\Projects\schematic.png
```

### Módulo LoRa

Manual fornecido: `DX-LR30-900M22S Module technical manual.pdf`, versão 2.0,
datado de 2024-12-17, da SHEN ZHEN DX-SMART TECHNOLOGY.

```text
C:\Users\Luis\Documents\aliexpress\lora\DX-LR30-900 Information Package\03 Module Technical Manual\DX-LR30-900M22S Module technical manual.pdf
```

O utilizador alertou que o pinout apresentado no PDF não devia ser interpretado
como o mapa de terminais do módulo físico. Não foram atribuídos números de
terminais de soldadura com base nessa numeração. A tabela deste documento usa
nomes de sinais do rádio e inscrições da placa nRF52840.

Dados elétricos apresentados pelo manual:

| Parâmetro | Valor |
| --- | --- |
| Banda do módulo | 850–930 MHz |
| Alimentação típica | 3,3 V |
| Intervalo indicado | 1,8–3,7 V |
| Corrente TX típica | 100 mA |
| Corrente RX típica | 10 mA |
| Potência RF máxima | 22 dBm |
| Oscilador | Sem compensação de temperatura |

O manual não fornece uma corrente TX máxima na tabela consultada. Também alerta
para possível desvio de frequência em larguras de banda inferiores a 125 kHz.
Foi mantida BW de 62,5 kHz por indicação explícita do utilizador, para corresponder
à rede pretendida.

## 3. Base escolhida e organização dos ficheiros

A variante existente `variants/promicro` foi a referência inicial. A definição
`boards/promicro_nrf52840.json` já declara nRF52840 e S140 6.1.1.
Porém, a variante original usa TCXO a 1,8 V e DIO2 como comutador RF, pelo que
não podia ser usada sem adaptação.

Foi criado o ambiente **`nicenano_dx_lr30_repeater`**.

| Ficheiro | Função |
| --- | --- |
| [platformio.ini da variante](../variants/nicenano_dx_lr30/platformio.ini) | Ambiente, rádio, fontes, monitor e UF2 automático |
| [DxLr30Board.h](../variants/nicenano_dx_lr30/DxLr30Board.h) | GPIOs e inicialização da placa, I²C e alimentação |
| [arduino/variant.h](../variants/nicenano_dx_lr30/arduino/variant.h) | Definições Arduino de SPI, I²C, UART e LED |
| [arduino/variant.cpp](../variants/nicenano_dx_lr30/arduino/variant.cpp) | Tradução dos índices Arduino para GPIOs Nordic |
| [target.h](../variants/nicenano_dx_lr30/target.h) | Declarações dos objetos da variante |
| [target.cpp](../variants/nicenano_dx_lr30/target.cpp) | Instâncias da placa, rádio, relógio e sensores |
| [README.md](../variants/nicenano_dx_lr30/README.md) | Resumo de utilização da variante |
| [platformio.local.ini](../platformio.local.ini) | Seleção do ambiente por defeito |
| [create-uf2.py](../create-uf2.py) | Conversão HEX → UF2 e adesão opcional ao Build normal |

Os ficheiros Arduino ficaram numa subpasta própria porque o construtor do
framework compila essa pasta como biblioteca de variante. O `target.cpp` é
incluído separadamente pelo filtro de fontes do projeto.

Não foram ativados ecrã, GPS, RTC externo ou sensores ambientais. O relógio é
`VolatileRTCClock`. A leitura de bateria devolve zero, pois não foi assumido um
divisor resistivo que não estivesse validado. A variante mantém o relógio de
baixa frequência RC usado pela base ProMicro.

## 4. Ligações aprovadas

| Sinal do DX-LR30 | GPIO Nordic | Inscrição na placa | Índice Arduino |
| --- | --- | --- | --- |
| SCK | P1.11 | 111 | 12 |
| MOSI | P1.15 | 115 | 14 |
| MISO | P0.02 | 002 | 15 |
| NSS / CS | P1.13 | 113 | 13 |
| BUSY | P0.29 | 029 | 16 |
| DIO1 | P0.20 | 020 | 3 |
| RESET do rádio | P0.22 | 022 | 4 |
| TXEN | P0.24 | 024 | 5 |
| RXEN | P0.17 | 017 | 2 |
| GND | GND | GND | — |

O RESET do rádio liga a **022**, não ao RST da placa.
P1.13, usado para NSS, não é P0.13, usado internamente para habilitar o LDO.
Os números usados em `digitalWrite()` são índices Arduino, não GPIOs Nordic
diretos nem posições físicas dos terminais.

Configuração RF:

```cpp
#define SX126X_DIO2_AS_RF_SWITCH false
#define SX126X_DIO3_TCXO_VOLTAGE (0.0f)
```

A RadioLib controla TXEN e RXEN como sinais ativos a HIGH.
Ambos são colocados a LOW na inicialização, antes da alimentação do rádio.

## 5. Alimentação e controlo do ME6217

O esquema indica um regulador externo ME6217 de 3,3 V, cuja entrada EN é
controlada por P0.13 (`POWER_EN`). A saída é a alimentação externa VCC.

```text
nRF52840 P0.13 → EN do ME6217 → saída regulada 3,3 V → VCC do rádio
```

Na primeira versão, a alimentação ficou pendente e esse controlo não foi
incluído. Depois da análise do manual e da autorização do utilizador, foi
adicionado:

```cpp
#define SX126X_POWER_EN 21 // índice Arduino 21 corresponde a P0.13

digitalWrite(SX126X_POWER_EN, HIGH);
pinMode(SX126X_POWER_EN, OUTPUT);
delay(10);
```

É o firmware executado pelo nRF52840 que dá a ordem ao pino EN. Não foi criado
um método alternativo de alimentação. A variante ProMicro original já fazia
esse controlo, mas o código não tinha sido inicialmente transportado para a
variante nova. O controlo dos reguladores internos do nRF52840 é distinto do
controlo deste componente externo.

A [ficha técnica do ME6217](https://datasheet.lcsc.com/lcsc/1912111437_MICRONE-Nanjing-Micro-One-Elec-ME6217C33M5G_C427602.pdf)
indica capacidade até 800 mA sob condições específicas de tensão e dissipação.
Isso torna plausível alimentar os 100 mA típicos do módulo, mas não substitui
a medição de tensão e estabilidade na placa. Não ficou registada uma medição
elétrica de VCC durante transmissão.

Foi recomendado confirmar aproximadamente 3,3 V entre VCC e GND antes de ligar
o módulo. O limite indicado de 3,7 V exclui alimentar diretamente o módulo
com 5 V ou com uma LiPo carregada a 4,2 V.

## 6. Parâmetros de rádio atuais

| Parâmetro | Valor por defeito |
| --- | --- |
| Frequência | 869,618 MHz |
| Largura de banda | 62,5 kHz |
| Spreading factor | SF7 |
| Taxa de codificação | 4/6 (`LORA_CR=6`) |
| Potência configurada | 14 dBm |
| Limite de corrente SX1262 | 140 mA |
| Ganho RX reforçado | Ativado |
| Nome anunciado | Luso DX-LR30 Repeater |
| Latitude / longitude iniciais | 0 / 0 |
| Palavra-passe administrativa inicial | `password` |
| Máximo de vizinhos | 50 |

A potência de 14 dBm é a configuração inicial, não uma medição de potência RF.
O limite de corrente do SX1262 não é uma garantia do consumo total da placa.

A primeira configuração herdava 433,375 MHz e SF9 do projeto principal.
Após identificar a banda 850–930 MHz no manual, foram aplicados os parâmetros
fornecidos pelo utilizador. `build_unflags` remove os valores antigos de
frequência e SF.

Houve uma falha de compilação intermédia: remover BW e CR por `build_unflags`
também removia os valores novos, pois eram idênticos aos herdados. A correção
foi deixar nesse campo apenas as definições antigas de frequência e SF.

**Uma atualização UF2 não implica apagar as preferências guardadas.** Nome e
parâmetros de rádio persistentes podem prevalecer sobre estes valores.

## 7. Configuração do PlatformIO e problemas de instalação

O VS Code deve abrir a pasta raiz `luso`, não apenas o ficheiro `.ini` nem a
subpasta da variante. O ambiente aparece em:

```text
PlatformIO → PROJECT TASKS → nicenano_dx_lr30_repeater
```

No início, a instalação apresentou:

```text
Failed to install PlatformIO IDE
Compatible PlatformIO Core not found
Could not import PlatformIO module
EPERM: operation not permitted, unlink ...\python3\python3.dll
Python executable does not exist!
PlatformIO Core is not installed
```

A inspeção de processos mostrou outras instâncias a usar o Python partilhado
de `C:\Users\Luis\.platformio`: uma compilação, três monitores série e um
script de captura. A pasta `penv` existia, mas `python3\python.exe` e
`python3\python3.dll` já não eram encontrados no disco durante essa inspeção.

O cenário era consistente com substituição do Python enquanto processos o
utilizavam. Não foi demonstrada corrupção física dos ficheiros. Foi recomendado
guardar trabalho, terminar tarefas e fechar as instâncias; depois de novo erro
de acesso a `penv\Scripts\python.exe`, foi sugerido reiniciar o Windows.

O utilizador comunicou a recuperação e confirmou:

```text
pio --version
PlatformIO Core 6.2.0
```

As tasks acabaram por aparecer após a inicialização/instalação. Não ficou
demonstrado quais os componentes que foram reinstalados ou instalados de novo.

Separadamente, a execução pelo assistente encontrou bloqueios do isolamento
ao escrever `platforms.lock` e a cache fora da pasta do projeto. As compilações
funcionaram após autorização de execução fora desse isolamento. Isso não foi
tratado como prova de uma nova avaria da instalação do utilizador.

## 8. Compilação e geração automática de UF2

Configuração de memória:

```ini
board = promicro_nrf52840
board_build.variant = arduino
board_build.variants_dir = variants/nicenano_dx_lr30
board_build.ldscript = boards/nrf52840_s140_v6_extrafs.ld
board_upload.maximum_size = 712704
```

O endereço inicial da aplicação é `0x26000`, com mapa S140 v6 e espaço
reservado para o sistema de ficheiros. A família usada no conversor UF2 é
`0xADA52840`.

Inicialmente, Build gerava HEX e ZIP; o UF2 era criado por tarefa explícita.
A pedido do utilizador, `create-uf2.py` passou a adicionar a tarefa UF2 aos
alvos por defeito quando o ambiente declara:

```ini
custom_auto_uf2 = yes
```

Também foram colocadas aspas nos caminhos do conversor e devolvido o seu
código de saída, para propagar falhas de conversão à compilação.

Build normal, que agora gera também UF2:

```powershell
pio run -e nicenano_dx_lr30_repeater
```

Tarefa explícita, que continua disponível:

```powershell
pio run -e nicenano_dx_lr30_repeater -t create_uf2
```

Ficheiro a copiar para a unidade USB do bootloader:

```text
.pio\build\nicenano_dx_lr30_repeater\firmware.uf2
```

Na última compilação desta sessão, após a correção do I²C:

| Resultado | Valor |
| --- | --- |
| Estado | SUCCESS |
| RAM | 27 724 / 235 520 bytes — 11,8% |
| Flash | 369 892 / 712 704 bytes — 51,9% |
| Tamanho UF2 | 739 840 bytes |

Versões observadas nessa compilação: Nordic nRF52 10.12.0, framework Adafruit
1.10701.0 com commit `d5413016`, GCC ARM 7.2.1 e RadioLib 7.7.1.
As dependências declaradas com intervalos podem resolver para outras versões
em instalações futuras.

## 9. COM29, consola e bloqueio depois do ID

O monitor foi configurado na variante:

```ini
monitor_port = COM29
monitor_speed = 115200
```

Para abrir com terminador compatível com a consola:

```powershell
pio device monitor -p COM29 -b 115200 --eol CR
```

O firmware executa o comando quando recebe CR (`\r`); LF (`\n`) é ignorado.
`--echo` pode ser acrescentado para eco local, mas esse eco não comprova que
a placa recebeu os caracteres. A consola do firmware também devolve caracteres.
O monitor tem de estar no terminal, não no painel OUTPUT da configuração.
Só um programa deve abrir a COM29 de cada vez; Ctrl+C fecha o monitor.

O comando `ver` existe em `src/helpers/CommonCLI.cpp` e devolve versão e data.
`board` devolve a identificação da placa. A consola não exige configuração
para deixar o repetidor trabalhar: o ciclo verifica a entrada sem esperar
obrigatoriamente por caracteres ou por um comando completo.

### Sintoma observado

A placa imprimia `Repeater ID: ...`, mas não respondia a `ver`.
Foi efetuado um teste direto com Python/pyserial: a COM29 abriu, `ver\r` foi
enviado e, durante cerca de quatro segundos, não houve eco nem resposta.
A porta foi fechada no final. Não foram enviados comandos de alteração.

A linha do ID aparece **depois** de `radio_init()` passar, mas **antes** de
`sensors.begin()` e `the_mesh.begin(fs)`. Por isso, comprovava apenas que o
arranque chegara a esse ponto, não que a consola e o repetidor já funcionavam.
A interpretação inicial excessivamente otimista foi corrigida.

### Causa encontrada e correção

`EnvironmentSensorManager::begin()` faz uma pesquisa I²C mesmo sem sensores
ativados. A variante não chamava `Wire.begin()`. A implementação nRF52 de
`endTransmission()` espera eventos do periférico, podendo ficar presa quando
esse periférico não foi inicializado.

Foi acrescentado em `DxLr30Board::begin()`:

```cpp
Wire.begin();
```

O I²C usa SDA P1.00 (índice 6) e SCL P0.11 (índice 7), sem conflito com os
pinos LoRa escolhidos. A correção inicializa o barramento; não remove a pesquisa
do gestor comum de sensores.

Depois da compilação e atualização pelo utilizador, este reportou eco na
consola e, posteriormente, **mensagens retransmitidas observadas no seu
cata-pacotes**. Esse último resultado confirma atividade real de RX/TX e é
consistente com a resolução do bloqueio de arranque.

## 10. Origem de CAD FREE

A mensagem vem de `src/helpers/radiolib/RadioLibWrappers.cpp`.
O firmware pede `scanChannel()` à RadioLib; o SX1262 executa a deteção CAD
(Channel Activity Detection), e o nRF52840 imprime o resultado:

| Mensagem | Interpretação |
| --- | --- |
| CAD FREE (clear) | Não foi detetada atividade LoRa nessa verificação |
| CAD DETECTED (busy) | Foi detetada atividade LoRa |
| CAD UNKNOWN | Resultado diferente dos dois anteriores; tratado como ocupado |

É uma verificação para evitar colisões antes da transmissão, mais próxima da
ideia CSMA/CA do que de CSMA/CD. Não deteta colisões durante a própria emissão
e não garante que outros nós não transmitam simultaneamente.

O LusoFW ativa CAD por defeito. A verificação RSSI relativa ao ruído é um
mecanismo separado, cujo limiar está a zero por defeito. Após mais de quatro
segundos de estado ocupado, o Dispatcher pode forçar uma tentativa de emissão
como recuperação de um eventual estado anómalo do rádio.

## 11. Critérios de repetição e duplicados inspecionados

Esta parte foi apenas analisada; não foi alterada a política de repetição.

Para mensagens normais em flood, os critérios encontrados incluem:

1. Encaminhamento habilitado nas preferências.
2. Limites de saltos e espaço disponível no percurso.
3. Região/âmbito admitido e deteção de ciclos no percurso.
4. Tabela de pacotes já conhecidos, com 160 entradas circulares.
5. Atraso aleatório antes da retransmissão e prioridade na fila.
6. Verificação do rádio/CAD e orçamento de tempo de emissão antes de enviar.
7. Expiração de pacotes que permaneçam mais de 60 segundos na fila de saída.

A assinatura de duplicados usa o tipo e conteúdo do pacote; normalmente não
inclui o percurso. Há exceções específicas, como TRACE. A tabela é circular,
não uma memória permanente de todas as mensagens.

Com `tx_delay_factor = 0.5`, o atraso flood é aproximadamente zero a 2,5 vezes
o tempo estimado de emissão. O fator direto por defeito é 0,3. Tráfego direto
respeita o próximo salto indicado e tem prioridade superior ao flood normal;
entre os floods, os percursos mais curtos recebem prioridade superior.

Existe uma pontuação baseada em SNR, SF e tamanho do pacote que pode afetar
um atraso de processamento na receção. Contudo, `rx_delay_base = 0` no
LusoFW desativa esse atraso por defeito. Não foi encontrado um veto geral
de retransmissão por RSSI ou SNR elevados.

### Receber outra cópia enquanto aguarda

O percurso normal observado é:

```text
Recebe A → regista assinatura → agenda A para transmissão
Recebe outra cópia de A → descarta essa nova cópia
A primeira cópia continua agendada → pode ser transmitida
```

Não foi encontrada, nesse percurso normal, uma regra que remova o primeiro
pacote da fila por ter ouvido outro repetidor enviar o mesmo conteúdo.
Eliminar uma cópia recebida não equivale a cancelar a transmissão pendente.

O utilizador propôs esperar e suprimir a retransmissão após ouvir uma segunda
cópia. A ideia foi discutida, incluindo a limitação de cobertura: ouvir outro
repetidor não garante que esse repetidor alcance os mesmos destinos.
**Não foi implementada essa proposta.**

Referências principais:

- [MyMesh.cpp](../examples/simple_repeater/MyMesh.cpp): permissões de encaminhamento e atrasos.
- [Mesh.cpp](../src/Mesh.cpp): processamento de pacotes e encaminhamento.
- [Dispatcher.cpp](../src/Dispatcher.cpp): filas, CAD, orçamento e expiração.
- [SimpleMeshTables.h](../src/helpers/SimpleMeshTables.h): assinaturas e duplicados.
- [Packet.cpp](../src/Packet.cpp): cálculo da assinatura.
- [LusoDefaults.cpp](../src/helpers/LusoDefaults.cpp): valores iniciais LusoFW.

## 12. Estado no fim da sessão

- Variante criada e reconhecida pelo PlatformIO.
- Build normal gera UF2 automaticamente.
- Monitor configurado para COM29 a 115200 baud.
- Alimentação externa habilitada por P0.13 no firmware.
- Rádio configurado por defeito para 869,618 MHz, BW 62,5 kHz, SF7, CR 4/6.
- Inicialização I²C corrigida antes da pesquisa de sensores.
- Compilação e geração UF2 concluídas com sucesso.
- Utilizador confirmou retransmissões no cata-pacotes após atualizar a placa.
- Sem ensaio documentado de alcance, estabilidade prolongada, corrente,
  temperatura do regulador ou potência RF medida.
- Sem alteração dos mecanismos de CAD, deduplicação ou política de encaminhamento.

Este documento regista o que foi observado e alterado; não certifica o hardware
nem substitui a confirmação dos parâmetros efetivamente guardados na placa.

## 13. Trabalho futuro — leitura dos reguladores internos

Atualização de 10 de setembro de 2026: implementação autorizada e acrescentada
à consola nRF52840 através do comando local `powerregs`. O nome correto do
registo de estado no SDK é `MAINREGSTATUS`, não `REGSTATUS` como indicado na
proposta inicial abaixo. A saída mostra os quatro valores em hexadecimal e a
interpretação de tensão configurada, bits DC/DC e modo VDD/VDDH. Não mede tensão
nem corrente e não escreve nos registos. A validação em hardware fica pendente.
Na mesma alteração, o advert automático passou a respeitar `path.hash.mode`.

Em 9 de setembro de 2026, o utilizador pediu para guardar esta tarefa para
mais tarde. **Não está autorizada a implementação nesta fase.**

Proposta: acrescentar um comando de diagnóstico `powerregs` à consola série
para consultar, sem escrever nos registos:

```cpp
NRF_UICR->REGOUT0
NRF_POWER->DCDCEN0
NRF_POWER->DCDCEN
NRF_POWER->REGSTATUS
```

O comando deverá apresentar os valores brutos e interpretar a tensão
configurada para REG0, os bits de ativação DC/DC de REG0 e REG1 e o modo de
alimentação indicado por REGSTATUS, conforme as definições Nordic.
Não deverá alterar UICR, ativar reguladores, apagar memória ou reiniciar a placa.

Após autorização futura: implementar o comando, compilar e gerar UF2;
a atualização da placa e a consulta pela COM29 serão passos separados.
Uma alternativa é ler os registos com um depurador SWD, sem acrescentar o comando.

Contexto: o ficheiro fornecido
`C:\Users\Luis\Documents\PlatformIO\update-nice_nano_bootloader-0.9.2_nosd.uf2`
foi comparado com o UF2 oficial Adafruit nice!nano 0.9.2 e tem o mesmo SHA-256:

```text
7c5a27809ce2cdbf536f8853afc4d72ef1913c8c6232625585f9df4799f29d0d
```

O código oficial dessa versão define REGOUT0 para 3,3 V e não ativa DC/DC
de REG0 ou REG1. A variante atual também não ativa esses conversores.
Isto descreve o comportamento esperado, não uma leitura do estado real da placa.
A consulta futura servirá para confirmar esse estado em execução.
## Registo da atualização do bootloader — confirmado pelo utilizador em 23/10/2026

Os três blocos seguintes identificam o estado de fábrica e as duas atualizações
do bootloader na mesma placa. A data de confirmação deste registo não indica
a data em que a atualização foi realizada; as datas nos blocos são as datas
de compilação dos bootloaders.

### 1. Bootloader de fábrica: nice!nano 0.6.0

A placa chegou sem aplicação e entrava diretamente em modo DFU.
Identificação original fornecida pelo utilizador:

```text
UF2 Bootloader 0.6.0 lib/nrfx (v2.0.0) lib/tinyusb (0.10.1-41-gdf0cda2d) lib/uf2 (remotes/origin/configupdate-9-gadbb8c7)
Model: nice!nano
Board-ID: nRF52840-nicenano
SoftDevice: S140 version 6.1.1
Date: Jun 19 2021
```

### 2. Primeira atualização: nice!nano 0.9.2

Foi usado `update-nice_nano_bootloader-0.9.2_nosd.uf2`.
Esta atualização manteve Model nice!nano e Board-ID nRF52840-nicenano.

```text
UF2 Bootloader 0.9.2 lib/nrfx (v2.0.0) lib/tinyusb (0.12.0-145-g9775e7691) lib/uf2 (remotes/origin/configupdate-9-gadbb8c7)
Model: nice!nano
Board-ID: nRF52840-nicenano
Date: Jul 19 2024
SoftDevice: S140 6.1.1
```

### 3. Segunda atualização: ProMicro OTAFIX

Foi utilizado o ficheiro:

```text
update-promicro_nrf52840_bootloader-0.9.2-OTAFIX2.2-BP1.3_nosd.uf2
```

Após a atualização, o duplo reset permitiu abrir a unidade USB e confirmar
no ficheiro de informação UF2:

```text
UF2 Bootloader 0.9.2-OTAFIX2.2-BP1.3 lib/nrfx (v2.0.0) lib/tinyusb (0.12.0-145-g9775e7691) lib/uf2 (remotes/origin/configupdate-9-gadbb8c7)
Model: ProMicro NRF52840
Board-ID: nRF52840-promicro
Date: Apr 13 2026
SoftDevice: S140 6.1.1
```

A identificação mudou de nice!nano para ProMicro NRF52840; a versão reportada
do SoftDevice continuou a ser S140 6.1.1. São etapas sucessivas da atualização,
não registos de duas placas diferentes.

### 4. Validação após a atualização

- A unidade USB/UF2 ficou acessível por duplo reset.
- O arranque com P1.00 (pino marcado `100`) ligado a GND permitiu entrar em
  DFU BLE, anunciado como `PROM_DFU`; a ligação a GND foi apenas necessária
  durante o arranque.
- Foi realizada uma atualização da aplicação com a app Nordic DFU e o
  `firmware.zip`, sem módulo SX1262 ligado.
- O utilizador confirmou o sucesso do teste com o firmware identificado pela
  mensagem `BLE UPDATE TEST 2`.
- Posteriormente, o utilizador confirmou outra atualização: iniciou o modo OTA
  através do comando `start ota` na app e efetuou a transferência com a aplicação
  **Nordic DFU**. Este teste valida também a entrada em OTA por comando, além
  da entrada física por P1.00 anteriormente testada. Não foram indicados o
  ficheiro usado nessa atualização.
- Em esclarecimento posterior, o utilizador confirmou **PRN de 5 pacotes**
  na Nordic DFU. O estado final de `Request high MTU` não foi confirmado.
- O utilizador recomenda, pela sua experiência, um **power cycle após a
  atualização terminar com sucesso**: cortar USB e bateria antes de voltar
  a alimentar. Esta recomendação foi acrescentada ao guia e não implica
  interromper uma transferência ou gravação em curso.

---

## Atualização da medição da bateria — registo de 23/10/2026

### Implementação autorizada

**Atualização de 24/10/2026:** a pedido do utilizador, foram retirados os filtros
por presença de USB e modo de alimentação. `getBattMilliVolts()` passa a devolver
sempre a leitura de VDDH, mantendo a aquisição e conversão descritas abaixo.
O comando `battery` mostra `VDDH: ... mV; USB VBUS: present/absent`.
O estado e a telemetria também recebem o valor sem filtro, embora a aplicação
possa continuar a identificar esse campo como bateria. A deteção USB consulta
`USBREGSTATUS.VBUSDETECT`, através do SoftDevice quando ativo, ou diretamente
no registo quando inativo; deteta alimentação VBUS, não uma ligação de dados.
As descrições de indisponibilidade por USB abaixo documentam a versão anterior.

O utilizador confirmou que a bateria está ligada diretamente a **B+ e B−**.
Anteriormente, `getBattMilliVolts()` devolvia sempre `0`, porque não havia um
divisor externo validado. A variante passou a medir a entrada interna
**VDDH/5** do SAADC, sem acrescentar resistências nem ocupar um GPIO.

Em `variants/nicenano_dx_lr30/DxLr30Board.h`, a leitura usa:

- Referência interna com escala de 1,2 V e resolução de 12 bits.
- Tempo de aquisição de 10 µs e oversampling de hardware desativado.
- Descarte da primeira conversão após configurar o ADC.
- Média de oito conversões, convertida em milivolts tendo em conta VDDH/5.
- Reposição de referência `AR_DEFAULT`, resolução de 10 bits e aquisição de
  3 µs após a leitura.

A conversão implementada é `(soma * 6000 + 16384) / 32768`, com arredondamento.
Não foi aplicado qualquer fator de calibração ou compensação fixa.

Com USB detetado, ou se o micro não estiver em modo de alimentação VDDH,
a função devolve `0` (indisponível). A deteção USB é repetida após a aquisição.
O objetivo é não apresentar a alimentação USB como se fosse tensão da bateria.
A correspondência e precisão de VDDH relativamente a B+/B− continuam sujeitas
a validação física.

### Consulta e ficheiros gerados

Foi acrescentado o comando **`battery`**, em minúsculas, em
`src/helpers/CommonCLI.cpp`, condicionado por `BATTERY_VIA_VDDH=1` no ambiente
`nicenano_dx_lr30_repeater`. O comando apresenta milivolts ou uma indicação
explícita de indisponibilidade, incluindo quando o USB está ligado.

O estado do repetidor e a telemetria usam a mesma função de leitura. O estado
envia milivolts; a telemetria converte para volts. Não foi encontrada uma
correção adicional nestes percursos do firmware. O comportamento interno da
aplicação do telemóvel não foi inspecionado.

A compilação do repetidor terminou com sucesso e gerou `firmware.uf2` e
`firmware.zip` em `.pio/build/nicenano_dx_lr30_repeater/`. A implementação
partilhada da placa também é usada pela variante companion, mas nesta etapa
foi compilado apenas o repetidor. O assistente não gravou a placa.

### Observações do utilizador e validação pendente

- Comparação inicial: multímetro **4,11 V**, ecrã da aplicação **4,26 V**.
- Mais tarde, o comando apresentou **3973 mV**, a telemetria **3,97 V / 80%**
  e **26 °C**, e o pedido de estado cerca de **3,96 V**.
- O utilizador identificou três locais de visualização na aplicação. A
  hipótese de atualização independente ou valores guardados foi considerada,
  mas não confirmada por inspeção da aplicação.
- Ao refrescar, foram reportados valores entre **3,96 V e 4,26 V**. Não ficou
  confirmado se a mesma amplitude ocorre em chamadas consecutivas de `battery`.
- Foi considerada a possibilidade de quedas momentâneas da alimentação durante
  transmissões LoRa. Não foi demonstrado que sejam a causa; a rotina do ADC e
  o percurso de alimentação também precisam de ser avaliados.

**Não há ainda uma calibração validada.** O fator `4,11 / 4,26 = 0,96479` foi
apenas discutido e não aplicado, porque a leitura do ecrã poderia não ser
contemporânea da medição e uma comparação isolada não distingue ganho de offset.

Ficou combinado esperar pela noite e comparar várias leituras recentes de
`battery` com o multímetro em B+/B−, aproximadamente ao mesmo tempo e com USB
desligado. À data deste registo, esses resultados ainda não foram fornecidos.

## Recuperação do companion e fotografias — 24/10/2026

Após instalar OTAFIX no companion com o firmware inicial (sem medição de
bateria), o utilizador observou falha de enumeração USB, código 43, e ausência
de BLE. O bootloader continuava a funcionar: por duplo reset apresentava
ProMicro NRF52840, VID 239A/PID 00B3, interfaces nRF Serial e nRF UF2.
O problema também ocorreu numa porta direta do PC; não ficou explicado apenas
pelo hub. Regravar o companion sem limpar os dados não resolveu.

Foi preparado um ambiente separado de diagnóstico por LED. Não foi reportada
uma sequência de diagnóstico desse firmware. O utilizador optou depois pela
limpeza autorizada dos dados persistentes.

O utilitário específico `nicenano_dx_lr30_clear_config` verifica o mapa de
flash e o endereço do bootloader antes de apagar apenas ExtraFS
(`0xD4000–0xECFFF`) e InternalFS (`0xED000–0xF3FFF`). Não inicializa o rádio
nem inclui apagamento do SoftDevice, UICR ou bootloader. Verifica a região
apagada antes de indicar sucesso com duas piscadelas e pausa.

O utilizador confirmou as duas piscadelas e a COM26. Seguiu-se recompilação
completa do companion normal, instalação do UF2 e confirmação de funcionamento.
A sequência limpeza + reinstalação resolveu, sem identificar exatamente quais
dados causavam a falha. O repetidor noutro dispositivo com o mesmo hardware
não apresentou esse problema após atualizar o bootloader.

Foram acrescentadas ao guia três fotografias próprias fornecidas pelo utilizador:
`companion chip lora.jpg`, `companion pinout.jpg` e `conjunto companion.jpg`.
Mostram o DX-LR30-900M22S, o verso do adaptador DX-PJ27-V1.2 com os sinais
identificados e a montagem com a caixa aberta.

### Confirmação de atualização do companion por BLE

Após a recuperação, o utilizador confirmou que também realizou com sucesso
uma atualização do companion por BLE usando o mesmo `firmware.zip`.
Foi uma reinstalação do mesmo firmware, sem mudança de versão, confirmando
que a atualização por BLE funcionou neste dispositivo com o bootloader OTAFIX.
