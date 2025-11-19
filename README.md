# YASEU — Yet Another Shoot'Em Up
Projeto da disciplina **Algoritmos e Estruturas de Dados** ministrada pelo Prof. (Dr.) **Roberto Ferrari Junior** (2025/2)

Desenvolvido em **C** e executado via **DOSBox**

---

## Sobre o jogo

**YASEU** (*Yet Another Shoot'Em Up*) é um jogo no estilo **space shooter**, criado como projeto acadêmico para aplicar conceitos fundamentais de **Algoritmos e Estruturas de Dados**, organização modular de código e desenvolvimento de jogos em C.

O jogo foi projetado para rodar em ambiente **MS-DOS**, utilizando **DOSBox** e a biblioteca **Allegro 4.2.2** compatível com DOS.

---

## Gameplay

Você controla uma nave espacial e enfrenta ondas de inimigos enquanto tenta sobreviver o máximo possível.

O jogo inclui:

- Movimentação da nave do jogador
- Sistema de tiros (três tipos de balas)
- Animações simples com sprites
- Colisões entre balas e entidades
- Inimigos com comportamento próprio
- Sistema de vidas (corações)
- Mapas desenhados dinamicamente
- Utilização de estruturas como **fila** e **árvore**
- HUD básico com informações da partida

---

## Controles

```text
w a s d   Movimentação
Espaço        Atirar
Esc           Sair do jogo
```
---

## Conceitos de Algoritmos e Estruturas de Dados Utilizados

Este projeto aplica diversos tópicos vistos na disciplina:

### **Estruturas de Dados**
- Vetores estáticos para entidades (balas, inimigos)
- **Fila** implementada manualmente (`fila.h`)
- **Árvore** binária para organização de dados (`tree.h`)
- Structs para modularizar entidades do jogo

### **Algoritmos**
- Lógica de colisão (distância e bounding box)
- Algoritmos para criação e desenho do mapa (`mapCreator.h`, `mapDraw.h`)
- Sistema de menu e fluxo do jogo
- Controle de tempo e estados com variáveis de cooldown

---

## Equipe
- Guilherme Aoki
- Lucas Okamoto Yoshimura
- Oliver Miyar Ugarte
- Nicolas Efraim Torres Albino
