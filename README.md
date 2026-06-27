# Computação Gráfica e Visualização I (INF01047) - INF/UFRGS

Este repositório contém o código base para o trabalho final. O enunciado completo do trabalho final está no Moodle:

https://moodle.ufrgs.br/mod/assign/view.php?id=6018620

## Relatório de Entrega

A aplicação desenvolvida é um jogo inspirado em _Star Wars: Starfighter (2001)_. 
Nela, o jogador controla uma nave espacial encarregada de exterminar as naves oponentes em um campo de asteróides.
Para isso, o jogador dispõe de um canhão _laser_ na ponta de cada uma de suas asas.
Entretanto, também seus oponentes estão munidos de armamento semelhante, e o jogador deve evitar ser atingido por eles.

Caetano: implementou colisões, movimentação dos asteróides e do player e os menus do jogo.

Luiz Augusto: inicialmente refatorou o código para se assemelhar a bibliotecas de mais alto nível.
Também foi responsável por implementar a iluminação do jogo, inserir os modelos de nave, oponentes e asteróides,
implementar a lógica de movimentação dos oponentes, a câmera de primeira pessoa.

Uso de IAs: durante toda a extensão do projeto, ambos participantes utilizaram ferramentas de IA para desenvolvimento.
Em concreto, foram utilizados o `gemini-cli`, o `antigravity-cli` (`agy`) e o `codex`, que são programas de IAs agênticas.
Entretanto, não utilizamos em nenhum ponto o chamado `vibe-coding`, pois sempre utilizamos as IAs para tarefas pequenas, supervisionando cada edição ou script rodado.
Além disso, a dupla realizou análises semanais ponta-a-ponta do código a fim de garantir total compreensão da codebase.