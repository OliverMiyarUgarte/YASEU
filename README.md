# 🎮 YASEU — Yet Another Shoot'Em Up
Projeto da disciplina **Algoritmos e Estruturas de Dados** ministrada pelo **Professor Roberto Ferrari** (2025/2)
Desenvolvido em **C** e executado via **DOSBox**

---

## ✨ Sobre o jogo

**YASEU** (*Yet Another Shoot'Em Up*) é um jogo no estilo **shoot'em up**, criado como parte de um projeto acadêmico para colocar em prática diferentes conceitos de **estruturas de dados**

O jogo foi desenvolvido para rodar em **MS-DOS**, utilizando **DOSBox** e a biblioteca **Allegro 4.2.2** compatível com DOS.

---

## 🚀 Gameplay

Você controla uma nave espacial e precisa sobreviver enquanto enfrenta ondas de inimigos.  
O jogo inclui:

- Movimentação suave da nave do jogador  
- Tiros do jogador e dos inimigos  
- Sistema de colisões  
- Inimigos com cooldown de ataque  
- Sistema de vidas
- Invencibilidade temporária com piscamento do sprite  
- Limite de entidades simultâneas (balas/inimigos)  
- HUD simples com informações da partida  

---

## 🧠 Conceitos de Algoritmos e Estruturas de Dados Utilizados

O desenvolvimento fez uso de diversos tópicos estudados em aula, incluindo:

- Vetores como estruturas de armazenamento fixas para balas e inimigos  
- Flags de estado (`active`, `is_enemy_bullet`, etc.)  
- Estruturas (`filas`) para a organização das ordens das balas e (`árvores`) para o mapeamento das fases
- Lógica de simulação quadro-a-quadro  
- Controle de tempo e estados  
- Modularização com múltiplos arquivos `.c` e `.h`  
- Colisões usando distância euclidiana  
- Gerenciamento de ponteiros e memória  
- Organização e abstração de subsistemas (player, inimigos, balas)  

---

## 📁 Estrutura do Projeto

```text
/
├── src/
│   ├── main.c
│   ├── player.c / player.h
│   ├── enemy.c / enemy.h
│   ├── bullets.c / bullets.h
│   ├── utils.c / utils.h
│   └── graphics/
│       ├── sprites.bmp
│       └── ...
├── docs/
│   └── relatorio.pdf
├── Makefile
└── README.md
