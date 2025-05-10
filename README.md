# 👾 Space Invaders – Projeto de Algoritmos

![Screenshot do Jogo](./screenshots/gameplay1.png)

## 🎮 Sobre o Projeto

Este é um projeto prático da disciplina de Algoritmos, com o objetivo de implementar uma versão simplificada do clássico **Space Invaders** utilizando a linguagem **C** e a biblioteca gráfica **Raylib**. O projeto foi desenvolvido a partir de um código base fornecido pelo professor Elverton Fazzion.

## 🧑‍💻 Desenvolvedores

- Rodrigo Rezende Silva  
- Antônio Rezende Carvalho Braga  

Universidade Federal de São João del-Rei  
São João del-Rei – 2025

## 🚀 Como Jogar

O jogador controla uma nave que deve destruir as naves inimigas sem ser atingido. O jogo avança por fases, aumentando a dificuldade conforme o progresso.

### 🎯 Pontuação

- 10 pontos por nave inimiga da primeira fileira.
- Cada fileira acima vale +10 pontos (20, 30, 40...).
- São **4 fileiras** com **7 naves cada**.
- A pontuação é acumulativa e pode ser vista:
  - Durante o jogo.
  - Na tela inicial (últimos 5 jogadores e suas pontuações).
  - Na tela final.

### 🎮 Controles

- **← / →**: mover a nave.
- **Espaço**: atirar.
- **Metoros**: bloqueiam tiros, mas são destruídos após 10 impactos.

## 🛠️ Compilação

Para compilar o jogo, você deve ter a [Raylib](https://www.raylib.com/index.html) instalada.

```bash
gcc spaceInvader.c -o jogo -lm -lraylib
```
Recomendado rodar no **Linux**.

## 🖼️ Imagens do Jogo

![HOME](./screenshots/gameplay2.png)  
![Gameplay](./screenshots/gameplay1.png)

## 📄 Licença

Este projeto é apenas para fins educacionais e acadêmicos.
