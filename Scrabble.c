#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define ESCAPE 0x1B
#define FONT_BLACK 0x1E
#define FONT_WHITE 0x25

#define BG_BLACK 0x28
#define BG_RED 0x29
#define BG_YELLOW 0x2B
#define BG_BLUE 0x2C
#define BG_MAGENTA 0x2D
#define BG_CYAN 0x2E
#define BG_WHITE 0x2F

#define WHT 100
#define RED 101
#define LBL 102
#define PNK 103
#define BLU 104
#define YEL 105

#define MAX_ROWS 17
#define MAX_COLS 17
#define BOARD_SIZE 15
#define MAX_STRING 1000

/******************ESTRUTURAS*****************/
typedef struct word /*todas as estruturas vao ser chamadas pela game, logo não precisam de ser declaradas no main*/
{ char name[16];
  char orientation;
  int row;
  char col;
  int player;
  int points;
} WORD;

typedef struct piece
{ char letter;
  int points;
} PIECE;

typedef struct player
{ char name[20];
  int points;
  PIECE hand[7];
} PLAYER;

typedef struct game
{ WORD words[50];        /*inicializada*/
  PLAYER players[2];    /*inicializada*/
  PIECE pool[115];       /*inicializada*/
  char board[BOARD_SIZE][BOARD_SIZE];   /*inicializada*/
  char dictionary[27718][16];  /*inicializada*/
  char turn;                   /*inicializada*/
  int word_count;          /*inicializada*/
} GAME;
	
/****************FUNCOES**************/
int menu_principal (int i, GAME *game){
	
	int opcao=0;
	do{
		printf("\n	========BEM-VINDO AO SCRABBLE=========\n");
		printf("	||    O que pretende fazer?         ||\n");
		printf("	||    1 - Novo jogo                 ||\n");
		printf("    	||    2 - Continuar jogo            ||\n");
		printf("	||    3 - Carregar jogo             ||\n");
		printf("	||    4 - Sair do jogo              ||\n");
		printf("    	||    5 - Regras do jogo            ||\n");
		printf("	==============BOM JOGO!===============\n");
		printf("Opção: ");
		scanf("%d", &opcao);
		fflush(stdin);
		switch(opcao) {
			case 1: novojogo(game); break;
			case 2: jogo_corrente(game, i); break;
			case 3: carregarjogo(game); break;
			case 4: printf("Obrigado por jogar!\n"); exit(1); break;
			case 5: regras(); break;
			default: printf("Opção inválida. Tente novamente.\n"); break;
		}
	}
	while(opcao != 3);
}

int regras(){ /*apesar de nao retornar um valor esta declarada como int porque dava erro de compilaçao*/
	printf("Bem-vindo ao menu de regras do Scrabble\n");
	printf("Regra número 1: As palavras só podem estar orientadas na vertical (para tal insira 'v') ou horizontal (para tal insira 'h'), sempre da esquerda para a direita e de cima para baixo.\n");
	printf("Regra número 2: Casas especiais só são contabilizadas na primeira palavra sobre as mesmas.\n");
	printf("Regra número 3: A casa central não tem pontuação especial.\n");
	printf("Regra número 4: Se a palavra introduzida utilizar letras já existentes no tabuleiro, os pontos dessas letras também são contabilizados.\n");
	printf("Regra número 5: Não existe peça branca.\n");
	printf("Regra número 6: As letras já colocadas no tabuleiro nunca podem ser substituídas.\n");
	printf("Regra número 7: Um jogador pode trocar apenas uma das letras que tem na mão.\n");
	printf("Regra número 8: A primeira jogada do jogo terá de utilizar a casa central do tabuleiro (c = 'H' e r = '8')\n");
	printf("BOA-SORTE!!!!!!!\n");
}
	
int menu_jogo (int i, GAME *game){

	int option;
    do {    
        printf("\n	==========================================\n");
        printf("	||    O que pretende fazer?             ||\n");
		printf("	||    1 - Escrever palavra              ||\n");
		printf("	||    2 - Passar a vez                  ||\n");
        printf("    	||    3 - Mostrar histórico de palavras ||\n");
        printf("    	||    4 - Voltar ao menu principal      ||\n");
		printf("    	||    5 - Guardar jogo                  ||\n");
        printf("	||    6 - Sair do jogo                  ||\n");
        printf("	==========================================\n");
		printf("Opção: ");
        scanf("%d", &option);
		fflush(stdin);
		switch(option){
			case 1: escrever_palavra(game, i); break;
			case 2: passar_vez(game); break;
            case 3: mostrar_historico(game); break;
			case 4: voltar_menu(game, i); break;
			case 5: guardarjogo(game); break;
            case 6: printf("Obrigado por jogar!\n"); exit(1);; break;
			default: printf("Opção inválida. Tente novamente.\n"); break;
	    }
	}
	while (!(option == 1 || option == 2 || option == 3 || option == 4 || option == 5 || option == 6));
	return option;
}

/********************TABULEIRO**************/
static void show_board(GAME game){	
	
	int i,j,row,col,orient;
	int colour,letter1,letter2;
	int board[17][17] = {{-1,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,-1},
						 {1,RED,WHT,WHT,LBL,WHT,WHT,WHT,RED,WHT,WHT,WHT,LBL,WHT,WHT,RED,1},
						 {2,WHT,PNK,WHT,WHT,WHT,BLU,WHT,WHT,WHT,BLU,WHT,WHT,WHT,PNK,WHT,2},
						 {3,WHT,WHT,PNK,WHT,WHT,WHT,LBL,WHT,LBL,WHT,WHT,WHT,PNK,WHT,WHT,3},
						 {4,LBL,WHT,WHT,PNK,WHT,WHT,WHT,LBL,WHT,WHT,WHT,PNK,WHT,WHT,LBL,4},
						 {5,WHT,WHT,WHT,WHT,PNK,WHT,WHT,WHT,WHT,WHT,PNK,WHT,WHT,WHT,WHT,5},
						 {6,WHT,BLU,WHT,WHT,WHT,BLU,WHT,WHT,WHT,BLU,WHT,WHT,WHT,BLU,WHT,6},
						 {7,WHT,WHT,LBL,WHT,WHT,WHT,LBL,WHT,LBL,WHT,WHT,WHT,LBL,WHT,WHT,7},
						 {8,RED,WHT,WHT,LBL,WHT,WHT,WHT,YEL,WHT,WHT,WHT,LBL,WHT,WHT,RED,8},
						 {9,WHT,WHT,LBL,WHT,WHT,WHT,LBL,WHT,LBL,WHT,WHT,WHT,LBL,WHT,WHT,9},
						 {10,WHT,BLU,WHT,WHT,WHT,BLU,WHT,WHT,WHT,BLU,WHT,WHT,WHT,BLU,WHT,10},
						 {11,WHT,WHT,WHT,WHT,PNK,WHT,WHT,WHT,WHT,WHT,PNK,WHT,WHT,WHT,WHT,11},
						 {12,LBL,WHT,WHT,PNK,WHT,WHT,WHT,LBL,WHT,WHT,WHT,PNK,WHT,WHT,LBL,12},
						 {13,WHT,WHT,PNK,WHT,WHT,WHT,LBL,WHT,LBL,WHT,WHT,WHT,PNK,WHT,WHT,13},
						 {14,WHT,PNK,WHT,WHT,WHT,BLU,WHT,WHT,WHT,BLU,WHT,WHT,WHT,PNK,WHT,14},
						 {15,RED,WHT,WHT,LBL,WHT,WHT,WHT,RED,WHT,WHT,WHT,LBL,WHT,WHT,RED,15},
						 {-1,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,-1}};	

	char game_board[BOARD_SIZE][BOARD_SIZE];
	for(i=0;i<BOARD_SIZE;i++)
	{
		for(j=0;j<BOARD_SIZE;j++)
			game_board[i][j] = ' ';
	}

	for(i=0;i < game.word_count;i++)
	{
		row = game.words[i].row - 1;
		col = game.words[i].col - 'A';
		orient = game.words[i].orientation;	

		for(j=0;game.words[i].name[j] != '\0';j++)
		{
			game_board[row][col] = game.words[i].name[j];
			if(orient == 'v')
				row++;
			else if(orient == 'h')
				col++;
		}		
	}	
	
	for(row = 0;row < MAX_ROWS;row++)
	{
		for(col = 0;col < MAX_COLS;col++)
		{
			switch(board[row][col]){	
				case WHT: colour = BG_WHITE; break;
				case RED: colour = BG_RED; break;
				case LBL: colour = BG_CYAN; break;
				case PNK: colour = BG_MAGENTA; break;
				case BLU: colour = BG_BLUE; break;
				case YEL: colour = BG_YELLOW; break;
				default: colour = BG_BLACK; break;
			}
			if((row == 0 &&  col == 0) || row == 0 &&  col == 16 || row == 16 &&  col == 0 || row == 16 &&  col == 16){
				printf("%c[%d;%dm %c%c %c[%dm",ESCAPE,FONT_WHITE,colour,' ',' ', ESCAPE, 0);	
			}else if((row == 0 || row == 16) && (col!= 0 || col!= 16)){
				letter1 = board[row][col];		
				printf("%c[%d;%dm %c %c[%dm",ESCAPE,FONT_WHITE,colour,letter1, ESCAPE, 0);
			}else if((col == 0 || col == 16) && (row != 0 || row != 16)){				
				if(board[row][col] > 9)
				{
					letter1 = 1 + '0';
					letter2 = board[row][col]%10;
					letter2 = letter2 + '0';
				}else{
					letter2 = board[row][col] + '0';
					letter1 = ' ';
				}									
				printf("%c[%d;%dm %c%c %c[%dm",ESCAPE,FONT_WHITE,colour,letter1,letter2, ESCAPE, 0);							
			}else{
				letter1 = ' ';
				if(game_board[row-1][col-1]!=' '){
					letter1 = game_board[row-1][col-1];
				}		
				if(letter1 != ' ')
					colour = BG_YELLOW;		
				printf("%c[%d;%dm %c %c[%dm",ESCAPE,FONT_BLACK,colour,letter1, ESCAPE, 0);	
			}			
		}
		putchar('\n');		
	}	
	putchar('\n');	
}

void inic_pool(GAME *game){

	FILE *fp;
	int xletras, pontos;
	char letra;
	int i=0;
	PIECE *ptr = game->pool; /*estava a dar erro sem renomear*/
	
	fp = fopen("/home/student/Desktop/scrabble/letters.txt", "r");/*abertura de ficheiro para leitura*/

	if (fp == NULL){
		printf("Erro na leitura do ficheiro.\n");
		exit(1); /*mensagem de erro*/
	}

	for ( ; fscanf( fp, "%d;%c;%d\n", &xletras,  &letra, &pontos) != EOF; ){
		for (i=0; i<xletras; i++){
			ptr->letter = letra;
			ptr->points = pontos;
			ptr++;
		}
	}
	
	fclose (fp);
}

char gen_cards(){
	
	int r;
    char random;

    r = rand()%26;     /*dá random de um inteiro e transforma em char*/
	random = 'A' + r;
		
    return random;
}

void puthand(GAME *game, int player){ /*mostrar as cartas dos jogadores*/

    int j=0;
	
	if (player == 0){   
		for (j=0; j<7;j++)
			printf("%c", game->players[player].hand[j].letter);
	}
	
	if (player == 1){
		for (j=0; j<7;j++)
			printf("%c", game->players[player].hand[j].letter);
	}

    putchar('\n');
}

int novojogo(GAME *game){ /*reinicializaçao da estrutura game*/

	int i,j;
	game->turn = '1';
	game->word_count= 0;
	
	for(i=0; i<=1; i++){
		printf("Insira o nome do jogador %d: ", i+1);
		scanf("%s", game->players[i].name);
		game->players[i].points=0;
	}
	
	for(i=0; i<=1; i++){
		for(j=0; j<7;j++)
			game->players[i].hand[j].letter = gen_cards();
	}
	
	for(i=0; i<BOARD_SIZE; i++){  /*inicialização do tabuleiro com espaços em branco*/
		for(j=0;j<BOARD_SIZE;j++)
			game->board[i][j] = ' ';
	}
	
	jogo(game, 0);
} 

typedef char String[MAX_STRING];

void last_line_string(String s, String out_str){

	sscanf(s, "%s", out_str);
}

int last_line_int(String s){

	int a;
	sscanf(s, "%d", &a);
	return a;
}

void processStr(String s, String out_str){

    String a;
    int i = 0;

    sscanf(s, "%[^;]", a); /*scan ate encontrar o caracter ;*/
    for(i = 0; i < (strlen(s) - strlen(a) - 1); i++){
        s[i] = s[strlen(a)+1+i];
    }
	s[strlen(s) - strlen(a) - 1] = '\0';
    sscanf(a, "%s", out_str);
}

char processChar(String s){

    char out_chr;
    String a;
    int i = 0;

    sscanf(s, "%[^;]", a);
    for(i = 0; i < (strlen(s) - strlen(a) - 1); i++){
        s[i] = s[strlen(a)+1+i];
    }
	s[strlen(s) - strlen(a) - 1] = '\0';
    sscanf(a, "%s", &out_chr);
    return out_chr;
}

int processInt(String s){

    String a;
    int out_int;
    int i = 0;

    sscanf(s, "%[^;]", a);
    for(i = 0; i < (strlen(s) - strlen(a) - 1); i++){
        s[i] = s[strlen(a)+1+i];
 	}
	s[strlen(s) - strlen(a) - 1] = '\0';
    sscanf(a, "%d", &out_int);
    return out_int;
}

void load_line(FILE *fp, String buffer){

    fgets(buffer, MAX_STRING, fp);
    buffer[strlen(buffer) - 1] = '\0'; /*termina a string*/
}

void loadVarT2(FILE *fp, GAME *game){

	int i=0;
    String buffer;

	for(i=0; i < game->word_count; i++){
    	load_line(fp, buffer);
		processStr(buffer, game->words[i].name);
		game->words[i].orientation = processChar(buffer);
		game->words[i].row = processInt(buffer);
		game->words[i].col = processChar(buffer);
		game->words[i].points = processInt(buffer);
		game->words[i].player = last_line_int(buffer);
	}
}

void loadVarT1(FILE *fp, GAME *game){

	int i=0, k=0;
    String buffer;
	String word;

	for(i=0; i < 2; i++){
    	load_line(fp, buffer);
    	processStr(buffer, game->players[i].name);
    	game->players[i].points = processInt(buffer);
    	last_line_string(buffer, word);
		for(k = 0; k < 7; k++)
			game->players[i].hand[k].letter = word[k];
	}

	load_line(fp, buffer);
	sscanf(buffer, "%d", &game->word_count);
}

int carregarjogo(GAME *game){

	FILE *fp;
	int x;
	int j;
	int c, l;

	fp = fopen("/home/student/Desktop/scrabble/save.txt", "r");
	
	if (fp == NULL){
		printf("Erro na leitura do ficheiro.\n");
		exit(1);
	}
	
    loadVarT1(fp, game);
	fscanf(fp, "%d", &game->word_count);
	loadVarT2(fp, game);
	fscanf(fp, "%c", &game->turn);
	show_board(*game);

	for(l=0; l<BOARD_SIZE; l++){  /*inicialização do tabuleiro com espaços em branco*/
		for(c=0;c<BOARD_SIZE;c++)
			game->board[l][c] = ' ';
	}

	for (j = 0; j < game->word_count; j++){ /*envia dados para game->board*/
		for(x = 0; x < strlen(game->words[j].name) ; x++){
			if(game->words[j].orientation == 'h'){
				game->board[game->words[j].row - 1][(game->words[j].col - 'A') + x] = game->words[j].name[x];
			}
			if(game->words[j].orientation == 'v'){
				game->board[game->words[j].row + x - 1][(game->words[j].col - 'A')] = game->words[j].name[x];
			}
		}
	}

	jogo(game, game->word_count);
}

int guardarjogo(GAME *game){

	FILE *fp = fopen("save.txt", "w");
	int i=0, j=0, k=0; 

	if (fp == NULL){
		printf("Erro na escrita do ficheiro.\n");
		exit(1);
	}
	while (k < 2){ /*K=linha a ser lida no momento*/
		fprintf(fp, "%s;%d;", game->players[i].name, game->players[i].points);
		while (j < 7){ /*HAND*/
			fprintf(fp, "%c", game->players[i].hand[j].letter);
			j++;
		}
		i++;
		k++;
		j = 0;
		fprintf(fp, "\n");
	}
	
	if (k == 2){ /*linha 3*/
		fprintf(fp, "%d\n", game->word_count);
		k++;
	}
	
	i=0;
	while (k < (game->word_count + 3)){ /*numero de linhas = wordcount + 3, porque é a linha do word_count e as duas linhas iniciais dos nomes e pontos*/
		fprintf(fp, "%s;%c;%d;%c;%d;%d\n", game->words[i].name, game->words[i].orientation, game->words[i].row, game->words[i].col, game->words[i].points, (game->words[i].player + 1));
		i++;
		k++;
	}
   
	if (k == (game->word_count + 3))
		fprintf (fp, "%d\n", (game->turn - '0'));/*turno seguinte*/
	printf ("Jogo guardado com sucesso!\n");
}

void dicionario(GAME *game){ /*inicializaçao do dicionario*/

	int i=0;
	FILE *fp;
	String buffer;
	fp = fopen("/home/student/Desktop/scrabble/words.txt", "r");
	
	if (fp == NULL)
		printf("Erro na leitura do dicionário.\n");
	for (i=0; i<27718 ; i++){
		fgets(buffer, 16, fp);
		buffer[strlen(buffer)] = '\0';
		sscanf(buffer, "%s", game->dictionary[i]);
		}
	fclose (fp);
}

int dicionario_check(GAME *game, int k){

	int cmp;
	int a;
	int i=0;

	for(a=0; a < strlen(game->words[k].name); a++)
		game->words[k].name[a] = toupper(game->words[k].name[a]); /*transformar em maiusculas*/

	for (i=0; i<27718; i++){
		cmp = strcmp (game->dictionary[i], game->words[k].name);
		if (cmp == 0)
			return cmp;
	}
	return cmp;
}

int escrever_palavra(GAME *game, int i){

	while(1)
	{
		printf ("Introduza uma palavra: \n");
		scanf ("%s", game->words[i].name);
		
		if(dicionario_check(game, i) == 0)
			break;
		else
			printf ("A palavra que escreveu não está presente no dicionário.\n");
	}
}

int passar_vez(GAME *game){

	turno(game);
}

int mostrar_historico(GAME *game){
	
	int i=0;
	game->turn = '1';
	for (i=0; i<(game->word_count); i++){
		printf ("%s jogou a palavra %s para um total de %d pontos.\n", game->players[game->turn - '0'-1].name, game->words[i].name, game->words[i].points);
		turno(game);
	}

	turno(game);
}

int turno(GAME *game){ /*alternar vez*/
	
	if (game->turn == '1')
		game->turn = '2';
	else 
		game->turn = '1';
}

int voltar_menu(GAME *game, int a){
	
	menu_principal(a, game);
}

int jogo_corrente(GAME *game, int a){
		
	jogo(game, game->word_count);
}

int encadeamento(GAME *game, int a){ /*obrigar o encadeamento de palavras*/

	int word_size = strlen(game->words[a].name);
	int l = game->words[a].row -1;
	int c = (game->words[a].col - 'A');
	int i, x = 0, k;

	for(i=0; i < word_size; i++){ /*verifica se encontra espaços em branco de casa em casa horizontalmente*/
		if(game->words[a].orientation == 'h'){
			if(game->board[l][c+i] != ' '){
				x++; 
				if(game->words[a].name[i] != game->board[l][c+i])/* se encontra uma letra ve se sao iguais*/
					return 0;
				else
					if((game->board[l][c-1] != ' ') || (game->board[l][c+word_size] != ' ')){					
						return 0;
					}
			}
		}
		else if (game->words[a].orientation == 'v'){ /* mesmo procedimento mas para horizontais*/
			if(game->board[l+i][c] != ' '){
				x++;
				if(game->words[a].name[i] != game->board[l+i][c])
					return 0;
                else
					if((game->board[l-1][c] != ' ') || (game->board[l+word_size][c] != ' '))	 					
						return 0;
			}	
		}
	}

	if(a == 0) /*primeira palavra do jogo*/
		return 1;
	if(strlen(game->words[a].name) == x)
		return 0;
	if(x == 0) /*espaços verificados preenchidos*/
		return 0;
	return 1; /*quando o encadeamento está correto*/
}

int jogo(GAME *game, int i){

	for (; i < 50; i++){ /* 50 - numero maximo de palavras*/
		int z = 0;
		int x;
		int k, j;

		if (game->turn == '1'){
			printf("%s é a sua vez.\n", game->players[0].name);
			printf("Hand: "); puthand(game, 0);
			printf("Tem %d pontos.\n", game->players[0].points);
			z = menu_jogo(i, game); /*restringir situaçao de passar de turno*/
		}
	
		if (game->turn == '2'){
			printf("%s é a sua vez.\n", game->players[1].name);
			printf("Hand: "); puthand(game, 1);
			printf("Tem %d pontos.\n", game->players[1].points);
			z = menu_jogo(i, game);
		}	
	
		if (z == 1){
			printf ("Introduza (o(orientaçao) c(coluna) r(linha)): \n");
			scanf (" %c", &game->words[i].orientation);
			scanf (" %c", &game->words[i].col);
			scanf ("%d", &game->words[i].row);
				
			if (game->word_count == 0){ /*casa central*/
				if (game->words[i].orientation == 'v'){
					while (!((game->words[i].col == 'H') && (strlen(game->words[i].name) > (8 - game->words[i].row)))){
						if(!(((game->words[i].orientation == 'v') || (game->words[i].orientation == 'h')) && ((game->words[i].col >= 'A') && (game->words[i].col <= 'O')) && ((game->words[i].row >= 1) && (game->words[i].row <= 15))))
							printf ("Os dados que introduziu estão errados. Por favor, tente novamente.\n");
						printf ("A primeira jogada tem de incluir a casa central do tabuleiro. Por favor, tente novamente.\n");
						printf ("Introduza (o(orientaçao) c(coluna) r(linha)): \n");
						scanf (" %c", &game->words[i].orientation);
						scanf (" %c", &game->words[i].col);
						scanf ("%d", &game->words[i].row);
					}
				}

				if (game->words[i].orientation == 'h'){
					while (!((game->words[i].row == 8) && (strlen(game->words[i].name) > (('H' - '0') - (game->words[i].col - '0'))))){
						if(!(((game->words[i].orientation == 'v') || (game->words[i].orientation == 'h')) && ((game->words[i].col >= 'A') && (game->words[i].col <= 'O')) && ((game->words[i].row >= 1) && (game->words[i].row <= 15))))
							printf ("Os dados que introduziu estão errados. Por favor, tente novamente.\n");
						printf ("A primeira jogada tem de incluir a casa central do tabuleiro. Por favor, tente novamente.\n");
						printf ("Introduza (o(orientaçao) c(coluna) r(linha)): \n");
						scanf (" %c", &game->words[i].orientation);
						scanf (" %c", &game->words[i].col);
						scanf ("%d", &game->words[i].row);
					}
				}
			}

			if (game->word_count > 0){		
				while (!(((game->words[i].orientation == 'v') || (game->words[i].orientation == 'h')) && ((game->words[i].col >= 'A') && (game->words[i].col <= 'O')) && ((game->words[i].row >= 1) && (game->words[i].row <= 15))) || (encadeamento(game, i) == 0)){
					
					if(!(((game->words[i].orientation == 'v') || (game->words[i].orientation == 'h')) && ((game->words[i].col >= 'A') && (game->words[i].col <= 'O')) && ((game->words[i].row >= 1) && (game->words[i].row <= 15)))){
						printf ("Os dados que introduziu estão errados. Por favor, tente novamente.\n");
						if(encadeamento(game, i) == 0)
							printf ("Os dados que introduziu não são aceitáveis. Dica: A palavra que inserir terá de utilizar uma outra já presente no tabuleiro.\n");
					}
					else 
						if(encadeamento(game, i) == 0)
							printf ("Os dados que introduziu não são aceitáveis. Dica: A palavra que inserir terá de utilizar uma outra já presente no tabuleiro.\n");
					printf ("Introduza (o(orientaçao) c(coluna) r(linha)): \n");
					scanf (" %c", &game->words[i].orientation);
					scanf (" %c", &game->words[i].col);
					scanf ("%d", &game->words[i].row);
				}
			}

		for(x = 0; x < strlen(game->words[i].name); x++){ /*gravaçao das palavras na game->board*/
			if(game->words[i].orientation == 'h')
				game->board[game->words[i].row - 1][(game->words[i].col - 'A') + x] = game->words[i].name[x];
			if(game->words[i].orientation == 'v')
				game->board[game->words[i].row + x - 1][(game->words[i].col - 'A')] = game->words[i].name[x];
		}		
		
		for (j=0; j<strlen(game->words[i].name); j++){ /*contagem dos pontos*/
			for (k=0; k<115; k++){
				if (game->pool[k].letter == game->words[i].name[j]){
					game->words[i].points = game->words[i].points + game->pool[k].points; 
					break;
				}
			}
		}
		game->words[i].player = ((game->turn - '0') - 1);
		game->players[(game->turn - '0') - 1].points = game->players[(game->turn - '0') - 1].points + game->words[i].points;
		
		if (game->players[game->turn - '0'].points >= 100){ /*limite de pontos a atingir*/
			printf ("Parabéns, %s é o/a vencedor/a com um total de %d pontos.\n" , game->players[(game->turn - '0') - 1].name, game->players[(game->turn - '0') - 1].points);
			exit(1);
		}

		game->word_count++;
		show_board(*game);
		turno(game);
		}
	}
}

/****************main**************/
int main(){

int opcao, i;
GAME game;
srand((unsigned) time(NULL));

inic_pool(&game);
dicionario(&game);
menu_principal(i, &game);
}	
