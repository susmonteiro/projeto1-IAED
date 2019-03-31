#include "proj1.h"

void adiciona_evento();
Evento cria_evento();
int incompatibilidade_sala(Evento e);
int incompatibilidade_pessoa(Evento e, char pessoa[MAXCHAR]);
int eventos_sobrepostos(Evento e1, Evento e2);
void lista_todos_eventos();
void lista_eventos_sala();
int compare();
void apaga_evento();
int procura_evento(char descricao[MAXCHAR], int *sala_coordenada, int *evento_coordenada);
void altera_hora();
void altera_duracao();
void muda_sala();
void adiciona_participante();
void remove_participante();

/* vetores globais, visto que sao utilizados e modificados por um grande numero de funcoes */

Evento eventos[SALAS][MAXPORSALA] = {0};    /* tabela de eventos por sala */
int ocup_sala[10] = {0};                    /* guarda a ocupação de cada sala */ 

int main() {
    char c;
    for (;;) 
        switch (c = getchar()) {
            case 'a':
                c = getchar();  /* ignorar o espaco dado entre a acao e o resto do input */
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
                c = getchar();  /* ignorar o espaco dado entre a acao e o resto do input */
                altera_hora();
                break;
            case 't':
                c = getchar();  /* ignorar o espaco dado entre a acao e o resto do input */
                altera_duracao();
                break;
            case 'm':
                c = getchar();
                muda_sala();
                break;
            case 'A':
                c = getchar();
                adiciona_participante();
                break;
            case 'R':
                c = getchar();
                remove_participante();
                break;
            case 'x':
                return EXIT_SUCCESS;
                /*
            default:
                fprintf(stderr, "Invalid input\n");
                return EXIT_FAILURE;*/
        }
}

void adiciona_evento() {
    int i, adiciona = 0;   /* flag que indica que o evento pode ou nao ser adicionado, 
                            de acordo com a disponibilidade da sala, do responsavel e dos participantes */
    Evento e;
    e = cria_evento();
    if (incompatibilidade_sala(e)) adiciona++;
    if (incompatibilidade_pessoa(e, e.responsavel)) {
        printf("Impossivel agendar evento %s. Participante %s tem um evento sobreposto.\n", e.descricao, e.responsavel);
        adiciona++;
    }
    for (i = 0; i < e.n_participantes; i++) {
        if (incompatibilidade_pessoa(e, e.participantes[i])) {
            printf("Impossivel agendar evento %s. Participante %s tem um evento sobreposto.\n", e.descricao, e.participantes[i]);
            adiciona++;
        }
    }
    
    if (adiciona == 0) {
        eventos[e.sala][ocup_sala[e.sala]++] = e;
    }
}

Evento cria_evento() {
    char buffer[MAXBUFFER];
    char *token;
    int i = 0;
    Evento e;

    fgets(buffer, MAXBUFFER, stdin);
    buffer[strlen(buffer) - 1] = '\0';
    token = strtok(buffer, ":");
    strcpy(e.descricao, token);
    token = strtok(NULL, ":");
    e.dia = atoi(token);
    token = strtok(NULL, ":");
    e.tempo = atoi(token);
    token = strtok(NULL, ":");
    e.duracao = atoi(token);
    token = strtok(NULL, ":");
    e.sala = atoi(token) - 1;   /* as salas 1 a 10 ocupam a posicao 0 a 9 do vetor eventos */
    token = strtok(NULL, ":");
    strcpy(e.responsavel, token);
    while ((token = strtok(NULL, ":"))!= NULL) strcpy(e.participantes[i++], token);

    /* calcular valor das restantes variaveis da struct Evento */

    e.n_participantes = i;
    e.inicio = (e.tempo/100 * 60 + e.tempo%100);
    e.fim = e.inicio + e.duracao;
    e.data = e.tempo + (e.dia/1000000)*10000 + ((e.dia/10000)%100)*1000000 + 
        ((e.dia%10000)-2019)*100000000; /* dia e tempo do evento no formato ammddhhmm -> toma-se 2019 como ano 0,
                                visto que nenhum evento pode ser agendado para uma data anterior a 2019 */
    return e;
}

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

int eventos_sobrepostos(Evento e1, Evento e2) {
    int incompatibilidade = 0;  /*incompatibilidade -> tem o valor 1 caso a sala ja esteja ocupada */
    if (e2.dia == e1.dia && strcmp(e1.descricao, e2.descricao)) {
        if (e2.inicio <= e1.inicio && e2.fim > e1.inicio) incompatibilidade++;
        if (e1.inicio <= e2.inicio && e1.fim > e2.inicio) incompatibilidade++;
    }
    return incompatibilidade;
}


/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/

int incompatibilidade_pessoa(Evento e, char pessoa[MAXCHAR]) {
    int sala, evento, i;
    for (sala = 0; sala < SALAS; sala++) {
        for (evento = 0; evento < ocup_sala[sala]; evento++) {
            if (!strcmp(pessoa, eventos[sala][evento].responsavel)) { 
                if (eventos[sala][evento].dia == e.dia && strcmp(eventos[sala][evento].descricao, e.descricao)) {
                    if (eventos[sala][evento].inicio <= e.inicio && eventos[sala][evento].fim > e.inicio) return 1;
                    if (e.inicio <= eventos[sala][evento].inicio && e.fim > eventos[sala][evento].inicio) return 1;
                }
            }
            for (i = 0; i < 3; i++) 
                if (!strcmp(pessoa, eventos[sala][evento].participantes[i])) {
                    if (eventos[sala][evento].dia == e.dia && strcmp(eventos[sala][evento].descricao, e.descricao)) {
                        if (eventos[sala][evento].inicio <= e.inicio && eventos[sala][evento].fim > e.inicio) return 1;
                        if (e.inicio <= eventos[sala][evento].inicio && e.fim > eventos[sala][evento].inicio) return 1;
                    }
                }
        }
    }
    return 0;
}

/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/

void lista_eventos_sala() {
    int j, evento, sala;

    scanf("%d", &sala);
    sala--;
    qsort(eventos[sala], ocup_sala[sala], sizeof(Evento), compare);
    for (evento = 0; evento < ocup_sala[sala]; evento++) {
        printf("%s %08d %04d %d Sala%d %s\n* ", eventos[sala][evento].descricao,
            eventos[sala][evento].dia, eventos[sala][evento].tempo, eventos[sala][evento].duracao,
            eventos[sala][evento].sala + 1, eventos[sala][evento].responsavel);
        for (j = 0; j < eventos[sala][evento].n_participantes; j++) {
            printf("%s", eventos[sala][evento].participantes[j]);
            if (j + 1 != eventos[sala][evento].n_participantes) putchar(' ');
        }
        putchar('\n');
    }
}

int compare(const void *a, const void *b) 
{ 
    Evento *ia = (Evento *)a;
    Evento *ib = (Evento *)b;
    return ia->data - ib->data;
}

void apaga_evento() {
    int i = 0;
    int sala = 0, evento = 0;
    char c, descricao[MAXCHAR];
    c = getchar();  /* ignorar o primeiro espaco */
    while ((c = getchar()) != '\n') descricao[i++] = c;
    descricao[i] = '\0';
    if (procura_evento(descricao, &sala, &evento)) {
        eventos[sala][evento] = eventos[sala][ocup_sala[sala]-1];
        ocup_sala[sala]--;
    }
}

void altera_hora() {
    char buffer[MAXBUFFER], descricao[MAXCHAR];
    char *token;
    int hora, i, adiciona = 0, sala = 0, evento = 0;
    Evento temp;

    fgets(buffer, MAXBUFFER, stdin);
    token = strtok(buffer, ":");
    strcpy(descricao, token);
    token = strtok(NULL, ":");
    hora = atoi(token);
    if (procura_evento(descricao, &sala, &evento)) {
        temp = eventos[sala][evento];
        temp.data = temp.data - temp.tempo + hora;
        temp.tempo = hora;
        temp.inicio = (temp.tempo/100 * 60 + temp.tempo%100);
        temp.fim = temp.inicio + temp.duracao; 
        if (incompatibilidade_sala(temp)) adiciona++;
        else {
            if (incompatibilidade_pessoa(temp, temp.responsavel)) {
            printf("Impossivel agendar evento %s. Participante %s tem um evento sobreposto.\n", temp.descricao, temp.responsavel);
            adiciona++;
            }
            for (i = 0; i < temp.n_participantes; i++) {
                if (incompatibilidade_pessoa(temp, temp.participantes[i])) {
                printf("Impossivel agendar evento %s. Participante %s tem um evento sobreposto.\n", temp.descricao, temp.participantes[i]);
                adiciona++;
                }
            }
        }
        if (adiciona == 0) {
            eventos[sala][evento] = temp;
        }
    }
}

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

void altera_duracao() {
    char buffer[MAXBUFFER], descricao[MAXCHAR];
    char *token;
    int duracao, i, adiciona = 0, sala = 0, evento = 0;
    Evento temp;

    fgets(buffer, MAXBUFFER, stdin);
    token = strtok(buffer, ":");
    strcpy(descricao, token);
    token = strtok(NULL, ":");
    duracao = atoi(token);
    if (procura_evento(descricao, &sala, &evento)) {
        temp = eventos[sala][evento];
        temp.duracao = duracao;
        temp.fim = temp.inicio + temp.duracao;
        if (incompatibilidade_sala(temp)) adiciona++;
        else {
            if (incompatibilidade_pessoa(temp, temp.responsavel)) {
            printf("Impossivel agendar evento %s. Participante %s tem um evento sobreposto.\n", temp.descricao, temp.responsavel);
            adiciona++;
            }
            for (i = 0; i < temp.n_participantes; i++) {
                if (incompatibilidade_pessoa(temp, temp.participantes[i])) {
                printf("Impossivel agendar evento %s. Participante %s tem um evento sobreposto.\n", temp.descricao, temp.participantes[i]);
                adiciona++;
                }
            }
        }
        if (adiciona == 0) {
            eventos[sala][evento] = temp;
        }
    }
}

void muda_sala() {
    char buffer[MAXBUFFER], descricao[MAXCHAR];
    char *token;
    int nsala, sala = 0, evento = 0;
    Evento temp;

    fgets(buffer, MAXBUFFER, stdin);
    token = strtok(buffer, ":");
    strcpy(descricao, token);
    token = strtok(NULL, ":");
    nsala = atoi(token) - 1;
    if (procura_evento(descricao, &sala, &evento)) {
        temp = eventos[sala][evento];
        temp.sala = nsala;
        if (!incompatibilidade_sala(temp)) {
            eventos[nsala][ocup_sala[nsala]++] = temp;
            eventos[sala][evento] = eventos[sala][ocup_sala[sala]-1];
            ocup_sala[sala]--;
        }
    }
}

void adiciona_participante() {
    char buffer[MAXBUFFER], descricao[MAXCHAR], participante[MAXCHAR];
    char *token;
    int i, sala = 0, evento = 0, stop = 1;

    fgets(buffer, MAXBUFFER, stdin);
    buffer[strlen(buffer) - 1] = '\0';
    token = strtok(buffer, ":");
    strcpy(descricao, token);
    token = strtok(NULL, ":");
    strcpy(participante, token);
    if (procura_evento(descricao, &sala, &evento)) {
        for (i = 0; i < eventos[sala][evento].n_participantes; i++)
            if (!strcmp(eventos[sala][evento].participantes[i], participante)) {
                stop = 0;
                break;
            }
        if (stop) {
            if (incompatibilidade_pessoa(eventos[sala][evento], participante))
                    printf("Impossivel adicionar participante. Participante %s tem um evento sobreposto.\n", participante);
            else {
                if (eventos[sala][evento].n_participantes == MAXPARTICIPANTES)
                    printf("Impossivel adicionar participante. Evento %s ja tem 3 participantes.\n", descricao);
                else {
                    strcpy(eventos[sala][evento].participantes[eventos[sala][evento].n_participantes], participante);
                    eventos[sala][evento].n_participantes++;
                }
            }
                
        }
    }
}

void remove_participante() {
    char buffer[MAXBUFFER], descricao[MAXCHAR], participante[MAXCHAR];
    char *token;
    int i, sala = 0, evento = 0;

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
                    printf("Impossivel remover participante. Participante %s e o unico participante no evento %s.\n", participante, descricao);
                else {
                    strcpy(eventos[sala][evento].participantes[i], eventos[sala][evento].participantes[eventos[sala][evento].n_participantes]);
                    eventos[sala][evento].n_participantes--;
                }
            }
    }
}

void lista_todos_eventos() {
    int sala, indice[SALAS], i, j, min;

    for (sala = 0; sala < SALAS; sala++) {
        qsort(eventos[sala], ocup_sala[sala], sizeof(Evento), compare);
        indice[sala] = ocup_sala[sala] == 0 ? -1 : 0;
    }
    for (;;) {
        i = -1;
        while (indice[++i] == -1 && i < 10);
        if (i == 10) {
            break;
        }
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