# Registro do uso de IA: régua do Elo (Physical Computing)

Segue o formato do `prompts.md` do app e do Elo Alertas.

Transparência: **o programa, o diagrama, o roteiro do vídeo e o README deste repositório foram gerados com IA** (Claude Code, da Anthropic), a partir dos pedidos do Bruno, e testados na simulação do Wokwi. **O vídeo da entrega também foi feito com IA**: a tela foi gravada por automação e a narração é a voz do Bruno clonada por IA, com a autorização dele (linha de 19/11/2026). Todos os integrantes precisam saber explicar cada linha do `sketch.ino`.

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
| 18/11/2026 | Bruno | Claude Code | Apontar a régua para a API publicada pelo João (http://elo-api-fiap.azurewebsites.net), com a chave de demonstração, testar o envio de verdade e salvar | `sketch.ino` (endereço, chave, espera por envio de 5 s para 8 s e comentários), projeto no Wokwi, `README.md`, roteiro e este registro. Teste: 11 envios com 204 e o `GET /regua` devolvendo o nível da régua | A conferir |
| 18 e 19/11/2026 | Bruno | Claude Code, com Playwright, ffmpeg, Whisper e VoxCPM2 | Gravar o vídeo da entrega seguindo um guia de vídeo tutorial narrado, com a voz do Bruno | Vídeo de 2 min 33 s. A tela (cartelas com prints do app, o projeto no Wokwi e a página pública da API) foi gravada por um navegador automático. A narração é a voz do Bruno clonada por IA (motor VoxCPM2, licença Apache-2.0) a partir de uma amostra de 28 s gravada por ele, com a autorização dele. Cada fala foi conferida com o Whisper. Falas em [`roteiro-video.md`](./roteiro-video.md) | Três versões até a aprovada pelo Bruno. A primeira voz clonada (outro motor, amostra gravada muito baixo) saiu com chiado, pausas artificiais e "ESP32" mal falado. Nas falas, "app" virou "aplicativo" e "id" virou "identificador", porque o motor lia "app" igual a "API" |

## O que a IA decidiu sozinha e a equipe precisa conferir

- O nível de atenção em 1,00 m. O app, o contrato e a proposta só falam do limite de 1,20 m.
- Os LEDs e o LCD. A proposta citava só o sensor e o buzzer.
- A leitura extra publicada quando a situação muda, além dos intervalos de 30 s e 10 s do contrato.
- A mediana de cinco leituras por segundo.
- Os pinos do ESP32 e os textos do LCD.
- O jeito do envio pela rede: uma fila e uma tarefa no outro núcleo do ESP32, para o pedido HTTP não parar o aviso local. É mais código do que um envio direto no `loop()`, mas o envio direto travava a régua por segundos no teste de 06/11/2026.
- Enquanto a API não tinha endereço público (até 17/11/2026), o destino do envio era um serviço público de eco (httpbin.org), por HTTP simples, com um valor de demonstração no lugar da chave. Desde 18/11/2026 o destino é a API publicada, com a chave de demonstração combinada com o grupo; o eco ficou só como opção comentada no programa.
- Os tempos de rede (até 8 s esperando o Wi-Fi ao ligar e até 8 s por envio, porque a API no plano gratuito demora a acordar) e o marcador `*`/`!` no canto do LCD.

A altura do sensor (3,00 m) e o limite (1,20 m) batem com a régua de exemplo do Elo Alertas e com o exemplo do app.

## Erros da IA que a equipe deve conhecer

- **Roteiro do vídeo.** A primeira versão mandava abrir a aba Agora do app para mostrar "acima do limite", que só aparece na aba Bairro, e dizia que a régua indica a hora de fazer a chamada. No app, ela indica a hora de seguir o plano da rua. Corrigido em 08/11/2026.
- **"A API não existe."** A revisão de 08/11/2026 e a issue 4 do `elo-app` disseram que a API que recebe as leituras ainda não existia. O Elo Alertas já tinha a rota `POST /leituras` desde 06/11/2026; a IA tinha olhado só o repositório do app. Corrigido em 09/11/2026 neste repositório e nas issues 4 e 1 do `elo-app`. A issue 1 dizia, em "Depende de", que as rotas eram da API de C#.
- **"O app vai ler pelo Elo Alertas."** O roteiro do vídeo (08/11/2026) dizia que o app leria a régua pelo Elo Alertas. O app lê pela API em C#, que também recebe as leituras. Corrigido em 15/11/2026.

- **Narração do vídeo.** A conferência automática da voz clonada usava uma lista de vocabulário que fazia o Whisper escrever "ESP32" mesmo quando a voz dizia outra coisa; o Bruno ouviu o erro. A conferência passou a ser feita sem a lista. O Whisper confere as palavras, não o timbre nem a naturalidade: quem aprovou a voz foi o Bruno, ouvindo.
- **"A simulação só tem Wi-Fi com login no Wokwi."** Em 17/11/2026 a IA concluiu isso e pediu o login do Bruno várias vezes. Na noite de 18/11/2026 a simulação enviou para a API sem login; o que falha de vez em quando é a rede da simulação ao ligar, com ou sem login. A gravação passou a testar o envio antes de cada tomada.

## O que foi testado na simulação

- Níveis de 0,00 m a 2,98 m, com a virada exata em 1,20 m (1,19 m ainda é atenção).
- LEDs, LCD, ritmo do buzzer e a leitura no Monitor Serial em cada situação.
- Intervalos de 30 s e de 10 s, e a leitura na hora em que a situação muda.
- Sensor sem resposta: o LCD mostra "SEM LEITURA" e nada é publicado.
- Em 15/11/2026, o envio pela rede: Wi-Fi conectado em 1,9 s e 12 envios ao serviço de eco com resposta 200, de 0,93 s a 1,2 s, seis deles com o buzzer tocando, sem atrasar as leituras.
- Em 18/11/2026, o envio para a API publicada: Wi-Fi conectado em 1,8 s e 11 envios com resposta 204, de 1,19 s a 1,72 s, seis deles com o buzzer tocando; duas leituras com um segundo de diferença, as duas enviadas; o `GET /regua` devolveu o nível que a régua mandou (0,50 m e 1,42 m) com a hora da medida.

## O que não foi feito

- No vídeo, o app aparece em prints do repositório do app, não rodando ao vivo. A leitura chegando ao sistema do grupo aparece pela página pública da API (http://elo-api-fiap.azurewebsites.net), que mostra o mesmo número do display da régua.
- A falha de rede (endereço fora do ar) e a API acordando do plano gratuito no meio da simulação não foram simuladas. O envio para o Elo Alertas não foi testado: a régua manda para a API em C#, de onde o app e o painel leem. O app e o painel mostrando a leitura enviada não foram conferidos aqui.
- A conexão segura (HTTPS). Em 06/11/2026 ela levou cerca de 7 segundos no ESP32 simulado e falhou enquanto o buzzer tocava; por isso o envio é em HTTP simples, e a API publicada aceita HTTP.
