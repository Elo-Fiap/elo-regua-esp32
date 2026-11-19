# Roteiro do vídeo (até 4 minutos)

A tarefa pede três coisas no vídeo: apresentar a solução do grupo, explicar como o circuito se integra ao projeto e mostrar a simulação funcionando no Wokwi. O roteiro abaixo cobre as três e fecha em cerca de 3min40.

As falas são sugestões. Digam com as palavras de vocês.

## Antes de gravar

- Abrir o projeto no Wokwi pelo link público, em janela anônima, para conferir que o link abre sem login.
- Deixar o app do Elo aberto. Com `EXPO_PUBLIC_API_URL` apontando para a API publicada (http://elo-api-fiap.azurewebsites.net), a aba Agora mostra o aviso da régua com o último nível que a régua simulada mandou, e a aba Bairro, a linha da régua. Sem esse endereço, o app mostra dados de exemplo ("Exemplo: régua acima do limite" no alto da aba Agora; "1,42 m, limite 1,20 m, acima do limite" na aba Bairro) e diz isso na tela. No app, uma leitura com mais de 5 minutos vira "antiga": deixar a simulação enviando enquanto mostra o app.
- Ao apertar o play, o Wokwi compila o programa antes de rodar. Nos testes isso levou de 10 a 40 segundos; sem login no Wokwi pode haver uma fila de compilação de alguns minutos. Vale deixar a simulação já rodando antes de gravar a parte 3, ou cortar a espera na edição.
- No Wokwi, o sensor começa em 250 cm. Para mudar: clicar no HC-SR04 e mexer no controle "Distance". As setas do teclado mudam de 1 em 1 cm, o que ajuda a acertar o número.
- Testar o volume do buzzer antes de gravar. Ele fica em `"volume": "0.2"` no `diagram.json` e aceita de 0.01 a 1.0.
- A leitura no Serial sai na hora quando a situação muda (normal, atenção, acima do limite). Dentro da mesma situação, ela sai no intervalo: 30 s, ou 10 s acima do limite.
- Para acertar 200, 181 e 180 cm: arrastar até perto, parando em um número maior, e terminar com a seta para a esquerda. Passar abaixo de 181 cm dispara o alarme antes da hora.
- Na volta de 158 para 250 cm, o LCD pode mostrar "ATENCAO" por um instante, ao cruzar a faixa de 181 a 200 cm. É esperado.
- Os tempos (1,5 s, 10 s, 30 s) seguem o relógio da simulação. Se o indicador de velocidade do Wokwi estiver abaixo de 100%, tudo demora um pouco mais. Deixar a janela do Wokwi visível: escondida, o navegador deixa a simulação quase parada.
- O envio pela rede usa a rede "Wokwi-GUEST" do simulador e precisa de internet no computador. O Wi-Fi conecta em cerca de 2 s e cada envio à API publicada leva de 1 a 2 s; o "*" no canto do LCD e o "Envio OK: HTTP 204" no Serial mostram que a API recebeu. A API fica no plano gratuito do Azure: parada por um tempo, demora até um minuto para acordar, e aí o primeiro envio falha ("!" no LCD). Antes de gravar, abrir http://elo-api-fiap.azurewebsites.net/health no navegador e esperar a resposta.
- Conferir se a gravação da tela captura o som do navegador. Sem isso, o buzzer não aparece no vídeo.
- Para mostrar a aba Plano, escolher no app uma rua que tenha plano no modo de exemplo: Travessa 7, Rua Nova, Beco do Rio ou Rua 3.

## Parte 1. A solução do grupo (0:00 a 0:40)

Tela: o app do Elo.

- O problema: na chuva forte, quem cuida da rua não sabe quem está bem e quem precisa de ajuda.
- O Elo: a liderança de cada rua responde em um toque (Tudo bem, Água subindo, Preciso de ajuda). A coordenação do bairro vê quem pediu ajuda e quais ruas ficaram em silêncio.
- O que falta é um sinal medido no lugar: quanto a água já subiu no ponto crítico. É aí que entra a régua.

## Parte 2. Como o circuito entra no projeto (0:40 a 1:40)

Tela: o diagrama no Wokwi, ainda parado. Depois o app, na linha da régua.

- A régua é um ESP32 com um sensor ultrassônico (HC-SR04) instalado acima do córrego, no ponto crítico do bairro.
- O sensor mede a distância até a água. O programa calcula o nível: altura do sensor (3,00 m) menos a distância medida.
- O limite da régua é 1,20 m, o mesmo que o app mostra. Na aba Plano, a rota de saída aparece sob o título "Quando a régua passar de 1,20 m": a régua é o sinal para seguir o plano da rua.
- No local, três LEDs e o LCD mostram a situação, e o buzzer avisa quem está perto. Isso funciona sem internet.
- A leitura sai no formato do contrato da API do Elo (`POST /leituras`), com o id da régua e o nível em metros. A API em C# do grupo já recebe essa leitura, e é dela que o app e o painel web leem a régua (`GET /regua`). O Elo Alertas, da matéria de SOA, tem a mesma rota.
- Dizer com clareza o estado atual: a API do grupo está publicada na nuvem, e a régua já manda cada leitura para ela por Wi-Fi, com uma chave de demonstração que vai ser trocada depois da entrega. Para mudar de servidor, mudam só o endereço e a chave no começo do programa. Se der, mostrar o número da régua chegando ao app ou ao painel.

## Parte 3. Demonstração no Wokwi (1:40 a 3:30)

Tela: o Wokwi com a simulação rodando e o Monitor Serial visível.

| Tempo | O que fazer | O que aparece | O que dizer |
| --- | --- | --- | --- |
| 1:40 | Iniciar a simulação. Sensor em 250 cm. | LCD "Nivel: 0,50 m" e "NORMAL". LED verde. Sem som. No Serial, "Wi-Fi conectado", a primeira leitura e "Envio OK: HTTP 204"; no canto do LCD, "*". | Córrego baixo: situação normal. A leitura sai no formato da API e vai pela rede: o "*" no LCD e o "Envio OK: HTTP 204" no Serial mostram que a API do grupo recebeu. |
| 2:05 | Sensor em 200 cm. | "Nivel: 1,00 m" e "ATENCAO". LED amarelo. Um bip curto a cada 1,5 s. Nova leitura no Serial. | A partir de 1,00 m a régua avisa que a água está subindo. O app conhece esse mesmo nível e mostra "em atenção" no ponto em que o LED amarelo acende. |
| 2:30 | Sensor em 181 cm e depois 180 cm. | Em 181 cm: "1,19 m", ainda atenção. Em 180 cm: "1,20 m" e "ACIMA DO LIMITE". LED vermelho. Bip rápido. | O alarme dispara exatamente no limite de 1,20 m, a mesma regra do app. |
| 2:55 | Sensor em 158 cm. Mostrar o app ao lado: o aviso da régua na aba Agora. | "Nivel: 1,42 m" na hora. No Serial, a leitura com `"nivelMetros":1.42` sai na hora e se repete a cada 10 s, cada uma seguida de "Envio OK: HTTP 204", mesmo com o buzzer tocando. | É o mesmo número do exemplo do app: "1,42 m, limite 1,20 m, acima do limite", com o aviso no alto da aba Agora. Agora ele vem da régua: a API devolve no `GET /regua` o que a régua acabou de mandar. Acima do limite a régua repete a leitura a cada 10 s, e o app busca a régua a cada 10 s. |
| 3:15 | Sensor de volta a 250 cm. | "0,50 m", "NORMAL", LED verde, silêncio. | A água baixou e o alarme para sozinho. No normal, o intervalo da leitura volta a 30 s (basta dizer; não dá tempo de mostrar). |

## Fecho (3:30 a 3:40)

- A régua avisa no local, mesmo sem internet, e dá ao Elo um número medido: é o sinal para seguir o plano da rua.
- Esse número já chega à API do grupo, publicada na nuvem, de onde o app e o painel leem, sem ninguém digitar. Depois da entrega, a chave de demonstração vai ser trocada.

## Perguntas prováveis

- **"O Elo não cria alertas próprios. E o buzzer?"** O app continua sem criar alerta: ele só mostra a leitura. Quem avisa é o aparelho no local, a partir de uma medida, como a proposta descreve ("a água avisa sozinha").
- **"O que é o nível de atenção?"** É o aviso a partir de 1,00 m, antes do limite. A régua acende o LED amarelo, e o app mostra "em atenção" no mesmo ponto.
- **"A leitura já chega ao app?"** Chega à API, publicada na nuvem: a régua envia por Wi-Fi, e a API devolve o número no `GET /regua`. O app e o painel mostram esse número quando apontam para esse endereço (`EXPO_PUBLIC_API_URL`, no app). Testado em 18/11/2026: 11 envios, todos com resposta 204.

## O vídeo gravado (19/11/2026)

O vídeo entregue tem 2 min 33 s e segue o roteiro acima em 13 falas. Ele foi gravado por um navegador automático, e a narração é a voz do Bruno clonada por IA, com a autorização dele (registro em [`prompts.md`](./prompts.md)). Duas diferenças em relação ao plano acima:

- O app aparece em prints, nas cartelas do começo, e não rodando ao lado da simulação.
- A prova de que a leitura chega ao sistema do grupo é a página pública da API, que aparece três vezes: antes da simulação, com 1,42 m e, no fim, com 0,50 m, sempre o mesmo número do display.

Falas, na ordem em que aparecem (todo integrante deve conseguir explicar cada uma):

1. **O problema.** Na chuva forte, quem cuida de uma rua que alaga não sabe quem está bem, quem precisa de ajuda e quem ficou em silêncio. O Elo nasceu para esse momento.
2. **O app do Elo.** No aplicativo do Elo, a liderança de cada rua responde em um toque: tudo bem, água subindo ou preciso de ajuda. A coordenação vê quem pediu ajuda e quais ruas não responderam.
3. **O que faltava.** Falta um sinal medido no lugar: quanto a água já subiu no ponto crítico. É aí que entra a régua, a parte física do Elo.
4. **O circuito.** A régua é um ESP32 com um sensor ultrassônico acima do córrego. O sensor mede a distância até a água, e o programa calcula o nível: três metros do sensor menos a distância medida.
5. **O aviso no local.** No local, três LEDs, um display e o buzzer avisam quem está perto, mesmo sem internet. O limite é um metro e vinte, o mesmo que o aplicativo usa na aba Plano para a rota de saída.
6. **O envio.** A cada leitura, o ESP32 monta a mensagem no formato da API do grupo, com o identificador da régua e o nível em metros, e envia por Wi-Fi. É dessa API que o aplicativo e o painel leem a régua.
7. **A API publicada.** A API do grupo já está publicada na nuvem. O programa aponta para ela, com uma chave de demonstração que vai ser trocada depois da entrega. Para mudar de servidor, mudam só o endereço e a chave.
8. **A simulação ligada.** Vamos ver funcionando. A duzentos e cinquenta centímetros da água, o nível é cinquenta centímetros: normal, LED verde, silêncio. No monitor serial, o Wi-Fi conecta e a primeira leitura chega à API: envio OK, HTTP duzentos e quatro. O asterisco no display confirma.
9. **Atenção.** Agora a água sobe. A duzentos centímetros, o nível chega a um metro: atenção. LED amarelo, um bip curto a cada um segundo e meio, e uma leitura sai na hora, porque a situação mudou. O aplicativo mostra "em atenção" nesse mesmo ponto.
10. **No limite.** A cento e oitenta e um centímetros, um metro e dezenove: ainda atenção. A cento e oitenta, um metro e vinte: acima do limite, LED vermelho e bip rápido. O alarme dispara exatamente no limite, como no aplicativo.
11. **Acima do limite.** A cento e cinquenta e oito centímetros, o nível é um metro e quarenta e dois, o número do exemplo do aplicativo. Acima do limite, a leitura sai a cada dez segundos, e cada envio continua chegando à API, mesmo com o buzzer tocando.
12. **A água baixa.** Quando a água baixa, o alarme para sozinho e o intervalo volta a trinta segundos.
13. **Fecho.** A régua avisa no local, mesmo sem internet, e dá ao Elo um número medido: o sinal para seguir o plano da rua. E esse número já chega à API do grupo, de onde o aplicativo e o painel leem.

## Conferência da entrega

A entrega é feita por um integrante só, pelo Teams, com três itens:

- [ ] Link público do projeto no Wokwi: https://wokwi.com/projects/479922926737544193 (aberto sem login em 15/11/2026; conferir de novo em janela anônima antes de enviar).
- [ ] Link do vídeo, com até 4 minutos, aberto para quem tem o link. O vídeo está gravado (2 min 33 s); falta subir e colar o link.
- [ ] Arquivo `integrantes.txt` com nome completo e RM de todos.

Prazo: 19/11/2026, 23h55.
