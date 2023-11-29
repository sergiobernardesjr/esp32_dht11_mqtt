---
author:
- Empresa EXEMPLO
encoding: utf-8
target: html
title: DOCUMENTAÇÃO DO PROJETO
---

# Contextualização do Projeto

Projeto para verificação de temperatura no ambiente, buscando obter
informações de temperatura e umidade no local onde o dispositivos está.

## Objetivo do projeto

Coletar informações de temperatura e umidade do ambiente através de
comandas recebidos através do protocolo MQTT, possibilitando alterar
configuração do ar condicionado no ambiente, visando condicionar as
necessidades do ambiente.

## Visão geral

Através da coleta de temperatura e umidade do ambiente busca facilitar o
controle e manutenção em salas que necessitam estar numa temperatura
para o bom funcionamento.

## Justificativa

Para manter o bom funcionamento e manutenção de certos equipamentos e
materiais, como , assim como ter uma umidade mais controlada, como sala
para estoques de folhas ou sala de servidores.

# Requisitos

## Requisitos Funcionais

1.  Tópicos do servidor MQTT

    Para atender o recebimento de comandos o dispositivo deve estar
    monitorando o tópico "comandos" do servidor MQTT e deve ter um
    método irá tratar os comandos.

2.  Coletar temperatura

    Deve coletar temperatura do ambiente conforme comando, onde: R\|H
    (Read \| Temperature) e o retorno será: R\|H\|\<temperatura lida em
    Celsius>, respondendo no tópico "reading".

3.  Coletar umidade

    Deve coletar temperatura do ambiente conforme comando, onde: R\|H
    (Read \| Temperature) e o retorno será: R\|T\|\<umidade coletada>,
    respondendo no tópico "reading".

4.  Configuração do ar condicionado

    Deve ser possível configurar o ar condicionando, a temperatura,
    velocidade do ar (FAN) e modo das palhetas devem ser configuráveis
    através do comando C. Comando que deve ser recebido
    C\|\<temperatura>\|\<modo palhetas>(S - Swing/F -
    Fixo)\|\<fan>(velocidades do ar, sendo 1 menor, 2 médio e 3 máximo.)
    quando receber este comando deve enviar as instruções para o
    dispositivo do ar condicionado para configuração necessária após
    isso deve retornar C\|ON informando que foi executado o comando,
    respondendo no tópico "configure".

5.  Configuração geral

    Deve ser possível ligar ou desligar o ar condicionado através do
    comando G\|ON ou G\|OFF, sendo que o G\|ON ligará o ar e o G\|OFF
    desligará o ar, caso o ar esteja ligado e recebe o comando G\|ON,
    deve permanecer ligado, contudo ao receber o G\|OFF deve ser
    desligado o ar condicionado, respondendo no tópico "general".

6.  Comandos não aceitos

    Caso venha a receber algum comando que não tenha configurado ou
    comando que não está na estrutura esperada deve ser retornando no
    tópico "error" do servidor MQTT o texto "Comando inválido.".

## Requisitos Não Funcionais

1.  Conexão WIFI

    Deve ser possível efetuar conexão com a rede através da WIFI, não
    sendo obrigatório conexão por fio.

2.  Criar uma api para comando de leitura

    Criar serviço na API: GET -> /api/comando/leitura, este serviço
    receberá via queryParam o atributo tipo (TEMPERATURA/UMIDADE).
    Receberá um json como retorno, onde terá em seu corpo o atributo
    temperatura ou umidade conforme o tipo informado.

3.  Criar uma api para comando de configuração

    Criar serviço na API: POST -> /api/comando/configuracao, este
    serviço receberá via body: temperatura, modo (\[S\]wing ou \[F\]ixo)
    e velocidade (1 - baixa, 2 - média e 3 - alta). Deverá criar um
    comando no tópico "comandos" conforme comando de configuração.
    Receberá como retorno um json com atributo success onde será true ou
    false.

4.  Criar uma api para comando de geral

    Criar serviço na API: POST -> /api/comando/geral, este serviço
    receberá via body: ligar (ON/OFF). Deverá criar um comando no tópico
    "comandos" conforme comando geral, receberá como retorno um objeto
    contendo atributo success onde será true ou false.

5.  Automatizar configuração

    Ter um scheduler na api que faz a leitura da temperatura de forma
    automática e envie o comando de configuração conforme necessidade
    para atender o requisito de manter temperatura default informada
    pelo usuário.

## Casos de Uso

Caso 01: Usuário quer obter uma leitura da temperatura no ambiente:
Efetuar a chamada para um endpoint GET ->
/api/comando/leitura?tipo=temperatura e terá como retorno um objeto
contendo atributo temperatura e o valor lido: { temperatura: 20 }.

Caso 02: Usuário quer alterar a configuração do ar condicionado: Efetuar
a chamada para um endpoint POST -> /api/comando/configuracao com body {
"temperatura": 25, "modo": "S", "velocidade": 2 } e deverá ter como
retorno um objeto { success: true }.

Caso 03: Usuário quer desligar o ar condicionado: Efetuar a chamada para
um endpoint POST -> /api/comando/geral com body { "ligar": OFF } e
deverá ter como retorno um objeto { success: true }.

# Premissas e Restrições

## Premissas

-   Controlar ar condicionado de forma remota, através de comandos.
-   Obter temperatura de um ambiente de forma remota, através de
    comandos.
-   Ligar ou desligar o ar conforme necessidade de forma remota, atrvés
    de comandos.
-   Gerenciar de maneira inteligente a temperatura no ambiente de forma
    automática conforme valores coletados.

## Restrições

-   Precisar ter uma API para tornar mais amigável a interação entre
    usuário e sensor, contudo o desenvolvimento da API demora mais tempo
    que a lógica do sensor.


# Arquitetura do Sistema

Será necessário uma ESP32 que irá ter a montagem do sensor de
temperatura e ambiente, assim como ligação para controle do ar
condicionado, terá que ter um computador que terá o servidor Brokker
MQTT configurado bem como a API para conexão do mesmo.

## Diagrama de Arquitetura

![](./images/ESTRUTURA.png)

![](./images/SENSOR.png)

## Componentes do Sistema

Listar os componentes do sistema

1.  ESP32
2.  DHT11
3.  Conectores
4.  Chip WIFI
5.  Conectar RJ45


## Integrações Externas

Listar as integrações externas

1.  Integração com placa de ar condicionado.

# Descrição Funcional

# Desenvolvimento

## Tecnologias Utilizadas

-   espressif
-   nodejs
-   fastify
-   mqtt

## Metodologia de Desenvolvimento

## Diagrama de Classe

## Diagrama de Sequência

# Testes

## Casos de Teste

1.  Usuário quer obter uma leitura da temperatura no ambiente: Efetuar a
    chamada para um endpoint GET ->
    /api/comando/leitura?tipo=temperatura.

2.  Usuário quer alterar a configuração do ar condicionado: Efetuar a
    chamada para um endpoint POST -> /api/comando/configuracao com body
    { "temperatura": 25, "modo": "S", "velocidade": 2

}

1.  Usuário quer desligar o ar condicionado: Efetuar a chamada para um
    endpoint POST -> /api/comando/geral com body { "ligar": OFF }


## Resultados dos testes

1.  Deve ter como retorno um objeto contendo atributo temperatura e o
    valor lido: { temperatura: 20 }.
2.  Deverá ter como retorno um objeto { success: true }.
3.  Deverá ter como retorno um objeto { success: true }.

## Testes de integração

1.  A API deve enviar o comando R\|T para o tópico "comandos" broker
    MQTT de forma autmática a cada 2 minutos.
2.  O sensor deve monitorar o tópico "comandos" para tratar as mensagens
    recebidas.

## Testes de aceitação

1.  O usuário deve poder visualizar o valor da temperatura de maneira
    automática.
2.  O comando de configuração da temperatura deve estar 100% funcional.

# Gerenciamento de projeto

O gerenciamento do projeto foi realizado através do Github, onde foram
cadastradas as issues para controle do que deve ser desenvolvido, bem
como disponibilização da documentação, instruções para novos
desenvolvedores e códigos.

## Cronograma

\|\| Cronograma \|\|\|\|

|            Entrega            | Data de início | Data de conclusão |   Responsável    |
|:-----------------------------:|:--------------:|:-----------------:|:----------------:|
|    Desenvolvimento sensor     |   01/11/2023   |    10/11/2023     | Sergio Bernardes |
| Desenvolvimento servidor MQTT |   10/11/2023   |    11/11/2023     | Sergio Bernardes |
|      Desenvolvimento API      |   12/11/2023   |    15/11/2023     | Sergio Bernardes |
|   Integração dos ambientes    |   16/12/2023   |    17/12/2023     | Sergio Bernardes |

## Orçamento

\|\| Orçamento \|\|\|

| Elemento da despesa | Valor específico |  Valor geral |
|:-------------------:|:----------------:|-------------:|
|     Componentes     |    R$ 100,00     |    R$ 100,00 |
|        VM's         |     R$ 56,00     | R$ 56,00/mês |

## Riscos e Mitigações

## Equipe

\|\| Equipe \|\|\|

|       Nome       |         Função         |
|:----------------:|:----------------------:|
| Sergio Bernardes | Desenvolvedor e Tester |

# Conclusão e Entrega

## Resumo do Projeto

Projeto buscar trazer automação no controle de temperatura de salas.

## Entrega

Descreva como foi a entrega do projeto em produção

# Anexos

[Anexo1](http://google.com.br)

[Anexo2](http://google.com.br)
