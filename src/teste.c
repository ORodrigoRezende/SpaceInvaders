#include "raylib.h"
#include <stdlib.h> // Para rand() e srand()
#include <time.h>   // Para time()

// Estrutura para representar uma estrela
typedef struct {
    Vector2 posicao;
    float velocidade;
} Estrela;

// Função para gerar estrelas aleatórias
void GerarEstrelas(Estrela estrelas[], int quantidade) {
    for (int i = 0; i < quantidade; i++) {
        estrelas[i].posicao = (Vector2){(float)GetRandomValue(0, GetScreenWidth()), (float)GetRandomValue(0, GetScreenHeight())};
        estrelas[i].velocidade = (float)GetRandomValue(1, 5);
    }
}

// Função para atualizar as estrelas
void AtualizarEstrelas(Estrela estrelas[], int quantidade) {
    for (int i = 0; i < quantidade; i++) {
        estrelas[i].posicao.y += estrelas[i].velocidade; // Move a estrela para baixo

        // Se a estrela sair da tela, reposiciona no topo
        if (estrelas[i].posicao.y > GetScreenHeight()) {
            estrelas[i].posicao.y = 0; // Reposiciona no topo
            estrelas[i].posicao.x = (float)GetRandomValue(0, GetScreenWidth()); // Nova posição x aleatória
            estrelas[i].velocidade = (float)GetRandomValue(1, 5); // Nova velocidade aleatória
        }
    }
}

// Função para desenhar as estrelas
void DesenharEstrelas(Estrela estrelas[], int quantidade) {
    for (int i = 0; i < quantidade; i++) {
        DrawPixelV(estrelas[i].posicao, WHITE); // Desenha a estrela como um pixel
    }
}

int main() {
    // Inicializa a janela
    InitWindow(800, 600, "Nave no Espaço com Estrelas");

    // Inicializa o gerador de números aleatórios
    srand(time(NULL));

    // Nave
    Texture2D nave = LoadTexture("assets/nave.png"); // Carrega a textura da nave
    Vector2 posicaoNave = {GetScreenWidth() / 2 - nave.width / 2, GetScreenHeight() - nave.height - 20}; // Posição inicial da nave
    float velocidadeNave = 5.0f;

    // Estrelas
    const int quantidadeEstrelas = 200; // Quantidade de estrelas
    Estrela estrelas[quantidadeEstrelas];
    GerarEstrelas(estrelas, quantidadeEstrelas); // Gera as estrelas iniciais

    // Loop principal
    SetTargetFPS(60); // Define o FPS máximo
    while (!WindowShouldClose()) {
        // Atualiza a posição da nave (movimento para esquerda e direita)
        if (IsKeyDown(KEY_LEFT) && posicaoNave.x > 0) {
            posicaoNave.x -= velocidadeNave;
        }
        if (IsKeyDown(KEY_RIGHT) && posicaoNave.x < GetScreenWidth() - nave.width) {
            posicaoNave.x += velocidadeNave;
        }

        // Atualiza as estrelas
        AtualizarEstrelas(estrelas, quantidadeEstrelas);

        // Desenha o frame
        BeginDrawing();
        ClearBackground(BLACK); // Fundo preto para simular o espaço

        // Desenha as estrelas
        DesenharEstrelas(estrelas, quantidadeEstrelas);

        // Desenha a nave
        DrawTextureV(nave, posicaoNave, WHITE);

        EndDrawing();
    }

    // Descarrega a textura da nave
    UnloadTexture(nave);

    // Fecha a janela
    CloseWindow();
    return 0;
}