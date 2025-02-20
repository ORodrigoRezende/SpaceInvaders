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
#define NUM_LINHA 4
#define NUM_NAVES_LINHA 7
#define OFFSET_X 100
#define MAX_TAM 100
#define MAX_PLACARES 5

typedef struct {
    Vector2 posicao;
    float velocidade;
} Estrela;

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
    int status;
    Vector2 frame;
    float tempoUltimaTroca;
    bool trocouFrame;
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

typedef struct {
    char player[20];
    int pontuacao;
} Placar;

typedef struct Assets{
    Texture2D naveHeroi;
    Texture2D naveVerde;
    Texture2D naveAzulClaro;
    Texture2D naveRosa;
    Texture2D barreira;
    Texture2D coracao;
    Texture2D naveRoxa;
    Texture2D naveAzul;
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
    double nivel;
    int alturaJanela;
    int larguraJanela;
    int tempoAnimacao;
    int status;
    char player[50];
    int pontuacao;
    bool playerEmEdicao;
    int statusPlacar;
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
void VerificaVidaHeroi(Jogo *j);
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
int VerificaNaves(Jogo *j);
void AtualizaNivel(Jogo *j);
void DesenhaScore(Jogo *j);
void DesenharEstrelas(Estrela estrelas[], int quantidade);
void AtualizarEstrelas(Estrela estrelas[], int quantidade);
void GerarEstrelas(Estrela estrelas[], int quantidade);

int main(){
    InitAudioDevice();

    Jogo jogo;

    jogo.alturaJanela = ALTURA_JANELA;
    jogo.larguraJanela = LARGURA_JANELA;

    InitWindow(jogo.larguraJanela, jogo.alturaJanela, "Space Invaders");
    SetTargetFPS(60);
    srand(time(NULL));
    jogo.status = 0;
    jogo.statusPlacar=0;
    CarregaImagens(&jogo);
    Music musicaJogo = LoadMusicStream("../assets/musica.mp3");
    PlayMusicStream(musicaJogo);

    const int quantidadeEstrelas = 150; // Quantidade de estrelas
    Estrela estrelas[quantidadeEstrelas];
    GerarEstrelas(estrelas, quantidadeEstrelas);

    while(!WindowShouldClose()){ //status 0 Home ;status 1 Jogo ; status 2 Pós jogo
        AtualizarEstrelas(estrelas, quantidadeEstrelas);
        DesenharEstrelas(estrelas, quantidadeEstrelas);
        if(jogo.status==1){
            UpdateMusicStream(musicaJogo);
            AtualizaFrameDesenho(&jogo);
            if (IsKeyPressed(KEY_F2)){
                jogo.status = 2;
            }
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

void AtualizaNivel(Jogo *j){
    if (VerificaNaves(j)==1){
        j->nivel++;
        for (int i = 0; i < NUM_LINHA; i++) { //Cria NAVES
            for (int k = 0; k < NUM_NAVES_LINHA; k++){
                j->linha[i].naves[k].pos = (Rectangle) {OFFSET_X + k * 45, i*45 + 47, STD_SIZE_X, STD_SIZE_Y}; // Espaçadas horizontalmente
                j->linha[i].naves[k].color = RED;
                j->linha[i].naves[k].bala.ativa = 0;
                j->linha[i].naves[k].bala.velocidade = 5;
                j->linha[i].naves[k].status = 1;
                j->linha[i].naves[k].velocidade = 1*j->nivel;
                j->linha[i].naves[k].frame = (Vector2){0, 0}; 
                j->linha[i].naves[k].tempoUltimaTroca = 0.0f; 
                j->linha[i].naves[k].trocouFrame = false;
            }
        j->linha[i].direcao = 1;
        j->linha[i].status = 1;
    }
    }
}

int VerificaNaves(Jogo *j){
    for (int i = 0; i < NUM_LINHA; i++){
        for (int k = 0; k < NUM_NAVES_LINHA; k++){
            if(j->linha[i].naves[k].status==1){
                return 0;
            }
        }
    }
    return 1;
}

void GerarEstrelas(Estrela estrelas[], int quantidade) {
    for (int i = 0; i < quantidade; i++) {
        estrelas[i].posicao = (Vector2){(float)GetRandomValue(0, GetScreenWidth()), (float)GetRandomValue(0, GetScreenHeight())};
        estrelas[i].velocidade = (float)GetRandomValue(1, 3);
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
            estrelas[i].velocidade = (float)GetRandomValue(1, 3); // Nova velocidade aleatória
        }
    }
}

// Função para desenhar as estrelas
void DesenharEstrelas(Estrela estrelas[], int quantidade) {
    for (int i = 0; i < quantidade; i++) {
        DrawPixelV(estrelas[i].posicao, WHITE); // Desenha a estrela como um pixel
    }
}

void AtualizaStatusJogo(Jogo *j) { //Reinicia o Jogo
    if (j->status == 1) {
        IniciaJogo(j);  
    }
}


void IniciaJogo(Jogo *j){
    j->tempoAnimacao = GetTime();
    j->pontuacao = 0;
    j->nivel = 1;
    
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
            j->linha[i].naves[k].pos = (Rectangle) {OFFSET_X + k * 45, i*45 + 47, STD_SIZE_X, STD_SIZE_Y}; // Espaçadas horizontalmente
            j->linha[i].naves[k].color = RED;
            j->linha[i].naves[k].bala.ativa = 0;
            j->linha[i].naves[k].bala.tempo = GetTime();
            j->linha[i].naves[k].bala.velocidade = 5;
            j->linha[i].naves[k].bala.tiro = LoadSound("../assets/shoot.wav");
            j->linha[i].naves[k].status = 1;
            j->linha[i].naves[k].velocidade = 1*j->nivel;
            j->linha[i].naves[k].frame = (Vector2){0, 0}; 
            j->linha[i].naves[k].tempoUltimaTroca = 0.0f; 
            j->linha[i].naves[k].trocouFrame = false;
        }
        j->linha[i].direcao = 1;
        j->linha[i].status = 1;
    }
}

void IniciaCoracao(Jogo *j){
    for (int i = 0; i < j->heroi.vida; i++) {
        j->coracao[i].pos = (Rectangle) {10 + (i * 40), 10, 20, 20};
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
    j->heroi.bala.tiro = LoadSound("../assets/shoot.wav");
    j->heroi.vida = 3;
}

void IniciaBarreira(Jogo *j){
    for (int i = 0; i < 5; i++) {
        j->barreiras[i].pos = (Rectangle){80 + (i * 150), 350, 25, 25};
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

    //DesenhaPlacar(j);
    EndDrawing();
}

void DesenhaScore(Jogo *j) {
    const char *textoBase = "Pontos: "; // Texto base
    char textoCompleto[50]; // Buffer para armazenar o texto completo
    int tamanhoFonte = 20; // Tamanho da fonte
    int espacoAltura = 47; // Espaço de 47px de altura
    Color corTexto = WHITE; // Cor do texto

    // Concatena o texto base com o valor da variável score
    snprintf(textoCompleto, sizeof(textoCompleto), "%s%d", textoBase, j->pontuacao);

    // Calcula a posição X para centralizar o texto horizontalmente
    int larguraTexto = MeasureText(textoCompleto, tamanhoFonte); // Mede a largura do texto completo
    int posX = (GetScreenWidth() - larguraTexto) / 2; // Centraliza horizontalmente

    // Calcula a posição Y para centralizar o texto verticalmente no espaço de 47px
    int posY = (espacoAltura - tamanhoFonte) / 2; // Centraliza verticalmente

    DrawText(textoCompleto, posX, posY, tamanhoFonte, corTexto);
}

void DesenhaPlacar(Jogo *j){
    for (int i = 0; i < MAX_PLACARES; i++) {
    char pontos[60];
    sprintf(pontos, "%d - %s : %d", i + 1, j->placar[i].player,j->placar[i].pontuacao);
    DrawText(pontos, 250, 425 + i * 30, 20, WHITE);
    }
}

void DesenhaBarreiras(Jogo *j) {
    for (int i = 0; i < 5; i++) {
        if (j->barreiras[i].vida > 0) {
            DrawTexture(j->assets.barreira, 80 + (i*150), 350 , WHITE);
        }
    }
}

void DesenhaVidas(Jogo *j) {
    for (int i = 0; i < j->heroi.vida; i++) {
        DrawTexture(j->assets.coracao, 10 + (i * 40), 10, WHITE);
    }
}

void AtualizaJogo(Jogo *j){
    AtualizaNavePos(j);
    AtualizaHeroiPos(j);
    AtiraBalas(j);
    AtiraBalasHeroi (j);
    AtualizaNivel(j);
    VerificaVidaHeroi(j);
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
    DesenhaScore(j);
    EndDrawing();
}

void DesenhaJogoPos(Jogo *j){
    BeginDrawing();
    ClearBackground(BLACK);
    DesenhaBordas(j);
    int textWidth = MeasureText("ACABOU!", 50);
    DrawText("ACABOU!", (GetScreenWidth() - textWidth) / 2, GetScreenHeight() / 2 - 100, 50, RED);

    char textoPontos[50];
    sprintf(textoPontos, "Pontuação Final: %d", j->pontuacao);
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
        VerificaVidaHeroi(j);
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
    j->assets.naveHeroi = LoadTexture("../assets/naveHeroi.png");
    j->assets.naveVerde = LoadTexture("../assets/GreenAnimation.png");
    j->assets.naveRosa = LoadTexture("../assets/PinkAnimation.png");
    j->assets.naveAzulClaro = LoadTexture("../assets/LightBlueAnimation.png");
    j->assets.naveAzul = LoadTexture("../assets/BlueAnimation.png");
    j->assets.naveRoxa = LoadTexture("../assets/PurpleAnimation.png");
    j->assets.barreira = LoadTexture("../assets/barreira.png");
    j->assets.coracao = LoadTexture("../assets/heart.png");
}

void DescarregaImagens(Jogo *j){
    UnloadTexture(j->assets.naveHeroi);
    UnloadTexture(j->assets.naveVerde);
    UnloadTexture(j->assets.naveAzulClaro);
    UnloadTexture(j->assets.naveRosa);
    UnloadTexture(j->assets.naveAzul);
    UnloadTexture(j->assets.naveRoxa);
    UnloadTexture(j->assets.barreira);
    UnloadTexture(j->assets.coracao);
}

void DesenhaNaves(Jogo *j){
    for (int i = 0; i < NUM_LINHA; i++){ 
        for (int k = 0; k < NUM_NAVES_LINHA; k++){
            Vector2 tamanhoFrame = {32, 32};
            const float duracaoTrocaFrame = 1.0f; // Duração da troca de frame (em segundos)
            const float duracaoFrameFinal = 0.35f; // Duração do Frame 2 (1 segundo)

            if (j->linha[i].naves[k].status == 1) {
                // Se a bala estiver ativa, muda para o Frame 1
                if (j->linha[i].naves[k].bala.ativa == 1) {
                    if (j->linha[i].naves[k].frame.x != 1) { // Se ainda não estiver no Frame 1
                        j->linha[i].naves[k].frame.x = 1; // Muda para o Frame 1
                        j->linha[i].naves[k].tempoUltimaTroca = GetTime(); // Atualiza o tempo da última troca
                    }

                    // Mantém o Frame 1 por duracaoTrocaFrame segundos
                    if (GetTime() - j->linha[i].naves[k].tempoUltimaTroca >= duracaoTrocaFrame) {
                        j->linha[i].naves[k].frame.x = 0; // Volta ao Frame 0 após duracaoTrocaFrame segundos
                    }
                } else {
                    j->linha[i].naves[k].frame.x = 0; // Volta ao Frame 0 se a bala não estiver ativa
                }
            }
            // Quando o status da nave for 2, inicia a animação do Frame 2
            else if (j->linha[i].naves[k].status == 2) {
                // Se ainda não estiver no Frame 2, muda para ele
                if (j->linha[i].naves[k].frame.x != 2) {
                    j->linha[i].naves[k].frame.x = 2; // Muda para o Frame 2
                    j->linha[i].naves[k].tempoUltimaTroca = GetTime(); // Atualiza o tempo da última troca
                }

                // Mantém o Frame 2 por duracaoFrameFinal segundos
                if (GetTime() - j->linha[i].naves[k].tempoUltimaTroca >= duracaoFrameFinal) {
                    j->linha[i].naves[k].status = 0; // Define o status como 0 após duracaoFrameFinal segundos
                    j->linha[i].naves[k].frame.x = 0; // Volta ao Frame 0
                }
            }
            Rectangle frameRecNave = {j->linha[i].naves[k].frame.x * tamanhoFrame.x,j->linha[i].naves[k].frame.y * tamanhoFrame.y,tamanhoFrame.x,tamanhoFrame.y};
            if (j->linha[i].naves[k].status >= 1) {
                if(i==0){
                    DrawTexturePro(j->assets.naveRoxa,frameRecNave,(Rectangle){j->linha[i].naves[k].pos.x, j->linha[i].naves[k].pos.y, 32, 32},(Vector2){0, 0},0.0f,WHITE);
                }else if(i==1){
                    DrawTexturePro(j->assets.naveAzul,frameRecNave,(Rectangle){j->linha[i].naves[k].pos.x, j->linha[i].naves[k].pos.y, 32, 32},(Vector2){0, 0},0.0f,WHITE);
                }else if(i==2){
                    DrawTexturePro(j->assets.naveAzulClaro,frameRecNave,(Rectangle){j->linha[i].naves[k].pos.x, j->linha[i].naves[k].pos.y, 32, 32},(Vector2){0, 0},0.0f,WHITE);
                }else if(i==3){
                    DrawTexturePro(j->assets.naveRosa,frameRecNave,(Rectangle){j->linha[i].naves[k].pos.x, j->linha[i].naves[k].pos.y, 32, 32},(Vector2){0, 0},0.0f,WHITE);
                }else if(i==4){
                    DrawTexturePro(j->assets.naveVerde,frameRecNave,(Rectangle){j->linha[i].naves[k].pos.x, j->linha[i].naves[k].pos.y, 32, 32},(Vector2){0, 0},0.0f,WHITE);
                }else if(i==5){
                    DrawTexturePro(j->assets.naveVerde,frameRecNave,(Rectangle){j->linha[i].naves[k].pos.x, j->linha[i].naves[k].pos.y, 32, 32},(Vector2){0, 0},0.0f,WHITE);
                }else if(i==6){
                    DrawTexturePro(j->assets.naveVerde,frameRecNave,(Rectangle){j->linha[i].naves[k].pos.x, j->linha[i].naves[k].pos.y, 32, 32},(Vector2){0, 0},0.0f,WHITE);
                }else if(i==7){
                    DrawTexturePro(j->assets.naveVerde,frameRecNave,(Rectangle){j->linha[i].naves[k].pos.x, j->linha[i].naves[k].pos.y, 32, 32},(Vector2){0, 0},0.0f,WHITE);
                }
            }
        }
    }
}

void LerPlacar(Jogo *j) {
    FILE *file = fopen("historico.txt", "r");
    int count = 0;
    char buf[MAX_TAM];

    if (file != NULL) {
        fgets(buf,MAX_TAM,file);
        while (count<MAX_PLACARES){
        strcpy(j->placar[count].player,strtok(buf,","));
        j->placar[count].pontuacao = atoi(strtok(NULL,","));
        fgets(buf,MAX_TAM,file);
        count++;
    }
        fclose(file);
    }
}
void AtualizaPlacar(Jogo *j) {
        for (int i = MAX_PLACARES; i > 0;i--){
            j->placar[i] = j->placar[i-1];
        }
        strcpy(j->placar[0].player,j->player);
        j->placar[0].pontuacao = j->pontuacao;
    

    FILE *file = fopen("historico.txt", "w");
    if (file != NULL) {
        for (int i = 0; i < MAX_PLACARES; i++) {
            fprintf(file, "%s,%d\n", j->placar[i].player, j->placar[i].pontuacao);
        }
        fclose(file);
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
                if(j->linha[i].naves[k].bala.ativa){
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
                    if (rand() % 200 < (j->nivel)) {
                    j->linha[i].naves[k].bala.pos = (Rectangle){j->linha[i].naves[k].pos.x+j->linha[i].naves[k].pos.width/2, j->linha[i].naves[k].pos.y+j->linha[i].naves[k].pos.height/2, 
                    LARGURA_BALA, ALTURA_BALA};
                    j->linha[i].naves[k].bala.ativa = 1;
                    j->linha[i].naves[k].bala.tempo = GetTime();
                    PlaySound(j->linha[i].naves[k].bala.tiro);
                }}
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
        if (j->barreiras[i].vida > 0 && j->heroi.bala.ativa) {
            if (CheckCollisionRecs(j->heroi.bala.pos, j->barreiras[i].pos)) {
                j->barreiras[i].vida--; 
                j->heroi.bala.ativa = 0; 
                colisão = 1;
            }
        }
    } 
            
    if(CheckCollisionRecs(j->heroi.bala.pos, j->bordas[0].pos)){
        colisão = 1;
    }
    // Verifica colisão da bala do herói com as naves
    for (int i = 0; i < NUM_LINHA; i++) {
        for (int k = 0; k < NUM_NAVES_LINHA; k++) {
            if (j->linha[i].naves[k].status && CheckCollisionRecs(j->heroi.bala.pos, j->linha[i].naves[k].pos) && j->heroi.bala.ativa) {
                j->linha[i].naves[k].status = 2;
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
        j->pontuacao+=40;
    }else if (linha==1){
        j->pontuacao+=30;
    }else if (linha==2){
        j->pontuacao+=20;
    }else if (linha==3){
        j->pontuacao+=10;
    }   
}

void VerificaVidaHeroi(Jogo *j){
    if(j->heroi.vida == 0){
        j->statusPlacar=1;
        if(j->statusPlacar==1 && j->status==1){
            AtualizaPlacar(j);
        }
        j->status = 2;
    }
}