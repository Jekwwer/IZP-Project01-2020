/* 										*
 *			Autor: Evgenii Shiliaev (xshili00)			*
 *										*
 *          		   Projekt 1 - Prace s textem  		 	 	*
 *										*
 *			   	  15.11.2020					*
 *										*/


/*Poznamka autora k pojmenovani promennych a argumentu funkcii:
* kdyz se jedna o retezci jako o radku tabulky je pouzivan identifikator "radek", jinak "retezec" jako pole znaku*/

/*vlozene knihovny*/
#include <stdbool.h> // pro datovy typ bool
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // pro strlen(), strcmp()


#define DELKA 10240        //maximalni delka retezce (10KiB)
#define DELKA_BUNKY 100    //maximakni delka bunky(sloupce)
#define DEL 127            //znak DEL pro indikaci jednotlivych znaku
#define VYCHOZI_DELIM ' '  //vychozi hodnota DELIM
#define KONEC_RETEZCE '\0' //znak null
#define ZMENA_VELIKOSTI 32 //rozdil 'a' i 'A'
#define PRAZDNY_RETEZEC ""

/*Deklarace nekterych funkci na upravu tabulky*/
void vypisPrazdnyRadek(char *radek, int delkaRadku, char DELIM);                          //irow R, arow
int vloziPrazdnySloupec(char *radek, int delkaRadku, int cisloNovehoSloupce, char DELIM); //icol C
int odstraniSloupec(char *radek, int delkaRadku, int cisloSloupce, char DELIM);           //dcol C, dcols N M

/*Deklarace nekterych funkci na zpracovani dat v tabulce*/
int vloziRetezecDoBunky(char *radek, int delkaRadku, int cisloSloupce, char *retezec, int delkaRetezce, char DELIM); //cset C STR
int tolowercase(char *radek, int delkaRadku, int cisloSloupce, char DELIM);                                          //tolower C
int touppercase(char *radek, int delkaRadku, int cisloSloupce, char DELIM);                                          //toupper C
int zaokrouhliCislo(char *radek, int delkaRadku, int cisloSloupce, char DELIM);                                      //round C
int odstraniDesetinnouCast(char *radek, int delkaRadku, int cisloSloupce, char DELIM);                               //int C
int kopirovatSloupec(char *radek, int delkaRadku, int cisloSloupce1, int cisloSloupce2, char DELIM);                 //copy N M
int vymenSloupce(char *radek, int delkaRadku, int cisloSloupce1, int cisloSloupce2, char DELIM);                     //swap N M
int presunSloupec(char *radek, int delkaRadku, int cisloSloupce1, int cisloSloupce2, char DELIM);                    //move N M

/*Deklarace nekterych funkci na selekci radku v tabulce*/
bool beginswith(char *radek, int delkaRadku, int cisloSloupce, char *retezec, int delkaRetezce, char DELIM); //beginswith C STR
bool contains(char *radek, int delkaRadku, int cisloSloupce, char *retezec, int delkaRetezce, char DELIM);   //contains C STR


/*Deklarace pomocnych funkci*/
void kopirovatRetezec(char *retezec1, int delka1, char *retezec2);
int vratitPocetSloupcu(char *radek, int delkaRadku, char DELIM);
int vratitMin(int cislo1, int cislo2);
int vratitMax(int cislo1, int cislo2);
bool jeArgument(int argc, int k);
bool jeChybaArgumentu(char **argv, int argc, int k);
char vratDELIM(char **argv, int argc, int *k);
void odstraniOddelovace(char *retezec, int delkaRetezce, char DELIM);
bool jeSloupec(char *radek, int delkaRadku, int cisloSloupce, char DELIM);

/*Zacatek programu*/
int main(int argc, char *argv[]) {
    int k = 0;                              //promenna pro prochazeni argumenty v argv
    char DELIM = vratDELIM(argv, argc, &k); //nastavujeme oddelovac

    /* vychoziHodnotaK uklada hodnotu promenne k po pripadne kontole argumenta -d*/
    int vychoziHodnotaK = k; //ukladani hodnoty promenne k pro budouci prochazeni argumenty v cyklu

    /*veskere osetreni chyb s argumenty se deje v jedne finkci*/
    /*pokud vstup nebyl zadan spravne*/
    if (jeChybaArgumentu(argv, argc, k)) {
        return 0; //konec programu
    }

    /*nektere flagy:
	* za behu tohoto programu neni mozne vedet index posledneho radku
	* proto je tady flag pro arow, ktery ukazuje, ze prikaz arow je a ma provest zmenu za urcitych podminek */
    bool potrebujemePosledniRadek = false;     //flag, ze budeme pracovat s poslednim radkem
    bool potrebujmePouzePosledniRadek = false; //flag pro rows - -
    bool jePosledniRadek = false;              //flag pro rows - -
    bool jeArow = false;                       //flag pro arow
    bool jeDrow = false;                       //flag pro drow, ktery ukazuje kdy mame odstranit radek

    char radek[DELKA]; //vytvarime retezec pro radek ze vstupu

    /*vytvarime retezec, do ktereho se bude ukladat posledni nacteny retezec ze vstupu, jen kdyz vyskituje argument arow*/
    char posledniRadek[DELKA];

    int cisloRadku = 1; //cislo soucastneho radku (protoze pocet radku a sloupcu se zacina jednickou)

    while (scanf("%[^\n]%*c", radek) != EOF || potrebujmePouzePosledniRadek) //cteme radek po radku dokud nenarazime na konec souboru
    {
        k = vychoziHodnotaK;  //vracime vychozi hodnotu promenne k
        for (; k < argc; k++) //prochazime argumenty
        {
            /*pokud rows - -*/
            if (potrebujmePouzePosledniRadek && strcmp(radek, PRAZDNY_RETEZEC) == 0) {
                kopirovatRetezec(posledniRadek, strlen(posledniRadek), radek);
                potrebujmePouzePosledniRadek = false;
                jePosledniRadek = true;
            }

            /*Prikazy upravy tabulky*/

            /*irow R: vlozi radek tabulky pred radek R > 0*/
            if (strcmp(argv[k], "irow") == 0) {
                int cisloNovehoRadku = atoi(argv[++k]); //inkrementujeme k a dostavame z argv argument pro irow
                if (cisloNovehoRadku == cisloRadku)     //pokud soucastny radek je ten, pred nemz potrebujeme vlozit radek
                {
                    vypisPrazdnyRadek(radek, strlen(radek), DELIM); //volame funkci vypisPrazdnyRadek
                }
            }

            /*arow: prida novy radek tabulky na konec tabulky*/
            /*tento program nevi jaky radek bude poslednim
			* proto aktivujeme flag, ktery bude ukladat kazdy radek ze vstupu, aby po EOF vypsat prazdny radek*/
            if (strcmp(argv[k], "arow") == 0) {
                /*aktivujeme flagy*/
                potrebujemePosledniRadek = true;
                jeArow = true;
            }

            /*drow R: odstrani radek cislo R > 0*/
            if (strcmp(argv[k], "drow") == 0) {
                int cisloRadkuNaOdstraneni = atoi(argv[++k]); //inkrementujeme k a dostavame z argv argument pro drow

                /*pokud cislo soucastneho radku se rovna cislu radku na odstraneni*/
                if (cisloRadkuNaOdstraneni == cisloRadku) {
                    jeDrow = true; //aktivujeme flag, ktery po prochazeni argumenty preskoci radek(odstrani z vystupu)
                }
            }

            /*drows N M: odstrani radky N az M (N <= M)*/
            if (strcmp(argv[k], "drows") == 0) {
                int cisloRadkuNaOdstraneni1 = atoi(argv[++k]); //inkrementujeme k a dostavame z argv 1. argument pro drows
                int cisloRadkuNaOdstraneni2 = atoi(argv[++k]); //inkrementujeme k a dostavame z argv 2. argument pro drows

                /*pokud cislo soucastneho radku je v diapazonu odstraneni*/
                if (cisloRadku >= cisloRadkuNaOdstraneni1 && cisloRadku <= cisloRadkuNaOdstraneni2) {
                    jeDrow = true; //aktivujeme flag, ktery po prochazeni argumenty preskoci radek(vymaze z vystupu)
                }
            }

            /*icol C: vlozi prazdny sloupec pred sloupec dany cislem C*/
            if (strcmp(argv[k], "icol") == 0) {
                int cisloNovehoSloupce = atoi(argv[++k]); //inkrementujeme k a dostavame z argv argument pro icol

                int prubehFunkce = vloziPrazdnySloupec(radek, strlen(radek), cisloNovehoSloupce, DELIM); //volame funkci vloziPrazdnySloupec

                /*pokud se neco pokazilo za behu funkce*/
                if (prubehFunkce != 0) {
                    return 0; //konec programu
                }
            }

            /*acol: vlozi prazdny sloupec pred za posledni sloupec*/
            if (strcmp(argv[k], "acol") == 0) {
                int delkaRadku = strlen(radek);        //vytvarime promennou a zapisujeme do ni delku radku
                radek[delkaRadku] = DELIM;             //memime '\0' na oddelovac
                radek[delkaRadku + 1] = KONEC_RETEZCE; //pridavame '\0'
            }

            /*dcol C: odstrani sloupec cislo C*/
            if (strcmp(argv[k], "dcol") == 0) {
                int cisloSloupceNaOdstraneni = atoi(argv[++k]); //inkrementujeme k a dostavame z argv argument pro dcol

                int prubehFunkce = odstraniSloupec(radek, strlen(radek), cisloSloupceNaOdstraneni, DELIM); //volame funkci odstraniSloupec

                /*pokud se neco pokazilo za behu funkce*/
                if (prubehFunkce != 0) {
                    return 0; //konec programu
                }
            }

            /*dcols N M: odstrani sloupce N az M*/
            if (strcmp(argv[k], "dcols") == 0) {
                int cisloSloupceNaOdstraneni1 = atoi(argv[++k]); //inkrementujeme k a dostavame z argv 1. argument pro dcols
                int cisloSloupceNaOdstraneni2 = atoi(argv[++k]); //inkrementujeme k a dostavame z argv 2. argument pro dcols

                /*celkovy pocet sloupcu na odstraneni*/
                int pocetSloupcuNaOdstraneni = cisloSloupceNaOdstraneni2 - cisloSloupceNaOdstraneni1 + 1; // |{2,3,4,5}| = 4 = 5-2+1

                /*pokud horni hranice intervalu sloupcu neexistuje (dolni hrenice se bude zkontrolovana ve funkci)*/
                if (jeSloupec(radek, strlen(radek), cisloSloupceNaOdstraneni2, DELIM) == false) {
                    return 0; //konec programu
                }

                /*postupne odstranujeme sloupec po sloupci*/
                for (int i = 0; i < pocetSloupcuNaOdstraneni; i++) {
                    int prubehFunkce = odstraniSloupec(radek, strlen(radek), cisloSloupceNaOdstraneni1, DELIM); //volame funkci odstraniSloupec

                    /*pokud se neco pokazilo za behu funkce*/
                    if (prubehFunkce != 0) {
                        return 0; //konec programu
                    }
                }
            }


            /*Prikazy zpracovani dat v tabulce*/

            /*cset C STR: do bunky ve sloupci C bude nastaven retezec STR*/
            if (strcmp(argv[k], "cset") == 0) {
                int cisloSloupce = atoi(argv[++k]);                  //inkrementujeme k a dostavame z argv 1. argument pro cset
                char retezec[DELKA_BUNKY];                           //vytvarime retezec pro 2. argument pro cset
                k++;                                                 //inkrementujeme k
                kopirovatRetezec(argv[k], strlen(argv[k]), retezec); //kopirujeme do retezce 2. argument pro cset

                int prubehFunkce = vloziRetezecDoBunky(radek, strlen(radek), cisloSloupce, retezec, strlen(retezec), DELIM); //volame funkci vloziRetezecDoBunky

                /*pokud se neco pokazilo za behu funkce*/
                if (prubehFunkce != 0) {
                    return 0; //konec programu
                }
            }

            /*tolower C: retezec ve sloupci C bude preveden na mala pismena*/
            if (strcmp(argv[k], "tolower") == 0) {
                int cisloSloupce = atoi(argv[++k]);                                        //inkrementujeme k a dostavame z argv argument pro tolower
                int prubehFunkce = tolowercase(radek, strlen(radek), cisloSloupce, DELIM); //volame funkci

                /*pokud se neco pokazilo za behu funkce*/
                if (prubehFunkce != 0) {
                    return 0; //konec programu
                }
            }

            /*toupper C: retezec ve sloupci C bude preveden na velka pismena*/
            if (strcmp(argv[k], "toupper") == 0) {
                int cisloSloupce = atoi(argv[++k]);                                        //inkrementujeme k a dostavame z argv argument pro toupper
                int prubehFunkce = touppercase(radek, strlen(radek), cisloSloupce, DELIM); //volame funkci

                /*pokud se neco pokazilo za behu funkce*/
                if (prubehFunkce != 0) {
                    return 0; //konec programu
                }
            }

            /*round C: ve sloupci C zaokrouhli cislo na cele cislo*/
            if (strcmp(argv[k], "round") == 0) {
                int cisloSloupce = atoi(argv[++k]);                                            //inkrementujeme k a dostavame z argv argument pro round
                int prubehFunkce = zaokrouhliCislo(radek, strlen(radek), cisloSloupce, DELIM); //volame funkci zaokrouhliCislo
                /*pokud se neco pokazilo za behu funkce*/
                if (prubehFunkce != 0) {
                    return 0; //konec programu
                }
            }

            /*int C: odstrani desetinnou cast cisla ve sloupci C*/
            if (strcmp(argv[k], "int") == 0) {
                int cisloSloupce = atoi(argv[++k]);                                                   //inkrementujeme k a dostavame z argv argument pro int
                int prubehFunkce = odstraniDesetinnouCast(radek, strlen(radek), cisloSloupce, DELIM); //volame funkci odstraniDesetinnouCast
                /*pokud se neco pokazilo za behu funkce*/
                if (prubehFunkce == 1) {
                    return 0; //konec programmu
                }
            }

            /*copy N M: prepise obsah bunek ve sloupci N hodnotami ve sloupci M*/
            if (strcmp(argv[k], "copy") == 0) {
                int cisloSloupce1 = atoi(argv[++k]); //inkrementujeme k a dostavame z argv 1. argument pro copy
                int cisloSloupce2 = atoi(argv[++k]); //inkrementujeme k a dostavame z argv 2. argument pro copy

                /*pokud cisla sloupcu se nerovnaji, abychom ne provadeli zbytocnou praci*/
                if (cisloSloupce1 != cisloSloupce2) {
                    int prubehFunkce = kopirovatSloupec(radek, strlen(radek), cisloSloupce1, cisloSloupce2, DELIM); //volame funkci kopirovatSloupec

                    /*pokud se neco pokazilo za behu funkce*/
                    if (prubehFunkce == 1) {
                        return 0; //konec programmu
                    }
                }
            }

            /*swap N M:zameni hodnoty bunek ve sloupcich N a M*/
            if (strcmp(argv[k], "swap") == 0) {
                int cisloSloupce1 = atoi(argv[++k]); //inkrementujeme k a dostavame z argv 1. argument pro swap
                int cisloSloupce2 = atoi(argv[++k]); //inkrementujeme k a dostavame z argv 1. argument pro swap

                /*pokud cisla sloupcu se nerovnaji, abychom ne provadeli zbytocnou praci*/
                if (cisloSloupce1 != cisloSloupce2) {
                    int prubehFunkce = vymenSloupce(radek, strlen(radek), cisloSloupce1, cisloSloupce2, DELIM); //volame funkci vymenSloupce

                    /*pokud se neco pokazilo za behu funkce*/
                    if (prubehFunkce == 1) {
                        return 0; //konec programmu
                    }
                }
            }

            /*move N M:presun sloupec N pred sloupec M*/
            if (strcmp(argv[k], "move") == 0) {
                int cisloSloupce1 = atoi(argv[++k]); //inkrementujeme k a dostavame z argv 1. argument pro move
                int cisloSloupce2 = atoi(argv[++k]); //inkrementujeme k a dostavame z argv 1. argument pro move

                /*pokud cisla sloupcu se nerovnaji, abychom ne provadeli zbytocnou praci*/
                if (cisloSloupce1 != cisloSloupce2) {
                    int prubehFunkce = presunSloupec(radek, strlen(radek), cisloSloupce1, cisloSloupce2, DELIM); //volame funkci presunSloupec

                    /*pokud se neco pokazilo za behu funkce*/
                    if (prubehFunkce == 1) {
                        return 0; //konec programmu
                    }
                }
            }


            /*Prikazy selekce radku*/

            /*rows N M:pracujeme pouze s radky N <= M*/
            if (strcmp(argv[k], "rows") == 0) {
                int cisloRadku1 = atoi(argv[++k]); //inkrementujeme k a dostavame z argv 1. argument pro rows
                int cisloRadku2 = atoi(argv[++k]); //inkrementujeme k a dostavame z argv 2. argument pro rows

                /*pokud rows - -*/
                if (jePosledniRadek) {
                    continue; //vracime se na zacatek tohoto cyklu
                }

                /*pokud rows - -*/
                if (cisloRadku1 == 0 && cisloRadku2 == 0) {
                    potrebujmePouzePosledniRadek = true;
                    potrebujemePosledniRadek = true;
                    break; //mimoradne ukoncijeme cyklus
                }
                /*jinak pokud rows N -*/
                else if (cisloRadku < cisloRadku1 && cisloRadku2 == 0) {
                    break; //mimoradne ukoncijeme cyklus
                }
                /*jinak pokud rows N M*/
                else if ((cisloRadku < cisloRadku1 || cisloRadku > cisloRadku2) && cisloRadku2 != 0) {
                    break; //mimoradne ukoncijeme cyklus
                }
            }

            /*beginswith C STR:pracuj pouze s radky, ve kterych obsah sloupec zacina STR*/
            if (strcmp(argv[k], "beginswith") == 0) {
                int cisloSloupce = atoi(argv[++k]); //inkrementujeme k a dostavame z argv 1. argument pro beginswith

                /*pokud sloupec s zadanem cislem neni v tabulce*/
                if (jeSloupec(radek, strlen(radek), cisloSloupce, DELIM) == false) {
                    return 0; //konec programu
                }

                char retezec[DELKA_BUNKY];                           //vytvarime retezec pro 2. argument pro beginswith
                k++;                                                 //inkrementujeme k
                kopirovatRetezec(argv[k], strlen(argv[k]), retezec); //kopirujeme do retezce 2. argument pro beginswith

                /*pokud se sloupec radku nezacina retezcem*/
                if (beginswith(radek, strlen(radek), cisloSloupce, retezec, strlen(retezec), DELIM) == false) //volame funkci
                {
                    break; //vracime se na zacatek nacitaciho cyklu (radek bude vypsan bez zmen)
                }
            }

            /*contains C STR:pracuj pouze s radky, ktere v zadanem sloupci obsahuji STR*/
            if (strcmp(argv[k], "contains") == 0) {
                int cisloSloupce = atoi(argv[++k]); //inkrementujeme k a dostavame z argv 1. argument pro contains

                /*pokud sloupec s zadanem cislem neni v tabulce*/
                if (jeSloupec(radek, strlen(radek), cisloSloupce, DELIM) == false) {
                    return 0; //konec programu
                }

                char retezec[DELKA_BUNKY];                           //vytvarime retezec pro 2. argument pro contains
                k++;                                                 //inkrementujeme k
                kopirovatRetezec(argv[k], strlen(argv[k]), retezec); //kopirujeme do retezce 2. argument pro contains

                /*pokud sloupec radku obsahuje retezec*/
                if (contains(radek, strlen(radek), cisloSloupce, retezec, strlen(retezec), DELIM) == false) //volame funkci
                {
                    break; //vracime se na zacatek nacitaciho cyklu (radek bude vypsan bez zmen)
                }
            }
        }
        cisloRadku++; //posouvame cislo radku

        /*pokud flag potrebujemePosledniRadek je true, kopirujeme radek pro budouci operace
		* jde o kopirovani radku, protoze pocet sloupcu se muze za behu programu zmenit*/
        if (potrebujemePosledniRadek) {
            kopirovatRetezec(radek, strlen(radek), posledniRadek); //kopirujeme radek do posledniho radku
        }

        /*pokud flag jeDrow je true, preskakujeme radek(odstranujeme z vystupu)*/
        if (jeDrow) {
            jeDrow = false; //deaktivujeme flag, aby dalsi radky nebyly preskocene
            continue;       //printf() se nesplni
        }

        printf("%s\n", radek); //vzpisujeme upraveny radek

        /*pokud rows - -*/
        if (potrebujmePouzePosledniRadek) {
            kopirovatRetezec(PRAZDNY_RETEZEC, strlen(PRAZDNY_RETEZEC), radek); //odstranujeme retezec po vzpisovani
        }
    }

    //pokud po konci nacitani tabulky flag jeArow je true
    if (jeArow) {
        vypisPrazdnyRadek(radek, strlen(radek), DELIM); //volame finkci vypisPrazdnyRadek
    }

    return 0; //konec programu
}


/*Definice nekterych funkci na upravu tabulky*/

/*funkce vypisPrazdnyRadek vytvari a vypisuje novy radek
* ma na vstupu radek, jeho delku a znak oddelovace(DELIM)
* nic nevraci */
void vypisPrazdnyRadek(char *radek, int delkaRadku, char DELIM) //irow R, arow
{
    int pocetSpoupcu = vratitPocetSloupcu(radek, delkaRadku, DELIM); //dostavame pocet sloupcu
    int i = 0;                                                       //pocitadlo vypsanych oddelovacu

    /*dokud vypsanych oddelovacu je min nez pocet sloupcu - 1 (5 sloupcu - 4 oddelovace)*/
    while (i < pocetSpoupcu - 1) {
        printf("%c", DELIM); //vytvarime sloupce v novem radku
        i++;                 //zvysujeme pocet vypsanych oddelovacu
    }
    printf("\n"); //posouvame se o jeden radek
} //konec funkce

/*funkce vloziPrazdnySloupec vklada prazdny sloupec pred slopec zadany uzivatelem
* ma na vstupu radek, jeho delku, cislo noveho sloupce a znak oddelovace(DELIM)
* vraci cislo typu int, ktere popiseje prubeh funkce */
int vloziPrazdnySloupec(char *radek, int delkaRadku, int cisloNovehoSloupce, char DELIM) //icol C
{
    /*pokud zadany sloupec neni v tabulce*/
    if (jeSloupec(radek, delkaRadku, cisloNovehoSloupce, DELIM) == false) {
        return 1; //mimoradne ukonceni funkce
    }

    char novyRadek[DELKA];           //vytvarime retezec pro novy radek
    int novaDelkaRadku;              //vytvarime promennou pro delku noveho radku
    novaDelkaRadku = delkaRadku + 1; //delka noveho radku je o jednicku vetsi, nez delka puvodniho radku (o jeden oddelovac)

    int aktualniSloupec = 1; //cislo aktualniho sloupce (zaciname vzdy jednickou)

    int i = 0; //inkrementacni promenna pro prochazeni v puvodnem radku
    int j = 0; //inkrementacni promenna pro prochazeni v novem radku

    /*pokud index znaku mensi nebo rovno nove delce ralku*/
    while (j <= novaDelkaRadku) //pouzivame <=, abychom zachytili '\0'
    {
        /*pokud cislo aktualniho sloupce se rovna cislu sloupce, pred kterym ma vyzniknout novy sloupec*/
        if (aktualniSloupec == cisloNovehoSloupce) {
            novyRadek[j] = DELIM; //pridavame oddelovac (resp. novy sloupec)
            j++;                  //posouvame se v poli noveho radku
            aktualniSloupec++;    //posouvame cislo aktualniho sloupce o 1
        }
        novyRadek[j] = radek[i]; //kopirujeme znaky z puvodniho radku do noveho radku

        /*pokud znak je oddelovac*/
        if (radek[i] == DELIM) {
            aktualniSloupec++; //posouvame cislo aktualniho sloupce
        }

        i++;
        j++; //posouvame se v radcich
    }
    kopirovatRetezec(novyRadek, novaDelkaRadku, radek); //kopirujeme novy radek do puvodniho radku
    return 0;                                           //konec funkci
}

/*funkce odstraniSloupec odstranuje sloupec podle zadaneho uzivatelem cisla
* ma na vstupu radek, jeho delku, cislo sloupce na odstraneni a znak oddelovace(DELIM)
* vraci cislo typu int, ktere popiseje prubeh funkce */
int odstraniSloupec(char *radek, int delkaRadku, int cisloSloupceNaOdstraneni, char DELIM) //dcol C, dcols N M
{
    /*pokud zadany sloupec neni v tabulce*/
    if (jeSloupec(radek, delkaRadku, cisloSloupceNaOdstraneni, DELIM) == false) {
        return 1; //mimoradne ukonceni funkce
    }

    char novyRadek[DELKA];                                           //vytvarime retezec pro novy radek
    int pocetSloupcu = vratitPocetSloupcu(radek, delkaRadku, DELIM); //dostavame celkovy pocet sloupcu
    int aktualniSloupec = 1;                                         //cislo aktualniho sloupce (zaciname vzdy jednickou)

    int i = 0; //inkrementacni promenna pro prochazeni v puvodnem radku
    int j = 0; //inkrementacni promenna pro prochazeni v novem radku

    /*pokud index znaku mensi nebo rovno nove delce radku*/
    while (j <= delkaRadku) //pouzivame <=, abychom zachytili '\0'
    {
        /*pokud cislo aktualniho sloupce se rovna cislu sloupce na odstraneni*/
        if (aktualniSloupec == cisloSloupceNaOdstraneni) {
            /*pokud slopec na odstraneni neni posledni sloupec v radku*/
            if (cisloSloupceNaOdstraneni < pocetSloupcu) {
                /*pokud znak je oddelovac*/
                if (radek[i] == DELIM) {
                    aktualniSloupec++; //posouvame cislo aktualniho sloupce
                }
                i++; //posouvame se v poli puvodniho radku

                /*vracime se na podminku (zacatek) cyklu, abychom nepridavali znaky slopce do noveho radku*/
                continue;
            }
            /*jinak pokud slopec na odstraneni je posledni sloupec v radku*/
            else if (cisloSloupceNaOdstraneni == pocetSloupcu) {
                /*pokud je to jediny sloupec*/
                if (cisloSloupceNaOdstraneni == 1) {
                    novyRadek[i] = KONEC_RETEZCE; //ukoncujeme radek znakem null ('\0')
                    break;                        //mimoradne ukonceni cyclu
                }
                /*posledni znak je vzdy oddelovac, proto misto neho piseme znak null*/
                novyRadek[i - 1] = KONEC_RETEZCE; //ukoncujeme radek znakem null ('\0')

                /*neni smysl pokracovat v cyklu dal*/
                break; //mimoradne ukonceni cyclu
            }
        }
        novyRadek[j] = radek[i]; //kopirujeme znaky z puvodniho radku do noveho radku

        /*pokud znak je oddelovac*/
        if (radek[i] == DELIM) {
            aktualniSloupec++; //posouvame cislo aktualniho sloupce
        }

        i++;
        j++; //posouvame se v radcich
    }
    kopirovatRetezec(novyRadek, strlen(novyRadek), radek); //kopirujeme novy radek do puvodniho radku
    return 0;                                              //konec funkci
}

/*Definice nekterych funkci na zpracovani dat*/

/*funkce vloziRetezecDoBunky vklada do zadaneho sloupce uzivatelsky retezec
* ma na vstupu radek, delku radku, cislo sloupce, retezec, delku retezce a znak oddelovace(DELIM)
* vraci cislo typu int, ktere popiseje prubeh funkce */
int vloziRetezecDoBunky(char *radek, int delkaRadku, int cisloSloupce, char *retezec, int delkaRetezce, char DELIM) //cset C STR
{
    /*pokud zadany sloupec neni v tabulce*/
    if (jeSloupec(radek, delkaRadku, cisloSloupce, DELIM) == false) {
        return 1; //mimoradne ukonceni funkce
    }

    char novyRadek[DELKA]; //vytvarime retezec pro novy radek

    int aktualniSloupec = 1; //cislo aktualniho sloupce

    int i = 0; //inkrementacni promenna pro prochazeni v puvodnem radku
    int j = 0; //inkrementacni promenna pro prochazeni v novem radku

    /*pokud index znaku je mensi nebo rovno delce radku*/
    while (i <= delkaRadku) //pouzivame <=, abychom zachytili '\0'
    {
        /*pokud aktualni sloupec je ten, do ktereho mame vlozit retezec*/
        if (aktualniSloupec == cisloSloupce) {
            for (int k = 0; k < delkaRetezce; k++) //prochazime retezcem
            {
                novyRadek[j + k] = retezec[k]; //vkladame znak po znaku
            }
            j += delkaRetezce; //aktualizujeme soucastny index znaku

            /*ted potrebujeme vyjit ze sloupce v puvodnim radku, abychom dale kopirovali jen znaky z jinych sloupcu*/
            /*dokud je pradva prave jedno tvrzeni: znak radku je oddelovac nebo znak radku neni posledni*/
            while ((radek[i] == DELIM) ^ (i < delkaRadku)) {
                i++; //prochazime v poli radku
            }
            aktualniSloupec++; //posouvame cislo aktualniho sloupce
        }
        novyRadek[j] = radek[i]; //kopirujeme znaky z puvodniho radku do noveho radku

        /*pokud znak je oddelovac*/
        if (radek[i] == DELIM) {
            aktualniSloupec++; //posouvame cislo aktualniho sloupce
        }

        i++;
        j++; //posouvame se v radcich
    }

    kopirovatRetezec(novyRadek, strlen(novyRadek), radek); //kopirujeme novy radek do puvodniho radku
    return 0;                                              //konec funkci
}

/*funkce tolowercase prevadi retezec v zadanem sloupci na mala pismena
* ma na vstupu radek, jeho radku, cislo sloupce a znak oddelovace(DELIM)
* vraci cislo typu int, ktere popiseje prubeh funkce */
int tolowercase(char *radek, int delkaRadku, int cisloSloupce, char DELIM) //tolower C
{
    /*pokud zadany sloupec neni v tabulce*/
    if (jeSloupec(radek, delkaRadku, cisloSloupce, DELIM) == false) {
        return 1; //mimoradne ukonceni funkce
    }

    int aktualniSloupec = 1; //cislo aktualniho sloupce

    for (int i = 0; i < delkaRadku; i++) //prochazime radkem
    {
        /*pokud znak je oddelovac*/
        if (radek[i] == DELIM) {
            aktualniSloupec++; //posouvame cislo aktualniho sloupce
            continue;          //preskakujeme podminku
        }

        /*pokud aktualni sloupec je ten, ve kterem mame zmenit pismena*/
        if (aktualniSloupec == cisloSloupce) {
            /*pokud znak je mezi A a Z*/
            if (radek[i] >= 'A' && radek[i] <= 'Z') {
                radek[i] += ZMENA_VELIKOSTI; //menime velikost pismen
            }
        }
    }
    return 0; //konec funkce
}

/*funkce touppercase prevadi retezec v zadanem sloupci na velka pismena
* ma na vstupu radek, jeho radku, cislo sloupce a znak oddelovace(DELIM)
* vraci cislo typu int, ktere popiseje prubeh funkce */
int touppercase(char *radek, int delkaRadku, int cisloSloupce, char DELIM) //toupper C
{
    /*pokud zadany sloupec neni v tabulce*/
    if (jeSloupec(radek, delkaRadku, cisloSloupce, DELIM) == false) {
        return 1; //mimoradne ukonceni funkce
    }

    int aktualniSloupec = 1; //cislo aktualniho sloupce

    for (int i = 0; i < delkaRadku; i++) //prochazime radkem
    {
        /*pokud znak je oddelovac*/
        if (radek[i] == DELIM) {
            aktualniSloupec++; //posouvame cislo aktualniho sloupce
            continue;          //preskakujeme podminku
        }

        /*pokud aktualni sloupec je ten, ve kterem mame zmenit pismena*/
        if (aktualniSloupec == cisloSloupce) {
            /*pokud znak je mezi a i z*/
            if (radek[i] >= 'a' && radek[i] <= 'z') {
                radek[i] -= ZMENA_VELIKOSTI; //menime velikost pismen
            }
        }
    }
    return 0; //konec funkce
}

/*funkce zaokrouhliCislo zaokrouhluje cislo v zadanem sloupci
* ma na vstupu radek, jeho radku, cislo sloupce a znak oddelovace(DELIM)
* vraci hodnotu typu int, ktera popuseje prubeh funkci */
int zaokrouhliCislo(char *radek, int delkaRadku, int cisloSloupce, char DELIM) //round C
{
    /*pokud zadany sloupec neni v tabulce*/
    if (jeSloupec(radek, delkaRadku, cisloSloupce, DELIM) == false) {
        return 1; //mimoradne ukonceni funkce
    }

    char novyRadek[DELKA];          //vytvarime retezec pro novy radek
    char hodnotaBunky[DELKA_BUNKY]; //vytvarime retezec hodnotu bunky(sloupce)
    int delkaHodnotyBunky = 0;      //vytvarime promennou pro delku hodnoty sloupce

    int aktualniSloupec = 1; //cislo aktualniho sloupce

    char indikator = DEL; //vytvarime znak-indikator

    int j = 0;                            //inkrementacni promenna pro prochazeni v retezci bunky
    for (int i = 0; i <= delkaRadku; i++) //prochazime radkem
    {                                     //pouzivame <=, abychom zachytili '\0'
        /*pokud znak je oddelovac*/
        if (radek[i] == DELIM) {
            aktualniSloupec++; //posouvame cislo aktualniho sloupce
            i++;               //posouvame se v radku
        }
        /*pokud cislo soucastneho sloupce se rovna cislu sloupce, ve kterem mame konvertovat hodnotu*/
        if (aktualniSloupec == cisloSloupce) {
            hodnotaBunky[j] = radek[i]; //kopirujeme znaky bunky do samostatneho retezce
            radek[i] = indikator;       //menime znaky sloupce na indikator
            j++;                        //pospivame se v retezci bunky
        }
    }

    hodnotaBunky[j] = KONEC_RETEZCE;           //ukoncujeme retezec hodnoty bunky
    double hodnotaBunkyD = atof(hodnotaBunky); //vytvarime promennou typu double a konvertujeme hodnotu bunky do double

    if (hodnotaBunkyD == 0.0 && strcmp(hodnotaBunky, "0.0") != 0 //osetreni chyb: atof vraci 0.0 pri neuspesne konvertaci
        && strcmp(hodnotaBunky, "0.00") != 0) {
        printf("Chyba prikazu round: v bunce neni cislo\n"); //vypisujeme chybu
        return 1;                                            //mimoradne ukonceni funkce
    }

    /*vytvarime kontrolni hodnotu, abychom zjistili jak mame zaokrouhlit cislo*/
    int kontrolniHodnota = atoi(hodnotaBunky); //vytvarime promennou typu int a konvertujeme hodnotu bunkz do int bez zaokrouhleni

    int zaokrouhlenaHodnota = 0; //vytvarime promennou pro ukladani zaokrouhlene hodnoty
    /*pokud desetina cast < 0.5*/
    if (hodnotaBunkyD - 0.5 < kontrolniHodnota) {
        zaokrouhlenaHodnota = kontrolniHodnota; //zaokrouhlujeme smerem dolu
    }
    /*jinak*/
    else {
        zaokrouhlenaHodnota = kontrolniHodnota + 1; //zaokrouhlujeme smerem nahoru
    }

    sprintf(hodnotaBunky, "%d", zaokrouhlenaHodnota); //konvertujeme hodnotu do retezce

    aktualniSloupec = 1;                      //vracime cislo aktualniho sloupce na jednicku
    delkaHodnotyBunky = strlen(hodnotaBunky); //zapisujeme novou delku hodnoty bunky

    for (int i = 0; i <= delkaRadku; i++) //prochazime radkem
    {                                     //pouzivame <=, abychom zachytili '\0'
        /*pokud znak je oddelovac*/
        if (radek[i] == DELIM) {
            aktualniSloupec++; //posouvame cislo aktualniho sloupce
            i++;               //posouvame se v radku
        }

        /*pokud cislo soucastneho sloupce se rovna cislu sloupce, ve kterem mame konvertovat hodnotu*/
        if (cisloSloupce == aktualniSloupec) {
            for (int j = 0; j < delkaHodnotyBunky; j++) //prochazime bunkou
            {
                radek[i] = hodnotaBunky[j]; //vracime hodnoty bunky do radku
                i++;                        //posouvame se v radku
            }
            /*kdyz vsechno prepiseme zpatky, prochazet radkem nema smysl*/
            break; //mimoradne ukoncujeme cyklus
        }
    }

    j = 0;                                //inkrementacni promenna pro prochazeni v novem radku
    for (int i = 0; i <= delkaRadku; i++) //prochazime radkem
    {
        /*delka radku se zmensila, protoze my zaokrouhlili desetinou cast, mame odstranit zbztecne znaky*/
        /*pokud znak je indikator*/
        if (radek[i] == indikator) {
            continue; //preskocujeme zbytecne znaky
        }

        novyRadek[j] = radek[i]; //kopirujeme znaky z puvodniho radku do noveho radku
        j++;                     //posouvame se v novem radku
    }

    kopirovatRetezec(novyRadek, strlen(novyRadek), radek); //kopirujeme novy radek do puvodniho radku
    return 0;                                              //konec funkci
}

/*funkce odstraniDesetinnouCast odstranuje desetinnou cast v zadanem sloupci
* ma na vstupu radek, jeho radku, cislo sloupce a znak oddelovace(DELIM)
* vraci hodnotu typu int, ktera popuseje prubeh funkci */
int odstraniDesetinnouCast(char *radek, int delkaRadku, int cisloSloupce, char DELIM) //int C
{
    /*pokud zadany sloupec neni v tabulce*/
    if (jeSloupec(radek, delkaRadku, cisloSloupce, DELIM) == false) {
        return 1; //mimoradne ukonceni funkce
    }

    char novyRadek[DELKA];          //vytvarime retezec pro novy radek
    char hodnotaBunky[DELKA_BUNKY]; //vytvarime retezec hodnotu bunky(sloupce)
    int delkaHodnotyBunky = 0;      //vytvarime promennou pro delku hodnoty sloupce

    int aktualniSloupec = 1; //cislo aktualniho sloupce

    char indikator = DEL; //vytvarime znak-indikator

    int j = 0;                            //inkrementacni promenna pro prochazeni v retezci bunky
    for (int i = 0; i <= delkaRadku; i++) //prochazime radkem
    {                                     //pouzivame <=, abychom zachytili '\0'
        /*pokud znak je oddelovac*/
        if (radek[i] == DELIM) {
            aktualniSloupec++; //posouvame cislo aktualniho sloupce
            i++;               //posouvame se v radku
        }
        /*pokud cislo soucastneho sloupce se rovna cislu sloupce, ve kterem mame konvertovat hodnotu*/
        if (aktualniSloupec == cisloSloupce) {
            hodnotaBunky[j] = radek[i]; //kopirujeme znaky bunky do samostatneho retezce
            radek[i] = indikator;       //menime znaky sloupce na indikator
            j++;                        //pospivame se v retezci bunky
        }
    }
    hodnotaBunky[j] = KONEC_RETEZCE;           //ukoncujeme retezec hodnoty bunky
    double hodnotaBunkyD = atof(hodnotaBunky); //vytvarime promennou typu double a konvertujeme hodnotu bunky do double

    if (hodnotaBunkyD == 0.0 && strcmp(hodnotaBunky, "0.0") != 0 //osetreni chyb: atof vraci 0.0 pri neuspesne konvertaci
        && strcmp(hodnotaBunky, "0.00") != 0) {
        printf("Chyba prikazu int: v bunce neni cislo\n"); //vypisujeme chybu
        return 1;                                          //mimoradne ukonceni funkce
    }

    int hodnotaBunkyI = atoi(hodnotaBunky); //vytvarime promennou typu int a konvertujeme hodnotu bunky do int

    sprintf(hodnotaBunky, "%d", hodnotaBunkyI); //konvertujeme hodnotu do retezce

    aktualniSloupec = 1;                      //vracime cislo aktualniho sloupce na jednicku
    delkaHodnotyBunky = strlen(hodnotaBunky); //zapisujeme novou delku hodnoty bunky

    for (int i = 0; i <= delkaRadku; i++) //prochazime radkem
    {                                     //pouzivame <=, abychom zachytili '\0'
        /*pokud znak je oddelovac*/
        if (radek[i] == DELIM) {
            aktualniSloupec++; //posouvame cislo aktualniho sloupce
            i++;               //posouvame se v radku
        }

        /*pokud cislo soucastneho sloupce se rovna cislu sloupce, ve kterem mame konvertovat hodnotu*/
        if (cisloSloupce == aktualniSloupec) {
            for (int j = 0; j < delkaHodnotyBunky; j++) //prochazime bunkou
            {
                radek[i] = hodnotaBunky[j]; //vracime hodnoty bunky do radku
                i++;                        //posouvame se v radku
            }
            /*kdyz vsechno prepiseme zpatky, prochazet radkem nema smysl*/
            break; //mimoradne ukoncujeme cyklus
        }
    }

    j = 0;                                //inkrementacni promenna pro prochazeni v novem radku
    for (int i = 0; i <= delkaRadku; i++) //prochazime radkem
    {
        /*delka radku se zmensila, protoze my zaokrouhlili desetinou cast, mame odstranit zbztecne znaky*/
        /*pokud znak je indikator*/
        if (radek[i] == indikator) {
            continue; //preskocujeme zbytecne znaky
        }

        novyRadek[j] = radek[i]; //kopirujeme znaky z puvodniho radku do noveho radku

        j++; //posouvame se v novem radku
    }

    kopirovatRetezec(novyRadek, strlen(novyRadek), radek); //kopirujeme novy radek do puvodniho radku
    return 0;                                              //konec funkci
}

/*funkce kopirovatSloupec kopiruje obsah jedne bunky sloupce a prepisuje ho do jine bunky sloupce
* ma na vstupu radek, jeho radku, cislo 1. sloupce, cislo 2. sloupce a znak oddelovace(DELIM)
* vraci hodnotu typu int, ktera popuseje prubeh funkci */
int kopirovatSloupec(char *radek, int delkaRadku, int cisloSloupceOdkud, int cisloSloupceKam, char DELIM) {
    /*pokud zadane sloupce nejsou v tabulce*/
    if (jeSloupec(radek, delkaRadku, cisloSloupceOdkud, DELIM) == false ||
        jeSloupec(radek, delkaRadku, cisloSloupceKam, DELIM) == false) {
        return 1; //mimoradne ukonceni funkce
    }

    char obsahBunky[DELKA_BUNKY]; //vytvarime retezec pro obsah bunky
    int delkaRetezceBunky = 0;    //vychozi delka bunky

    char zacatekRadku[DELKA];  //vytvarime retezec pro zacatek radku (do bunky(sloupce), do ktere budeme prepisovat obsah)
    int delkaZacatkuRadku = 0; //vychozi delka zacatku radku

    char konecRadku[DELKA];        //vytvarime retezec pro konec radku (od bunky(sloupce), do ktere budeme prepisovat obsah)
    int delkaKonceRadku = 0;       //vychozi delka konce radku
    konecRadku[0] = KONEC_RETEZCE; //vychozi hodnota knce radku;

    int aktualniSloupec = 1; //cislo aktualniho sloupce

    int b = 0; //inkrementacni promenna pro prochazeni retezcem bunky
    int z = 0; //inkrementacni promenna pro prochazeni zacatkem radku
    int k = 0; //inkrementacni promenna pro prochazeni koncem radku

    for (int i = 0; i < delkaRadku; i++) //prochazime radkem
    {
        /*pokud znak je oddelovac*/
        if (radek[i] == DELIM) {
            aktualniSloupec++; //posouvame cislo aktualniho sloupce
        }

        /*pokud cislo aktualniho sloupce je mensi nez cislo sloupce, do ktereho mame prepsat obsah*/
        if (aktualniSloupec < cisloSloupceKam) {
            zacatekRadku[z] = radek[i]; //kopirujeme znaky z puvodniho radku
            z++;                        //posouvame se v retezci zacatku radku
        }

        /*pokud cislo aktualniho sloupce se rovna cislu sloupce, ktery mame zkopirovat*/
        if (aktualniSloupec == cisloSloupceOdkud) {
            obsahBunky[b] = radek[i]; //kopirujeme znaky z puvodniho radku
            b++;                      //posouvame se v retezci obsahu bunky
        }

        /*pokud cislo aktualniho sloupce je vetsi nez cislo sloupce, do ktereho mame prepsat obsah*/
        if (aktualniSloupec > cisloSloupceKam) {
            konecRadku[k] = radek[i]; //kopirujeme znaky z puvodniho radku
            k++;                      //posouvame se v retezci konce radku
        }
    }

    /*ukoncujeme jednotlive retezce*/
    obsahBunky[b] = KONEC_RETEZCE;
    zacatekRadku[z] = KONEC_RETEZCE;
    konecRadku[k] = KONEC_RETEZCE;

    /*odstranime oddelovace (pokud jsou), aby se nam pak ne vytvoril zbytecny sloupec*/
    odstraniOddelovace(obsahBunky, strlen(obsahBunky), DELIM);

    /*obnovujeme delky jednotlivych retezcu*/
    delkaRetezceBunky = strlen(obsahBunky);
    delkaZacatkuRadku = strlen(zacatekRadku);
    delkaKonceRadku = strlen(konecRadku);

    int i = 0; //inkrementacni promenna pro prochazeni radkem

    /*ted budeme spojovat casti radku*/
    for (int z = 0; z < delkaZacatkuRadku; z++) //prochazime zacatkem radku
    {
        radek[i] = zacatekRadku[z]; //kopirujeme znaky ze zacatku radku do puvodniho radku
        i++;                        //posouvame se v radku
    }
    /*pokud zacatek nebyl prazdnym*/
    if (i != 0) {
        radek[i] = DELIM; //pridavame oddelovac
        i++;              //posouvame se v radku
    }
    for (int b = 0; b < delkaRetezceBunky; b++) //prochazime obsahem bunky
    {
        radek[i] = obsahBunky[b]; //kopirujeme znaky obsahu bunky do puvodniho radku
        i++;                      //posouvame se v radku
    }
    for (int k = 0; k < delkaKonceRadku; k++) //prochazime koncem radku
    {
        radek[i] = konecRadku[k]; //kopirujeme znaky konce radku do puvodniho radku
        i++;
    }
    radek[i] = KONEC_RETEZCE; //ukoncujeme radek;
    return 0;                 //konec funkce
}

/*funkce vymenSloupce vymenuje zadane sloupce
* ma na vstupu radek, jeho radku, cislo 1. sloupce, cislo 2. sloupce a znak oddelovace(DELIM)
* vraci hodnotu typu int, ktera popuseje prubeh funkci */
int vymenSloupce(char *radek, int delkaRadku, int cisloSloupce1, int cisloSloupce2, char DELIM) //swap N M
{
    /*pokud zadane sloupce nejsou v tabulce*/
    if (jeSloupec(radek, delkaRadku, cisloSloupce1, DELIM) == false ||
        jeSloupec(radek, delkaRadku, cisloSloupce2, DELIM) == false) {
        return 1; //mimoradne ukonceni funkce
    }

    /*vymenujeme cisla sloupcu, aby byly v posloupnosti, aby se nam lepe kopirovaly do retezcu*/
    int min = vratitMin(cisloSloupce1, cisloSloupce2);
    int max = vratitMax(cisloSloupce1, cisloSloupce2);

    cisloSloupce1 = min;
    cisloSloupce2 = max;

    /*vytvarime retezce pro jednotlive casti radku s vychozi delkou*/
    char zacatekRadku[DELKA];
    int delkaZacatkuRadku = 0;

    char obsahSloupce1[DELKA_BUNKY];
    int delkaObsahuSloupce1 = 0;

    char obsahSloupce2[DELKA_BUNKY];
    int delkaObsahuSloupce2 = 0;

    char retezecMezi[DELKA];
    int delkaRetezceMezi = 0;

    char konecRadku[DELKA];
    int delkaKonceRadku = 0;

    konecRadku[0] = KONEC_RETEZCE; //vychozi hodnota konce radku

    int aktualniSloupec = 1; //cislo aktualniho sloupcu (vzdy zaciname jednickou)

    int s1 = 0; //inkrementacni promenna pro prochazeni obsahem 1. sloupce
    int s2 = 0; //inkrementacni promenna pro prochazeni obsahem 2. sloupce
    int z = 0;  //inkrementacni promenna pro prochazeni zacatkem radku
    int m = 0;  //inkrementacni promenna pro prochazeni retezcem mezi 1. a 2. sloupci
    int k = 0;  //inkrementacni promenna pro prochazeni koncem radku

    /*budeme prepisovat radek do ruznych casti, pak vymenime sloupce a spojime dohromady znova*/

    for (int i = 0; i < delkaRadku; i++) //prochazime radkem
    {
        /*pokud znak je oddelovac*/
        if (radek[i] == DELIM) {
            aktualniSloupec++; //posouvame cislo aktualniho sloupce
        }

        /*pokud cislo aktualniho sloupce je mensi nez cislo 1. sloupce*/
        if (aktualniSloupec < cisloSloupce1) {
            zacatekRadku[z] = radek[i]; //kopirujeme znaky z puvodniho radku
            z++;                        //posouvame se v retezci zacatku radku
        }

        /*pokud cislo aktualniho sloupce se rovna cislu 1. sloupce*/
        if (aktualniSloupec == cisloSloupce1) {
            obsahSloupce1[s1] = radek[i]; //kopirujeme znaky z puvodniho radku
            s1++;                         //posouvame se v retezci obsahu 1. sloupce
        }

        if (aktualniSloupec > cisloSloupce1 && aktualniSloupec < cisloSloupce2) {
            retezecMezi[m] = radek[i]; //kopirujeme znaky z puvodniho radku
            m++;                       //posouvame se v retezci mezi 1. a 2. sloupci
        }

        /*pokud cislo aktualniho sloupce se rovna cislu 2. sloupce*/
        if (aktualniSloupec == cisloSloupce2) {
            obsahSloupce2[s2] = radek[i]; //kopirujeme znaky z puvodniho radku
            s2++;                         //posouvame se v retezci obsahu 2. sloupce
        }

        /*pokud cislo aktualniho sloupce je vetsi nez cislo sloupce, do ktereho mame prepsat obsah*/
        if (aktualniSloupec > cisloSloupce2) {
            konecRadku[k] = radek[i]; //kopirujeme znaky z puvodniho radku
            k++;                      //posouvame se v retezci konce radku
        }
    }

    /*ukoncujeme jednotlive retezce*/
    zacatekRadku[z] = KONEC_RETEZCE;
    obsahSloupce1[s1] = KONEC_RETEZCE;
    retezecMezi[m] = KONEC_RETEZCE;
    obsahSloupce2[s2] = KONEC_RETEZCE;
    konecRadku[k] = KONEC_RETEZCE;

    /*odstranime oddelovace (pokud jsou), aby se nam pak ne vytvoril zbytecny sloupec*/
    odstraniOddelovace(obsahSloupce1, strlen(obsahSloupce1), DELIM);
    odstraniOddelovace(retezecMezi, strlen(retezecMezi), DELIM);
    odstraniOddelovace(obsahSloupce2, strlen(obsahSloupce2), DELIM);

    /*osbovujeme delky retezcu*/
    delkaZacatkuRadku = strlen(zacatekRadku);
    delkaObsahuSloupce1 = strlen(obsahSloupce1);
    delkaRetezceMezi = strlen(retezecMezi);
    delkaObsahuSloupce2 = strlen(obsahSloupce2);
    delkaKonceRadku = strlen(konecRadku);

    int i = 0; //inkrementacni hodnota pro prochazeni radkem

    /*ted budeme spojovat casti radku*/
    for (int z = 0; z < delkaZacatkuRadku; z++) //prochazime zacatkem radku
    {
        radek[i] = zacatekRadku[z]; //kopirujeme znaky ze zacatku radku do puvodniho radku
        i++;                        //posouvame se v radku
    }
    /*pokud zacatek nebyl prazdnym*/
    if (i != 0) {
        radek[i] = DELIM; //pridavame oddelovac
        i++;              //posouvame se v radku
    }
    for (int s2 = 0; s2 < delkaObsahuSloupce2; s2++) //prochazime obsahem 2. sloupce
    {
        radek[i] = obsahSloupce2[s2]; //kopirujeme znaky obsahu 2. sloupce do puvodniho radku
        i++;                          //posouvame se v radku
    }
    radek[i] = DELIM; //pridavame oddelovac
    i++;              //posouvame se v radku
    for (int m = 0; m < delkaRetezceMezi; m++) {
        radek[i] = retezecMezi[m]; //kopirujeme znaky retezce mezi 1. a 2. sloupci do puvodniho radku
        i++;                       //posouvame se v radku
    }
    if (m != 0) {
        radek[i] = DELIM; //pridavame oddelovac
        i++;
    }                                                //posouvame se v radku
    for (int s1 = 0; s1 < delkaObsahuSloupce1; s1++) //prochazime obsahem 1. sloupce
    {
        radek[i] = obsahSloupce1[s1]; //kopirujeme znaky obsahu 1. sloupce do puvodniho radku
        i++;                          //posouvame se v radku
    }
    for (int k = 0; k < delkaKonceRadku; k++) //prochazime koncem radku
    {
        radek[i] = konecRadku[k]; //kopirujeme znaky konce radku do puvodniho radku
        i++;
    }
    radek[i] = KONEC_RETEZCE; //ukoncujeme radek;
    return 0;                 //konec funkci
}

/*funkce presunSloupec presouva zadany sloupec pred dalsim zadanym sloupcem
* ma na vstupu radek, jeho radku, cislo 1. sloupce, cislo 2. sloupce a znak oddelovace(DELIM)
* vraci hodnotu typu int, ktera popuseje prubeh funkci */
int presunSloupec(char *radek, int delkaRadku, int cisloSloupce1, int cisloSloupce2, char DELIM) //move N M
{
    /*pokud zadane sloupce nejsou v tabulce*/
    if (jeSloupec(radek, delkaRadku, cisloSloupce1, DELIM) == false ||
        jeSloupec(radek, delkaRadku, cisloSloupce2, DELIM) == false) {
        return 1; //mimoradne ukonceni funkce
    }

    /*vytvarime retezce pro jednotlive casti radku s vychozi delkou*/
    char zacatekRadku[DELKA];
    int delkaZacatkuRadku = 0;

    char obsahSloupce1[DELKA_BUNKY];
    int delkaObsahuSloupce1 = 0;

    char obsahSloupce2[DELKA_BUNKY];
    int delkaObsahuSloupce2 = 0;

    char retezecMezi[DELKA];
    int delkaRetezceMezi = 0;

    char konecRadku[DELKA];
    int delkaKonceRadku = 0;

    /*vytvarime flag, kterz bude uchovovat stav je-li zadane cisla sloupcu v posloupnosti*/
    bool jeSpravnePoradiZadanychSloupcu = true; //predpokladame, ze jsou

    konecRadku[0] = KONEC_RETEZCE; //vychozi hodnota konce radku

    int aktualniSloupec = 1; //cislo aktualniho sloupcu (vzdy zaciname jednickou)

    int s1 = 0; //inkrementacni promenna pro prochazeni obsahem 1. sloupce
    int s2 = 0; //inkrementacni promenna pro prochazeni obsahem 2. sloupce
    int z = 0;  //inkrementacni promenna pro prochazeni zacatkem radku
    int m = 0;  //inkrementacni promenna pro prochazeni retezcem mezi 1. a 2. sloupci
    int k = 0;  //inkrementacni promenna pro prochazeni koncem radku

    /*idea funkce:
	* pokud mame cisla sloupcu ze zacatku v posloupnosti: napr. 3 5
	* nas radek vypada takto Z.3.M.5.K a budeme delat Z.M.3.5.K
	*
	* pokud NEmame cisla sloupcu ze zacatku v posloupnosti: napr. 5 3
	* nas radek vypada takto Z.3.M.5.K a budeme delat Z.5.3.M.K	*/

    /*pokud cisla sloupcu nejsou v posloupnosti*/
    if (cisloSloupce1 > cisloSloupce2) {
        /*vymenujeme cisla sloupcu, aby byly v posloupnosti, aby se nam lepe kopirovaly do retezcu*/
        jeSpravnePoradiZadanychSloupcu = false;
        int min = vratitMin(cisloSloupce1, cisloSloupce2);
        int max = vratitMax(cisloSloupce1, cisloSloupce2);

        cisloSloupce1 = min;
        cisloSloupce2 = max;
    }

    for (int i = 0; i < delkaRadku; i++) //prochazime radkem
    {
        /*pokud znak je oddelovac*/
        if (radek[i] == DELIM) {
            aktualniSloupec++; //posouvame cislo aktualniho sloupce
        }

        /*pokud cislo aktualniho sloupce je mensi nez cislo 1. sloupce*/
        if (aktualniSloupec < cisloSloupce1) {
            zacatekRadku[z] = radek[i]; //kopirujeme znaky z puvodniho radku
            z++;                        //posouvame se v retezci zacatku radku
        }

        /*pokud cislo aktualniho sloupce se rovna cislu 1. sloupce*/
        if (aktualniSloupec == cisloSloupce1) {
            obsahSloupce1[s1] = radek[i]; //kopirujeme znaky z puvodniho radku
            s1++;                         //posouvame se v retezci obsahu 1. sloupce
        }

        if (aktualniSloupec > cisloSloupce1 && aktualniSloupec < cisloSloupce2) {
            retezecMezi[m] = radek[i]; //kopirujeme znaky z puvodniho radku
            m++;                       //posouvame se v retezci mezi 1. a 2. sloupci
        }

        /*pokud cislo aktualniho sloupce se rovna cislu 2. sloupce*/
        if (aktualniSloupec == cisloSloupce2) {
            obsahSloupce2[s2] = radek[i]; //kopirujeme znaky z puvodniho radku
            s2++;                         //posouvame se v retezci obsahu 2. sloupce
        }

        /*pokud cislo aktualniho sloupce je vetsi nez cislo sloupce, do ktereho mame prepsat obsah*/
        if (aktualniSloupec > cisloSloupce2) {
            konecRadku[k] = radek[i]; //kopirujeme znaky z puvodniho radku
            k++;                      //posouvame se v retezci konce radku
        }
    }

    /*ukoncujeme jednotlive retezce*/
    zacatekRadku[z] = KONEC_RETEZCE;
    obsahSloupce1[s1] = KONEC_RETEZCE;
    retezecMezi[m] = KONEC_RETEZCE;
    obsahSloupce2[s2] = KONEC_RETEZCE;
    konecRadku[k] = KONEC_RETEZCE;

    /*odstranime oddelovace (pokud jsou), aby se nam pak ne vytvoril zbytecny sloupec*/
    odstraniOddelovace(obsahSloupce1, strlen(obsahSloupce1), DELIM);
    odstraniOddelovace(retezecMezi, strlen(retezecMezi), DELIM);
    odstraniOddelovace(obsahSloupce2, strlen(obsahSloupce2), DELIM);

    /*osbovujeme delky retezcu*/
    delkaZacatkuRadku = strlen(zacatekRadku);
    delkaObsahuSloupce1 = strlen(obsahSloupce1);
    delkaRetezceMezi = strlen(retezecMezi);
    delkaObsahuSloupce2 = strlen(obsahSloupce2);
    delkaKonceRadku = strlen(konecRadku);

    int i = 0; //inkrementacni hodnota pro prochazeni radkem

    /*pokud cisla zadanych sloupcu ze zacatku byly v poradku*/
    if (jeSpravnePoradiZadanychSloupcu) {
        /*ted budeme spojovat casti radku*/
        for (int z = 0; z < delkaZacatkuRadku; z++) //prochazime zacatkem radku
        {
            radek[i] = zacatekRadku[z]; //kopirujeme znaky ze zacatku radku do puvodniho radku
            i++;                        //posouvame se v radku
        }
        /*pokud zacatek nebyl prazdnym*/
        if (i != 0) {
            radek[i] = DELIM; //pridavame oddelovac
            i++;              //posouvame se v radku
        }
        for (int m = 0; m < delkaRetezceMezi; m++) {
            radek[i] = retezecMezi[m]; //kopirujeme znaky retezce mezi 1. a 2. sloupci do puvodniho radku
            i++;                       //posouvame se v radku
        }
        if (m != 0) {
            radek[i] = DELIM; //pridavame oddelovac
            i++;              //posouvame se v radku
        }
        for (int s1 = 0; s1 < delkaObsahuSloupce1; s1++) //prochazime obsahem 1. sloupce
        {
            radek[i] = obsahSloupce1[s1]; //kopirujeme znaky obsahu 1. sloupce do puvodniho radku
            i++;                          //posouvame se v radku
        }
        radek[i] = DELIM;                                //pridavame oddelovac
        i++;                                             //posouvame se v radku
        for (int s2 = 0; s2 < delkaObsahuSloupce2; s2++) //prochazime obsahem 2. sloupce
        {
            radek[i] = obsahSloupce2[s2]; //kopirujeme znaky obsahu 2. sloupce do puvodniho radku
            i++;                          //posouvame se v radku
        }
        for (int k = 0; k < delkaKonceRadku; k++) //prochazime koncem radku
        {
            radek[i] = konecRadku[k]; //kopirujeme znaky konce radku do puvodniho radku
            i++;
        }
        radek[i] = KONEC_RETEZCE; //ukoncujeme radek;
    }
    /*jinak*/
    else {
        /*ted budeme spojovat casti radku*/
        for (int z = 0; z < delkaZacatkuRadku; z++) //prochazime zacatkem radku
        {
            radek[i] = zacatekRadku[z]; //kopirujeme znaky ze zacatku radku do puvodniho radku
            i++;                        //posouvame se v radku
        }
        /*pokud zacatek nebyl prazdnym*/
        if (i != 0) {
            radek[i] = DELIM; //pridavame oddelovac
            i++;              //posouvame se v radku
        }
        for (int s2 = 0; s2 < delkaObsahuSloupce2; s2++) //prochazime obsahem 2. sloupce
        {
            radek[i] = obsahSloupce2[s2]; //kopirujeme znaky obsahu 2. sloupce do puvodniho radku
            i++;                          //posouvame se v radku
        }
        radek[i] = DELIM;                                //pridavame oddelovac
        i++;                                             //posouvame se v radku
        for (int s1 = 0; s1 < delkaObsahuSloupce1; s1++) //prochazime obsahem 1. sloupce
        {
            radek[i] = obsahSloupce1[s1]; //kopirujeme znaky obsahu 1. sloupce do puvodniho radku
            i++;                          //posouvame se v radku
        }
        radek[i] = DELIM; //pridavame oddelovac
        i++;              //posouvame se v radku
        for (int m = 0; m < delkaRetezceMezi; m++) {
            radek[i] = retezecMezi[m]; //kopirujeme znaky retezce mezi 1. a 2. sloupci do puvodniho radku
            i++;                       //posouvame se v radku
        }
        for (int k = 0; k < delkaKonceRadku; k++) //prochazime koncem radku
        {
            radek[i] = konecRadku[k]; //kopirujeme znaky konce radku do puvodniho radku
            i++;
        }
        radek[i] = KONEC_RETEZCE; //ukoncujeme radek;
    }
    return 0; //konec funkce
}


/*Definice Definice nekterych funkci na selekci radku*/

/*funkce beginswith kontroluje zacina-li zadany sloupec z zadaneho retezce
* ma na vstupu radek, jeho radku, cislo sloupce, retezec, delku retezce a znak oddelovace(DELIM)
* vraci hodnotu typu bool */
bool beginswith(char *radek, int delkaRadku, int cisloSloupce, char *retezec, int delkaRetezce, char DELIM) //beginswith C STR
{
    int aktualniSloupec = 1; //cislo aktualniho sloupcu (vzdy zaciname jednickou)

    int j = 0;                           //inkrementacni promenna pro prochazeni retezcem
    for (int i = 0; i < delkaRadku; i++) //prochazime radkem
    {
        /*pokud znak je oddelovac*/
        if (radek[i] == DELIM) {
            aktualniSloupec++; //posouvame cislo aktualniho sloupce
            continue;          //vracime se na zacatek cyklu
        }
        /*pokud cislo aktualniho slouce se rovna cislu sloupce, ve kterem mame provest kontrolu*/
        if (aktualniSloupec == cisloSloupce) {
            /*jestli znaky se neschoduji (do konce retezce)*/
            if (retezec[j] != radek[i] && j < delkaRetezce) {
                return false; //vracime false		//konec funkce
            }
            j++; //posouvame se v retezci
        }
    }
    return true; //konec funkce
}

/*funkce contains kontroluje obsahuje-li zadany sloupec zadany retezec
* ma na vstupu radek, jeho radku, cislo sloupce, retezec, delku retezce a znak oddelovace(DELIM)
* vraci hodnotu typu bool */
bool contains(char *radek, int delkaRadku, int cisloSloupce, char *retezec, int delkaRetezce, char DELIM) //contains C STR
{
    int aktualniSloupec = 1; //cislo aktualniho sloupcu (vzdy zaciname jednickou)

    for (int i = 0; i < delkaRadku; i++) //prochazime radkem
    {
        /*pokud znak je oddelovac*/
        if (radek[i] == DELIM) {
            aktualniSloupec++; //posouvame cislo aktualniho sloupce
            continue;          //vracime se na zacatek cyklu
        }
        /*pokud cislo aktualniho slouce se rovna cislu sloupce, ve kterem mame provest kontrolu*/
        if (aktualniSloupec == cisloSloupce) {
            /*pokud prvni znak retezce se rovna akrualnimu znaku radku*/
            if (retezec[0] == radek[i]) {
                int k = i;                             //vytvarime promennou pro dalsi prochazeni radkem
                for (int j = 0; j < delkaRetezce; j++) //prochazime retezcem
                {
                    /*pokud znaky se nerovnaji*/
                    if (retezec[j] != radek[k]) {
                        break; //mimoradne ukoncujeme cykcus
                    }
                    k++; //posouvame se v loli radku
                    /*pokud posledni znak retesce (jako vse pred nim) je stejny*/
                    if (j == delkaRetezce - 1) {
                        return true; //vracime true		//konec funkce
                    }
                }
            }
        }
    }
    return false; //vracime false		//konec funkce
}


/*Definice pomocnych funkci*/

/*funkce kopirovatRetezec kopiruje retezce1 do retezce2:
* ma na vstupu retezec, z ktereho kopirujeme, jeho delku a retezec, do ktereho kopirujeme
* nic nevraci */
void kopirovatRetezec(char *retezec1, int delka1, char *retezec2) {
    for (int i = 0; i <= delka1; i++) //prrochazime 1. retezcem
    {
        retezec2[i] = retezec1[i]; //kopirujeme znaky 1. retezce do 2. retezce
    }
} //konec funkce

/*funkce vratitPocetSloupcu vraci pocet sloupcu predaneho radku:
* ma na vstupu radek, jeho delku a znak oddelovace
* vraci pocet slopcu (cislo typu int)*/
int vratitPocetSloupcu(char *radek, int delkaRadku, char DELIM) {
    int pocetSloupcu = 1;                //kazdy radek bez oddeovacu ma alespon 1 slopec
    for (int i = 0; i < delkaRadku; i++) //prochazime polem znaku radku
    {
        if (radek[i] == DELIM) //pokud znak je oddelovac(DELIM),
            pocetSloupcu++;    //zvysujeme pocet sloupcu o 1
    }
    return pocetSloupcu; //vracime pocet sloupcu		//konec funkce
}

/*funkce vratitMin vraci nejmensi ze dvou cisel:
* ma na vstupu dva cisla typu int
* vraci cislo typu int */
int vratitMin(int cislo1, int cislo2) {
    /*pokud cislo1 je vetsi nez cislo2*/
    if (cislo1 > cislo2) {
        return cislo2; //vracime cislo2	//konec funkce
    }
    /*jinak*/
    return cislo1; //vracime cislo1	//konec funkce
}

/*funkce vratitMax vraci nejvetsi ze dvou cisel:
* ma na vstupu dva cisla typu int
* vraci cislo typu int */
int vratitMax(int cislo1, int cislo2) {
    /*pokud cislo1  je vetsi nez cislo2*/
    if (cislo1 > cislo2) {
        return cislo1; //vracime cislo1	//konec funkce
    }
    /*jinak*/
    return cislo2; //vracime cislo2	//konec funkce
}

/*funkce jeArgument kontroluje je-li dalsi argument v poli agrumentu:
* ma na vstupu delku pole argumentu a iterator polem k
* (k ukazuje na kterem elementu jsme skoncili od zacatku programu)
* vraci hodnotu typu bool*/
bool jeArgument(int argc, int k) {
    bool jeArgument = true; //predpokladame, ze argument je
    /*pokud index dalsiho argumentu se rovna delce pole argumentu (k je posledni)*/
    if (++k == argc) {
        jeArgument = false; //menime stav kontroly
    }
    return jeArgument; //vracime vysledek kontroly		//konec funkce
}

/*funkce jeChybaArgumentu kontroluje zadal-li uzivatel sprevne argumenty(resp. prikazy):
* ma na vstupu pole argumentu(argv), jeho delku a iterator polem k
* (k ukazuje na kterem elementu jsme skoncili od zacatku programu)
* vraci hodnotu typu bool*/
bool jeChybaArgumentu(char **argv, int argc, int k) {
    /*vytvarime promennou, ktera uklada stav kontroly argumentu
	* predpokladame, ze ukyvatel zadal argumenty spravne*/
    bool jeChyba = false;

    /*nektera chybova hlaseni*/
    char *chybaArgumentu = "Chyba. Chybi argument pro prikaz";
    char *chybaHodnotyArgumentu = "Chyba. Neocekavana hodnota argumentu pro prikaz";
    char *chybaPoradiArgumentu = "Chyba. Spatne poradi argumentu pro prikaz";

    char nazevPrikazu[DELKA_BUNKY]; //vytvarime retezec pro ukladani nazvu prikazu

    for (; k < argc; k++) //prochazime argumenty
    {
        /*pokud prikaz ma jeden cislovy argument*/
        if (strcmp(argv[k], "irow") == 0 ||
            strcmp(argv[k], "drow") == 0 ||
            strcmp(argv[k], "icol") == 0 ||
            strcmp(argv[k], "dcol") == 0 ||
            strcmp(argv[k], "cset") == 0 ||
            strcmp(argv[k], "tolower") == 0 ||
            strcmp(argv[k], "toupper") == 0 ||
            strcmp(argv[k], "round") == 0 ||
            strcmp(argv[k], "int") == 0 ||
            strcmp(argv[k], "beginswith") == 0 ||
            strcmp(argv[k], "contains") == 0) {
            kopirovatRetezec(argv[k], strlen(argv[k]), nazevPrikazu); //kopirujeme nazev prikazu
            /*je-li dalsi argument*/
            if (jeArgument(argc, k)) {
                k++;                          //posuvame se v poli argumentu
                int argument = atoi(argv[k]); //ukladame argument

                /*pokud argument je mensi jednicky nebo neni cislo(atoi() vraci 0)*/
                if (argument < 1) {
                    jeChyba = true;                                         //je chyba //menime stav
                    printf("%s %s\n", chybaHodnotyArgumentu, nazevPrikazu); //vypisujeme chybu
                    break;                                                  //mimoradne ukonceni cyklu
                }

                /*pokud prikaz ma jeste jeden necislovy argument*/
                if (strcmp(nazevPrikazu, "cset") == 0 ||
                    strcmp(nazevPrikazu, "beginswith") == 0 ||
                    strcmp(nazevPrikazu, "contains") == 0) {
                    /*je-li dalsi argument*/
                    if (jeArgument(argc, k)) {
                        continue; //vracime se na zacatek cyklu
                    }
                    /*jinak*/
                    else {
                        jeChyba = true;                                  //je chyba //menime stav
                        printf("%s %s\n", chybaArgumentu, nazevPrikazu); //vypisujeme chybu
                        break;                                           //mimoradne ukonceni cyklu
                    }
                }
            }
            /*jinak (pokud dalsi argument neni)*/
            else {
                jeChyba = true;                                  //je chyba //menime stav
                printf("%s %s\n", chybaArgumentu, nazevPrikazu); //vypisujeme chybu
                break;                                           //mimoradne ukonceni cyklu
            }
        }
        /*jinak pokud prikaz ma dva cislove argumenty*/
        else if (strcmp(argv[k], "drows") == 0 ||
                 strcmp(argv[k], "dcols") == 0 ||
                 strcmp(argv[k], "copy") == 0 ||
                 strcmp(argv[k], "swap") == 0 ||
                 strcmp(argv[k], "move") == 0) {
            kopirovatRetezec(argv[k], strlen(argv[k]), nazevPrikazu); //kopirujeme nazev prikazu
            /*je-li 1. argument*/
            if (jeArgument(argc, k)) {
                k++;                           //posuvame se v poli argumentu
                int argument1 = atoi(argv[k]); //ukladame 1. argument

                /*pokud 1. argument je mensi jednicky nebo neni cislo(atoi() vraci 0)*/
                if (argument1 < 1) {
                    jeChyba = true;                                         //je chyba //menime stav
                    printf("%s %s\n", chybaHodnotyArgumentu, nazevPrikazu); //vypisujeme chybu
                    break;                                                  //mimoradne ukonceni cyklu
                }
                /*pokud je 2. argument*/
                if (jeArgument(argc, k)) {
                    k++;                           //posuvame se v poli argumentu
                    int argument2 = atoi(argv[k]); //ukladame 2. argument
                    /*pokud 2. argument je mensi jednicky nebo neni cislo(atoi() vraci 0)*/
                    if (argument2 < 1) {
                        jeChyba = true;                                         //je chyba //menime stav
                        printf("%s %s\n", chybaHodnotyArgumentu, nazevPrikazu); //vypisujeme chybu
                        break;                                                  //mimoradne ukonceni cyklu
                    }
                    /*pokud je to prikaz, kde argumenty maji byt ve spravnem poradi*/
                    if (strcmp(nazevPrikazu, "drows") == 0 ||
                        strcmp(nazevPrikazu, "dcols") == 0) {
                        /*pokud 1. argument je vetsi nez 2. argument*/
                        if (argument1 > argument2) {
                            jeChyba = true;                                        //je chyba //menime stav
                            printf("%s %s\n", chybaPoradiArgumentu, nazevPrikazu); //vypisujeme chybu
                            break;
                        }
                    }
                }
                /*jinak (pokud 2. argument neni)*/
                else {
                    jeChyba = true;                                  //je chyba //menime stav
                    printf("%s %s\n", chybaArgumentu, nazevPrikazu); //vypisujeme chybu
                    break;                                           //mimoradne ukonceni cyklu
                }
            }
            /*jinak (pokud 1. argument neni)*/
            else {
                jeChyba = true;                                  //je chyba //menime stav
                printf("%s %s\n", chybaArgumentu, nazevPrikazu); //vypisujeme chybu
                break;                                           //mimoradne ukonceni cyklu
            }
        }
        /*jinak pokud prikaz je rows*/
        else if (strcmp(argv[k], "rows") == 0) {
            kopirovatRetezec(argv[k], strlen(argv[k]), nazevPrikazu); //kopirujeme nazev prikazu

            /*je-li 1. argument*/
            if (jeArgument(argc, k)) {
                k++;                                                   //posuvame se v poli argumentu
                char argument1[DELKA_BUNKY];                           //vytvarime retezec pro 1. argument
                kopirovatRetezec(argv[k], strlen(argv[k]), argument1); //kopirujeme 1. argument do retezce

                /*pokud 1. argument neni "-" nebo je mensi jednicky nebo neni cislo(atoi() vraci 0)*/
                if (strcmp(argument1, "-") != 0 && atoi(argument1) < 1) {
                    jeChyba = true;                                         //je chyba //menime stav
                    printf("%s %s\n", chybaHodnotyArgumentu, nazevPrikazu); //vypisujeme chybu
                    break;                                                  //mimoradne ukonceni cyklu
                }
                /*pokud je 2. argument*/
                if (jeArgument(argc, k)) {
                    k++;                                                   //posuvame se v poli argumentu
                    char argument2[DELKA_BUNKY];                           //vytvarime retezec pro 2. argument
                    kopirovatRetezec(argv[k], strlen(argv[k]), argument2); //kopirujeme 2. argument do retezce

                    /*pokud 2. argument neni "-" nebo je mensi jednicky nebo neni cislo(atoi() vraci 0)*/
                    if (strcmp(argument2, "-") != 0 && atoi(argument2) < 1) {
                        jeChyba = true;                                         //je chyba //menime stav
                        printf("%s %s\n", chybaHodnotyArgumentu, nazevPrikazu); //vypisujeme chybu
                        break;                                                  //mimoradne ukonceni cyklu
                    }

                    /*pokud argumentu jsou cisla*/
                    if (atoi(argument1) != 0 && atoi(argument2) != 0) {
                        int arg1 = atoi(argument1);
                        int arg2 = atoi(argument2);

                        /*pokud 1. argument je vetsi nez 2. argument*/
                        if (arg1 > arg2) {
                            jeChyba = true;                                        //je chyba //menime stav
                            printf("%s %s\n", chybaPoradiArgumentu, nazevPrikazu); //vypisujeme chybu
                            break;                                                 //mimoradne ukoncujeme cyklus
                        }
                    }
                }
                /*jinak (pokud 2. argument neni)*/
                else {
                    jeChyba = true;                                  //je chyba //menime stav
                    printf("%s %s\n", chybaArgumentu, nazevPrikazu); //vypisujeme chybu
                    break;                                           //mimoradne ukonceni cyklu
                }
            }
            /*jinak (pokud 1. argument neni)*/
            else {
                jeChyba = true;                                  //je chyba //menime stav
                printf("%s %s\n", chybaArgumentu, nazevPrikazu); //vypisujeme chybu
                break;                                           //mimoradne ukonceni cyklu
            }
        }
        kopirovatRetezec(PRAZDNY_RETEZEC, strlen(PRAZDNY_RETEZEC), nazevPrikazu); //odstranujeme nazev prikazu
    }
    return jeChyba; //vracime vysledek kontroly	//konec funkce
}

/*Funkce vratDELIM kontrolije zadal-li uzivatel svou hodnotu pro oddelovac, pripadne nastavuje ji misto vzchozi:
* ma na vstupu pole argumentu(argv), jeho delku(argc) a ukazatel na iterator polem k
* vraci hodnotu typu char*/
char vratDELIM(char **argv, int argc, int *k) //pouzivame ukazatel na k, abychom pak pokracovali s prvniho prikazu
{
    char DELIM = VYCHOZI_DELIM; //vychozi hodnota oddelovace
    *k += 1;                    //nazev programu nas nezajima, preskakujeme ho
    /*pokud jsou nejake uzivatekske argumenty*/
    if (argc > 1) {
        /*pokud je flag pro zmenu oddelovace*/
        if (strcmp(argv[*k], "-d") == 0) {
            *k += 1;             //posouvame se v poli argumentu
            DELIM = argv[*k][0]; //zmena hodnoty oddelovace
            *k += 1;             //posouvame se v poli argumentu
        }
    }
    return DELIM; //vracime oddelovac	//konec funkce
}

/*funkce odstraniOddelovace odstranuje oddelovace ze zacatku a konce retezce:
* ma na vstupu retezec, jeho delku a znak oddelovace(DELIM)
* nic ne vraci*/
void odstraniOddelovace(char *retezec, int delkaRetezce, char DELIM) {
    int j = 0;                              //inkrementacni promenna pro prochazeni upravenem retezci
    for (int i = 0; i <= delkaRetezce; i++) //prochazime obsahem bunky
    {                                       //pouzivame <=, abychom zachytili '\0'
        /*pokud znak je oddelovac*/
        if (retezec[i] == DELIM && (i == 0 || i == delkaRetezce - 1)) {
            continue; //preskakujeme znak
        }
        retezec[j] = retezec[i]; //kopirujeme znaky z puvodniho retezce do upraveneho
        j++;                     //posouvame se v upravenem retezci
    }
} //konec funkce

/*funkce jeSloupec kontroluje je-li sloupec s zadanym cislem v radku:
* ma na vstupu retezec, jeho delku a znak oddelovace(DELIM)
* vraci hodnotu typu bool*/
bool jeSloupec(char *radek, int delkaRadku, int cisloSloupce, char DELIM) {
    bool jeSloupec = true;                                           //predpokladame, ze cislo sloupce je v radku
    int pocetSloupcu = vratitPocetSloupcu(radek, delkaRadku, DELIM); //dostavme pocet sloupcu
    /*pokud zadany sloupec neni v tabulce*/
    if (cisloSloupce > pocetSloupcu) {
        printf("Chyba. Neocekavana vstupni data.\n"); //vypisujeme chybu
        printf("Sloupec s cislem %i neni v tabulce.\n", cisloSloupce);
        jeSloupec = false;
    }
    return jeSloupec; //vracime vysledek	//konec funkce
}