#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int saveGame(char * saveName, int ** gameBoard, int n, int * score){
    // Cette fonction permet de sauvegarder les données de la partie actuelle dans un fichier texte. La partie pourra ensuite être chargé par la fonction loadGame().

    strcat(saveName, ".txt"); // On ajoute l'extension .txt au nom de la sauvegarde.

    // Création du fichier texte de sauvegarde. Si celui-ci existe déjà on le remplace.
    FILE * file = fopen(saveName, "w+");
    if(file==NULL){ return -1; }

    // Ecriture du score actuel à la première ligne du fichier texte.
    if(fprintf(file, "%i\n", *score) == -1){
        // Si erreur, suppression du fichier et renvoie -1.
        remove(saveName);
        return -1;
    }

    // On parcourt le gameBoard de gauche à droite et de haut en bas et on copie la valeur de chaque case une par une en lignes dans le fichier. (une ligne = une case)
    int x, y;
    for(x=0; x<n; x++){
        for(y=0; y<n; y++){
            if(fprintf(file, "%i\n", gameBoard[x][y]) == -1){
                remove(saveName);
                return -1;
            }
        }
    }

    fclose(file); // Fermeture du fichier texte.

    return 0;
}

int loadGame(char * saveName, int ** gameBoard, int n, int * score){
    // Cette fonction permet de charger une partie à partir d'un fichier de sauvegarde créé à l'aide de la fonction saveGame().

    // Ouverture du fichier de sauvegarde.
    FILE * file = fopen(saveName, "r");
    if(file==NULL){
        // S'il n'existe pas on renvoie le code d'erreur 10.
        return 10;
    }

    char * c = malloc(sizeof(char)); // Création de la variable c qui contiendra le caractère lu.
    char * buffer = malloc(10*sizeof(char)); // Création d'un buffer qui contiendra la ligne lue.
    int i, j, line, x , y;
    line = i = x = y = 0;
    /*
    La boucle traite chaque caractère du fichier un par un.
    Elle ajoute chaque caractère lu au buffer.
    Lorsque le caractère \n est rencontré:
        Si c'est la première ligne qui est traitée alors la valeur est affectée au score.
        Si c'est un autre ligne, la valeur est affectée à la case du gameBoard correspondante.
        Enfin, on reinitialise le buffer pour traiter la ligne suivante.
    */
    while(fread(c, sizeof(char), 1, file) != 0){
        if(*c == '\n'){
            if(line == 0){
                *score = atoi(buffer);
            }else{
                gameBoard[x%n][y%n] = atoi(buffer);
                y++;
                if((y%n)==0){
                    x++;
                }
            }

            line++;
            i=0;
            for(j=0; j<10; j++){
                buffer[j] = '\0';
            }
        }else{
            buffer[i] = *c;
            i++;
        }
    }

    // On libère l'espace alloué par les malloc.
    free(c);
    free(buffer);

    return 0;
}

int ** create2DTab(int x, int y){
    // Cette fonction créé un tableau à 2 dimension de taille x lignes et y colonnes.

    int ** tab = malloc(sizeof(int *)*x);
    if(tab==NULL){return NULL;}
    int i, j;
    for(i=0; i<x; i++){
        tab[i] = malloc(sizeof(int)*y);
        if(tab[i]==NULL){
            int f;
            for(f=0; f<i; f++){
                free(tab[f]);
            }
            free(tab);
            return NULL;
        }
        for(j=0; j<y; j++){
            tab[i][j] = 0;
        }
    }
    return tab;
}

int free2DTab(int ** tab, int n){
    // Cette fonction libère l'espace alloué pour un tableau à 2 dimensions de n lignes.

    int i;
    if(tab != NULL){
        for(i=0; i<n; i++){
            if(tab[i] != NULL){
                free(tab[i]);
            }
        }
        free(tab);
    }
    return 0;
}

int displayGameBoard(int ** gameBoard, int n){
    // Cette fonction affiche le gameBoard du jeu.

    int x, y;
    printf("_____________________\n");

    for(x=0; x<n; x++){
        printf("|");
        for(y=0; y<n; y++){
            if(gameBoard[x][y]!=0){
                if(gameBoard[x][y]<10){
                    printf(" %d  |", gameBoard[x][y]);
                }else if(gameBoard[x][y]<100){
                    printf(" %d |", gameBoard[x][y]);
                }else if(gameBoard[x][y]<1000){
                    printf("%d |", gameBoard[x][y]);
                }else{
                    printf("%d|", gameBoard[x][y]);
                }
            }else{
                printf("    |");
            }

            if(y==3){
                printf("\n|____|____|____|____|");
            }
        }
        printf("\n");
    }

    return 0;
}

int countFreeBoxGameBoard(int ** gameBoard, int n){
    // Cette fonction compte le nombre de case vide restante sur le gameBoard et renvoie la réponse.

    int x, y, countFreeBox;
    countFreeBox = 0;
    // La boucle traite le gameBoard case par case et chaque case vide trouvée ajoute 1 à countFreeBox.
    for(x=0; x<n; x++){
        for(y=0; y<n; y++){
            if(gameBoard[x][y]==0){
                countFreeBox++;
            }
        }
    }

    // On renvoie finalement la variable countFreeBox.
    return countFreeBox;
}

int fillInRandomBox(int ** gameBoard, int n, int choice){
    // Cette fonction affecte à une case vide aléatoire du gameBoard soit 2 soit 4 (ou seulement 2 selon le paramètre "choice" donné).

    int countFreeBox = countFreeBoxGameBoard(gameBoard, n);
    if(countFreeBox==0){ return 10; } // On teste si le gameBoard est déjà complet ou non. Si il l'est on renvoie le code d'erreur 10.

    // On créé un tableau qui contiendra les coordonnées x et y de toute les cases vide du gameBoard.
    int ** coordTab = create2DTab(countFreeBox, 2);
    if(coordTab == NULL){ return -1; }

    // On rempli ce tableau en vérifiant pour chaque case du gameBoard si elle est vide. Si oui, les coordonnées sont ajoutées à "coordTab".
    int i, x, y;
    i=0;
    for(x=0; x<n; x++){
        for(y=0; y<n; y++){
            if(gameBoard[x][y]==0){
                coordTab[i][0] = x;
                coordTab[i][1] = y;
                i++;
            }
        }
    }

    // On choisi une case au hasard dans le tableau "coordTab".
    srand(time(NULL));
    int randomFreeBox = rand()%countFreeBox;
    x = coordTab[randomFreeBox][0];
    y = coordTab[randomFreeBox][1];

    /*
    Si "choice" vaut 2, ont affecte à la case choisie la valeur 2.
    Si "choice" vaut 4, ont affecte à la case choisie soit 2 soit 4.
    */
    if(choice == 2){
        gameBoard[x][y] = 2;
    }else if(choice == 4){
        srand(time(NULL));
        gameBoard[x][y] = ((rand()%2)+1)*2;
    }else{
        free2DTab(coordTab, countFreeBox);
        return -1;
    }

    // On libère le tableau des coordonnées créé.
    free2DTab(coordTab, countFreeBox);

    return 0;
}

int rightMove(int ** gameBoard, int n, int * score, int * win){
    // Cette fonction déplace toutes les cases du gameBoard vers la droite en effectuant la fusion des cases adjacentes horizontalement de même valeur.

    // On traite le gameBoard ligne par ligne.
    int x, y, t;
    for(x=0; x<n; x++){
        /*
        On affecte à t la valeur n-1. La variable t désignera la case vide la plus à droite de la ligne.
        On traite la ligne de la dernière case à la première.
            Si la case traitée possède une valeur non nulle:
                Si la case traitée n'est pas la case de coordonnées [x][t]:
                    - on affecte a la case de coordonnée [x][t] la valeur de la case traitée.
                    - on assigne à la case traité la valeur 0.
                On soustrait 1 à t car la case n'est pas nulle est donc ne doit pas être changée.
        */
        t = n-1;
        for(y=n-1; y>=0; y--){
            if(gameBoard[x][y] != 0){
                if(y!=t){
                    gameBoard[x][t] = gameBoard[x][y];
                    gameBoard[x][y] = 0;
                }
                t--;
            }
        }

        /*
        On traite la ligne de la dernière case à la deuxième.
        Pour chaque case traitée on vérifie si la case à gauche de celle-ci est de valeur égale non nulle.
            Si oui, on multiplie la case traitée par 2 et on rend nulle l'autre case. On incrémente également le score et on vérifie si le jeu est gagné ou non.
            Sinon on ne fait rien.
        */
        for(y=n-1; y>0; y--){
            if(gameBoard[x][y] == gameBoard[x][y-1] && gameBoard[x][y] != 0){
                gameBoard[x][y] *= 2;
                gameBoard[x][y-1] = 0;
                *score += gameBoard[x][y]; // On ajoute au score la valeur calculée pour la case traitée.
                // On teste si la nouvelle case vaut 2048. Si oui, la variable win est passée à 1.
                if(gameBoard[x][y]==2048){
                    *win = 1;
                }
                y--; // Il est inutile de traiter la case de gauche si elle vient d'être fusionnée avec l'actuelle.
            }
        }

        // Ce block est identique au premier. Il est nécessaire de le refaire car la phase de "fusion" créé des espaces lorsque des cases sont fusionnées.
        t = n-1;
        for(y=n-1; y>=0; y--){
            if(gameBoard[x][y] != 0){
                if(y!=t){
                    gameBoard[x][t] = gameBoard[x][y];
                    gameBoard[x][y] = 0;
                }
                t--;
            }
        }
    }

    return 0;
}

int leftMove(int ** gameBoard, int n, int * score, int * win){
    // Cette fonction déplace toutes les cases du gameBoard vers la gauche en effectuant la fusion des cases adjacentes horizontalement de même valeur.

    // On traite le gameBoard ligne par ligne.
    int x, y, t;
    for(x=0; x<n; x++){
        /*
        On affecte à t la valeur 0. La variable t désignera la case vide la plus à gauche de la ligne.
        On traite la ligne de la première case à la dernière.
            Si la case traitée possède une valeur non nulle:
                Si la case traitée n'est pas la case de coordonnées [x][t]:
                    - on affecte a la case de coordonnée [x][t] la valeur de la case traitée.
                    - on assigne à la case traité la valeur 0.
                On ajoute 1 à t car la case n'est pas nulle est donc ne doit pas être changée.
        */
        t = 0;
        for(y=0; y<n; y++){
            if(gameBoard[x][y] != 0){
                if(y!=t){
                    gameBoard[x][t] = gameBoard[x][y];
                    gameBoard[x][y] = 0;
                }
                t++;
            }
        }

        /*
        On traite la ligne de la première case à l'avant dernière.
        Pour chaque case traitée on vérifie si la case à droite de celle-ci est de valeur égale non nulle.
            Si oui, on multiplie la case traitée par 2 et on rend nulle l'autre case. On incrémente également le score et on vérifie si le jeu est gagné ou non.
            Sinon on ne fait rien.
        */
        for(y=0; y<n-1; y++){
            if(gameBoard[x][y] == gameBoard[x][y+1] && gameBoard[x][y] != 0){
                gameBoard[x][y] *= 2;
                gameBoard[x][y+1] = 0;
                *score += gameBoard[x][y]; // On ajoute au score la valeur calculée pour la case traitée.
                // On teste si la nouvelle case vaut 2048. Si oui, la variable win est passée à 1.
                if(gameBoard[x][y]==2048){
                    *win = 1;
                }
                y++; // Il est inutile de traiter la case de droite si elle vient d'être fusionnée avec l'actuelle.
            }
        }

        // Ce block est identique au premier. Il est nécessaire de le refaire car la phase de "fusion" créé des espaces lorsque des cases sont fusionnées.
        t = 0;
        for(y=0; y<n; y++){
            if(gameBoard[x][y] != 0){
                if(y!=t){
                    gameBoard[x][t] = gameBoard[x][y];
                    gameBoard[x][y] = 0;
                }
                t++;
            }
        }
    }

    return 0;
}

int upMove(int ** gameBoard, int n, int * score, int * win){
    // Cette fonction déplace toutes les cases du gameBoard vers le haut en effectuant la fusion des cases adjacentes verticalement de même valeur.

    // On traite le gameBoard colonne par colonne.
    int x, y, t;
    for(y=0; y<n; y++){
        /*
        On affecte à t la valeur 0. La variable t désignera la case vide la plus haute de la colonne.
        On traite la colonne de la première case à la dernière.
            Si la case traitée possède une valeur non nulle:
                Si la case traitée n'est pas la case de coordonnées [x][t]:
                    - on affecte a la case de coordonnée [x][t] la valeur de la case traitée.
                    - on assigne à la case traité la valeur 0.
                On ajoute 1 à t car la case n'est pas nulle est donc ne doit pas être changée.
        */
        t = 0;
        for(x=0; x<n; x++){
            if(gameBoard[x][y] != 0){
                if(x!=t){
                    gameBoard[t][y] = gameBoard[x][y];
                    gameBoard[x][y] = 0;
                }
                t++;
            }
        }

        /*
        On traite la colonne de la première case à l'avant dernière.
        Pour chaque case traitée on vérifie si la case en dessous de celle-ci est de valeur égale non nulle.
            Si oui, on multiplie la case traitée par 2 et on rend nulle l'autre case. On incrémente également le score et on vérifie si le jeu est gagné ou non.
            Sinon on ne fait rien.
        */
        for(x=0; x<n-1; x++){
            if(gameBoard[x][y] == gameBoard[x+1][y] && gameBoard[x][y] != 0){
                gameBoard[x][y] *= 2;
                gameBoard[x+1][y] = 0;
                *score += gameBoard[x][y]; // On ajoute au score la valeur calculée pour la case traitée.
                // On teste si la nouvelle case vaut 2048. Si oui, la variable win est passée à 1.
                if(gameBoard[x][y]==2048){
                    *win = 1;
                }
                x++; // Il est inutile de traiter la case en dessous si elle vient d'être fusionnée avec l'actuelle.
            }
        }

        // Ce block est identique au premier. Il est nécessaire de le refaire car la phase de "fusion" créé des espaces lorsque des cases sont fusionnées.
        t = 0;
        for(x=0; x<n; x++){
            if(gameBoard[x][y] != 0){
                if(x!=t){
                    gameBoard[t][y] = gameBoard[x][y];
                    gameBoard[x][y] = 0;
                }
                t++;
            }
        }
    }

    return 0;
}

int downMove(int ** gameBoard, int n, int * score, int * win){
    // Cette fonction déplace toutes les cases du gameBoard vers le bas en effectuant la fusion des cases adjacentes verticalement de même valeur.

    // On traite le gameBoard colonne par colonne.
    int x, y, t;
    for(y=0; y<n; y++){
        /*
        On affecte à t la valeur n-1. La variable t désignera la case vide la plus basse de la colonne.
        On traite la colonne de la dernière case à la première.
            Si la case traitée possède une valeur non nulle:
                Si la case traitée n'est pas la case de coordonnées [x][t]:
                    - on affecte a la case de coordonnée [x][t] la valeur de la case traitée.
                    - on assigne à la case traité la valeur 0.
                On ajoute 1 à t car la case n'est pas nulle est donc ne doit pas être changée.
        */
        t = n-1;
        for(x=n-1; x>=0; x--){
            if(gameBoard[x][y] != 0){
                if(x!=t){
                    gameBoard[t][y] = gameBoard[x][y];
                    gameBoard[x][y] = 0;
                }
                t--;
            }
        }

        /*
        On traite la colonne de la dernière case à la deuxième.
        Pour chaque case traitée on vérifie si la case au dessus de celle-ci est de valeur égale non nulle.
            Si oui, on multiplie la case traitée par 2 et on rend nulle l'autre case. On incrémente également le score et on vérifie si le jeu est gagné ou non.
            Sinon on ne fait rien.
        */
        for(x=n-1; x>0; x--){
            if(gameBoard[x][y] == gameBoard[x-1][y] && gameBoard[x][y] != 0){
                gameBoard[x][y] *= 2;
                gameBoard[x-1][y] = 0;
                *score += gameBoard[x][y]; // On ajoute au score la valeur calculée pour la case traitée.
                // On teste si la nouvelle case vaut 2048. Si oui, la variable win est passée à 1.
                if(gameBoard[x][y]==2048){
                    *win = 1;
                }
                x--; // Il est inutile de traiter la case au dessus si elle vient d'être fusionnée avec l'actuelle.
            }
        }

        // Ce block est identique au premier. Il est nécessaire de le refaire car la phase de "fusion" créé des espaces lorsque des cases sont fusionnées.
        t = n-1;
        for(x=n-1; x>=0; x--){
            if(gameBoard[x][y] != 0){
                if(x!=t){
                    gameBoard[t][y] = gameBoard[x][y];
                    gameBoard[x][y] = 0;
                }
                t--;
            }
        }
    }

    return 0;
}

int continuationOfTheGame(int ** gameBoard, int n){
    // Cette fonction teste si le jeu peut continuer lorsque le plateau est plein

    // On teste si il reste des cases vides. Si oui, on retourne la valeur 1 qui signifie que le joueur peut encore jouer.
    if(countFreeBoxGameBoard(gameBoard, n) > 0){
        return 1;
    }else{
        // On teste chaque case du gameBoard une par une.
        int x, y;
        for(x=0; x<n; x++){
            for(y=0; y<n; y++){
                /*
                Pour toute les cases sauf celle de la dernière ligne on teste si la case du dessous est la même:
                    Si oui, on retourne la valeur 1 qui signifie que le joueur peut encore jouer.
                */
                if(x!=(n-1)){
                    if(gameBoard[x][y] == gameBoard[x+1][y]){
                        return 1;
                    }
                }
                /*
                Pour toute les cases sauf celle de la dernière colonne on teste si la case de droite est la même:
                    Si oui, on retourne la valeur 1 qui signifie que le joueur peut encore jouer.
                */
                if(y!=(n-1)){
                    if(gameBoard[x][y] == gameBoard[x][y+1]){
                        return 1;
                    }
                }
            }
        }
    }

    return 0; // On retourne 0 si aucune case adjacente n'est identique. Le joueur ne peux plus jouer.
}

int winGame(int * play, int * score){
    // Cette fonction est appelée quand la partie est gagnée.

    *play = 0; // Stop la partie
    printf("Félicitation! Vous avez gagné la partie!\n");
    printf("Vous avez obtenu un score de %i points.\n", *score); // Affichage du score

    return 0;
}

int gameOver(int * play, int * score){
    // Cette fonction est appelée quand la partie est perdue.

    *play = 0; // Stop la partie
    printf("Game over! Le jeu n'est plus possible.\n");
    printf("Vous avez obtenu un score de %i points.\n", *score); // Affichage du score

    return 0;
}

int initGameboard(int ** gameBoard, int n){
    // Initialisation du plateau de jeu pour le début de la partie

    if(fillInRandomBox(gameBoard, n, 2) != 0){ return -1; } // Le paramètre 2 signifie ici que les cases ajoutées seront forcément de valeur 2.
    if(fillInRandomBox(gameBoard, n, 2) != 0){ return -1; }

    /*
    // Code permettant de tester la situation de victoire.
    gameBoard[0][0] = 1024;
    gameBoard[0][1] = 1024;
    */

   /*
    // Code permettant de tester la situation de défaite.
    int x, y, i;
    i=0;
    for(x=0;x<n;x++){
        for(y=0;y<n;y++){
            gameBoard[x][y] = i;
            i++;
        }
    }
    */

    return 0;
}

int startGame(int ** gameBoard, int n, int * play, int * score){
    // Cette fonction lance la partie.

    // On affiche la gameBoard et le score.
    displayGameBoard(gameBoard, n);
    printf("Score: %i\n", *score);

    int win = 0; // On initialise la varibale win à 0. Elle sera passée à 1 si le jeu est gagné.

    // On demande à l'utilisateur d'entrer une lettre qui correspond à une action.
    char e;
    char saveName[24] = "";
    while(*play){
        printf("\ng = gauche\nd = droite\nh = haut\nb = bas\n\ns = sauvegarder la partie\nq = quitter la partie (la partie ne sera pas sauvegardée)\n\n--> ");
        scanf(" %c", &e);
        if(e == 'd'){
            rightMove(gameBoard, n, score, &win);
            // On ajoute une case de valeur 2 ou 4 à la place d'une case vide aléatoire du gameBoard. Si une erreur se produit on retourne -1.
            if(fillInRandomBox(gameBoard, n, 4) == -1){
                return -1;
            }
        }else if(e == 'g'){
            leftMove(gameBoard, n, score, &win);
            if(fillInRandomBox(gameBoard, n, 4) == -1){
                return -1;
            }
        }else if(e == 'h'){
            upMove(gameBoard, n, score, &win);
            if(fillInRandomBox(gameBoard, n, 4) == -1){
                return -1;
            }
        }else if(e == 'b'){
            downMove(gameBoard, n, score, &win);
            if(fillInRandomBox(gameBoard, n, 4) == -1){
                return -1;
            }
        }else if(e == 's'){
            // On demande le nom que l'utilisateur veut donner à la sauvegarde et on appelle la fonction saveGame pour sauvegarder la partie.
            printf("Nom de la sauvegarde : ");
            scanf(" %19s", saveName);
            if(saveGame(saveName, gameBoard, n, score) == 0){
                printf("\nPartie sauvegardée avec succès\n");
            }else{
                printf("Erreur lors de la sauvagarde de la partie.\n");
            }
        }else if(e == 'q'){
            printf("Merci d'avoir joué! À bientôt!\n");
            return 0; // On renvoie 0 pour quitter la partie.
        }else{
            printf("Entrée invalide\n");
        }

        // On affiche le gameBoard et le score.
        displayGameBoard(gameBoard, n);
        printf("Score: %i\n", *score);
        
        /*
        Si "win" vaut 1 alors la partie est gagnée.
        Sinon, on teste s'il reste des cases vides sur le gameBoard. Si non, on regarde s'il est possible de continuer le jeu. Si non, game over.
        */
        if(win){
            winGame(play, score);
            return 0;
        }else if(!(continuationOfTheGame(gameBoard, n))){
            gameOver(play, score);
            return 0;
        }
    }

    return 0;
}

int main(int argc, char ** argv) {
    int n = 4; // Taille du plateau de jeu.
    //(On stock cette valeur dans une variable pour plus de clarté étant donnée qu'elle sera réutilisée souvent dans le code)

    // On créé le plateau de jeu que l'on nommera gameBoard.
    int ** gameBoard = create2DTab(n, n);
    if(gameBoard == NULL){ return -1; }

    int score = 0; // Initialisation de la variable de score.

    // On demande si le joueur souhaite commencer une nouvelle partie ou en charger une enregistrée précédemment.
    char choice;
    printf("n : Commencer une nouvelle partie\nl : Charger une partie enregistrée\n\n--> ");
    scanf(" %c", &choice);

    if(choice == 'n'){
        /*
        Si l'utilisateur entre 'n':
            On appelle la fonction "initGameboard". Si la fonction échoue, on libère l'espace alloué au plateau de jeu et retourne -1 pour mettre fin au programme.
        */
        if(initGameboard(gameBoard, n) == -1){
            printf("Erreur lors de l'initialisation du plateau de jeu.\n");
            free2DTab(gameBoard, n);
            return -1;
        }
    }else if(choice == 'l'){
        // Si l'utilisateur entre 'l':

        // On demande à l'utilisateur le nom de lauvegarde qu'il souhaite charger.
        char saveName[24] = "";
        printf("Nom de la sauvegarde à charger : ");
        scanf(" %19s", saveName);

        strcat(saveName, ".txt"); // On ajoute l'xtension ".txt" à la fin du nom

        int r = loadGame(saveName, gameBoard, n, &score); // On appelle la fonction "loadGame" et on affecte la valeur retournée par la fonction à r

        // On traite la valeur retournée par loadGame
        if(r == 10){
            // Si la valeur est 10 alors le nom saisie par l'utilisateur n'est pas correct. On libère l'espace alloué au plateau de jeu et retourne O pour mettre fin au programme.
            printf("Aucune sauvegarde pour le nom saisi.\n");
            free2DTab(gameBoard, n);
            return 0;
        }else if(r == -1){
            // Si la valeur est -A alors une erreur s'est produite. On libère l'espace alloué au plateau de jeu et retourne O pour mettre fin au programme.
            printf("Erreur lors du chargement de la partie.\n");
            free2DTab(gameBoard, n);
            return -1;
        }
    }else{
        /*
        Si l'utilisateur entre un caractère inconnu:
            On libère l'espace alloué au plateau de jeu et retourne 0 pour mettre fin au programme.
        */
        printf("Saisie incorrect.\n");
        free2DTab(gameBoard, n);
        return 0;
    }

    // On initialise la variable "play" à 1 et on appelle startGame pour lancer la partie.
    // Si une erreur se produit pendant la partie, on libère l'espace alloué au plateau de jeu et retourne -1 pour mettre fin au programme.
    int play = 1;
    if(startGame(gameBoard, n, &play, &score) == -1){
        free2DTab(gameBoard, n);
        return -1;
    }

    free2DTab(gameBoard, n); // On libère l'espace alloué pour le plateau de jeu.

    return 0;
}