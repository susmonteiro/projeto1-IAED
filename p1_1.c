# include <stdio.h>
# include <string.h>

# define MAXCHAR 63
# define PARTICIPANTES 3
# define SALAS 10
# define MAXPORSALA 100

typedef struct {

    /* informacoes acerca do evento que explicitadas na funcao a */

    char descricao[MAXCHAR];
    int dia;                /*formato ddmmaaaa*/
    int tempo;              /*formato hhmm*/
    int duracao;            /*em minutos, entre 1 e 1440*/
    int sala;               /*inteiro de 1 a 10*/
    char responsavel[MAXCHAR];
    char participantes[PARTICIPANTES][MAXCHAR];

    /* valores extra que sao usados ao longo do programa, de modo a facilitar algumas funcoes */

    int n_participantes;    /* numero de participantes do respetivo evento */
    int inicio;             /* inicio do evento em minutos */
    int fim;                /* fim do evento em minutos*/
    int data;               /* dia e tempo do evento no formato ammddhhmm -> toma-se 2019 como ano 0,
                                visto que nenhum evento pode ser agendado para uma data anterior a 2019 */

} Evento;


/* vetores globais, visto que sao utilizados e modificados por um grande numero de funcoes */

Evento eventos[SALAS][MAXPORSALA] = {0};    /* tabela de eventos por sala */
int ocup_sala[10] = {0};                    /* guarda a ocupação de cada sala */ 


void funcao_a();
Evento le_input_a();
Evento calcula_variaveis_extra(Evento e, int ultimo_participante);


int incompatibilidade_sala(Evento e);
int incompatibilidade_pessoa(Evento e, char pessoa[MAXCHAR]);
void eventos_por_sala(Evento eventos[MAXPORSALA], int n_eventos);


int main () {
    char c;
    int sala;
    for (;;) 
        switch (c = getchar()) {
            case 'a':
                funcao_a();
                break;
            case 's':
                scanf("%d", &sala);
                eventos_por_sala(eventos[sala], ocup_sala[sala]);
                break;
            case 'x':
                return 0;
        }
}

void funcao_a() {
    int i, adiciona = 0;
    Evento e = le_input_a();
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

Evento le_input_a() {
    Evento e;
    char c;
    int i = 0, j = 0;
    c = getchar();      /* o primeiro caracter a ser lido e um espaco, vamos ignorar esse espaco */
    while ((c = getchar()) != ':') e.descricao[i++] = c;
    e.descricao[i] = '\0';
    scanf("%d:%d:%d:%d:", &e.dia, &e.tempo, &e.duracao, &e.sala);
    i = 0;
    while ((c = getchar()) != ':') e.responsavel[i++] = c;
    e.responsavel[i] = '\0';
    i = 0;
    while ((c = getchar()) != '\n') {
        if (c == ':') {
            e.participantes[j][i] = '\0';
            i = 0;
            j++;
        }
        else e.participantes[j][i++] = c;
    }
    e.participantes[j][i] = '\0';
    return calcula_variaveis_extra(e, j);
}

Evento calcula_variaveis_extra(Evento e, int ultimo_participante) {
    /* calcular valor das restantes variaveis da struct Evento */

    e.n_participantes = ultimo_participante + 1;
    e.inicio = (e.tempo/100 * 60 + e.tempo%100);
    e.fim = e.inicio + e.duracao;
    e.data = e.tempo + (e.dia/1000000)*10000 + ((e.dia/10000)%100)*1000000 + ((e.dia%10000)-2019)*100000000; /* consideramos que 2019 e o ano 0 */
    return e;
}

int incompatibilidade_sala(Evento e) {
    int evento, incompatibilidade = 0;
    for (evento = 0; evento < ocup_sala[e.sala]; evento++) {
        if (eventos[e.sala][evento].dia == e.dia) {
            if (eventos[e.sala][evento].inicio <= e.inicio && eventos[e.sala][evento].fim > e.inicio) incompatibilidade++;
            if (e.inicio <= eventos[e.sala][evento].inicio && e.fim > eventos[e.sala][evento].inicio) incompatibilidade++;
        }
    }
    if (incompatibilidade) printf("Impossivel agendar evento %s. Sala%d ocupada.\n", e.descricao, e.sala);
    return incompatibilidade;
}

int incompatibilidade_pessoa(Evento e, char pessoa[MAXCHAR]) {
    int sala, evento, i;
    for (sala = 0; sala < SALAS; sala++) {
        for (evento = 0; evento < ocup_sala[sala]; evento++) {
            if (!strcmp(pessoa,eventos[sala][evento].responsavel)) { 
                if (eventos[sala][evento].dia == e.dia) {
                    if (eventos[sala][evento].inicio <= e.inicio && eventos[sala][evento].fim > e.inicio) return 1;
                    if (e.inicio <= eventos[sala][evento].inicio && e.fim > eventos[sala][evento].inicio) return 1;
                }
            }
            for (i = 0; i < 3; i++) 
                if (!strcmp(pessoa, eventos[sala][evento].participantes[i])) {
                    if (eventos[sala][evento].dia == e.dia) {
                        if (eventos[sala][evento].inicio <= e.inicio && eventos[sala][evento].fim > e.inicio) return 1;
                        if (e.inicio <= eventos[sala][evento].inicio && e.fim > eventos[sala][evento].inicio) return 1;
                    }
                }
        }
    }
    return 0;
}
/*
int incompatibilidade_sala(Evento e) {
    int evento, incompatibilidade = 0;    incompatibilidade -> tem o valor 1 caso a sala ja esteja ocupada 
    for (evento = 0; evento < ocup_sala[e.sala]; evento++) {
        eventos_sobrepostos(e, e.sala, i);
    }
    if (incompatibilidade) printf("Impossivel agendar evento %s. Sala%d ocupada.\n", e.descricao, e.sala);
    return incompatibilidade;
}

int incompatibilidade_pessoa(Evento e, char pessoa[MAXCHAR]) {
    int sala, evento, i, incompatibilidade = 0;  incompatibilidade -> tem o valor 1 caso a pessoa ja esteja ocupada 
    for (sala = 0; sala < SALAS; sala++) {
        for (evento = 0; evento < ocup_sala[sala]; evento++) {
            if (!strcmp(pessoa,eventos[sala][evento].responsavel)) { 
                if (eventos[sala][evento].dia == e.dia) {
                    if (eventos[sala][evento].inicio <= e.inicio && eventos[sala][evento].fim > e.inicio) return 1;
                    if (e.inicio <= eventos[sala][evento].inicio && e.fim > eventos[sala][evento].inicio) return 1;
                }
            }
            for (i = 0; i < 3; i++) 
                if (!strcmp(pessoa, eventos[sala][evento].participantes[i])) {
                    if (eventos[sala][evento].dia == e.dia) {
                        if (eventos[sala][evento].inicio <= e.inicio && eventos[sala][evento].fim > e.inicio) return 1;
                        if (e.inicio <= eventos[sala][evento].inicio && e.fim > eventos[sala][evento].inicio) return 1;
                    }
                }
        }
    }
    return 0;
}

int eventos_sobrepostos(Evento e, int sala, int evento) {
    if (eventos[sala][evento].dia == e.dia) {
        if (eventos[isala][evento].inicio <= e.inicio && eventos[sala][evento].fim > e.inicio) incompatibilidade++;
        if (e.inicio <= eventos[sala][evento].inicio && e.fim > eventos[sala][evento].inicio) incompatibilidade++;
    }
    return incompatibilidade;
}
*/

void eventos_por_sala(Evento eventos[MAXPORSALA], int n_eventos) {
    int i, j, min;
    Evento temp;    /* guarda temporariamente um evento */
    for (i = 0; i < n_eventos; i++) {
        min = i;
        for (j = i + 1; j < n_eventos; j++) {
            min = (eventos[i].data < eventos[min].data) ? i : min;
            if (eventos[min].data == eventos[i].data) min = (eventos[i].data < eventos[min].data) ? i : min;
        }
        temp = eventos[min], eventos[min] = eventos[i], eventos[i] = temp;
    }
    for (i = 0; i < n_eventos; i++) {
        printf("%s %d %d %d Sala%d %s\n* ", eventos[i].descricao, eventos[i].dia, eventos[i].tempo, eventos[i].duracao, eventos[i].sala, eventos[i].responsavel);
        for (j = 0; j < eventos[i].n_participantes; j++) {
            printf("%s", eventos[i].participantes[j]);
            if (j + 1 != eventos[i].n_participantes) putchar(' ');
        }
        putchar('\n');
    }
}



/*                          */
                /* apagar proximas 2 linhas */
                /*                          */
                /*
                printf("descricao: %s\ndia: %i\ntempo: %i\nduracao: %i\nsala: %i\nresponsavel: %s\nparticipantes:\n", e.descricao, e.dia, e.tempo, e.duracao, e.sala, e.responsavel);
                printf("%d\n", e.n_participantes);
                for (i=0; i < e.n_participantes; i++) printf("%s\n", e.participantes[i]); 
                printf("%d\n",e.fim);  */


/*
long long dt;  dia + hora do evento no formato aaaammddhhmm 


long long ano;

ano = e.dia%(10000)*(100000000);
    printf("%lld\n", ano);
    e.dt = e.tempo + (e.dia/1000000)*(10000) + ((e.dia/(10000))%100)*(1000000) + (e.dia%(10000)*(100000000));
*/


/* determinar a posicao em que o evento vai ser guardado, para fique guardado de forma cronologica  
                for (i = 0; i < ocup_sala[e.sala]; i++) {
                    if (e.dia == eventos[e.sala][i].dia) {
                        for (;i < ocup_sala[e.sala]; i++) {

                        }
                    }
                    if (e.dia < eventos[e.sala][i].dia) {
                        p = i;
                        break;
                    }
                }
                mover todos os eventos a seguir a este uma posicao para a frente 
                for (i = ocup_sala[e.sala]; i >= p; i--) eventos[e.sala][i] = eventos[e.sala][i-1];
                eventos[e.sala][i] = e;
                */
