# Revisão local para publicação

Esta revisão prepara a publicação; não foi criado repositório remoto, commit
ou release. A pasta local ainda não é um repositório Git. Sem histórico Git,
não foi possível obter um diff completo contra a versão exata de origem.

## Preparado

- README principal com acesso ao guia, histórico e variantes.
- Origem MeshCore → Meshcore-Portugal/lusofw explicitada; licença MIT e créditos
  existentes preservados.
- Versões mantidas: `FIRMWARE_VERSION` v1.16.0 e
  `LUSOFW_FIRMWARE_VERSION` 2026.7.1 no repetidor. Sem nova numeração.
- Exemplo `platformio.local.ini.example` para criar a configuração local,
  excluída pelo `.gitignore` existente.
- `.pio`, ambientes Python e configurações locais já cobertos pelo `.gitignore`.
- Guia com caminhos relativos para imagens e suporte STL.
- README da variante atualizado com os testes e a sequência dos bootloaders.

Foi feita uma pesquisa limitada por padrões de chaves privadas e tokens GitHub,
sem correspondências. Isto não equivale a uma auditoria completa de segredos.
As credenciais padrão de firmware (`password` e PIN BLE `123456`) são valores
de fábrica; não foram substituídas por credenciais pessoais nesta revisão.

## Pontos a resolver antes do primeiro envio

1. **Origem e histórico Git:** preferir partir de um fork/checkout de
   Meshcore-Portugal/lusofw e integrar as alterações locais com comparação,
   preservando o histórico. Não substituir esta pasta nem perder alterações.
2. **Automação herdada:** `.github/workflows/main-release.yaml` e
   `dev-prerelease.yaml` reagem a pushes nas respetivas branches e têm permissão
   de escrita. Rever/adaptar ou desativar esses workflows antes do primeiro
   push para evitar uma release automática não pretendida. Não foram alterados.
3. **Materiais de terceiros:** confirmar a proveniência e as condições de
   redistribuição das fotografias, esquema, PDF, STL e binários de bootloader
   em `docs/projectoGit`. A licença MIT do código não determina automaticamente
   a licença destes materiais. Se necessário, publicar links de origem em vez
   dos ficheiros. Créditos NOLOGO e referências externas já aparecem no guia,
   mas isso não constitui confirmação de autorização de redistribuição.
4. **Mensagens de diagnóstico:** o repetidor ainda inclui `BLE UPDATE TEST 2`
   no arranque e no aviso de rádio ausente. Decidir se se mantém o diagnóstico
   ou se se troca por uma mensagem final antes de distribuir binários.
5. **Reprodutibilidade:** esta revisão documental não recompilou firmware nem
   testou uma instalação limpa. As dependências incluem intervalos de versões;
   uma compilação futura pode usar versões diferentes das testadas anteriormente.

## Limitações já identificadas no guia

- Medição de bateria com oscilações reportadas; calibração não aplicada.
- Ligações completas da alimentação solar ainda por documentar.
- Saída DFU por comando Reset isolado e recuperação de todas as falhas ainda
  sem ensaio físico documentado.
- COM29 é uma configuração local, não uma porta universal.

Não foram alterados o firmware, a versão nem a configuração rádio nesta revisão.
