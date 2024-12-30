#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

enum {
  ROWS = 22,
  COLS = 10,
  N_BLOCKS = 4,
  MAX_ORIENTATION = 4,
  CLOCKWISE = 1,
  UNCLOCKWISE = -1 ,
  N_PIECES = 7 ,
} ;

struct Orientation {
  int ori[N_BLOCKS][2];
};
typedef struct Orientation Orientation ;

struct Piece {
  char name ;
  uint n_oris ;
  int actual_ori ;
  Orientation *orientations[MAX_ORIENTATION] ;
};
typedef struct Piece Piece ;

int valid_rotation(Piece *piece , int r ){
 return r >= piece->n_oris || r < 0 ;
}

int set_rotation( Piece *piece , int r ) {

  if ( valid_rotation(piece,r) ) {
    return 0 ;
  }
  piece->actual_ori = r ;
  return 1 ;
}

void rotate(Piece *piece , int rotation ){

  piece->actual_ori = piece->actual_ori + rotation ;

  if ( piece->actual_ori == piece->n_oris ) {
     piece->actual_ori = 0 ;
  }

  if ( piece->actual_ori < 0  ) {
     piece->actual_ori = piece->n_oris -1  ;
  }


}

void reset_rotation(Piece *piece ){
   piece->actual_ori = 0 ;
}

void printPiece(Piece *piece){

   int index , i , j ;
   Orientation *ori ;

   int pieceBoard[5][5] ;
   memset(pieceBoard, 0, sizeof(pieceBoard));

   index = piece->actual_ori ;
   ori = piece->orientations[index] ;

   for ( i = 0 ; i < N_BLOCKS ; i++ ){
       pieceBoard[ori->ori[i][0]+2][ori->ori[i][1]+2] = 1 ;
   }

   fprintf(stderr,"   ");
   for (i = 0; i < 5; i++) {
       fprintf(stderr,"%d ", i);
   }
   fprintf(stderr,"\n");

   for (i = 0; i < 5; i++) {
       fprintf(stderr,"%2d|", i-2);
       for (j = 0; j < 5; j++) {
           if (pieceBoard[j][i] == 0) {
               fprintf(stderr,"  ");
           } else {
               fprintf(stderr,"██");
           }
       }
       fprintf(stderr,"|\n");
   }


}

// T
Orientation Tu = { .ori = { { -1,  0 }, {  0,  0 }, {  1,  0 }, {  0, -1 } } };  // Up
Orientation Tr = { .ori = { {  0, -1 }, {  0,  0 }, {  1,  0 }, {  0,  1 } } };  // Right
Orientation Td = { .ori = { { -1,  0 }, {  0,  0 }, {  1,  0 }, {  0,  1 } } };  // Down
Orientation Tl = { .ori = { {  0, -1 }, { -1,  0 }, {  0,  0 }, {  0,  1 } } };  // Left
Piece T = { .name = 'T', .n_oris = 4, .actual_ori = 0, .orientations = {&Td, &Tl, &Tu, &Tr} };

// J
Orientation Ju = { .ori = { { -1, -1 }, { -1,  0 }, {  0,  0 }, {  1,  0 } } };  // Up
Orientation Jr = { .ori = { {  0, -1 }, {  1, -1 }, {  0,  0 }, {  0,  1 } } };  // Right
Orientation Jd = { .ori = { { -1,  0 }, {  0,  0 }, {  1,  0 }, {  1,  1 } } };  // Down
Orientation Jl = { .ori = { {  0, -1 }, {  0,  0 }, { -1,  1 }, {  0,  1 } } };  // Left

Piece J = { .name = 'J', .n_oris = 4, .actual_ori = 0, .orientations = {&Jd, &Jl, &Ju, &Jr} };

// Z
Orientation Zh = { .ori = { { -1,  0 }, {  0,  0 }, {  0,  1 }, {  1,  1 } } };  // Horizontal (spawn)
Orientation Zv = { .ori = { {  1, -1 }, {  0,  0 }, {  1,  0 }, {  0,  1 } } };  // Vertical
Piece Z = { .name = 'Z', .n_oris = 2, .actual_ori = 0, .orientations = {&Zh, &Zv} };

// O
Orientation O_ = { .ori = { { -1,  0 }, {  0,  0 }, { -1,  1 }, {  0,  1 } } };  // (spawn)
Piece O = { .name = 'O', .n_oris = 1, .actual_ori = 0, .orientations = {&O_} };

// S
Orientation Sh = { .ori = { {  0,  0 }, {  1,  0 }, { -1,  1 }, {  0,  1 } } };  // Horizontal (spawn)
Orientation Sv = { .ori = { {  0, -1 }, {  0,  0 }, {  1,  0 }, {  1,  1 } } };  // Vertical
Piece S = { .name = 'S', .n_oris = 2, .actual_ori = 0, .orientations = {&Sh, &Sv} };

// L
Orientation Lr = { .ori = { { 0, -1 }, {  0,  0 }, {  0,  1 }, {  1,  1 } } };  // Right
Orientation Lu = { .ori = { {  1, -1 }, { -1,  0 }, {  0,  0 }, {  1,  0 } } };  // Up
Orientation Ld = { .ori = { { -1,  0 }, {  0,  0 }, {  1,  0 }, { -1,  1 } } };  // Down (spawn)
Orientation Ll = { .ori = { { -1, -1 }, {  0, -1 }, {  0,  0 }, {  0,  1 } } };  // Left
Piece L = { .name = 'L', .n_oris = 4, .actual_ori = 0, .orientations = {&Ld, &Ll, &Lu, &Lr} };

// I
Orientation Iv = { .ori = { {  0, -2 }, {  0, -1 }, {  0,  0 }, {  0,  1 } } };  // Vertical
Orientation Ih = { .ori = { { -2,  0 }, { -1,  0 }, {  0,  0 }, {  1,  0 } } };  // Horizontal (spawn)
Piece I = { .name = 'I', .n_oris = 2, .actual_ori = 0, .orientations = {&Ih, &Iv} };

Piece *pieces[N_PIECES] = {&T,&J,&Z,&O,&S,&L,&I};
char *pieces_indice = "TJZOSLI";

int get_piece_indice(char c){
    int i;
    for (i = 0 ; i < N_PIECES ; i++ ){
         if ( pieces_indice[i] == c ) return i ;
    }
    return -1 ;
}

// ----------------------------------

struct Board {
  uint board_[ROWS][COLS];
};
typedef struct Board Board ;

void copyBoard(Board *copy, Board *board) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            copy->board_[i][j] = board->board_[i][j];
        }
    }
}

void boardToString(Board *board, char *result) {

    int pos = 0;
    for (int i = 0; i < ROWS - 2; i++) {
        for (int j = 0; j < COLS; j++) {

          if  (board->board_[i+2][j] == 0 ) {
            result[pos] =  '0';
          } else{
            result[pos] =  '1';
          }
          pos++;

        }
    }
    result[pos] = '\0'; // Agregar el carácter nulo al final
}

void reset ( Board *board ){

    int i , j ;

    for (i = 0 ; i < ROWS ; i++) {
      for (j = 0 ; j < COLS ; j++) {
          board->board_[i][j] = 0 ;
      }
    }

}

void setRandom ( Board *board ){

   srand(time(NULL));

    int i , j ;

    for (i = 18 ; i < ROWS   ; i++) {
      for (j = 0 ; j < COLS  ; j++) {

          board->board_[i][j] = rand() % 2;
      }
    }

}

void setBoard( Board *board  , char *data ){
  int i , j ,index;
  index = 0 ;
  for (i = 0; i < ROWS - 2 ; i++) {
        for (j = 0; j < COLS; j++) {

            if (data[index++] == '0'){
               board->board_[i+2][j] = 0 ;
            }else {
               board->board_[i+2][j] = 1 ;
            }


        }
    }
}

void printBoard(Board *board) {
    int i, j;

    fprintf(stderr,"   ");
    for (i = 0; i < COLS; i++) {
        fprintf(stderr,"%d ", i);
    }
    fprintf(stderr,"\n");

    for (i = 0; i < ROWS; i++) {
        fprintf(stderr,"%2d|", i-2);
        for (j = 0; j < COLS; j++) {
            if (board->board_[i][j] == 0) {
                fprintf(stderr,"  ");
            } else {
                fprintf(stderr,"██");
            }
        }
        fprintf(stderr,"|\n");
    }
}

int position_on_board(int x , int y){

  int cond_x , cond_y ;

  cond_x = x < COLS && x >= 0 ;
  cond_y = y+2 < ROWS && y+2 >= 0;

  return cond_x && cond_y ;
}

int has_block(Board *board , int x , int y) {
  return board->board_[y+2][x] == 1;
}

int can_add_block(Board *board , int x , int y){
   return position_on_board(x,y) && ! has_block(board, x,y) ;
}

int add_block(Board *board , int x , int y){
  int can_add ;
  can_add =  can_add_block(board,x,y);
  if ( can_add ) board->board_[y+2][x] = 1 ;
  return can_add ;
}

int remove_block(Board * board , int x , int y){
   int can_remove ;
   can_remove = has_block(board,x,y) ;
   board->board_[y+2][x] = 0  ;
   return can_remove ;
}

int can_add_piece(Board *board , Piece *piece , int x , int y ){

  int i , index , piece_board_x , piece_board_y , board_x , board_y ;
  Orientation *ori ;

  if (!position_on_board(x,y) ) return 0 ;

  index = piece->actual_ori ;
  ori = piece->orientations[index] ;

  for ( i = 0 ; i < N_BLOCKS ; i++ ){

      piece_board_x = ori->ori[i][0] ;
      piece_board_y = ori->ori[i][1] ;

      board_x = piece_board_x  + x ;
      board_y = piece_board_y  + y ;

      if ( !position_on_board(board_x,board_y) || !can_add_block(board,board_x,board_y) ){
         return 0 ;
      }

  }

  return 1 ;
}

int add_piece(Board *board , Piece *piece , int x , int y ){

    if ( ! can_add_piece( board , piece , x , y ) ){
      return 0 ;
    }

    int i , index , piece_board_x , piece_board_y , board_x , board_y ;
    Orientation *ori ;

    index = piece->actual_ori ;
    ori = piece->orientations[index] ;

    for ( i = 0 ; i < N_BLOCKS ; i++ ){

        piece_board_x = ori->ori[i][0] ;
        piece_board_y = ori->ori[i][1] ;

        board_x = piece_board_x  + x ;
        board_y = piece_board_y  + y ;

        add_block(board,board_x,board_y);
    }

    return 1 ;

}


// ------------- Evaluate Board --------------

int hight_block (  Board *board  , int x ) {

  int y ;

  for ( y = 0 ; y < 20 ; y++ ) {
      if ( has_block(board,x,y)) {
         break;
      }
  }
  y-- ;
  return (19 - y) ;

}

int sum_hight_block( Board *board ){

    int x , sum  , hight_block_ ;

    sum = 0 ;
    //fprintf(stderr,"  ");

    for ( x = 0 ; x < COLS ; x++) {
      hight_block_ = hight_block(board,x) ;
      sum += hight_block_ ;
      //fprintf(stderr," %d", hight_block_ );
    }

    return sum ;
}


int is_line(Board *board , int y ) {
    int x ;

    for (x = 0 ; x < COLS ; x++ ) {
        if ( ! has_block(board,x,y) ) return 0 ;
    }

    return 1 ;
}

int count_lines(Board *board){
   int y,sum ;
   sum = 0;
   for ( y = 0 ; y < ROWS -2 ; y++ ){
       if ( is_line(board,y) ) { sum++ ; }
   }
   return sum ;
}


int has_any_block_up(Board *board , int x , int y ){

    int j ;

    for ( j = y - 1 ; j >= 0  ; j-- ){
         if ( has_block(board,x,j) ) return 1 ;
    }

    return 0 ;

}

int is_hole(Board *board , int x , int y ) {

    return  !has_block(board,x,y) && has_any_block_up(board,x,y)  ;
}

int count_holes(Board *board) {

  int i , j , sum ;

  sum = 0;

  for (i = 0 ; i < ROWS - 2 ; i++) {
    for (j = 0 ; j < COLS ; j++) {
         if (is_hole(board,j,i)) { sum++ ; }
    }
  }
  return sum ;
}


int abs (int x) {
   if ( x >= 0 ){
     return x ;
   }else{
     return x * -1 ;
   }
}

int compute_bulpines(Board *board){

   int hight_blocks[COLS] ;
   int x , hight  , sum ;

   for ( x = 0 ; x < COLS ; x++) {
     hight = hight_block(board,x) ;
     hight_blocks[x] = hight ;
   }

   sum = 0;

   for (x = 0 ; x < COLS -1 ; x++) {
        sum += abs( hight_blocks[x] - hight_blocks[x+1] );
   }

   return sum ;

}

float evauate(Board *board){

    int aggregate_height = sum_hight_block(board) ;
    int lines_count = count_lines(board);
    int holes_count = count_holes(board);
    int bulpines = compute_bulpines(board) ;

    float score = -0.510066 * aggregate_height +  0.760666 * lines_count + -0.35663 * holes_count + -0.184483 * bulpines ;

    //fprintf(stderr,"Aggregate_height %d , Lines_count %d  , Count_holes %d , Bulpines %d ",aggregate_height,lines_count,holes_count,bulpines);
    //fprintf(stderr,"%.2f \n",score);

    return score ;
}

// -------------------------------------------

struct State {
   int x ;
   int y ;
   int rotation ;
   struct State * next ;
};

typedef struct State State ;

void printState(State *s) {
  fprintf(stderr,"X : %d , Y : %d , R : %d \n",s->x ,s->y ,s->rotation );
}

int get_all_terminate_states( Board *board , char *board_str , Piece *piece  , State *states ) {

  int i , j ;
  State *previous , *actual ;

  previous = states ;

  for ( j = 0 ; j < piece->n_oris ; j++ ){
   for ( i = 0 ; i < COLS ; i++ ){

   if (can_add_piece(board,piece,i,0) ){

        // bajar
        int  a = 0;
        while ( 1 ) {
          if ( ! can_add_piece(board,piece,i,a) ) break;
          a++;
        }
        add_piece(board,piece,i,--a);

        //printBoard(board);
        //evauate(board);

        actual  = (State*) malloc(sizeof(State));
        memset(actual, 0 , sizeof(State) );

        previous->next = actual ;
        actual->x = i ;
        actual->y = a ;
        actual->rotation = piece->actual_ori ;
        actual->next = NULL ;

        previous = actual ;

        setBoard(board,board_str);

     }
   }
   rotate(piece,CLOCKWISE);
  }

  return 0 ;
}

void set_board_state(Piece *piece , State *st , Board *board ) {

     set_rotation(piece,st->rotation);
     add_piece(board,piece,st->x,st->y);

}


struct Secuence {
   int rot_n_dir ;
   int x_move ;
};
typedef struct Secuence Secuence ;

void print_secuence( Secuence *sec ){
  fprintf(stderr,"Rotacion : %d , Move x : %d \n", sec->rot_n_dir , sec->x_move );
}

int get_minimal_rotation(Piece *piece ,  int end_r ) {

    int i , count_clockwise , count_unclockwise ;

    count_clockwise = 0;
    count_unclockwise = 0;

    if ( end_r >= piece->n_oris  ) return 0 ;

    // CLOCKWISE
    for ( i = 0 ; i < piece->n_oris ; i++ ) {
         rotate(piece,CLOCKWISE);
         count_clockwise++;
         if ( piece->actual_ori == end_r ) break ;

    }

    reset_rotation(piece);

    // UNCLOCKWISE
    for ( i = 0 ; i < piece->n_oris ; i++ ) {
      rotate(piece,UNCLOCKWISE);
      count_unclockwise++;
      if ( piece->actual_ori == end_r ) break ;
    }

    reset_rotation(piece);

    int rota ;

    rota = 0 ;

    if ( abs(count_clockwise) <= abs(count_unclockwise) ) {
       rota = count_clockwise ;
    }else {
       rota = -count_unclockwise ;
    }

    rota = rota % piece->n_oris ;

    if ( rota == -3 ) rota = 1 ;

    if (rota == 3) rota = -1 ;

   //if ( abs(rota) == piece->n_oris ) return 0 ;
   return rota ;


}

void get_secuence(Piece *piece , State *st , Secuence *sec ){

    int rotation ;

    rotation = get_minimal_rotation(piece,st->rotation);


    sec->rot_n_dir = rotation ;

    int diff = st->x-5 ;

    if (diff > 0 ){
      sec->x_move = st->x - 5 ;
    }else{
      sec->x_move = st->x  - 5 ;
    }


}

// -------------------------------------------

void initializeBoard(Board *board) {
    // Limpiar el tablero inicializándolo con ceros
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            board->board_[i][j] = 0;
        }
    }

    // Alturas de las columnas según la imagen
    int heights[COLS] = {3, 5, 5, 5, 6, 6, 5, 4,4, 5};

    // Rellenar el tablero con los bloques basados en las alturas
    for (int j = 0; j < COLS; j++) {
        for (int i = ROWS - 1; i >= ROWS - heights[j]; i--) {
            board->board_[i][j] = 1; // Colocar bloque
        }
    }
}

// -------------------------------------------

int main (int argc , char **argv ) {

 char board_str[200];
 char board_str2[200];
 char actual_chr , next_chr;
 int actual_i , next_i ;
 Board board , board2 ;
 State init , *actual , *best1 , *best2 , init2 , *actual2 ;
 float best_score , score ;
 Secuence secuence ;

 argv++;
 argc--;

 if (argc != 3 ) exit(EXIT_FAILURE) ;

 reset(&board);
 reset(&board2);

 strcpy(board_str,argv[2]);
 setBoard(&board,board_str);

 fprintf(stderr,"Actaul %s  Next %s \n", argv[0] , argv[1] );
 printBoard(&board);
 fprintf(stderr,"-------------------------------------------\n");


 actual_chr = argv[0][0];
 next_chr = argv[1][0];
 actual_i = get_piece_indice(actual_chr);
 next_i = get_piece_indice(next_chr);

 init.next = NULL ;

 get_all_terminate_states( &board , board_str , pieces[actual_i]  , &init );

 actual = init.next ;

 best_score = -100000.0f ;

 while ( actual != NULL){

   set_board_state(pieces[actual_i],actual,&board);

   init2.next = NULL ;

   boardToString(&board,board_str2);
   setBoard(&board2,board_str2);
   //printBoard(&board2);

   get_all_terminate_states( &board2 , board_str2 , pieces[next_i]  , &init2 );
   actual2 = init2.next ;

   //printf("############ Jugadas  ######################\n");
   while (actual2 != NULL ){

     set_board_state(pieces[next_i],actual2,&board2);
     //printBoard(&board2);

     score = evauate(&board2);
     if ( score > best_score ){
        best_score = score ;
        best1 = actual;
        best2 = actual2;
        //printBoard(&board2);
     }

     actual2 = actual2->next ;
     setBoard(&board2,board_str2);
     reset_rotation(pieces[next_i]);

   }

   actual2 = init2.next ;
   State *pre ;

   while ( actual2 != NULL){
     pre = actual2;
     actual2 = actual2->next ;
     free(pre);
   }

   //printf("###########################################\n");


   actual = actual->next ;
   setBoard(&board,board_str);
   reset_rotation(pieces[actual_i]);

 }

 set_board_state(pieces[actual_i],best1,&board);
 get_secuence(pieces[actual_i],best1,&secuence);

 printBoard(&board);
 printState(best1);
 print_secuence(&secuence);

 fprintf(stderr,"Score : %f \n", evauate(&board) );
 printf("(%d,%d)",secuence.rot_n_dir,secuence.x_move);

 actual = init.next ;
 State *pre ;

 while ( actual != NULL){
   pre = actual;
   actual = actual->next ;
   free(pre);
 }

 exit(EXIT_SUCCESS);

}
