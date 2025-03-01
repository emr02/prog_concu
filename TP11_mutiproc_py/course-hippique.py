import multiprocessing as mp
import os, time, math, random, sys, ctypes

# Juin 2019
# Cours hippique
# Version très basique, sans mutex sur l'écran, sans arbitre, sans annoncer le gagant, ... ...

# Quelques codes d'échappement (tous ne sont pas utilisés)
CLEARSCR="\x1B[2J\x1B[;H"          #  Clear SCReen
CLEAREOS = "\x1B[J"                #  Clear End Of Screen
CLEARELN = "\x1B[2K"               #  Clear Entire LiNe
CLEARCUP = "\x1B[1J"               #  Clear Curseur UP
GOTOYX   = "\x1B[%.2d;%.2dH"       #  ('H' ou 'f') : Goto at (y,x), voir le code

DELAFCURSOR = "\x1B[K"             #  effacer après la position du curseur
CRLF  = "\r\n"                     #  Retour à la ligne

# VT100 : Actions sur le curseur
CURSON   = "\x1B[?25h"             #  Curseur visible
CURSOFF  = "\x1B[?25l"             #  Curseur invisible

# VT100 : Actions sur les caractères affichables
NORMAL = "\x1B[0m"                  #  Normal
BOLD = "\x1B[1m"                    #  Gras
UNDERLINE = "\x1B[4m"               #  Souligné


# VT100 : Couleurs : "22" pour normal intensity
CL_BLACK="\033[22;30m"                  #  Noir. NE PAS UTILISER. On verra rien !!
CL_RED="\033[22;31m"                    #  Rouge
CL_GREEN="\033[22;32m"                  #  Vert
CL_BROWN = "\033[22;33m"                #  Brun
CL_BLUE="\033[22;34m"                   #  Bleu
CL_MAGENTA="\033[22;35m"                #  Magenta
CL_CYAN="\033[22;36m"                   #  Cyan
CL_GRAY="\033[22;37m"                   #  Gris

# "01" pour quoi ? (bold ?)
CL_DARKGRAY="\033[01;30m"               #  Gris foncé
CL_LIGHTRED="\033[01;31m"               #  Rouge clair
CL_LIGHTGREEN="\033[01;32m"             #  Vert clair
CL_YELLOW="\033[01;33m"                 #  Jaune
CL_LIGHTBLU= "\033[01;34m"              #  Bleu clair
CL_LIGHTMAGENTA="\033[01;35m"           #  Magenta clair
CL_LIGHTCYAN="\033[01;36m"              #  Cyan clair
CL_WHITE="\033[01;37m"                  #  Blanc

#-------------------------------------------------------
# Juin 2019
# Cours hippique
# Version très basique, sans mutex sur l'écran, sans arbitre, sans annoncer le gagant, ... ...

# Quelques codes d'échappement (tous ne sont pas utilisés)
CLEARSCR="\x1B[2J\x1B[;H"          #  Clear SCReen
CLEAREOS = "\x1B[J"                #  Clear End Of Screen
CLEARELN = "\x1B[2K"               #  Clear Entire LiNe
CLEARCUP = "\x1B[1J"               #  Clear Curseur UP
GOTOYX   = "\x1B[%.2d;%.2dH"       #  ('H' ou 'f') : Goto at (y,x), voir le code

DELAFCURSOR = "\x1B[K"             #  effacer après la position du curseur
CRLF  = "\r\n"                     #  Retour à la ligne

# VT100 : Actions sur le curseur
CURSON   = "\x1B[?25h"             #  Curseur visible
CURSOFF  = "\x1B[?25l"             #  Curseur invisible

# VT100 : Actions sur les caracteres affichables
NORMAL = "\x1B[0m"                  #  Normal
BOLD = "\x1B[1m"                    #  Gras
UNDERLINE = "\x1B[4m"               #  Souligné


# VT100 : Couleurs : "22" pour normal intensity
CL_BLACK="\033[22;30m"                  #  Noir. NE PAS UTILISER. On verra rien !!
CL_RED="\033[22;31m"                    #  Rouge
CL_GREEN="\033[22;32m"                  #  Vert
CL_BROWN = "\033[22;33m"                #  Brun
CL_BLUE="\033[22;34m"                   #  Bleu
CL_MAGENTA="\033[22;35m"                #  Magenta
CL_CYAN="\033[22;36m"                   #  Cyan
CL_GRAY="\033[22;37m"                   #  Gris

# "01" pour quoi ? (bold ?)
CL_DARKGRAY="\033[01;30m"               #  Gris foncé
CL_LIGHTRED="\033[01;31m"               #  Rouge clair
CL_LIGHTGREEN="\033[01;32m"             #  Vert clair
CL_YELLOW="\033[01;33m"                 #  Jaune
CL_LIGHTBLU= "\033[01;34m"              #  Bleu clair
CL_LIGHTMAGENTA="\033[01;35m"           #  Magenta clair
CL_LIGHTCYAN="\033[01;36m"              #  Cyan clair
CL_WHITE="\033[01;37m"                  #  Blanc

#-------------------------------------------------------

LONGEUR_COURSE = 100
keep_running = mp.Value(ctypes.c_bool, True)
lock = mp.Lock()  # Ajout d'un verrou pour la synchronisation

# Une liste de couleurs à affecter aléatoirement aux chevaux
lyst_colors=[CL_WHITE, CL_RED, CL_GREEN, CL_BROWN , CL_BLUE, CL_MAGENTA, CL_CYAN, CL_GRAY,
             CL_DARKGRAY, CL_LIGHTRED, CL_LIGHTGREEN,  CL_LIGHTBLU, CL_YELLOW, CL_LIGHTMAGENTA, CL_LIGHTCYAN]

def effacer_ecran() : print(CLEARSCR,end='')
def erase_line_from_beg_to_curs() : print("\033[1K",end='')
def curseur_invisible() : print(CURSOFF,end='')
def curseur_visible() : print(CURSON,end='')
def move_to(lig, col) : print("\033[" + str(lig) + ";" + str(col) + "f",end='')

def en_couleur(Coul) : print(Coul,end='')
def en_rouge() : print(CL_RED,end='') # Un exemple !


# La tache d'un cheval
def un_cheval(ma_ligne : int, positions):
    col=1

    while col < LONGEUR_COURSE and keep_running.value :
        with lock:  # Protéger l'affichage avec un verrou
            move_to(ma_ligne * 3 + 1, col)
            erase_line_from_beg_to_curs()
            en_couleur(lyst_colors[ma_ligne % len(lyst_colors)])
            print('_______\/')
            move_to(ma_ligne * 3 + 2, col)
            print('/|__{}__/\\/'.format(chr(ord('A') + ma_ligne)))
            move_to(ma_ligne * 3 + 3, col)
            print('/ \\ / \\')
        col += 1
        positions[ma_ligne] = col  # Mettre à jour la position du cheval
        time.sleep(0.1 * random.randint(1, 5))

# Processus Arbitre
def arbitre(positions):
    while keep_running.value:
        with lock:
            leader = max(range(len(positions)), key=lambda i: positions[i])
            last = min(range(len(positions)), key=lambda i: positions[i])
            move_to(len(positions) * 3 + 5, 1)
            print(f"En tête: {chr(ord('A') + leader)}")
            move_to(len(positions) * 3 + 6, 1)
            print(f"En dernier: {chr(ord('A') + last)}")
        time.sleep(1)

# La partie principale :
def course_hippique() :
    Nb_process=20
    mes_process = [0 for i in range(Nb_process)]
    positions = mp.Array('i', Nb_process)  # Tableau partagé pour les positions des chevaux

    effacer_ecran()

    # Pari
    pari = input("Pariez sur le cheval gagnant (A-T): ").upper()
    if pari < 'A' or pari > 'T':
        print("Pari invalide")
        return

    for i in range(Nb_process):  # Lancer Nb_process processus
        # Crée un nouveau processus pour chaque cheval
        mes_process[i] = mp.Process(target=un_cheval, args=(i, positions))
        mes_process[i].start()  # Démarre le processus

    # Lancer le processus Arbitre
    # Crée et démarre un processus pour l'arbitre
    arbitre_process = mp.Process(target=arbitre, args=(positions,))
    arbitre_process.start()

    move_to(Nb_process * 3 + 10, 1)
    print("Tous lancés")

    for i in range(Nb_process): mes_process[i].join()  # Attend que tous les processus chevaux se terminent

    keep_running.value = False
    arbitre_process.join()  # Attend que le processus arbitre se termine

    # Annonce du gagnant
    winner = max(range(len(positions)), key=lambda i: positions[i])
    move_to(24, 1)
    print(f"Fini! Le gagnant est {chr(ord('A') + winner)}")
    if chr(ord('A') + winner) == pari:
        print("Félicitations! Vous avez parié sur le bon cheval!")
    else:
        print("Dommage! Vous avez perdu votre pari.")
    
# La partie principale :
if __name__ == "__main__" :
    course_hippique()

