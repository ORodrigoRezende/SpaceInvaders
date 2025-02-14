#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>

#define LARGURA_JANELA 800
#define ALTURA_JANELA 600
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
    int vida;
    
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
    int status;
    char player[20];
    int placar[5];
    bool playerEmEdicao;
}Jogo;

void IniciaJogo(Jogo *j);
void IniciaNaves(Jogo *j);
void AtualizaJogo(Jogo *j);
void DesenhaJogo(Jogo *j);
void DesenhaPlacar(Jogo *j);
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
void Vencedor(Jogo *j);
void DrawHome(Jogo *j);

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

    while(!WindowShouldClose()){ //status 0 Home ;status 1 Jogo ; status 2 Pós jogo
        if(jogo.status==1){
            UpdateMusicStream(musicaJogo);
            AtualizaFrameDesenho(&jogo);
        }else if(jogo.status==2){
            DesenhaJogoPos(&jogo);
            
        }else{
            if (IsKeyPressed(KEY_ENTER)){
                jogo.status = 1;
            }
            DrawHome(&jogo);
        }
    }
    
    UnloadMusicStream(musicaJogo);
    DescarregaImagens(&jogo);
    CloseWindow(); 
    return 0;
   
}

void IniciaJogo(Jogo *j){
    j->tempoAnimacao = GetTime();
    j->status = 0;

    j->heroi.pos = (Rectangle) {LARGURA_JANELA/2 - STD_SIZE_X/2, ALTURA_JANELA - STD_SIZE_Y -10, STD_SIZE_X, STD_SIZE_Y};
    j->heroi.color = BLUE;
    j->heroi.velocidade = 3;
    j->heroi.bala.ativa = 0;
    j->heroi.bala.tempo = GetTime();
    j->heroi.bala.velocidade = 5;
    j->heroi.bala.tiro = LoadSound("assets/shoot.wav");
    j->heroi.vida = 3;


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

void DrawHome(Jogo *j){ //Draw the game's home page
    BeginDrawing();
    ClearBackground(BLACK);
    DrawText("Placar dos 5 últimos jogos:", 10, 150, 20, WHITE);
    DesenhaPlacar(j);
    DrawText("Space Invaders", 300, 100, 40, WHITE);
    char buffer[50] = ""; // Buffer para armazenar o player
    // Solicitar player
        if (j->playerEmEdicao) {
        DrawText("Digite seu player:", 10, 250, 20, WHITE);
        DrawText(buffer, 10, 300, 20, WHITE);
        if (IsKeyPressed(KEY_ENTER)) {
            strncpy(j->player, buffer, sizeof(j->player) - 1);
            j->playerEmEdicao = 0; // Termina a edição do player
            
        }
        if (IsKeyPressed(KEY_BACKSPACE) && strlen(buffer) > 0) {
            buffer[strlen(buffer) - 1] = '\0'; // Remove o último caractere
        } else {
            for (int i = 0; i < 26; i++) {
                if (IsKeyPressed(KEY_A + i)) {
                    char ch = 'A' + i;
                    int len = strlen(buffer);
                    if (len < 49) {
                        buffer[len] = ch;
                        buffer[len + 1] = '\0';
                    }
                }
            }
        }
    } else {
        DrawText("aperte ENTER para começar", 250, 350, 20, WHITE);
        if (IsKeyPressed(KEY_ENTER)) {
            j->playerEmEdicao = true; // Começa a edição do player
        }
    }

    DesenhaPlacar(j);
    EndDrawing();
}

void DesenhaPlacar(Jogo *j){
    for (int i = 0; i < 5; i++) {
    char pontos[20];
    sprintf(pontos, "Jogo %d: %d", i + 1, j->placar[i]);
    DrawText(pontos, 10, 50 + i * 30, 20, WHITE);
    }
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
    //Home button
    Rectangle button = {300, 250, 200, 60};
    Vector2 mouse = GetMousePosition();
    bool hover = CheckCollisionPointRec(mouse, button);
    DrawRectangleRec(button, hover ? GRAY : DARKGRAY);
    DrawText("Home", button.x + 40, button.y + 20, 20, WHITE);
    if (hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        j->status = 0;
        j->heroi.vida = 3;
    }else{
        Vencedor(j);
    }

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
        j->heroi.vida--; //Diminuição da vida Heroi
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
void Vencedor(Jogo *j){
    int textWidth = MeasureText("VOCÊ PERDEU!", 50);
    int textWidth2 = MeasureText("VOCÊ VENCEU!", 50);
    if(j->heroi.vida == 0){
    ClearBackground(BLACK);
    DrawText("VOCÊ PERDEU!", (GetScreenWidth() - textWidth) / 2, GetScreenHeight() / 2 - 100, 50, RED);
    j->status = 2;
    }
    if(ColisaoBalasHeroi(j) == 2){
    ClearBackground(BLACK);
    DrawText("VOCÊ VENCEU!", (GetScreenWidth() - textWidth2)/ 2, GetScreenHeight() / 2 - 100, 50, GREEN);
    j->status = 2;
    }
}