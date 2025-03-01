import threading
import time
import random
import sys

class BillesManager:
    def __init__(self, N, nb_max_billes, m):
        # Initialize variables similar to the C code
        self.N = N
        self.nb_max_billes = nb_max_billes
        self.nb_billes_dispo = nb_max_billes
        self.m = m
        self.mutex = threading.Lock()  # Equivalent to pthread_mutex_t
        self.semaphore = threading.Semaphore(0)  # Equivalent to sem_t
        self.canEnd = False

    def controller(self):
        # Equivalent to the controller function in C
        while not self.canEnd:
            with self.mutex:
                if self.nb_billes_dispo < 0 or self.nb_billes_dispo > self.nb_max_billes:
                    print("erreur :/")
                    print(self.nb_billes_dispo)
                    sys.exit(-1)
            time.sleep(1)

    def demander(self, nBilles):
        # Equivalent to the demander function in C
        with self.mutex:
            while nBilles > self.nb_billes_dispo:
                self.mutex.release()
                self.semaphore.acquire()
                self.mutex.acquire()
            self.nb_billes_dispo -= nBilles

    def rendre(self, nBilles):
        # Equivalent to the rendre function in C
        with self.mutex:
            self.nb_billes_dispo += nBilles
            for _ in range(self.N):
                self.semaphore.release()

    def travailleur(self, nb_bille_necessaires):
        # Equivalent to the travailleur function in C
        for _ in range(self.m):
            self.demander(nb_bille_necessaires)
            print(f"travaille avec {nb_bille_necessaires} billes")
            time.sleep(random.randint(1, 10))
            print(f"rend {nb_bille_necessaires} billes")
            self.rendre(nb_bille_necessaires)

def main():
    # Equivalent to the main function in C
    if len(sys.argv) < 4:
        print("./billes <nombre de threads> <nombre de billes max> <nombre d'itération pour chaque thread>")
        sys.exit(-1)

    N = int(sys.argv[1])
    nb_max_billes = int(sys.argv[2])
    m = int(sys.argv[3])

    manager = BillesManager(N, nb_max_billes, m)
    billesNecessaires = [random.randint(1, nb_max_billes) for _ in range(N)]

    threads = []
    for i in range(N):
     
        t = threading.Thread(target=manager.travailleur, args=(billesNecessaires[i],))
        threads.append(t)
        t.start()

    controller_thread = threading.Thread(target=manager.controller)
    controller_thread.start()

    for t in threads:
        t.join()

    manager.canEnd = True
    controller_thread.join()

if __name__ == "__main__":
    main()