# wifibalancer

Este projeto é um protótipo de um balanceador de carga de clientes conectados em APs wifi.

A ideia é prover uma forma para a rede gerenciar clientes wireless de forma transparente para os clientes, para balancear os recursos dos APs.

## Modo de uso

O sistema consiste em 2 módulos: um que roda no computador controlador (controller.sh) e outro que roda nas estações (station.sh).

### Para usar, voce vai precisar:

1- Ter pelo menos 2 APs;

2- Ter pelo menos 2 computadores, um para simular o controlador, e outro pra simular uma estação;

3- Um cabo ethernet;

### Procedimento:

1- Conecte um cabo entre as portas LAN dos 2 APs;

2- Desative o serviço DHCP de um dos APs e certifique-se que ambos estão com a mesma configuração de rede e máscara;

3- Conecte o computador controlador e o computador estação a um dos APs e anote o IP recebido no computador controlador;

4- No computador controlador, inicie o processo "controller.sh";

5- No computador estação, inicie o processo "station.sh", passando o IP do controlador como parametro. Esse processo vai precisar da senha de sudo pois ele gerencia a conectividade wireless da maquina.

6- No controlador, siga as instruções na tela do programa para comandar a estação para que se conecte a outros APs.

## Funcionamento

### Sobre o módulo Controller

O módulo controller é um shell script que inicia 2 aplicações:

- controller-manager: é a principal aplicação cuja função é mostrar o status atual dos APs e as estações conectadas a cada AP e oferecer uma interface para o usuário controlar para qual AP deseja que uma estação se conecte. Quando o usuário faz uma escolha, ele envia um comando de mudança de AP para a aplicação station-manager, que roda na estação.
- controller-database: essa aplicação mantém uma porta UDP aberta que espera receber informações das estações sobre quais APs elas estão conectadas. Quando recebe uma informação de uma estação, atualiza os arquivos que são lidos pela aplicação controller-manager.

### Sobre o módulo Station

O módulo station é um shell script que inicia 2 aplicações:

- station-manager: essa aplicação recebe o comando da aplicação controller-manager para ordenar a estação a mudar de AP. A aplicação então envia esse comando ao Network Manager, que se encarrega da troca de APs. Se o Network Manager não conseguir se conectar em outro AP, ele automaticamente volta a se conectar no AP anterior.
- station-notifier: a única função dessa aplicação é enviar uma mensagem ao controller-database a cada 20 segundos com o AP onde a estação está conectada.
