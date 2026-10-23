# Adverts manuais e automáticos no repetidor LusoFW

> Atualização de 10 de setembro de 2026: a diferença de bytes por salto
> descrita abaixo foi corrigida. O envio automático passou a usar
> `sendFlood(pkt, uint32_t(0), uint8_t(_prefs.path_hash_mode + 1))`.
> Agora respeita a configuração, tal como o manual. O âmbito e o calendário
> automático mantêm-se. As secções seguintes registam a análise anterior.

Análise de 9 de setembro de 2026, referente ao código local e à variante
`nicenano_dx_lr30_repeater`, com `DISABLE_LEGACY_ADVERT` ativo.
Não foram feitas alterações ao firmware para esta análise.

## Diferenças encontradas

| Aspeto | Manual — comando `advert` | Automático |
| --- | --- | --- |
| Momento do envio | Agenda para daí a 1,5 segundos | Horário calculado num ciclo de 23 horas |
| Região/âmbito | Usa o âmbito predefinido | Envia flood sem âmbito |
| Bytes por salto | Respeita `path.hash.mode + 1`; com modo 1, usa 2 bytes | Usa o argumento por defeito: 1 byte |
| Criação do advert | `createSelfAdvert()` | A mesma função |

## Inconsistência nos bytes por salto

O envio manual passa explicitamente a preferência configurada:

```cpp
sendFloodScoped(default_scope, pkt, delay_millis, _prefs.path_hash_mode + 1);
```

O envio automático neste modo chama:

```cpp
sendFlood(pkt);
```

A declaração de `sendFlood()` usa `path_hash_size=1` por defeito.
Assim, o automático usa 1 byte por salto mesmo quando `path.hash.mode=1`
seleciona 2 bytes para o envio manual. É uma inconsistência no código analisado.

## Calendário automático

O horário é calculado a partir do nome e dos primeiros quatro bytes da chave
pública, num ciclo fixo de 23 horas, com uma variação de até ±3 segundos.

Neste modo, `flood.advert.interval` maior que zero habilita o envio automático;
o seu valor numérico não define a duração do ciclo, que está fixada em 23 horas
na função `updateFloodAdvertTimer()`.

O envio manual não reinicia o calendário automático.

## Comportamento comum e comando zero-hop

- Ambos os envios continuam sujeitos à fila, à verificação CAD e ao orçamento
  de tempo de emissão. O atraso de 1,5 segundos do comando manual não garante
  que a transmissão ocorra exatamente nesse instante.
- `advert.zerohop` é um comando manual distinto: envia para receção direta,
  sem propagação por outros repetidores.
- A resposta `OK - Advert sent` é uma confirmação do tratamento do comando;
  não confirma que o pacote já saiu pelo rádio.

## Referências no projeto

- [CommonCLI.cpp](../src/helpers/CommonCLI.cpp): comandos `advert` e `advert.zerohop`.
- [MyMesh.cpp](../examples/simple_repeater/MyMesh.cpp): `sendSelfAdvertisement()`,
  `updateFloodAdvertTimer()` e ramo automático de `loop()`.
- [Mesh.h](../src/Mesh.h): argumentos por defeito de `sendFlood()`.
- [Mesh.cpp](../src/Mesh.cpp): implementação do envio flood.
- [platformio.ini](../platformio.ini): ativação de `DISABLE_LEGACY_ADVERT`.

Esta comparação descreve o modo ativo nesta variante. O ramo antigo,
compilado sem `DISABLE_LEGACY_ADVERT`, tem outro comportamento.
