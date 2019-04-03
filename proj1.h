#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAXCHAR 64              /* numero maximo de caracteres da descricao, responsavel e participantes */
#define RESPONSAVEL 1           /* numero de responsaveis por evento */
#define MAXPARTICIPANTES 3      /* numero maximo de participantes */
#define MINPARTICIPANTES 1      /* numero minimo de participantes */
#define SALAS 10                /* numero maximo de salas */ 
#define MAXPORSALA 100          /* numero maximo de eventos por sala */
#define MAXBUFFER 500           /* espaco suficiente para o input dado a funcao adiciona_evento */
#define SUCESSO 1
#define INSUCESSO 0

typedef struct {
    /* informacoes acerca do evento que explicitadas na funcao a */

    char descricao[MAXCHAR];
    int dia;
    int tempo;
    int duracao;
    int sala;
    char responsavel[MAXCHAR];
    char participantes[MAXPARTICIPANTES][MAXCHAR];

    /* valores extra que sao usados ao longo do programa, de modo a facilitar algumas funcoes */
    char pessoas[MAXPARTICIPANTES + 1][MAXCHAR];    /* responsavel + participantes do evento */
    int n_participantes;                            /* numero de participantes do respetivo evento */
    int inicio;                                     /* inicio do evento em minutos */
    int fim;                                        /* fim do evento em minutos*/
    int data;                                       /* dia e tempo do evento no formato ammddhhmm -> toma-se 2019 como ano 0,
                                                    visto que nenhum evento pode ser agendado para uma data anterior a 2019 */
} Evento;

/* funcoes principais */
void adiciona_evento();
void lista_todos_eventos();
void lista_eventos_sala();
void apaga_evento();
void altera_hora();
void altera_duracao();
void muda_sala();
void adiciona_participante();
void remove_participante();

/* funcoes auxiliares */
Evento cria_evento();
int incompatibilidade_sala(Evento e);
int incompatibilidade_pessoas(Evento e);
int incompatibilidade_participante(Evento e, char participante[MAXCHAR]);
int eventos_sobrepostos(Evento e1, Evento e2);
int compare();
int procura_evento(char descricao[MAXCHAR], int *sala_coordenada, int *evento_coordenada);




