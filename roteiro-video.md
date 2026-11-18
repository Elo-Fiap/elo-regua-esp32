# Roteiro do vídeo (até 4 minutos)

A tarefa pede três coisas no vídeo: apresentar a solução do grupo, explicar como o circuito se integra ao projeto e mostrar a simulação funcionando no Wokwi. O roteiro abaixo cobre as três e fecha em cerca de 3min40.

As falas são sugestões. Digam com as palavras de vocês.

## Antes de gravar

- Abrir o projeto no Wokwi pelo link público, em janela anônima, para conferir que o link abre sem login.
- Deixar o app do Elo aberto em modo de exemplo. Na aba Agora, o aviso "Exemplo: régua acima do limite" aparece no alto, com nível, limite, local, hora da medida e o atalho "Ver o plano da rua". Na aba Bairro, a linha da régua mostra "1,42 m, limite 1,20 m, acima do limite". Sem a API ligada, os dois são dados de exemplo, e o app diz isso na tela.
- Ao apertar o play, o Wokwi compila o programa antes de rodar. Nos testes isso levou de 10 a 40 segundos. Vale deixar a simulação já rodando antes de gravar a parte 3, ou cortar a espera na edição.
- No Wokwi, o sensor começa em 250 cm. Para mudar: clicar no HC-SR04 e mexer no controle "Distance". As setas do teclado mudam de 1 em 1 cm, o que ajuda a acertar o número.
- Testar o volume do buzzer antes de gravar. Ele fica em `"volume": "0.2"` no `diagram.json` e aceita de 0.01 a 1.0.
- A leitura no Serial sai na hora quando a situação muda (normal, atenção, acima do limite). Dentro da mesma situação, ela sai no intervalo: 30 s, ou 10 s acima do limite.
- Para acertar 200, 181 e 180 cm: arrastar até perto, parando em um número maior, e terminar com a seta para a esquerda. Passar abaixo de 181 cm dispara o alarme antes da hora.
- Na volta de 158 para 250 cm, o LCD pode mostrar "ATENCAO" por um instante, ao cruzar a faixa de 181 a 200 cm. É esperado.
- Os tempos (1,5 s, 10 s, 30 s) seguem o relógio da simulação. Se o indicador de velocidade do Wokwi estiver abaixo de 100%, tudo demora um pouco mais. Deixar a janela do Wokwi visível: escondida, o navegador deixa a simulação quase parada.
- O envio pela rede usa a rede "Wokwi-GUEST" do simulador e precisa de internet no computador. O Wi-Fi conecta em cerca de 2 s e cada envio leva cerca de 1 s; o "*" no canto do LCD e o "Envio OK: HTTP 200" no Serial mostram que deu certo.
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
- Dizer com clareza o estado atual: a régua já envia cada leitura por Wi-Fi, no formato da API. A API em C# já está publicada (`http://elo-api-fiap.azurewebsites.net`); se o programa ainda apontar para o serviço público de eco, que responde 200, muda só o endereço e a chave. Enquanto isso, o app mostra dado de exemplo. Se até a gravação a API estiver no ar, mostrar o número da régua chegando ao app.

## Parte 3. Demonstração no Wokwi (1:40 a 3:30)

Tela: o Wokwi com a simulação rodando e o Monitor Serial visível.

| Tempo | O que fazer | O que aparece | O que dizer |
| --- | --- | --- | --- |
| 1:40 | Iniciar a simulação. Sensor em 250 cm. | LCD "Nivel: 0,50 m" e "NORMAL". LED verde. Sem som. No Serial, "Wi-Fi conectado", a primeira leitura e "Envio OK: HTTP 200"; no canto do LCD, "*". | Córrego baixo: situação normal. A leitura sai no formato da API e já é enviada pela rede: o "*" no LCD e o "Envio OK" no Serial mostram que o servidor recebeu. |
| 2:05 | Sensor em 200 cm. | "Nivel: 1,00 m" e "ATENCAO". LED amarelo. Um bip curto a cada 1,5 s. Nova leitura no Serial. | A partir de 1,00 m a régua avisa que a água está subindo. O app conhece esse mesmo nível: com a API ligada, ele mostra "em atenção" no ponto em que o LED amarelo acende. |
| 2:30 | Sensor em 181 cm e depois 180 cm. | Em 181 cm: "1,19 m", ainda atenção. Em 180 cm: "1,20 m" e "ACIMA DO LIMITE". LED vermelho. Bip rápido. | O alarme dispara exatamente no limite de 1,20 m, a mesma regra do app. |
| 2:55 | Sensor em 158 cm. Mostrar o app ao lado: o aviso da régua na aba Agora. | "Nivel: 1,42 m" na hora. No Serial, a leitura com `"nivelMetros":1.42` sai na hora e se repete a cada 10 s, cada uma seguida de "Envio OK: HTTP 200", mesmo com o buzzer tocando. | É o mesmo número do exemplo fixo que o app mostra hoje: "1,42 m, limite 1,20 m, acima do limite", com o aviso no alto da aba Agora. Quando a régua estiver ligada à API, esse número virá dela. Acima do limite a régua repete a leitura a cada 10 s, e o app busca a régua a cada 10 s. |
| 3:15 | Sensor de volta a 250 cm. | "0,50 m", "NORMAL", LED verde, silêncio. | A água baixou e o alarme para sozinho. No normal, o intervalo da leitura volta a 30 s (basta dizer; não dá tempo de mostrar). |

## Fecho (3:30 a 3:40)

- A régua avisa no local, mesmo sem internet, e dá ao Elo um número medido: é o sinal para seguir o plano da rua.
- Próximo passo: apontar o envio para a API do grupo, em endereço público, para o número chegar ao app e ao painel sem ninguém digitar.

## Perguntas prováveis

- **"O Elo não cria alertas próprios. E o buzzer?"** O app continua sem criar alerta: ele só mostra a leitura. Quem avisa é o aparelho no local, a partir de uma medida, como a proposta descreve ("a água avisa sozinha").
- **"O que é o nível de atenção?"** É o aviso a partir de 1,00 m, antes do limite. A régua acende o LED amarelo, e o app, com a API ligada, mostra "em atenção" no mesmo ponto.
- **"A leitura já chega ao app?"** A régua já envia a leitura por Wi-Fi, no formato que a API recebe. A API já tem endereço público; trocando o endereço e a chave no programa, o app passa a mostrar o número da régua.

## Conferência da entrega

A entrega é feita por um integrante só, pelo Teams, com três itens:

- [ ] Link público do projeto no Wokwi: https://wokwi.com/projects/479922926737544193 (aberto sem login em 15/11/2026; conferir de novo em janela anônima antes de enviar).
- [ ] Link do vídeo, com até 4 minutos, aberto para quem tem o link.
- [ ] Arquivo `integrantes.txt` com nome completo e RM de todos.

Prazo: 19/11/2026, 23h55.
