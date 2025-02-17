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
#define NUM_LINHA 3
#define NUM_NAVES_LINHA 7
#define OFFSET_X 100
#define MAX_TAM 100
#define MAX_PLACARES 5

typedef struct Bala{
    Rectangle pos;
    Color color;
    int ativa;
    int tempo;
    int velocidade;
    Sound tiro;
}Bala;

typedef struct Placar{
    char player[50];
    int pontuacao;
}Placar;

typedef struct Nave{
    Rectangle pos;
    Color color;
    Bala bala;
    int velocidade;
    int status;
}Nave;

typedef struct Barreiras {
    Rectangle pos;
    Color color;
    int vida;
}Barreiras;

typedef struct{
    Nave naves[NUM_NAVES_LINHA];
    int status;
    int direcao;
}LinhaNave;

typedef struct Coracao{
    Rectangle pos;
    Color color;
}CoracaoVida;

typedef struct Heroi{
    Rectangle pos;
    Color color;
    int velocidade;
    Bala bala;
    int vida;
}Heroi;

typedef struct Bordas{
    Rectangle pos;
}Bordas;

typedef struct Assets{
    Texture2D naveHeroi;
    Texture2D naveVerde;
    Texture2D barreira;
    Texture2D coracao;
    Sound tiro;
}Assets;

typedef struct Jogo{
    LinhaNave linha[NUM_LINHA];
    Heroi heroi;
    Bordas bordas[4];
    Assets assets;
    Barreiras barreiras[5]; 
    CoracaoVida coracao[3];
    Placar placar[5];
    int alturaJanela;
    int larguraJanela;
    int tempoAnimacao;
    int status;
    char player[20];
    int pontos;
    bool playerEmEdicao;
}Jogo;

void IniciaJogo(Jogo *j);
void IniciaNaves(Jogo *j);
void AtualizaJogo(Jogo *j);
void DesenhaJogo(Jogo *j);
void DesenhaPlacar(Jogo *j);
void DesenhaBarreiras(Jogo *j);
void DesenhaJogoPos(Jogo *j);
void AtualizaFrameDesenho(Jogo *j);
void DesenhaBalasHeroi(Jogo *j);
void AtualizaNavePos(Jogo *j);
void AtualizaHeroiPos(Jogo *j);
void DesenhaNaves(Jogo *j);
void DesenhaHeroi(Jogo *j);
bool ColisaoBordas(Jogo *j);
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
void DesenhaBalas(Jogo *j);
void IniciaBarreira(Jogo *j);
void IniciaHeroi(Jogo *j);
void IniciaGameplay(Jogo *j);
void IniciaCoracao(Jogo *j);
void DesenhaVidas(Jogo *j);
void AtualizaStatusJogo(Jogo *j);
void Pontuacao(Jogo *j,int linha);
void LerPlacar(Jogo *j);
void AtualizaPlacar(Jogo *j);

int main(){
    InitAudioDevice();

    Jogo jogo;

    jogo.alturaJanela = ALTURA_JANELA;
    jogo.larguraJanela = LARGURA_JANELA;

    InitWindow(jogo.larguraJanela, jogo.alturaJanela, "Space Invaders");
    SetTargetFPS(60);
    jogo.status = 0;
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
            DrawHome(&jogo);
            if (IsKeyPressed(KEY_ENTER)){
                jogo.status = 1;
                AtualizaStatusJogo(&jogo);
            }
        }
    }
    
    UnloadMusicStream(musicaJogo);
    DescarregaImagens(&jogo);
    CloseWindow(); 
    return 0;
   
}

void AtualizaStatusJogo(Jogo *j) { //Reinicia o Jogo
    if (j->status == 1) {
        AtualizaPlacar(j);
        IniciaJogo(j);  
    }
}

void AtualizaPlacar(Jogo *j){
    Placar placares[MAX_PLACARES + 1]; // Criamos espaço extra para a nova pontuação
    int count = 0;

    // Lê os placares do arquivo
    FILE *file = fopen("historico.txt", "r");
    if (file != NULL) {
        while (count < MAX_PLACARES && fscanf(file, "%19s %d", placares[count].player, &placares[count].pontuacao) == 2) {
            count++;
        }
        fclose(file);
    }

    // Adiciona o novo placar ao final da lista
    if (count < MAX_PLACARES) {
        strcpy(placares[count].player, j->player);
        placares[count].pontuacao = j->pontos;
        count++;
    } else {
        // Se já houver 5 registros, remove o mais antigo (o primeiro) e adiciona no final
        for (int i = 1; i < MAX_PLACARES; i++) {
            placares[i - 1] = placares[i];
        }
        strcpy(placares[MAX_PLACARES - 1].player, j->player);
        placares[MAX_PLACARES - 1].pontuacao = j->pontos;
    }

    // Salva os placares no arquivo
    file = fopen("historico.txt", "w");
    if (file != NULL) {
        for (int i = 0; i < count; i++) {
            fprintf(file, "%s %d\n", placares[i].player, placares[i].pontuacao);
        }
        fclose(file);
    }
}





void IniciaJogo(Jogo *j){
    j->tempoAnimacao = GetTime();
    j->pontos = 0;
    
    IniciaGameplay(j);
    j->bordas[0].pos = (Rectangle){0, 0, LARGURA_JANELA, 10}; //borda encima
    j->bordas[1].pos = (Rectangle){0, ALTURA_JANELA-10, LARGURA_JANELA, 10}; //borda embaixo
    j->bordas[2].pos = (Rectangle){0, 0, 10, ALTURA_JANELA}; //borda esquerda
    j->bordas[3].pos = (Rectangle){LARGURA_JANELA-10, 0, 10, ALTURA_JANELA}; //borda direita
    
}

void IniciaGameplay(Jogo *j){
    IniciaNaves(j);
    IniciaBarreira(j);
    IniciaHeroi(j);
    IniciaCoracao(j);
}

void IniciaNaves(Jogo *j){
    for (int i = 0; i < NUM_LINHA; i++) { //Cria NAVES
        for (int k = 0; k < NUM_NAVES_LINHA; k++){
            j->linha[i].naves[k].pos = (Rectangle) {OFFSET_X + k * 50, i*50 + 15, STD_SIZE_X, STD_SIZE_Y}; // Espaçadas horizontalmente
            j->linha[i].naves[k].color = RED;
            j->linha[i].naves[k].bala.ativa = 0;
            j->linha[i].naves[k].bala.tempo = GetTime();
            j->linha[i].naves[k].bala.velocidade = 5;
            j->linha[i].naves[k].bala.tiro = LoadSound("assets/shoot.wav");
            j->linha[i].naves[k].status = 1;
            j->linha[i].naves[k].velocidade = 1;
        }
        j->linha[i].direcao = 1;
        j->linha[i].status = 1;
    }
}

void IniciaCoracao(Jogo *j){
    for (int i = 0; i < j->heroi.vida; i++) {
        j->coracao[i].pos = (Rectangle) {10 + (i * 40), 550, 20, 20};
        j->coracao[i].color = WHITE;
    }
}

void IniciaHeroi(Jogo *j){
    j->heroi.pos = (Rectangle) {LARGURA_JANELA/2 - STD_SIZE_X/2, ALTURA_JANELA - STD_SIZE_Y -10, STD_SIZE_X, STD_SIZE_Y};
    j->heroi.color = BLUE;
    j->heroi.velocidade = 3;
    j->heroi.bala.ativa = 0;
    j->heroi.bala.tempo = GetTime();
    j->heroi.bala.velocidade = 5;
    j->heroi.bala.tiro = LoadSound("assets/shoot.wav");
    j->heroi.vida = 3;
}

void IniciaBarreira(Jogo *j){
    for (int i = 0; i < 5; i++) {
        j->barreiras[i].pos = (Rectangle){80 + (i * 150), 500, 25, 25};
        j->barreiras[i].color = GRAY;
        j->barreiras[i].vida = 5;
    }
}

void DrawHome(Jogo *j){ //Draw the game's home page
    BeginDrawing();
    ClearBackground(BLACK);
    DrawText("Placar dos 5 últimos jogos:", 250, 375, 20, WHITE);
    LerPlacar(j);
    DesenhaPlacar(j);
    DrawText("Space Invaders", 250, 100, 40, WHITE);
    DrawText("Digite seu nome:", 250, 200, 20, WHITE);
    DrawText(j->player, 250, 250, 20, WHITE); // Exibe o nome digitado

    // Captura entrada de texto
    int key = GetCharPressed();
    while (key > 0) {
        int len = strlen(j->player);
        if (len < sizeof(j->player) - 1 && key >= 32 && key <= 125) { 
            j->player[len] = (char)key;
            j->player[len + 1] = '\0';
        }
        key = GetCharPressed(); // Pega caracteres subsequentes
    }

    // Remover último caractere com Backspace
    if (IsKeyPressed(KEY_BACKSPACE) && strlen(j->player) > 0) {
        j->player[strlen(j->player) - 1] = '\0';
    }

    // Pressionar Enter para confirmar o nome
    if (IsKeyPressed(KEY_ENTER) && strlen(j->player) > 0) {
        j->playerEmEdicao = false;
        j->status = 1; // Inicia o jogo
    }

    DesenhaPlacar(j);
    EndDrawing();
}

void LerPlacar(Jogo *j){
    char buf[MAX_TAM];
    char nome[50];
    int pontuacao;
    int i=0;
    FILE *arq;
    arq = fopen("historico.txt", "r");
    if(arq == NULL) return;

    while (i < 5 && fgets(buf, MAX_TAM, arq) != NULL) {
        char *token = strtok(buf, ",");
        if (token != NULL) {
            strcpy(nome, token);
            token = strtok(NULL, ",");
            if (token != NULL) {
                pontuacao = atoi(token);  // Usando atoi para converter para int
                strcpy(j->placar[i].player, nome);
                j->placar[i].pontuacao = pontuacao;
                i++;
            }
        }
    }
    
    fclose(arq);
}

void DesenhaPlacar(Jogo *j){
    for (int i = 0; i < 5; i++) {
    char pontos[60];
    sprintf(pontos, "%d %s : %d", i + 1, j->placar[i].player,j->placar[i].pontuacao);
    DrawText(pontos, 250, 425 + i * 30, 20, WHITE);
    }
}

void DesenhaBarreiras(Jogo *j) {
    for (int i = 0; i < 5; i++) {
        if (j->barreiras[i].vida > 0) {
            DrawTexture(j->assets.barreira, 80 + (i*150), 500 , WHITE);
        }
    }
}

void DesenhaVidas(Jogo *j) {
    for (int i = 0; i < j->heroi.vida; i++) {
        DrawTexture(j->assets.coracao, 10 + (i * 40), 550, WHITE);
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
    DesenhaBarreiras(j);
    DesenhaBalas(j);
    DesenhaVidas(j);
    EndDrawing();
}

void DesenhaJogoPos(Jogo *j){
    BeginDrawing();
    ClearBackground(BLACK);
    DesenhaBordas(j);

    char textoPontos[50];
    sprintf(textoPontos, "Pontuação Final: %d", j->pontos);
    DrawText(textoPontos, 250, 325, 30, WHITE);

    //Home button
    Rectangle button = {300, 400, 200, 60};
    Vector2 mouse = GetMousePosition();
    bool hover = CheckCollisionPointRec(mouse, button);
    DrawRectangleRec(button, hover ? DARKGRAY:BLACK);
    DrawRectangleLinesEx(button, 3, WHITE);
    DrawText("Home", button.x + 70, button.y + 20, 20, WHITE);
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

    bool bateuBorda = ColisaoBordas(j);

    if (bateuBorda) {
        for (int i = 0; i < NUM_LINHA; i++) {
            j->linha[i].direcao *= -1;  // Inverte a direção de TODAS as linhas
            for (int k = 0; k < NUM_NAVES_LINHA; k++) {
                j->linha[i].naves[k].pos.y += 32;  // Move TODAS as naves para baixo
            }
        }
    }

    // Movimenta todas as naves na direção correta
    for (int i = 0; i < NUM_LINHA; i++) {
        for (int k = 0; k < NUM_NAVES_LINHA; k++) {
            if (j->linha[i].naves[k].status) {  // Somente move as naves ativas
                j->linha[i].naves[k].pos.x += j->linha[i].naves[k].velocidade * j->linha[i].direcao;
            }
        }
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
    j->assets.barreira = LoadTexture("assets/barreira.png");
    j->assets.coracao = LoadTexture("assets/heart.png");
}

void DescarregaImagens(Jogo *j){
    UnloadTexture(j->assets.naveHeroi);
    UnloadTexture(j->assets.naveVerde);
    UnloadTexture(j->assets.barreira);
    UnloadTexture(j->assets.coracao);
}

void DesenhaNaves(Jogo *j){
    for (int i = 0; i < NUM_LINHA; i++){ 
        for (int k = 0; k < NUM_NAVES_LINHA; k++){
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
            if(j->linha[i].naves[k].status==1){
                DrawTexturePro(j->assets.naveVerde, frameRecNave, (Rectangle){j->linha[i].naves[k].pos.x, j->linha[i].naves[k].pos.y, 32, 32},
                (Vector2){0, 0}, 0.0f, WHITE);
            }
        }

     }
}

void DesenhaHeroi(Jogo *j){
     Vector2 tamanhoFrame = {32, 32};
    
    static Vector2 frame = {0, 0};
    frame.x = 1;
    frame.y = 1;

    Rectangle frameRecNave = {frame.x * tamanhoFrame.x, frame.y*tamanhoFrame.y,
     tamanhoFrame.x, tamanhoFrame.y};
    DrawTexturePro(j->assets.naveHeroi, frameRecNave, (Rectangle){j->heroi.pos.x, j->heroi.pos.y, 32, 32}, (Vector2){0, 0}, 0.0f, WHITE);
}

void DesenhaBordas(Jogo *j){
    for(int i = 0; i < 4; i++){
        DrawRectangleRec(j->bordas[i].pos, LIGHTGRAY);
    }
}

void DesenhaBalas(Jogo *j){
    for (int i = 0; i < NUM_LINHA; i++){
        for (int k = 0; k < NUM_NAVES_LINHA; k++){
                if(j->linha[i].naves[k].bala.ativa==1){
                    DrawRectangleRec(j->linha[i].naves[k].bala.pos, GREEN);
                }
        }
    }
}

void DesenhaBalasHeroi(Jogo *j){
    DrawRectangleRec(j->heroi.bala.pos, BLUE);
}

void AtiraBalas(Jogo *j){
    for (int i = 0; i < NUM_LINHA; i++){
        for (int k = 0; k < NUM_NAVES_LINHA; k++){
            if(j->linha[i].naves[k].status==1){
                if(j->linha[i].naves[k].bala.ativa == 0 && GetTime()-j->linha[i].naves[k].bala.tempo > 3){
                    if (rand() % 100 < 1){
                    j->linha[i].naves[k].bala.pos = (Rectangle){j->linha[i].naves[k].pos.x+j->linha[i].naves[k].pos.width/2, j->linha[i].naves[k].pos.y+j->linha[i].naves[k].pos.height/2, 
                    LARGURA_BALA, ALTURA_BALA};
                    j->linha[i].naves[k].bala.ativa = 1;
                    j->linha[i].naves[k].bala.tempo = GetTime();
                    PlaySound(j->linha[i].naves[k].bala.tiro);
                    }
                }
                if(j->linha[i].naves[k].bala.ativa){
                    if(ColisaoBalas(j)){
                        j->linha[i].naves[k].bala.ativa = 0;
                    }
                    else{
                        j->linha[i].naves[k].bala.pos.y += j->linha[i].naves[k].bala.velocidade;
                        
                    }
                }
            }else{
                j->linha[i].naves[k].bala.ativa = 0;
            }
        }
        
        
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

bool ColisaoBordas(Jogo *j){
    for (int i = 0; i < NUM_LINHA; i++){
        for (int k = 0; k < NUM_NAVES_LINHA; k++){
            if (j->linha[i].naves[k].status) {  // Verifica apenas naves ativas
                if (CheckCollisionRecs(j->linha[i].naves[k].pos, j->bordas[2].pos) || 
                    CheckCollisionRecs(j->linha[i].naves[k].pos, j->bordas[3].pos)) {
                    return true;  // Se qualquer nave colidiu, retorna verdadeiro
                }
            }
        }
    }
    return false;
}

int ColisaoBalas(Jogo *j) {
    int colidiu = 0;

    for (int i = 0; i < NUM_LINHA; i++) {
        for (int k = 0; k < NUM_NAVES_LINHA; k++) {
            if(j->linha[i].naves[k].status==1){
                if (j->linha[i].naves[k].bala.ativa) {

                    // Verifica colisão da bala da nave com as barreiras
                    for (int b = 0; b < 5; b++) {
                        if (j->barreiras[b].vida > 0 && CheckCollisionRecs(j->linha[i].naves[k].bala.pos, j->barreiras[b].pos)) {
                            j->barreiras[b].vida--; 
                            j->linha[i].naves[k].bala.ativa = 0; 
                            colidiu = 1; 
                            break;
                        }
                    }

                    if (j->linha[i].naves[k].bala.ativa && j->heroi.bala.ativa && (CheckCollisionRecs(j->heroi.bala.pos, j->linha[i].naves[k].bala.pos) || CheckCollisionRecs(j->heroi.bala.pos, j->linha[i].naves[k].bala.pos))) { //Colisao Bala com bala
                        j->heroi.bala.ativa =0;
                        j->linha[i].naves[k].bala.ativa = 0; 
                        colidiu = 1;
                    }

                    // Verifica colisão da bala da nave com o herói
                    if (j->linha[i].naves[k].bala.ativa && CheckCollisionRecs(j->heroi.pos, j->linha[i].naves[k].bala.pos)) {
                        j->heroi.vida--; 
                        j->linha[i].naves[k].bala.ativa = 0; 
                        colidiu = 1;
                    }
                }
            }
        }
    }
    for (int i = 0; i < NUM_LINHA; i++) {
        for (int k = 0; k < NUM_NAVES_LINHA; k++) {
            if(j->linha[i].naves[k].status){
                if (j->linha[i].naves[k].bala.ativa) {
                    if (CheckCollisionRecs(j->linha[i].naves[k].bala.pos, j->bordas[1].pos)) { 
                        j->linha[i].naves[k].bala.ativa = 0;
                    }
                }
            }
        }
    }

    return colidiu;
}




int ColisaoBalasHeroi(Jogo *j){
    int colisão = 0;

    // Verifica colisão da bala do herói com as barreiras
    for (int i = 0; i < 5; i++) {
        if (j->barreiras[i].vida > 0 && CheckCollisionRecs(j->heroi.bala.pos, j->barreiras[i].pos)) {
            j->barreiras[i].vida--; 
            j->heroi.bala.ativa = 0; // Desativa a bala
            colisão = 1;
        }
    }
    if(CheckCollisionRecs(j->heroi.bala.pos, j->bordas[0].pos)){
        colisão = 1;
    }
    // Verifica colisão da bala do herói com as naves
    for (int i = 0; i < NUM_LINHA; i++) {
        for (int k = 0; k < NUM_NAVES_LINHA; k++) {
            if (j->linha[i].naves[k].status && CheckCollisionRecs(j->heroi.bala.pos, j->linha[i].naves[k].pos) && j->heroi.bala.ativa) {
                j->linha[i].naves[k].status = 0;
                Pontuacao(j,i);
                j->heroi.bala.ativa = 0; 
                colisão = 1;
            }
        }
    }
    return colisão;
}


void Pontuacao(Jogo *j,int linha){
    if (linha==0){
        j->pontos+=30;
    }else if (linha==1){
        j->pontos+=20;
    }else if (linha==2){
        j->pontos+=10;
    }  
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