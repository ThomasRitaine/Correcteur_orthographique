#include <stdio.h>                              //  Inclusion des bibliothèques nécessaires
#include <stdlib.h>
#include <string.h>                             //  Essentielle pour la manipulation des chaînes de caractères

#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))           //  Code trouvé sur internet, calcule la distance de Levenshtein


int levenshtein(char *s1, char *s2) {
    unsigned int x, y, s1len, s2len;
    s1len = strlen(s1);
    s2len = strlen(s2);
    unsigned int matrix[s2len+1][s1len+1];
    matrix[0][0] = 0;
    for (x = 1; x <= s2len; x++)
        matrix[x][0] = matrix[x-1][0] + 1;
    for (y = 1; y <= s1len; y++)
        matrix[0][y] = matrix[0][y-1] + 1;
    for (x = 1; x <= s2len; x++)
        for (y = 1; y <= s1len; y++)
            matrix[x][y] = MIN3(matrix[x-1][y] + 1, matrix[x][y-1] + 1, matrix[x-1][y-1] + (s1[y-1] == s2[x-1] ? 0 : 1));

    return(matrix[s2len][s1len]);
}



char *replaceWord(const char *s, const char *oldW, const char *newW) {              //  Fonction trouvée sur internet, dans la chaine du premier argument,
	char *result;                                                                   //  remplace toutes les occurences du deuxième argument par le 3ème argument
	int i, cnt = 0;
	int newWlen = strlen(newW);
	int oldWlen = strlen(oldW);

	// Counting the number of times old word
	// occur in the string
	for (i = 0; s[i] != '\0'; i++)
	{
		if (strstr(&s[i], oldW) == &s[i])
		{
			cnt++;

			// Jumping to index after the old word.
			i += oldWlen - 1;
		}
	}

	// Making new string of enough length
	result = (char *)malloc(i + cnt * (newWlen - oldWlen) + 1);

	i = 0;
	while (*s)
	{
		// compare the substring with the result
		if (strstr(s, oldW) == s)
		{
			strcpy(&result[i], newW);
			i += newWlen;
			s += oldWlen;
		}
		else
			result[i++] = *s++;
	}

	result[i] = '\0';
	return result;
}




int interactif() {                      //  Mode interactif

        /*     Déclaration des variables     */
    int choix;
    FILE *dictionnaire;                     //  Déclaration du pointeur de fichier
    char motACorriger[50];
    char motDictionnaire[50];
    int distanceTresProche = 51;                        //  On fixe le distance minimale à 51 car les chaînes de caractères sont de
    int distanceProche = 51;                            //  longueur 50 donc dans le cas extrème où le mot donné n'a rien à voir
    int distance;                                       //  avec les mots du dictionnaire, la distance serait de 50, et donc un mot serait matché.
    int nbMotTresProche = 0;
    int nbMotProche = 0;
    char motTresProche[5][50];                //  Tableau des mots avec la distance de Levenshtein minimale. On peut stocker 5 mots de 49 caractères. La case 5 est le '\0'
    char motProche[5][50];                    //  Tableau des mots avec la distance de Levenshtein un cran supérieur à la minimale

        /*       Choix du dictionnaire        */
    printf("\n\n------  Partie interactive ------\n\n\nQuel dictionnaire voulez-vous utiliser\n\n1 : Sans modification\n2 : Avec modification\n\nVotre choix : ");
    scanf("%d", &choix);
    switch(choix) {
        case 1 :    if ( ( dictionnaire = fopen("dictionnaires/dictionnaire_original.txt" , "r+") ) == NULL ) {                 //  Comme toujours : test de si le dictionnaire s'est bien ouvert
                        printf("\n Erreur : Impossible de lire le fichier dictionnaire_original.txt");                          //  le cas échéant, on envoie un message d'erreur adapté
                        return 1;
                    }
                    break;

        case 2 :    if ( ( dictionnaire = fopen("dictionnaires/dictionnaire_modifie.txt" , "r+") ) == NULL ) {
                        printf("\n Erreur : Impossible de lire le fichier dictionnaire_modifie.txt");
                        return 1;
                    }
                    break;

        default :   printf("\nCe numero n'est pas dans le menu.");
                    return 1;
    }

        /*       Saisi de la chaîne de caractère à corriger        */
    printf("\n\nMaintenant, entrez le mot que vous voulez tester (Maximum 50 caracteres) : ");
    scanf("%s", motACorriger);

    for(int i=0; motACorriger[i] != '\0'; i++) {                //  mettre la chaîne entrée en minuscule
        if (motACorriger[i] >= 'A' && motACorriger[i] <= 'Z') {
            motACorriger[i] += 32;              //  Les lettres minuscules sont 32 places plus loin dans la tables ASCII que les lettres majuscules
        }
    }

    while ( feof(dictionnaire) == 0 ) {         //  Pour parcourir le fichier dictionnaire

        fgets(motDictionnaire, 50, dictionnaire);            //  On copie un mot du dictionnaire en commencant au début, le suivant est pris à chaque fois automatiquement
        for(int i=0; motDictionnaire[i] != '\0'; i++) {         //  Pour enlever retour à la ligne
            if(motDictionnaire[i] == '\n') {
                motDictionnaire[i] = '\0';
            }
        }

        distance = levenshtein(motACorriger, motDictionnaire);          //  Calcule de la distance entre le mot du dictionnaire et le mot saisi

        if(distance == distanceTresProche && nbMotTresProche < 5) {                 //  Si la distance est déjà la minimale et que nous n'avons pas dépassé les 5 mots à afficher
            strcpy(motTresProche[nbMotTresProche], motDictionnaire);                //  Alors on ajoute le mot à la liste
            nbMotTresProche++;                                                      //  Et on incrémente le compteur
        }
        if( (distance == distanceProche || distanceProche == 51) && nbMotProche < 5) {
            switch (distanceProche) {
                case 51 :   distanceProche = distance;                          //  Dans la cas où il n'y a pas d'élément dans motProche, c'est à dire distance proche toujours à sa valeur initiale = 51
                            strcpy(motProche[nbMotProche], motDictionnaire);
                            nbMotProche++;
                            break;

                default :   strcpy(motProche[nbMotProche], motDictionnaire);        //  On ajoute le mot du dictionnaire à la liste des mots proches
                            nbMotProche++;                                          //  Puis on incrémente le nombre d'élément de la liste
            }
        }
        if(distance < distanceTresProche) {                             //  Si la nouvelle distance est inférieure à la distance minimale précédente
            distanceTresProche = distance;                              //  Alors on change la distance minimale
            for(int i=0; i<5; i++) {                    //  Copie du tableau motTresProche dans motProche
                strcpy(motProche[i], motTresProche[i]);
            }
            strcpy(motTresProche[0], motDictionnaire);                  //  On copie le mot avec la nouvelle plus petite distance
            nbMotProche = nbMotTresProche;                              //  On met à jour les nombres d'éléments dans chaque catégorie
            nbMotTresProche = 1;
        }
    }

        /*      Affichage des résultats     */
    if(distanceTresProche == 0) {                       //  Si le mot cherché est dans le dictionnaire
        printf("\n\nLe mot '%s' est present dans le dictionnaire, vous n'avez pas fait de faute.\n", motACorriger);
    } else {                                                                                        //  Sinon, on affiche les mots très proches et proches
        printf("\n\nLe mot '%s' n'est pas present dans le dictionnaire.\n\n", motACorriger);
        if(nbMotTresProche == 1) {
            printf("Le mot le plus proche de celui saisi est '%s'.\n", motTresProche[0]);
        } else {
            printf("Les mots les plus proches de celui saisi sont '%s'", motTresProche[0]);
            for(int i=1; i<nbMotTresProche; i++) {                      //  Affichage respectant les règles de la langue francaise
                if(i == nbMotTresProche-1) {
                    printf(" et '%s'.\n", motTresProche[i]);
                } else {
                    printf(", '%s'", motTresProche[i]);
                }
            }
        }
        if(nbMotProche == 1) {
            printf("Un autre mot ressemblant a celui saisi est '%s'.", motProche[0]);
        } else {
            printf("D'autres mots ressemblants a '%s' sont '%s'", motACorriger, motProche[0]);
            for(int i=1; i<nbMotProche; i++) {                                //  On commence à 1 pour empêcher le premier mot de se répéter
                if(i == nbMotProche-1) {
                    printf(" et '%s'.\n", motProche[i]);
                } else {
                    printf(", '%s'", motProche[i]);
                }
            }
        }
        fclose(dictionnaire);               //  On ferme le dictionnaire utilisé précédement pour ouvrir le dictionnaire modifié


            /*        On recherche si le mot entré est déjà dans le dictionnaire modifié         */
        if( ( dictionnaire = fopen("dictionnaires/dictionnaire_modifie.txt" , "r+") ) == NULL ) {           //  Vérification
            printf("\nErreur : Impossible de lire le fichier dictionnaire_modifie.txt");                    //  Message d'erreu adapté si echec
            return 1;
        }

        fgets(motDictionnaire, 50, dictionnaire);
        for(int i=0; motDictionnaire[i] != '\0'; i++) {         //  Pour enlever retour à la ligne
            if(motDictionnaire[i] == '\n') {
                motDictionnaire[i] = '\0';
            }
        }

        while( strcmp(motACorriger, motDictionnaire) != 0 && feof(dictionnaire) == 0 ) {            //  On cherche si le mot est déjà dans le dictionnaire modifié
            fgets(motDictionnaire, 50, dictionnaire);
            for(int i=0; motDictionnaire[i] != '\0'; i++) {                 //  On enlève le retour à la ligne
                if(motDictionnaire[i] == '\n') {
                    motDictionnaire[i] = '\0';
                }
            }
        }

        if(strcmp(motACorriger, motDictionnaire) == 0){                //  Si on est sorti avant la fin du fichir, c'est à dire si le mot à été trouvé dans le dictionnaire
            printf("\nEtes-vous satisfaits des corrections proposees ?\nLe mot '%s' est deja dans le dictionnaire avec modification, impossible de l'y ajouter une nouvelle fois.", motACorriger);
            fclose(dictionnaire);
        } else {            //  Si on a atteint la fin du fichier sans trouver le mot à corriger alors on ajoute le mot au dictionnaire
            printf("\n\nEtes-vous satisfaits des corrections proposees (1) ou souhaitez-vous ajouter le mot '%s' au dictionnaire (2) ?\n\nVotre choix : ", motACorriger);
            scanf("%d", &choix);

            switch(choix) {
            case 1 :    printf("\nMerci !");
                        fclose(dictionnaire);
                        break;

            case 2 :    fprintf(dictionnaire, "\n%s", motACorriger);
                        printf("\n\nLe mot '%s' a ete ajoute au dictionnaire.", motACorriger);
                        fclose(dictionnaire);
                        break;

            default :   printf("\nCe numero n'est pas dans le menu.\n");
                        return 1;
            }

        }
    }

    return 0;
}







int correction() {

    FILE *aCorriger;
    FILE *dictionnaire;
    int choix;
    char nomFichierACorriger[100];                  //  Le nom du fichier peut aller jusqu'à 99 caractères
    char chemin[118];                   //  Car 'textes_a_corriger/' prend 18 caractères et on va copier le contenu de nomFichierACorriger dans chemin
    strcpy(chemin, "textes_a_corriger/");
    int nbMot = 0;

        /*       Choix du dictionnaire        */
    printf("\n\n------  Mode correction ------\n\n\nQuel dictionnaire voulez-vous utiliser\n\n1 : Sans modification\n2 : Avec modification\n\nVotre choix : ");
    scanf("%d", &choix);
    switch(choix) {
        case 1 :    if ( ( dictionnaire = fopen("dictionnaires/dictionnaire_original.txt" , "r+") ) == NULL ) {
                        printf("\n Erreur : Impossible de lire le fichier dictionnaire_original.txt");
                        return 1;
                    }
                    break;

        case 2 :    if ( ( dictionnaire = fopen("dictionnaires/dictionnaire_modifie.txt" , "r+") ) == NULL ) {
                        printf("\n Erreur : Impossible de lire le fichier dictionnaire_modifie.txt");
                        return 1;
                    }
                    break;

        default :   printf("\nCe numero n'est pas dans le menu.");
                    return 1;
    }

        /*       Choix du fichier à corriger        */
    printf("\n\nVoulez-vous corriger le contenu de 'acorriger.txt' (Choix n°1) ou corriger un autre fichier (Choix n°2) ?\n\nVotre choix : ");
    scanf("%d", &choix);
    switch(choix) {
        case 1 :    strcpy(nomFichierACorriger, "acorriger.txt");
                    strcat(chemin, nomFichierACorriger);                    //  On crée le chemin vers le fichier
                    break;

        case 2 :    printf("\n\nQuel est le nom du fichier avec son extension que vous voulez ouvrir et corriger (Maximum 100 caracteres) ?\n");
                    scanf("%s", nomFichierACorriger);                       //  Saisi du nom à corriger
                    strcat(chemin, nomFichierACorriger);                    //  Puis on crée le chemin vers le fichier
                    break;

        default :   printf("\nCe numero n'est pas dans le menu.\n");
                    return 1;
    }

    if( ( aCorriger = fopen(chemin , "r") ) == NULL ) {                         //  On ouvre le fichier
        printf("\nErreur : Impossible de lire le fichier '%s'.\n", chemin);     //  Toujours avec les messages d'erreur nécessaires et adaptés
        return 1;
    }

    int i=0;
    while(fgetc(aCorriger) != EOF) {        //  Compte le nombre de caractères dans le fichier à corriger
        i++;
    }

    char *separators = " ,.!?;:€$£+-*/=<>()[]{}'\"\n0123456789";                //  On met \" sinon le caractère " fermerait la déclaration de la chaîne de caractères
    char *phraseACorriger;
    char *copiePhraseACorriger;
    phraseACorriger = (char*)malloc((i+1)*sizeof(char));                //  i+1 car il faut une case pour le "\0" final
    copiePhraseACorriger = (char*)malloc((2*i+1)*sizeof(char));             //  copie est 2 fois plus grande pour accueillir plus tard les mots corrigés qui peuvent êtres plus grands

    if(phraseACorriger == NULL || copiePhraseACorriger == NULL) {         //  Vérification de si malloc a marché ou non
        exit(EXIT_FAILURE);
    }

    fseek (aCorriger , 0 , SEEK_SET);       //  On remet le curseur au début du fichier
    for(int j=0; j<(i+1); j++) {                   //  Copie du contenu du fichier dans phraseACorriger
        phraseACorriger[j] = fgetc(aCorriger);
        if(65 <= phraseACorriger[j] && phraseACorriger[j] <= 90) {                //  Si caractère en majuscule, on le met en minuscule
            phraseACorriger[j] += 32;
        }
    }
    phraseACorriger[i] = '\0';
    strcpy(copiePhraseACorriger, phraseACorriger);                      //  Création d'une copie de la phrase du fichier à corriger

    char *strToken = strtok ( copiePhraseACorriger, separators );               //  Cette fonction détruit le str d'entré, donc on donne une copie de la phrase à corriger
    while ( strToken != NULL ) {                    //  On compte le nombre de mots dans la phrase
        nbMot++;
        strToken = strtok ( NULL, separators );
    }

    strcpy(copiePhraseACorriger, phraseACorriger);                              //  On restaure la copie pour pouvoir réuiliser la fonction

    char **motsACorriger;
    motsACorriger = (char**)malloc(nbMot * sizeof(char*));
    for(int j=0; j<nbMot; j++) {
        motsACorriger[j] = (char*)malloc(30*sizeof(char));              //  On fixe à 29 caractères la taille maximale d'un mot
    }

    if(motsACorriger == NULL) {         //  Vérification de si malloc a marché ou non
        exit(EXIT_FAILURE);
    }
    for(int j=0; j<nbMot; j++) {
        if(motsACorriger[j] == NULL) {
            exit(EXIT_FAILURE);
        }
    }

        /*        Séparation des mots à corriger, on les range dans un tableau          */
    i=0;
    strToken = strtok ( copiePhraseACorriger, separators );
    while ( strToken != NULL ) {
        strcpy(motsACorriger[i] , strToken);
        strToken = strtok ( NULL, separators );
        i++;
    }

    printf("\n\nLa phrase a corriger est :\n%s", phraseACorriger);          //  On informe l'utilisateur du contenu du fichier que nous allons corriger

    int distanceProche;
    int *nbMotProche;

    nbMotProche = (int*)malloc(nbMot * sizeof(int));             //  Une case par mot dans la phrase qui va contenir ses corrections
    if(nbMotProche == NULL) {         //  Vérification de si malloc a marché ou non
        exit(EXIT_FAILURE);
    }

    int distance;
    char motDictionnaire[50];
    char*** correction;

        /*        Déclaration du tableau de caractère à 3 dimensions         */
    correction = (char***)malloc(nbMot * sizeof(char**));             //  Une case par mot dans la phrase qui va contenir ses corrections
    if(correction == NULL) {         //  Vérification de si malloc a marché ou non
        exit(EXIT_FAILURE);
    }
    for(int j=0; j<nbMot; j++) {                                //  On initialise chaque case contenant un tableau de chaîne de caractères.
        correction[j] = (char**)malloc(10*sizeof(char*));       //  Il y a maximum 5 corrections par mot de la phrase à corriger.
        if(correction[j] == NULL) {
            exit(EXIT_FAILURE);
        }
        for(i=0; i<10; i++) {
            correction[j][i] = (char*)malloc(50*sizeof(char));                //  On fixe à 49 la taille d'un mot de correction
            if(correction[j][i] == NULL) {
                exit(EXIT_FAILURE);
            }
        }
    }

    for(int j=0; j<nbMot; j++) {                //  Pour chaque mot de la phrase à corriger, on regarde ses corrections

        nbMotProche[j] = 0;
        distanceProche = 51;

        fseek (dictionnaire, 0 , SEEK_SET);           //  On remet le curseur au début du fichier
        while ( feof(dictionnaire) == 0 ) {         //  Pour parcourir le fichier dictionnaire

            fgets(motDictionnaire, 50, dictionnaire);
            for(i=0; motDictionnaire[i] != '\0'; i++) {         //  Pour enlever retour à la ligne
                if(motDictionnaire[i] == '\n') {
                    motDictionnaire[i] = '\0';
                }
            }
            distance = levenshtein(motsACorriger[j], motDictionnaire);
            if(distance == distanceProche && nbMotProche[j] < 10) {         //  Comme pour le mode interactif
                strcpy(correction[j][nbMotProche[j]], motDictionnaire);
                nbMotProche[j]++;
            }
            if(distance < distanceProche) {
                distanceProche = distance;
                strcpy(correction[j][0], motDictionnaire);
                nbMotProche[j] = 1;
            }
        }
    }

    fclose(dictionnaire);


    if( ( dictionnaire = fopen("dictionnaires/dictionnaire_modifie.txt" , "r+") ) == NULL ) {
        printf("\nErreur : Impossible de lire le fichier dictionnaire_modifie.txt");
        return 1;
    }

    char** motsCorriges;
    motsCorriges = (char**)malloc(nbMot * sizeof(char*));
    for(int j=0; j<nbMot; j++) {
        motsCorriges[j] = (char*)malloc(30*sizeof(char));              //  On fixe à 29 caractères la taille maximale d'un mot
    }


    for(int j=0; j<nbMot; j++) {

        if( strcmp(correction[j][0], motsACorriger[j]) == 0 ) {            //  Si le mot est dans le dictionnaire
            printf("\n\n\n    Le mot '%s' faisait partie du dictionnaire utilise, vous n'avez pas fait de faute.", motsACorriger[j]);
            strcpy(motsCorriges[j], motsACorriger[j]);
        } else {

            printf("\n\n\n    Le mot '%s' ne fait pas parti du dictionnaire utilise.\n\nEntrer le numéro de la correction que vous souhaitez utiliser.\nSi vous souhaitez garder ce mot et l'ajouter au dictionnire, saisissez '-1'.", motsACorriger[j]);

            for(i=0; i<nbMotProche[j]; i++) {           //  On affiche toutes les corrections, l'utilisateur choisi celle qu'il préfère.
                printf("\n%d : %s", i, correction[j][i]);
            }
            printf("\n\nVotre choix : ");
            scanf("%d", &choix);

            if(choix == -1) {
                fgets(motDictionnaire, 50, dictionnaire);           //  On regarde si le mot est dans le dictionnaire modifié
                for(i=0; motDictionnaire[i] != '\0'; i++) {         //  Pour enlever retour à la ligne
                    if(motDictionnaire[i] == '\n') {
                        motDictionnaire[i] = '\0';
                    }
                }
                while( strcmp(motsACorriger[j], motDictionnaire) != 0 && feof(dictionnaire) == 0 ) {        //  On parcourt le dictionnaire modifié à la recherche du mot à corriger pour savoir s'il est déjà dedans
                    fgets(motDictionnaire, 50, dictionnaire);
                    for(i=0; motDictionnaire[i] != '\0'; i++) {
                        if(motDictionnaire[i] == '\n') {
                            motDictionnaire[i] = '\0';
                        }
                    }
                }
                if(strcmp(motsACorriger[j], motDictionnaire) == 0) {                //  Si on est sorti avant la fin du fichir, c'est à dire si le mot à été trouvé dans le dictionnaire
                    printf("\nLe mot '%s' est deja dans le dictionnaire modifie, impossible de l'y ajouter une nouvelle fois.", motsACorriger[j]);
                } else {
                    fprintf(dictionnaire, "\n%s", motsACorriger[j]);
                    printf("\nLe mot '%s' a ete ajoute au dictionnaire.", motsACorriger[j]);
                }

                strcpy(motsCorriges[j], motsACorriger[j]);          //  Ajouter motsACorriger[j] à la phrase finale

            } else if( 0<=choix && choix<=nbMotProche[j] ) {

                strcpy(motsCorriges[j], correction[j][choix]);              //  Ajouter correction[j][choix] à la phrase finale

            } else {                                                            //  Si le numéro n'est ni -1 ni parmi ceux proposés
                printf("\nCe numero n'est pas dans le menu.\n");
                return 1;
            }

        }

    }

    strcpy(copiePhraseACorriger, phraseACorriger);          //  copiePhraseACorriger va devenir notre phrase corrigée, donc on l'initialise avec la phrase à corriger
    for(int j=0; j<nbMot; j++) {
        char* control;
        control = replaceWord(copiePhraseACorriger, motsACorriger[j], motsCorriges[j]);             //  On remplace le mot que nous devions corriger par la correction choisie par l'utilisateur
        if(control == NULL) {
            printf("\nERREUR : replaceWord\n");         //  Si erreur, on arrête le programme avec un message d'erreur adapté
            return 1;
        }
        strcpy(copiePhraseACorriger, control);                  //  La chaîne de caractère control a été modifiée, on copie son contenu dans la phrase finale
    }

    printf("\n\nLa phrase avec correction est : %s\nElle a ete inscrite dans le fichier texte '%s' du dossier 'textes_corriges'.", copiePhraseACorriger, nomFichierACorriger);


        /*          Copie du contenu de la phrae finale dans le fichier qui a
                    le même nom que celui à corriger, mais dans le dossier "textes_corriges/"
        */
    FILE *fichierCorrige;
    strcpy(chemin, "textes_corriges/");
    strcat(chemin, nomFichierACorriger);

    if( ( fichierCorrige = fopen(chemin , "w") ) == NULL ) {                //  On ouvre en mode "w" car si un fichier un fichier porte déjà ce nom
        printf("\nErreur : Impossible de lire le fichier %s", chemin);      //  dans le dossier, il doit être supprimé et recréé avec le bon contenu
        return 1;
    }

    fprintf(fichierCorrige , "%s", copiePhraseACorriger);



        /*  Fin de la fonction, on ferme ce qui doit l'être     */
    for(int j=0; j<nbMot; j++) {
        for(i=0; i<10; i++) {
            free(correction[j][i]);
        }
        free(correction[j]);
    }
    free(correction);

    for(int j=0; j<nbMot; j++) {
        free(motsCorriges[j]);
    }
    free(motsCorriges);

    for(int j=0; j<nbMot; j++) {
        free(motsACorriger[j]);
    }
    free(motsACorriger);

    free(phraseACorriger);
    free(copiePhraseACorriger);
    free(nbMotProche);
    fclose(dictionnaire);
    fclose(aCorriger);
    fclose(fichierCorrige);
    return 0;
}








int main() {

    system("chcp 65001");               //  Pour afficher les accents
    int choix, reussite;
    printf("------  Menu d'accueil ------\n\n\nQuel est le mode que vous voulez utiliser ?\n\n1 : Interactif\n2 : Correction\n\nVotre choix : ");
    scanf("%d", &choix);
    switch(choix) {
        case 1 :    reussite = interactif();
                    if(reussite != 0) {
                        printf("\nERREUR : Something went wrong...");
                    }
                    break;

        case 2 :    reussite = correction();
                    if(reussite != 0) {
                        printf("\nERREUR : Something went wrong...");
                    }
                    break;

        default :   printf("\nCe numero n'est pas dans le menu.");
    }

}
