#include <stdlib.h>
#include <time.h>
#include <ncurses.h>

// compilation : gcc snake.c -o snake -lncurses
// passer un argument quelconque au programme pour activer le mode lolilol

#define CORP_SNAKE '-'
#define TETE_SNAKE 'B'
#define DU_MANGER '*'
#define CASE_VIDE ' '


// VARIABLES GLOBALES
char ** grille = NULL;
int nbLignes = 0;
int nbColonnes = 0;
int modeEvolueDansLequelLeSerpentPeutPasserATraversLesMursEtCeSansAvoirMalDuToutCeQuiEstTresPratiqueCarCaPermetDeJouerPlusLongtemps = 0; // mode lolilol

// STRUCTURES POUR LE SNAKE
struct uneCellule {
  int ligne;
  int colonne;
  struct uneCellule * suiv;
};
typedef struct uneCellule uneCellule;

struct unSnake {
  uneCellule * teteSnake;
  uneCellule * queueSnake;
};
typedef struct unSnake unSnake;

struct uneDirection {
  int ligne;
  int colonne;
};
typedef struct uneDirection uneDirection;

// Creation d'un snake de base
unSnake creerSnake() {

  unSnake snake;

  uneCellule * teteSnake = malloc(sizeof(uneCellule));
  uneCellule * queueSnake = malloc(sizeof(uneCellule));
  uneCellule * milieuSnake = malloc(sizeof(uneCellule));

  teteSnake->ligne = 1;
  teteSnake->colonne = 4;
  teteSnake->suiv = NULL;

  milieuSnake->ligne = 1;
  milieuSnake->colonne = 3;
  milieuSnake->suiv = teteSnake;

  queueSnake->ligne = 1;
  queueSnake->colonne = 2;
  queueSnake->suiv = milieuSnake;

  snake.teteSnake = teteSnake;
  snake.queueSnake = queueSnake;

  return snake;
}

// Redéfini la tête du snake aux coordonnées indiquées
// Gère les collisions avec les bords (sauf si mode lolilol) et le snake
// Vérifie si le snake mange quelque chose
void ajouterEnTete (unSnake * snake, int ligne, int colonne,int * aMange, int * fail) {
  uneCellule * nouvelleTete = malloc (sizeof(uneCellule));

  nouvelleTete->ligne = ligne;
  nouvelleTete->colonne = colonne;
  nouvelleTete->suiv = NULL;

  snake->teteSnake->suiv = nouvelleTete;
  grille[(snake->teteSnake->ligne)][(snake->teteSnake->colonne)] = CORP_SNAKE;
  snake->teteSnake = snake->teteSnake->suiv;

  // GESTION DES COLLISIONS 
  if ( modeEvolueDansLequelLeSerpentPeutPasserATraversLesMursEtCeSansAvoirMalDuToutCeQuiEstTresPratiqueCarCaPermetDeJouerPlusLongtemps) { 
    if( snake->teteSnake->ligne < 0) {
      snake->teteSnake->ligne = nbLignes - 1; 
    }

    else if( snake->teteSnake->ligne > nbLignes-1) {
      snake->teteSnake->ligne = 0; 
    }
    else if (snake->teteSnake->colonne < 0) {
      snake->teteSnake->colonne = nbColonnes - 1; 
    }
    else if ( snake->teteSnake->colonne > nbColonnes-1) {
      snake->teteSnake->colonne = 0; 
    }
    else if (grille[snake->teteSnake->ligne][snake->teteSnake->colonne] == CORP_SNAKE) {
      *fail = 1;
    }
  }
  else {
    if( snake->teteSnake->ligne < 0 ||
	snake->teteSnake->ligne > nbLignes-1 ||
	snake->teteSnake->colonne < 0 ||
	snake->teteSnake->colonne > nbColonnes-1)
      *fail = 1;
  }


  if(!*fail) {
    *aMange = (grille[snake->teteSnake->ligne][snake->teteSnake->colonne] == DU_MANGER) ? 1 : 0;
    grille[snake->teteSnake->ligne][snake->teteSnake->colonne] = TETE_SNAKE;
  }
}

// Supprime la queue du snake
void supprimerQueue(unSnake * snake) {
  uneCellule * auxi;

  auxi = snake->queueSnake;
  grille[snake->queueSnake->ligne][snake->queueSnake->colonne] = CASE_VIDE;
  snake->queueSnake = snake->queueSnake->suiv;
  free(auxi);
}

void initGrille() {
  int i , j = 0;
  for (i = 0; i<nbLignes;i++) {
    for (j=0;j<nbColonnes;j++) {
      grille[i][j] = CASE_VIDE;
    }
  }
}

void afficherGrille(unSnake snake) {
  int i , j = 0;
  for (i = 0; i<nbLignes;i++) {
    for (j=0;j<nbColonnes;j++) {
      printw("%c",grille[i][j]);
    }
  }
}

void gererEvenement(unSnake * snake, int touche, int * fail, uneDirection * direction,int * aMange) {
  if(direction->ligne == 0) { // Pour ne pas 'aller en arrière'
    if (touche == KEY_UP){ 
      direction->ligne = -1;
      direction->colonne = 0; // Pour ne pas aller en diagonale
    }
    if (touche == KEY_DOWN) {
      direction->ligne = 1;
      direction->colonne = 0;
    }
  }
  if (direction->colonne == 0) {
    if (touche == KEY_LEFT) {
      direction->colonne = -1;
      direction->ligne = 0;
    }
    if (touche == KEY_RIGHT) {
      direction->colonne = 1;
      direction->ligne = 0;
    }
  }

  ajouterEnTete(snake,snake->teteSnake->ligne + direction->ligne,snake->teteSnake->colonne + direction->colonne, aMange,fail);
  if(!*aMange)
    supprimerQueue(snake);
}

// genere une case de bouffe à des coordonnées aléatoires
void genererDuManger(char ** grille) {
  int ligne = 0;
  int colonne = 0;
  int done = 0;

  srand(time(NULL));
  while (!done){
    ligne = rand() % (nbLignes-1);
    colonne = rand() % (nbColonnes-1);
    if(grille[ligne][colonne] == CASE_VIDE) {
      grille[ligne][colonne] = DU_MANGER;
      done = 1;
    }
  }
}

void printFail() {
  move(nbLignes/2 - 5/2,nbColonnes/2 - 37/2);
  printw("    _/_/_/_/    _/_/    _/_/_/  _/   \n");
  move(1 + nbLignes/2 - 5/2,nbColonnes/2 - 37/2);
  printw("   _/        _/    _/    _/    _/    \n");
  move(2 + nbLignes/2 - 5/2,nbColonnes/2 - 37/2);
  printw("  _/_/_/    _/_/_/_/    _/    _/     \n");
  move(3 + nbLignes/2 - 5/2,nbColonnes/2 - 37/2);
  printw(" _/        _/    _/    _/    _/      \n");  
  move(4 + nbLignes/2 - 5/2,nbColonnes/2 - 37/2);
  printw("_/        _/    _/  _/_/_/  _/_/_/_/ \n");
}

int main (int argc, char * argv []) {

  int i = 0;
  int touche = 0; // touche pressee par le joueur
  int fail = 0; // bool
  unSnake snake = creerSnake();
  uneDirection direction = {0,1};
  int aMange = 1; // bool
  int nbCasesMangees = 0;
  int delay = 0;

  // INITIALISATIONS
  if (argc > 1) {
    modeEvolueDansLequelLeSerpentPeutPasserATraversLesMursEtCeSansAvoirMalDuToutCeQuiEstTresPratiqueCarCaPermetDeJouerPlusLongtemps = 1; 
  }
  initscr();
  keypad(stdscr, TRUE);
  noecho();
  cbreak();

  getmaxyx(stdscr,nbLignes,nbColonnes);

  grille = malloc(nbLignes * sizeof(char *));
  for (i=0;i<nbLignes;i++) {
    grille[i] = malloc(nbColonnes*sizeof(char));
  }
  initGrille();

  // BOUCLE DE JEU
  while (!fail) {
    if (aMange){
      genererDuManger(grille);
      nbCasesMangees ++;
      delay = 101 - nbCasesMangees;
      delay = (delay < 60) ? 60 : delay;
      timeout(delay); // On raffraichi toutes les 60 ms au max
    }
    afficherGrille(snake);
    touche = getch();
    gererEvenement(&snake,touche,&fail,&direction,&aMange);
    erase();
  }

  // Le joueur a fail
  timeout(5000);
  erase();
  printFail();
  getch();

  endwin();
  return 0;
}
