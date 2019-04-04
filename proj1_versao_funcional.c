/*
file: proj1.c
autor: Susana Monteiro, 92560
descricao: sistema de reserva de salas
*/

#include "proj1.h"

/* vetores globais, visto que sao utilizados e modificados por um grande numero de funcoes */

Evento eventos[SALAS][MAXPORSALA] = {0};    /* tabela de eventos por sala */
int ocup_sala[SALAS] = {0};                 /* guarda a ocupação de cada sala */ 

int main() {
    char c;
    for (;;) 
        switch (c = getchar()) {
            case 'a':
                adiciona_evento();
                break;
            case 'l':
                lista_todos_eventos();
                break;
            case 's':
                lista_eventos_sala();
                break;
            case 'r':
                apaga_evento();
                break;
            case 'i':
                altera_hora();
                break;
            case 't':
                altera_duracao();
                break;
            case 'm':
                muda_sala();
                break;
            case 'A':
                adiciona_participante();
                break;
            case 'R':
                remove_participante();
                break;
            case 'x':
                return EXIT_SUCCESS;
            default:
                fprintf(stderr, "Invalid input: %d\n", c);
                return EXIT_FAILURE;
        }
}


/*                        FUNCOES PRINCIPAIS                        */


/* funcao principal do comando a 
recebe um novo evento e, se nao houver incompatibilidade de salas e de participantes,
adiciona o evento a respetiva sala e incrementa a ocupacao dessa sala 
*/
void adiciona_evento() {
    Evento e;

    getchar();              /* ignorar o espaco dado entre o comando e o resto do input */
    e = cria_evento();
    if (!incompatibilidade_sala(e))
        if (!incompatibilidade_pessoas(e))
            eventos[e.sala][ocup_sala[e.sala]++] = e;
}

/* funcao principal do comando l
lista todos os eventos por ordem cronológica */
void lista_todos_eventos() {
    int sala, i, j;
    int min;            /* guarda a sala com o evento mais proximo */
    int indice[SALAS];  /* guarda o primeiro evento de cada sala que ainda nao foi impresso 
                            se todos os eventos de uma sala já tiverem sido impressos, o indice dessa sala e -1 */
    int fim = 0;        /* marca o fim da listagem de todos os eventos */

    getchar();          /* ignorar o espaco dado entre o comando e o resto do input */
    /* ordenar cronologicamente todos os eventos de cada sala, individualmente */
    for (sala = 0; sala < SALAS; sala++) {
        qsort(eventos[sala], ocup_sala[sala], sizeof(Evento), compare);
        indice[sala] = ocup_sala[sala] == 0 ? -1 : 0;
    }
    /* escolher o evento mais proximo entre os primeiros de cada sala(ou seja, entre o mais proximo de cada sala) */
    while (!fim) {
        for (i = 0; i < 10 && indice[i] == -1; i++);    /* i guarda a primeira sala com eventos por imprimir */
        if (i == 10) {  /* todos os eventos ja foram impressos quando o indice de todas as salas for -1 */
            fim = 1;    
        }
        else {
            min = i;
            for (sala = i + 1; sala < SALAS; sala++)
                if (indice[sala] != -1 && eventos[sala][indice[sala]].data < eventos[min][indice[min]].data) min = sala;
            printf("%s %08d %04d %d Sala%d %s\n* ", eventos[min][indice[min]].descricao,
                eventos[min][indice[min]].dia, eventos[min][indice[min]].tempo, eventos[min][indice[min]].duracao,
                eventos[min][indice[min]].sala + 1, eventos[min][indice[min]].responsavel);
            for (j = 0; j < eventos[min][indice[min]].n_participantes; j++) {
                printf("%s", eventos[min][indice[min]].participantes[j]);
                    if (j + 1 != eventos[min][indice[min]].n_participantes) putchar(' ');
            }
            putchar('\n');
            if ((++indice[min]) == ocup_sala[min]) indice[min] = -1;
        }
    }
}

/* funcao principal do comando s 
lista todos os eventos de uma dada sala por ordem cronológica */
void lista_eventos_sala() {
    int i, evento, sala;

    scanf("%d", &sala);
    getchar();      /* le o \n */
    sala--;
    /* ordenar cronologicamente todos os eventos  */
    qsort(eventos[sala], ocup_sala[sala], sizeof(Evento), compare);
    for (evento = 0; evento < ocup_sala[sala]; evento++) {
        printf("%s %08d %04d %d Sala%d %s\n* ", eventos[sala][evento].descricao,
            eventos[sala][evento].dia, eventos[sala][evento].tempo, eventos[sala][evento].duracao,
            eventos[sala][evento].sala + 1, eventos[sala][evento].responsavel);
        for (i = 0; i < eventos[sala][evento].n_participantes; i++) {
            printf("%s", eventos[sala][evento].participantes[i]);
            if (i + 1 != eventos[sala][evento].n_participantes) putchar(' ');
        }
        putchar('\n');
    }
}

/* funcao principal do comando r
apaga um evento */
void apaga_evento() { 
    int i = 0;
    int sala = 0, evento = 0;
    char c, descricao[MAXCHAR];

    c = getchar();  /* ignorar o espaco dado entre o comando e o resto do input */
    while ((c = getchar()) != '\n') descricao[i++] = c;
    descricao[i] = '\0';
    if (procura_evento(descricao, &sala, &evento)) {
        /* colocar o ultimo evento da sala na posicao do evento a ser apagado e decrementar a ocupacao dessa sala */
        eventos[sala][evento] = eventos[sala][--ocup_sala[sala]];
    }
}

/* funcao principal do comando i 
altera a hora de início de um evento */
void altera_hora() {    
    char buffer[MAXBUFFER], descricao[MAXCHAR];
    char *token;
    int hora, sala = 0, evento = 0;
    Evento temp;    /* evento temporario, com as caracteristicas do novo evento */

    getchar();      /* ignorar o espaco dado entre o comando e o resto do input */
    fgets(buffer, MAXBUFFER, stdin);
    token = strtok(buffer, ":");
    strcpy(descricao, token);
    token = strtok(NULL, ":");
    hora = atoi(token);
    if (procura_evento(descricao, &sala, &evento)) {
        temp = eventos[sala][evento];
        /* calcular valor das restantes variaveis da struct Evento que dependem do novo valor da hora */
        temp.data = temp.data - temp.tempo + hora;
        temp.tempo = hora;
        temp.inicio = (temp.tempo/100 * 60 + temp.tempo%100);
        temp.fim = temp.inicio + temp.duracao; 
        if (!incompatibilidade_sala(temp))
            if (!incompatibilidade_pessoas(temp))
                eventos[sala][evento] = temp;
    }
}

/* funcao principal do comando t 
altera a duração de um evento */
void altera_duracao() { 
    
    char buffer[MAXBUFFER], descricao[MAXCHAR];
    char *token;
    int duracao, sala = 0, evento = 0;
    Evento temp;    /* evento temporario, com as caracteristicas do novo evento */

    getchar();      /* ignorar o espaco dado entre o comando e o resto do input */
    fgets(buffer, MAXBUFFER, stdin);
    token = strtok(buffer, ":");
    strcpy(descricao, token);
    token = strtok(NULL, ":");
    duracao = atoi(token);
    if (procura_evento(descricao, &sala, &evento)) {
        temp = eventos[sala][evento];
        /* calcular valor das restantes variaveis da struct Evento que dependem do novo valor da duracao */
        temp.duracao = duracao;
        temp.fim = temp.inicio + temp.duracao;
        if (!incompatibilidade_sala(temp))
            if (!incompatibilidade_pessoas(temp))
                eventos[sala][evento] = temp;
    }
}

/* funcao principal do comando m 
muda a sala de um evento */
void muda_sala() {  
    char buffer[MAXBUFFER], descricao[MAXCHAR];
    char *token;
    int nsala, sala = 0, evento = 0;
    Evento temp;    /* evento temporario, com as caracteristicas do novo evento */

    getchar();      /* ignorar o espaco dado entre o comando e o resto do input */
    fgets(buffer, MAXBUFFER, stdin);
    token = strtok(buffer, ":");
    strcpy(descricao, token);
    token = strtok(NULL, ":");
    nsala = atoi(token) - 1;
    if (procura_evento(descricao, &sala, &evento)) {
        temp = eventos[sala][evento];
        temp.sala = nsala;
        if (!incompatibilidade_sala(temp)) {
            eventos[nsala][ocup_sala[nsala]++] = temp;                  /* adicionar o evento a nova sala */
            eventos[sala][evento] = eventos[sala][--ocup_sala[sala]];   /* apagar o evento da sala antiga */
        }
    }
}

/* funcao principal do comando A
adiciona um participante a um evento
*/
void adiciona_participante() {
    char buffer[MAXBUFFER], descricao[MAXCHAR], participante[MAXCHAR];
    char *token;
    int i, sala = 0, evento = 0, stop = 0;  /* stop: indica se o participante ja faz parte do evento ou nao */

    getchar();                              /* ignorar o espaco dado entre o comando e o resto do input */
    fgets(buffer, MAXBUFFER, stdin);
    buffer[strlen(buffer) - 1] = '\0';
    token = strtok(buffer, ":");
    strcpy(descricao, token);
    token = strtok(NULL, ":");
    strcpy(participante, token);
    if (procura_evento(descricao, &sala, &evento)) {
        /* verificar que o novo participante nao participava ja no evento */
        for (i = 0; i < (eventos[sala][evento].n_participantes) && !stop; i++) 
        /* n_participantes + 1, na linha de cima, permite obter os participantes + responsavel */
            if (!strcmp(eventos[sala][evento].participantes[i], participante)) {
                stop = 1;
            }
        if (!stop) {
            if (!incompatibilidade_participante(eventos[sala][evento], participante)) {
                if (eventos[sala][evento].n_participantes == MAXPARTICIPANTES)
                    printf("Impossivel adicionar participante. Evento %s ja tem 3 participantes.\n", descricao);
                else
                    strcpy(eventos[sala][evento].participantes[eventos[sala][evento].n_participantes++], participante);
            }
                
        }
    }
}

/* funcao principal do comando R
remove um participante de um evento */
void remove_participante() {
    char buffer[MAXBUFFER], descricao[MAXCHAR], participante[MAXCHAR];
    char *token;
    int i, j, sala = 0, evento = 0;

    getchar();              /* ignorar o espaco dado entre o comando e o resto do input */
    fgets(buffer, MAXBUFFER, stdin);
    buffer[strlen(buffer) - 1] = '\0';
    token = strtok(buffer, ":");
    strcpy(descricao, token);
    token = strtok(NULL, ":");
    strcpy(participante, token);
    if (procura_evento(descricao, &sala, &evento)) {
        for (i = 0; i < eventos[sala][evento].n_participantes; i++)
            if (!strcmp(eventos[sala][evento].participantes[i], participante)) {
                if (eventos[sala][evento].n_participantes == MINPARTICIPANTES)
                    printf("Impossivel remover participante. Participante %s e o unico participante no evento %s.\n",
                            participante, descricao);
                else {
                    /* se o participante puder ser removido, decrementa-se o numero total de participantes nesse evento e,
                        para todos os participantes a seguir a esse, move-se uma posicao para tras no vetor participantes */
                    eventos[sala][evento].n_participantes--;
                    for (j = i; j < eventos[sala][evento].n_participantes; j++) /* i tem o indice do participante a remover */
                        strcpy(eventos[sala][evento].participantes[j], eventos[sala][evento].participantes[j + 1]);
                }
            }
    }
}


/*                        FUNCOES AUXILIARES                        */


/* funcao que recebe o input para a criacao do evento e devolve o novo evento */
Evento cria_evento() {
    char buffer[MAXBUFFER];
    char *token;
    int i = 0;
    Evento e;

    fgets(buffer, MAXBUFFER, stdin);
    buffer[strlen(buffer) - 1] = '\0';  /* ignorar o \n do final do input */
    token = strtok(buffer, ":");
    strcpy(e.descricao, token);
    token = strtok(NULL, ":");
    e.dia = atoi(token);
    token = strtok(NULL, ":");
    e.tempo = atoi(token);
    token = strtok(NULL, ":");
    e.duracao = atoi(token);
    token = strtok(NULL, ":");
    e.sala = atoi(token) - 1;           /* as salas 1 a 10 ocupam a posicao 0 a 9 do vetor eventos */
    token = strtok(NULL, ":");
    strcpy(e.responsavel, token);
    while ((token = strtok(NULL, ":"))!= NULL) strcpy(e.participantes[i++], token);

    /* calcular valor das restantes variaveis da struct Evento */
    e.n_participantes = i;                          /* numero de participantes do evento */
    strcpy(e.pessoas[0], e.responsavel);            /* e.pessoas = responsavel + participantes do evento */
    for (i = 0; i < e.n_participantes; i++)
        strcpy(e.pessoas[i + 1], e.participantes[i]);                        
    e.inicio = (e.tempo/100 * 60 + e.tempo%100);    /* inicio do evento em minutos */
    e.fim = e.inicio + e.duracao;                   /* fim do evento em minutos*/
    /* e.data: dia e tempo do evento no formato ammddhhmm -> toma-se 2019 como ano 0,
        visto que nenhum evento pode ser agendado para uma data anterior a 2019 */
    e.data = e.tempo + (e.dia/1000000)*10000;       /* hora + dia */
    e.data += ((e.dia/10000)%100)*1000000;          /* hora + dia + mes */
    e.data += ((e.dia%10000)-2019)*100000000;       /* hora + dia + mes + ano */
    return e;
}

/* funcao que avalia se a sala esta ocupada no periodo em causa */
int incompatibilidade_sala(Evento e) { 
    int evento;  

    for (evento = 0; evento < ocup_sala[e.sala]; evento++) {
        if (eventos_sobrepostos(e, eventos[e.sala][evento])) {
            printf("Impossivel agendar evento %s. Sala%d ocupada.\n", e.descricao, ++e.sala);
            return SUCESSO;
        }
    }
    return INSUCESSO;
}

/* funcao que verifica se todos os participantes e o responsavel do evento estao disponiveis nesse periodo */
int incompatibilidade_pessoas(Evento e) {
    int sala, evento, p1, p2, incompatibilidade = 0; /*incompatibilidade -> tem o valor 0 caso nao haja incompatibilidade */

    for (sala = 0; sala < SALAS; sala++) {
        for (evento = 0; evento < ocup_sala[sala]; evento++) {
            if (eventos_sobrepostos(e, eventos[sala][evento])) {
                for (p1 = 0; p1 < e.n_participantes + RESPONSAVEL; p1++) {
                    for (p2 = 0; p2 < eventos[sala][evento].n_participantes + RESPONSAVEL; p2++) {
                        if (!strcmp(e.pessoas[p1], eventos[sala][evento].pessoas[p2])) {
                            printf("Impossivel agendar evento %s. Participante %s tem um evento sobreposto.\n", 
                                    e.descricao, e.pessoas[p1]);
                            incompatibilidade++;
                        }
                    }
                }
            }
        }
    }
    return incompatibilidade;
}

/* funcao que verifica se um participante esta disponivel nesse periodo de tempo 

esta funcao e a incompatibilidade_pessoas sao semelhantes, mas esta verifica a incompatibilidade de um unico 
participante e devolve um erro diferente, por isso optou-se por deixar numa funcao diferente */
int incompatibilidade_participante(Evento e, char participante[MAXCHAR]) {
    int sala, evento, p, incompatibilidade = 0; /*incompatibilidade -> tem o valor 0 caso nao haja incompatibilidade */

    for (sala = 0; sala < SALAS; sala++) {
        for (evento = 0; evento < ocup_sala[sala]; evento++) {
            if (eventos_sobrepostos(e, eventos[sala][evento])) {
                for (p = 0; p < eventos[sala][evento].n_participantes + RESPONSAVEL; p++) {
                    if (!strcmp(participante, eventos[sala][evento].pessoas[p])) {
                        printf("Impossivel adicionar participante. Participante %s tem um evento sobreposto.\n", participante);
                        incompatibilidade++;
                    }
                }
            }
        }
    }
    return incompatibilidade;
}

/* avalia se dois eventos estao sobrepostos */
int eventos_sobrepostos(Evento e1, Evento e2) {
    int incompatibilidade = 0;  /*incompatibilidade -> tem o valor 1 caso haja incompatibilidade */

    if (e2.dia == e1.dia && strcmp(e1.descricao, e2.descricao)) {   /* no caso da descricao ser igual, entao estamos a comparar
                                                                        o mesmo evento e queremos descartar esse caso */
        if (e2.inicio <= e1.inicio && e2.fim > e1.inicio) incompatibilidade++;
        if (e1.inicio <= e2.inicio && e1.fim > e2.inicio) incompatibilidade++;
    }
    return incompatibilidade;
}

/* funcao compare utilizada no qsort e que compara a data de dois eventos */
int compare(const void *a, const void *b) { /*  */ 
    Evento *ia = (Evento *)a;
    Evento *ib = (Evento *)b;
    return ia->data - ib->data;
}

/* percorre todos os eventos de todas as salas ate encontrar o evento-objetivo */
int procura_evento(char descricao[MAXCHAR], int *sala_coordenada, int *evento_coordenada) {
    int sala, evento;

    for (sala = 0; sala < SALAS; sala++) {
        for (evento = 0; evento < ocup_sala[sala]; evento++) {
            if (!strcmp(eventos[sala][evento].descricao, descricao)) {
                *sala_coordenada = sala;
                *evento_coordenada = evento;
                return SUCESSO;
            }
        }
    }
    printf("Evento %s inexistente.\n", descricao);
    return INSUCESSO;
}
