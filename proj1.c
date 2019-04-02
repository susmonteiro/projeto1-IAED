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
                getchar();
                adiciona_participante();
                break;
            case 'R':
                getchar();
                remove_participante();
                break;
            case 'x':
                return EXIT_SUCCESS;
            default:
                fprintf(stderr, "Invalid input: %d\n", c);
                return EXIT_FAILURE;
        }
}

void adiciona_evento() {    /* funcao principal do comando a */
    Evento e;

    getchar();              /* ignorar o espaco dado entre o comando e o resto do input */
    e = cria_evento();
    if (!incompatibilidade_sala(e))
        if (!incompatibilidade_pessoas(e))
            eventos[e.sala][ocup_sala[e.sala]++] = e;
}

Evento cria_evento() {      /* funcao que recebe o input para a criacao do evento e devolve o novo evento */
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
    e.sala = atoi(token) - 1;   /* as salas 1 a 10 ocupam a posicao 0 a 9 do vetor eventos */
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

int incompatibilidade_sala(Evento e) {  /* avalia se a sala esta livre */
    int evento;  

    for (evento = 0; evento < ocup_sala[e.sala]; evento++) {
        if (eventos_sobrepostos(e, eventos[e.sala][evento])) {
            printf("Impossivel agendar evento %s. Sala%d ocupada.\n", e.descricao, ++e.sala);
            return SUCESSO;
        }
    }
    return INSUCESSO;
}

int eventos_sobrepostos(Evento e1, Evento e2) {     /* avalia se dois eventos estao sobrepostos */
    int incompatibilidade = 0;  /*incompatibilidade -> tem o valor 1 caso haja incompatibilidade */

    if (e2.dia == e1.dia && strcmp(e1.descricao, e2.descricao)) {   /* no caso da descricao ser igual, entao estamos a comparar
                                                                    o mesmo evento e queremos descartar esse caso */
        if (e2.inicio <= e1.inicio && e2.fim > e1.inicio) incompatibilidade++;
        if (e1.inicio <= e2.inicio && e1.fim > e2.inicio) incompatibilidade++;
    }
    return incompatibilidade;
}

int incompatibilidade_pessoas(Evento e) {       /* avalia se todas as pessoas de um evento (responsaveis e participantes) estao livres*/
    int sala, evento, p1, p2, incompatibilidade = 0; /*incompatibilidade -> tem o valor 0 caso nao haja incompatibilidade */

    for (sala = 0; sala < SALAS; sala++) {
        for (evento = 0; evento < ocup_sala[sala]; evento++) {
            if (eventos_sobrepostos(e, eventos[sala][evento])) {
                for (p1 = 0; p1 < e.n_participantes + 1; p1++) {
                    for (p2 = 0; p2 < eventos[sala][evento].n_participantes + 1; p2++) {
                        if (!strcmp(e.pessoas[p1], eventos[sala][evento].pessoas[p2])) {
                            printf("Impossivel agendar evento %s. Participante %s tem um evento sobreposto.\n", e.descricao, e.pessoas[p1]);
                            incompatibilidade++;
                        }
                    }
                }
            }
        }
    }
    return incompatibilidade;
}


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
            for (i = 0; i < eventos[sala][evento].n_participantes; i++) 
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

void lista_eventos_sala() { /* funcao principal do comando s */
    int i, evento, sala;

    scanf("%d", &sala);
    getchar();      /* le o \n */
    sala--;
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

int compare(const void *a, const void *b) { /* funcao compare utilizada no qsort */ 
    Evento *ia = (Evento *)a;
    Evento *ib = (Evento *)b;
    return ia->data - ib->data;
}

void apaga_evento() { /* funcao principal do comando r */
    /*
    1) procurar o evento com a mesma descricao 
    2) colocar o ultimo evento da sala na posicao do evento a ser apagado e decrementar a ocupacao dessa sala 
    */
    int i = 0;
    int sala = 0, evento = 0;
    char c, descricao[MAXCHAR];

    c = getchar();  /* ignorar o espaco dado entre o comando e o resto do input */
    while ((c = getchar()) != '\n') descricao[i++] = c;
    descricao[i] = '\0';
    if (procura_evento(descricao, &sala, &evento)) {
        eventos[sala][evento] = eventos[sala][--ocup_sala[sala]];
    }
}

void altera_hora() {    /* funcao principal do comando i */
    /*
    1) procurar o evento com a mesma descricao 
    2) criar um evento temporario com as mesmas caracteristicas do novo evento 
    3) avaliar se ha alguma incompatibilidade em termos de sala e de participantes
    4) em caso negativo, atualizar a tabela de eventos 
    */
    char buffer[MAXBUFFER], descricao[MAXCHAR];
    char *token;
    int hora, sala = 0, evento = 0;
    Evento temp;    /* evento temporario, com as caracteristicas do novo evento */

    getchar();  /* ignorar o espaco dado entre o comando e o resto do input */
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

int procura_evento(char descricao[MAXCHAR], int *sala_coordenada, int *evento_coordenada) {
    /* procura o evento com a descricao inserida e, em caso de insucesso, devolve um erro */
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

/* funcao principal do comando t 
1) procurar o evento com a mesma descricao 
2) criar um evento temporario com as mesmas caracteristicas do novo evento 
3) avaliar se ha alguma incompatibilidade em termos de sala e de participantes
4) em caso negativo, atualizar a tabela de eventos 
*/
void altera_duracao() { 
    
    char buffer[MAXBUFFER], descricao[MAXCHAR];
    char *token;
    int duracao, sala = 0, evento = 0;
    Evento temp;    /* evento temporario, com as caracteristicas do novo evento */

    getchar();  /* ignorar o espaco dado entre o comando e o resto do input */
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

void muda_sala() {  /* funcao principal do comando m */
    /*
    1) procurar o evento com a mesma descricao 
    2) criar um evento temporario com as mesmas caracteristicas do novo evento 
    3) avaliar se ha alguma incompatibilidade em termos de sala 
        (como o horario nao se altera, os participantes e o responsavel estarao certamente disponiveis)
    4) em caso negativo, atualizar a tabela de eventos, isto e:
        - juntar o evento a nova sala
        - apagar o evento da sala antiga, ou seja, colocar o ultimo evento da sala no lugar
        do evento a apagar e decrementar a ocupacao dessa sala
    */
    char buffer[MAXBUFFER], descricao[MAXCHAR];
    char *token;
    int nsala, sala = 0, evento = 0;
    Evento temp; /* evento temporario, com as caracteristicas do novo evento */

    getchar();  /* ignorar o espaco dado entre o comando e o resto do input */
    fgets(buffer, MAXBUFFER, stdin);
    token = strtok(buffer, ":");
    strcpy(descricao, token);
    token = strtok(NULL, ":");
    nsala = atoi(token) - 1;
    if (procura_evento(descricao, &sala, &evento)) {
        temp = eventos[sala][evento];
        temp.sala = nsala;
        if (!incompatibilidade_sala(temp)) {
            eventos[nsala][ocup_sala[nsala]++] = temp;  /* adicionar o evento a nova sala */
            eventos[sala][evento] = eventos[sala][--ocup_sala[sala]]; /* apagar o evento da sala antiga */
        }
    }
}

void adiciona_participante() {
    char buffer[MAXBUFFER], descricao[MAXCHAR], participante[MAXCHAR];
    char *token;
    int i, sala = 0, evento = 0, stop = 0;

    fgets(buffer, MAXBUFFER, stdin);
    buffer[strlen(buffer) - 1] = '\0';
    token = strtok(buffer, ":");
    strcpy(descricao, token);
    token = strtok(NULL, ":");
    strcpy(participante, token);
    if (procura_evento(descricao, &sala, &evento)) {
        if (!strcmp(eventos[sala][evento].responsavel, participante)) {
            stop = 1;
        }
        for (i = 0; i < eventos[sala][evento].n_participantes && !stop; i++)
            if (!strcmp(eventos[sala][evento].participantes[i], participante)) {
                stop = 1;
            }
        if (!stop) {
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
    int i, j, sala = 0, evento = 0;

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
                    eventos[sala][evento].n_participantes--;
                    for (j = i; j < eventos[sala][evento].n_participantes; j ++)
                        strcpy(eventos[sala][evento].participantes[j], eventos[sala][evento].participantes[j + 1]);
                }
            }
    }
}

void lista_todos_eventos() {    /* funcao principal do comando l */
    /* 
    1) ordenar cronologicamente todos os eventos de cada sala 
    2) escolher o evento mais proximo entre os primeiros (ou seja, os mais proximos) de cada sala 
        para tal, foi criado o vetor indice que guarda o primeiro evento de cada sala que ainda nao foi impresso
        se todos os eventos de uma sala já tiverem sido impressos, o indice dessa sala e -1
    */

    int sala, indice[SALAS], i, j, min, fim = 0;

    getchar();      /* ignorar o espaco dado entre o comando e o resto do input */
    for (sala = 0; sala < SALAS; sala++) {
        qsort(eventos[sala], ocup_sala[sala], sizeof(Evento), compare);
        indice[sala] = ocup_sala[sala] == 0 ? -1 : 0;
    }
    while (!fim) {
        for (i = 0; i < 10 && indice[i] == -1; i++);
        if (i == 10) {
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