#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAXCHAR 63
#define MAXPARTICIPANTES 3
#define MINPARTICIPANTES 1
#define SALAS 10
#define MAXPORSALA 100
#define MAXBUFFER 500 /* espaco suficiente para o input dado a funcao adiciona_evento */
#define SUCESSO 1
#define INSUCESSO 0

typedef struct {
    /* informacoes acerca do evento que explicitadas na funcao a */

    char descricao[MAXCHAR];
    int dia;                /*formato ddmmaaaa*/
    int tempo;              /*formato hhmm*/
    int duracao;            /*em minutos, entre 1 e 1440*/
    int sala;               /*inteiro de 1 a 10*/
    char responsavel[MAXCHAR];
    char participantes[MAXPARTICIPANTES][MAXCHAR];

    /* valores extra que sao usados ao longo do programa, de modo a facilitar algumas funcoes */

    int n_participantes;    /* numero de participantes do respetivo evento */
    int inicio;             /* inicio do evento em minutos */
    int fim;                /* fim do evento em minutos*/
    int data;               /* dia e tempo do evento no formato ammddhhmm -> toma-se 2019 como ano 0,
                                visto que nenhum evento pode ser agendado para uma data anterior a 2019 */
} Evento;
