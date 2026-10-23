# LusoFW — nRF52840 + DX-LR30

Adaptações para repetidor e companion BLE, mantendo o nome e a versão
existentes do LusoFW. Origem: [MeshCore](https://github.com/meshcore-dev/MeshCore)
→ [Meshcore-Portugal/lusofw](https://github.com/Meshcore-Portugal/lusofw)
→ variantes TENSTAR ProMicro nRF52840 + DX-LR30 deste projeto.

- **[Guia de montagem e instalação em português](docs/guia_nicenano_dx_lr30.md)**:
  material, imagens, bootloader, ligações, PlatformIO e caixas.
- [Histórico de configuração e testes](docs/historico_nicenano_dx_lr30.md).
- [Configuração das variantes](variants/nicenano_dx_lr30/platformio.ini).
- [Revisão para publicação](docs/revisao_publicacao.md).

Antes da primeira compilação, copia `platformio.local.ini.example` para
`platformio.local.ini`. Este último é local e não entra no Git.
O guia explica como selecionar o repetidor ou o companion BLE.

O repetidor e a atualização BLE foram testados pelo utilizador. A precisão da
medição da bateria e o esquema completo de alimentação solar continuam em
validação/documentação. As versões definidas no código não foram alteradas.

A [licença MIT existente](license.txt) e os créditos foram preservados.
O texto seguinte descreve os projetos de origem; esta adaptação não representa
uma nova distribuição oficial.

## About MeshCore

MeshCore is a lightweight, portable C++ library that enables multi-hop packet routing for embedded projects using LoRa and other packet radios. It is designed for developers who want to create resilient, decentralized communication networks that work without the internet.

## About lusofw
Lusofw is the official MeshCore firmware distribution tailored specifically for the Portuguese community. This project aims to enhance the user experience with quality-of-life improvements while maintaining compatibility with the upstream MeshCore repository.

You might support this project by [buying me a coffee](https://buymeacoffee.com/kasvoton).

### Philosophy

This is not intended to be a hard fork of the upstream repository. Instead, lusofw serves as:

- A release channel with curated quality-of-life improvements
- A testing ground for features that will eventually be merged upstream
- A community-focused distribution that provides faster access to enhancements

## Key Features

- Change default configuration to use 433 MHz frequency band
- Configure bridge mode to be disabled by default
- Consensus time sync over the network based on advert data
- Disable advertising functionality during system initialization
- Disable all sensor features and interfaces except AHTx0, BME280, BMP280 and INA3221
- Enable CLI boosted gain settings for SX126X radio modules (LNA)
- Enforce duty cycle limits using token bucket algorithm
- Implement hardware support for T114 sensor modules
- Limit repeater flood advert packet forwarding using a probabilistic reduction
- Limit repeater flood adverts to the maintenance window between 02:00 to 07:00
- Neighbours older than 48h will be automatically removed.
- The #portugal region is added and set as flood by default.

### Links

- Official Portuguese website: https://meshcore.pt
- Online flasher: https://flasher.meshcore.pt
- Original website: https://meshcore.io/
