#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>

#define LARGURA_JANELA 800
#define ALTURA_JANELA 800
#define STD_SIZE_X 32
#define STD_SIZE_Y 32
#define LARGURA_BALA 10
#define ALTURA_BALA 15

typedef struct Bala{
    Rectangle pos;
    Color color;
    int ativa;
    int tempo;
    int velocidade;
    Sound tiro;
}Bala;

typedef struct Nave{
    Rectangle pos;
    Color color;
    Bala bala;
    int velocidade;
    int direcao;
}Nave;

typedef struct Heroi{
    Rectangle pos;
    Color color;
    int velocidade;
    Bala bala;
}Heroi;

typedef struct Bordas{
    Rectangle pos;
} Bordas;

typedef struct Assets{
    Texture2D naveHeroi;
    Texture2D naveVerde;
    Sound tiro;
}Assets;

typedef struct Jogo{
    Nave nave;
    Heroi heroi;
    Bordas bordas[4];
    Assets assets;
    int alturaJanela;
    int larguraJanela;
    int tempoAnimacao;
}Jogo;

void IniciaJogo(Jogo *j);
void IniciaNaves(Jogo *j);
void AtualizaJogo(Jogo *j);
void DesenhaJogo(Jogo *j);
void DesenhaJogoPos(Jogo *j);
void AtualizaFrameDesenho(Jogo *j);
void DesenhaBalasHeroi(Jogo *j);
void AtualizaNavePos(Jogo *j);
void AtualizaHeroiPos(Jogo *j);
void DesenhaNaves(Jogo *j);
void DesenhaHeroi(Jogo *j);
void ColisaoBordas(Jogo *j);
void DesenhaBordas(Jogo *j);
int ColisaoBalas(Jogo *j);
int ColisaoBalasHeroi(Jogo *j);
void DesenhaBordas(Jogo *j);
void AtiraBalas(Jogo *j);
void AtiraBalasHeroi(Jogo *j);
void CarregaImagens(Jogo *j);
void DescarregaImagens(Jogo *j);
int Vencedor(Jogo *j);
int main(){
    InitAudioDevice();

    Jogo jogo;

    jogo.alturaJanela = ALTURA_JANELA;
    jogo.larguraJanela = LARGURA_JANELA;

    InitWindow(jogo.larguraJanela, jogo.alturaJanela, "Space Invaders");
    SetTargetFPS(60);
    IniciaJogo(&jogo);
    CarregaImagens(&jogo);
    Music musicaJogo = LoadMusicStream("assets/musica.mp3");
    PlayMusicStream(musicaJogo);

    while(!WindowShouldClose() && Vencedor(&jogo)){ 
      
        UpdateMusicStream(musicaJogo);
        AtualizaFrameDesenho(&jogo);
    }
    Vencedor(&jogo);
    while (!WindowShouldClose() ){
    DesenhaJogoPos(&jogo);
    Vencedor(&jogo);
    }
    UnloadMusicStream(musicaJogo);
    DescarregaImagens(&jogo);
    CloseWindow(); 
    return 0;
   
}

void IniciaJogo(Jogo *j){
    j->tempoAnimacao = GetTime();

    j->heroi.pos = (Rectangle) {LARGURA_JANELA/2 - STD_SIZE_X/2, ALTURA_JANELA - STD_SIZE_Y -10, STD_SIZE_X, STD_SIZE_Y};
    j->heroi.color = BLUE;
    j->heroi.velocidade = 3;
    j->heroi.bala.ativa = 0;
    j->heroi.bala.tempo = GetTime();
    j->heroi.bala.velocidade = 5;
    j->heroi.bala.tiro = LoadSound("assets/shoot.wav");


    j->nave.pos = (Rectangle) {0, 15, STD_SIZE_X, STD_SIZE_Y};
    j->nave.color = RED;
    j->nave.velocidade = 3;
    /*direcao = 1 faz nave mover para direita, direcao = 0 faz mover para esquerda*/
    j->nave.direcao = 1;
    j->nave.bala.ativa = 0;
    j->nave.bala.tempo = GetTime();
    j->nave.bala.velocidade = 5;
    j->nave.bala.tiro = LoadSound("assets/shoot.wav");

    //borda encima
    j->bordas[0].pos = (Rectangle){0, 0, LARGURA_JANELA, 10};
    //borda embaixo
    j->bordas[1].pos = (Rectangle){0, ALTURA_JANELA-10, LARGURA_JANELA, 10};
    //borda esquerda
    j->bordas[2].pos = (Rectangle){0, 0, 10, ALTURA_JANELA};
    //borda direita
    j->bordas[3].pos = (Rectangle){LARGURA_JANELA-10, 0, 10, ALTURA_JANELA};
    
}

void IniciaNaves(Jogo *j){

}

void AtualizaJogo(Jogo *j){
    AtualizaNavePos(j);
    AtualizaHeroiPos(j);
    AtiraBalas(j);
    AtiraBalasHeroi (j);
    Vencedor(j);
}
 
void DesenhaJogo(Jogo *j){
    BeginDrawing();
    ClearBackground(BLACK);
    DesenhaNaves(j);
    DesenhaHeroi(j);
    DesenhaBordas(j);
    EndDrawing();
}

void DesenhaJogoPos(Jogo *j){
    BeginDrawing();
    ClearBackground(BLACK);
    DesenhaBordas(j);
    EndDrawing();
}

void AtualizaFrameDesenho(Jogo *j){
    AtualizaJogo(j);
    DesenhaJogo(j);
}

void AtualizaNavePos(Jogo *j){
    ColisaoBordas(j);
    if(j->nave.direcao == 1){
        j->nave.pos.x += j->nave.velocidade;
    }else{
        j->nave.pos.x -= j->nave.velocidade;
    }
}

void AtualizaHeroiPos(Jogo *j){
    if(IsKeyDown(KEY_RIGHT) && CheckCollisionRecs(j->heroi.pos, j->bordas[3].pos)!=1){
        j->heroi.pos.x += j->heroi.velocidade;
    }if(IsKeyDown(KEY_LEFT) && CheckCollisionRecs(j->heroi.pos, j->bordas[2].pos)!=1){
        j->heroi.pos.x -= j->heroi.velocidade;
    }

}

void CarregaImagens(Jogo *j){
    j->assets.naveHeroi = LoadTexture("assets/naveHeroi.png");
    j->assets.naveVerde = LoadTexture("assets/GreenAnimation.png");
}

void DescarregaImagens(Jogo *j){
    UnloadTexture(j->assets.naveHeroi);
    UnloadTexture(j->assets.naveVerde);
}

void DesenhaNaves(Jogo *j){
    Vector2 tamanhoFrame = {32, 32};
    
    static Vector2 frame = {0, 0};
    static float tempoUltimaTroca = 0;
    
    if(GetTime() - tempoUltimaTroca >= 1){
        if(frame.x == 0){
            frame.x = 1;
        }else{
            frame.x = 0;
        }

        tempoUltimaTroca = GetTime();
    }
    Rectangle frameRecNave = {frame.x * tamanhoFrame.x, frame.y*tamanhoFrame.y,
     tamanhoFrame.x, tamanhoFrame.y};
    DrawTexturePro(j->assets.naveVerde, frameRecNave, (Rectangle){j->nave.pos.x, j->nave.pos.y, 32, 32},
    (Vector2){0, 0}, 0.0f, WHITE);
    
}

void DesenhaHeroi(Jogo *j){
     Vector2 tamanhoFrame = {32, 32};
    
    static Vector2 frame = {0, 0};
    frame.x = 1;
    frame.y = 1;

    Rectangle frameRecNave = {frame.x * tamanhoFrame.x, frame.y*tamanhoFrame.y,
     tamanhoFrame.x, tamanhoFrame.y};
    DrawTexturePro(j->assets.naveHeroi, frameRecNave, (Rectangle){j->heroi.pos.x, j->heroi.pos.y, 32, 32}, (Vector2){0, 0}, 0.0f, WHITE);
     //DrawRectangle(j->heroi.pos.x, j->heroi.pos.y, j->heroi.pos.width, j->heroi.pos.height, j->heroi.color); 

}

void DesenhaBordas(Jogo *j){
    for(int i = 0; i < 4; i++){
        DrawRectangleRec(j->bordas[i].pos, LIGHTGRAY);
    }
}

void DesenhaBalas(Jogo *j){
    DrawRectangleRec(j->nave.bala.pos, GREEN);
}

void DesenhaBalasHeroi(Jogo *j){
    DrawRectangleRec(j->heroi.bala.pos, BLUE);
}

void AtiraBalas(Jogo *j){
    if(j->nave.bala.ativa == 0 && GetTime()-j->nave.bala.tempo > 3){
        j->nave.bala.pos = (Rectangle){j->nave.pos.x+j->nave.pos.width/2, j->nave.pos.y+j->nave.pos.height/2, 
        LARGURA_BALA, ALTURA_BALA};
        j->nave.bala.ativa = 1;
        j->nave.bala.tempo = GetTime();
        PlaySound(j->nave.bala.tiro);
    }
    else if(ColisaoBalas(j)){
        j->nave.bala.ativa = 0;
    }
    if(j->nave.bala.ativa == 1){
        j->nave.bala.pos.y += j->nave.bala.velocidade;
        DesenhaBalas(j);
    }
}

void AtiraBalasHeroi(Jogo *j){
    if(j->heroi.bala.ativa == 0 && IsKeyPressed(KEY_SPACE)){
        j->heroi.bala.pos = (Rectangle){j->heroi.pos.x+j->heroi.pos.width/2 - LARGURA_BALA/2, j->heroi.pos.y,
        LARGURA_BALA, ALTURA_BALA};
        j->heroi.bala.ativa = 1;
        //j->heroi.bala.tempo = GetTime();
        PlaySound(j->heroi.bala.tiro);
    }
    else if(ColisaoBalasHeroi(j)){
        j->heroi.bala.ativa = 0;
    }
    if(j->heroi.bala.ativa == 1){
        j->heroi.bala.pos.y -= j->heroi.bala.velocidade;
        DesenhaBalasHeroi(j);
    }
}

void ColisaoBordas(Jogo *j){
    if(CheckCollisionRecs(j->nave.pos, j->bordas[2].pos)){
        j->nave.direcao = 1;
    }else if(CheckCollisionRecs(j->nave.pos, j->bordas[3].pos)){
        j->nave.direcao = 0;
    }
}

int ColisaoBalas(Jogo *j){
    // Colisao bala com heroi
    if(CheckCollisionRecs(j->heroi.pos, j->nave.bala.pos) && (j->nave.bala.ativa==1)){
        return 2;
    }
    // Colisao bala com borda de baixo
    if(CheckCollisionRecs(j->nave.bala.pos, j->bordas[1].pos)){
        return 1;
    }
    return 0;
}
int ColisaoBalasHeroi(Jogo *j){
    // Colisao bala com nave
    if(CheckCollisionRecs(j->nave.pos, j->heroi.bala.pos) && (j->heroi.bala.ativa==1)){
        return 2;
    }
    // Colisao bala com borda de cima
    if(CheckCollisionRecs(j->heroi.bala.pos, j->bordas[0].pos)){
        return 1;
    }
    return 0;
}
int Vencedor(Jogo *j){
    int textWidth = MeasureText("VOCÊ PERDEU!", 50);
    int textWidth2 = MeasureText("VOCÊ VENCEU!", 50);
    if(ColisaoBalas(j) == 2){
    ClearBackground(BLACK);
    DrawText("VOCÊ PERDEU!", (GetScreenWidth() - textWidth) / 2, GetScreenHeight() / 2 - 100, 50, RED);
    return 0;
    }
    if(ColisaoBalasHeroi(j) == 2){
    ClearBackground(BLACK);
    DrawText("VOCÊ VENCEU!", (GetScreenWidth() - textWidth2)/ 2, GetScreenHeight() / 2 - 100, 50, GREEN);
    return 0;
    }
    return 1;
}
