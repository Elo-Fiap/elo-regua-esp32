# Registro do uso de IA: régua do Elo (Physical Computing)

Segue o formato do `prompts.md` do app e do Elo Alertas.

Transparência: **o programa, o diagrama, o roteiro do vídeo e o README deste repositório foram gerados com IA** (Claude Code, da Anthropic), a partir dos pedidos do Bruno, e testados na simulação do Wokwi. Todos os integrantes precisam saber explicar cada linha do `sketch.ino`.

## Registro

| Data | Quem | Ferramenta | O que foi pedido | O que usamos | O que mudamos ou corrigimos |
| --- | --- | --- | --- | --- | --- |
| 05/11/2026 | Bruno | Claude Code | Entender o desafio e o projeto, ler o contrato da API e o app, e propor o circuito | Proposta: ESP32, HC-SR04, buzzer, três LEDs e LCD | A conferir |
| 06/11/2026 | Bruno | Claude Code | Abrir issues no repositório do app com as atualizações pendentes (aviso da régua, aba de réguas, favoritos) | Issues 1, 2 e 3 do `elo-app` | A conferir |
| 06/11/2026 | Bruno | Claude Code | "gostaria de montar o circuito do projeto com base no app no wokwi, mas não quero mexer no app [...] leve em consideração também os requisitos de entrega" | `sketch.ino`, `diagram.json`, `libraries.txt`, `roteiro-video.md` e o modelo do `integrantes.txt` | A conferir |
| 06/11/2026 | Bruno | Claude Code | Salvar o projeto na conta do Wokwi | Projeto público: https://wokwi.com/projects/479922926737544193 | A conferir |
| 08/11/2026 | Bruno | Claude Code | Revisar tudo, comparar com o app e conferir os requisitos | Correções no roteiro e este registro | A conferir |
| 09/11/2026 | Bruno | Claude Code | Abrir a issue do envio da régua à API | Issue 4 do `elo-app` | A conferir |
| 09/11/2026 | Bruno | Claude Code | Ajustar os comentários do programa e atualizar o projeto no Wokwi | `sketch.ino`, só comentários; o código não mudou | A conferir |
| 09/11/2026 | Bruno | Claude Code | Corrigir uma frase da issue 1: a régua indica a hora de seguir o plano da rua, não de fazer a chamada | Issue 1 do `elo-app` | A conferir |
| 09/11/2026 | Bruno | Claude Code | Subir o projeto para o GitHub, com os detalhes para quem cuida da API, e preencher os integrantes | Este repositório, o `README.md` e o `integrantes.txt` | A conferir |
| 09/11/2026 | Bruno | Claude Code | Corrigir as issues 4 e 1 do `elo-app`, que davam as rotas da régua como parte da API em C# | Issues 4 e 1 do `elo-app` e o item 8 dos pontos em aberto do `README.md` | A conferir |
| 09/11/2026 | Bruno | Claude Code | Explicar os repositórios da organização e deixar claro qual é o de IoT | Descrição deste repositório e um parágrafo no topo do `README.md` | A conferir |
| 14/11/2026 | João | Claude Code | Atualizar os projetos do grupo para receber os dados do painel web de administração (posição da régua no mapa, nível de atenção) | Só o `README.md`: o diagrama, onde a posição da régua fica cadastrada e os pontos em aberto 1, 6, 7 e 8. O `sketch.ino` não mudou | A conferir |
| 15/11/2026 | Bruno | Claude Code | Conferir se o app já lê a régua pela API e revisar a régua contra o app, a documentação e os requisitos da GS | Conferência com a API em C# no `README.md`, roteiro do vídeo atualizado (o app lê da API em C#; aviso na aba Agora; "em atenção") e issue 4 do `elo-app` | A conferir |
| 15/11/2026 | Bruno | Claude Code | Levantar as pendências da régua frente ao app e aos requisitos e aplicar o que faltava | Envio por Wi-Fi no `sketch.ino` (fila e tarefa no outro núcleo, HTTP simples, 2xx = sucesso, marcador no LCD), testado contra um serviço de eco; `README.md`, roteiro e este registro | A conferir |

## O que a IA decidiu sozinha e a equipe precisa conferir

- O nível de atenção em 1,00 m. O app, o contrato e a proposta só falam do limite de 1,20 m.
- Os LEDs e o LCD. A proposta citava só o sensor e o buzzer.
- A leitura extra publicada quando a situação muda, além dos intervalos de 30 s e 10 s do contrato.
- A mediana de cinco leituras por segundo.
- Os pinos do ESP32 e os textos do LCD.
- O jeito do envio pela rede: uma fila e uma tarefa no outro núcleo do ESP32, para o pedido HTTP não parar o aviso local. É mais código do que um envio direto no `loop()`, mas o envio direto travava a régua por segundos no teste de 06/11/2026.
- O destino padrão do envio enquanto a API não tem endereço público: o serviço de eco httpbin.org, por HTTP simples, com um valor de demonstração no lugar da chave, visível no projeto público.
- Os tempos de rede (até 8 s esperando o Wi-Fi ao ligar e até 5 s por envio) e o marcador `*`/`!` no canto do LCD.

A altura do sensor (3,00 m) e o limite (1,20 m) batem com a régua de exemplo do Elo Alertas e com o exemplo do app.

## Erros da IA que a equipe deve conhecer

- **Roteiro do vídeo.** A primeira versão mandava abrir a aba Agora do app para mostrar "acima do limite", que só aparece na aba Bairro, e dizia que a régua indica a hora de fazer a chamada. No app, ela indica a hora de seguir o plano da rua. Corrigido em 08/11/2026.
- **"A API não existe."** A revisão de 08/11/2026 e a issue 4 do `elo-app` disseram que a API que recebe as leituras ainda não existia. O Elo Alertas já tinha a rota `POST /leituras` desde 06/11/2026; a IA tinha olhado só o repositório do app. Corrigido em 09/11/2026 neste repositório e nas issues 4 e 1 do `elo-app`. A issue 1 dizia, em "Depende de", que as rotas eram da API de C#.
- **"O app vai ler pelo Elo Alertas."** O roteiro do vídeo (08/11/2026) dizia que o app leria a régua pelo Elo Alertas. O app lê pela API em C#, que também recebe as leituras. Corrigido em 15/11/2026.

## O que foi testado na simulação

- Níveis de 0,00 m a 2,98 m, com a virada exata em 1,20 m (1,19 m ainda é atenção).
- LEDs, LCD, ritmo do buzzer e a leitura no Monitor Serial em cada situação.
- Intervalos de 30 s e de 10 s, e a leitura na hora em que a situação muda.
- Sensor sem resposta: o LCD mostra "SEM LEITURA" e nada é publicado.
- Em 15/11/2026, o envio pela rede: Wi-Fi conectado em 1,9 s e 12 envios ao serviço de eco com resposta 200, de 0,93 s a 1,2 s, seis deles com o buzzer tocando, sem atrasar as leituras.

## O que não foi feito

- O envio para a API de verdade. O programa envia, mas para um serviço de eco: a API em C# e o Elo Alertas rodam no computador de quem desenvolve, e o ESP32 do Wokwi só alcança endereço público. Com a API na VM Linux do grupo, é trocar `URL_LEITURAS` e `CHAVE_DISPOSITIVO` e testar de novo. A falha de rede (endereço fora do ar) não foi simulada. Em 06/11/2026, a conexão segura (HTTPS) levou cerca de 7 segundos no ESP32 simulado e falhou enquanto o buzzer tocava; por isso o envio é em HTTP simples.
