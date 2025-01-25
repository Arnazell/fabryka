#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/prctl.h>
#include <dirent.h>
#include "utilities.h"
#include "machine.h"
#include "supplier.cpp"
#include "director.h"

void cleanup(int signal);
void child_terminated(int signal);
bool is_pid_active(pid_t pid);
pid_t pids[6];
int active_processes = 6;
bool stopped_processes[6] = {false, false, false, false, false, false};

int main(int argc, char *argv[])
{
  // Inicjacja IPC;
  {
    long capacity_units = utils::calculate_capacity_units(argc, argv);
    utils::detect_issue(capacity_units==INVALID_USER_INPUT, "Nieprawidlowa pojemnosc magazynu.");

    // klucz
    key_t key_ipc = ftok("/tmp", 32 );
    utils::detect_issue(key_ipc==IPC_RESULT_ERROR, "Blad przy tworzeniu klucza ftok");

    // zbior semaforow
    int semid = semget(key_ipc, 12, IPC_CREAT | 0600);
    utils::detect_issue(semid==IPC_RESULT_ERROR, "Blad przy tworzeniu zbioru semaforow");

    // pamiec wspoldzielona
    long segment_size = capacity_units*UNIT_SIZE + sizeof(warehouse::warehouse_data);
    int memid =  shmget(key_ipc, segment_size, IPC_CREAT | 0600);
    utils::detect_issue(memid==IPC_RESULT_ERROR, "Blad przy tworzeniu segmentu pamieci");

    // api magazynu
    auto warehouse = warehouse::WarehouseManager(key_ipc, semid);
    warehouse.initiailze(capacity_units);
    utils::semafor_set(semid, sem_wareohuse_working, 1);

    // po zainicjowaniu, nie ma sensu kopiowac tych obiektow do nowycnh procesow
  }
  prctl(PR_SET_NAME, "so_Fabryka", NULL, NULL, NULL);

  // PROCESY DOSTAWCY
  pids[0] = fork();
  if (pids[0] == 0){
    prctl(PR_SET_NAME, "so_SupplierX", NULL, NULL, NULL);
    supplier<utils::ProductX>(speed_supply_x);
    std::exit(0);
  }
  utils::detect_issue(pids[0]==FORK_RESULT_ERROR, "Blad operacji fork");

  pids[1] = fork();
  if (pids[1]  == 0){
    prctl(PR_SET_NAME, "so_SupplierY", NULL, NULL, NULL);
    supplier<utils::ProductY>(speed_supply_y);

    std::exit(0);
  }
  utils::detect_issue( pids[1] ==FORK_RESULT_ERROR, "Blad operacji fork");

  pids[2] = fork();
  if (pids[2] == 0){
    prctl(PR_SET_NAME, "so_SupplierZ", NULL, NULL, NULL);
    supplier<utils::ProductZ>(speed_supply_z);

    std::exit(0);
  }
  utils::detect_issue(pids[2]==FORK_RESULT_ERROR, "Blad operacji fork");

  // PROCESY MASZYN
  pids[3] = fork();
  if (pids[3] == 0){
    prctl(PR_SET_NAME, "so_MachineA", NULL, NULL, NULL);
    machine(speed_machine_a);
    std::exit(0);
  }
  utils::detect_issue(pids[3]==FORK_RESULT_ERROR, "Blad operacji fork");

  pids[4]= fork();
  if (pids[4] == 0){
    prctl(PR_SET_NAME, "so_MachineB", NULL, NULL, NULL);
    machine(speed_machine_b);
    std::exit(0);
  }
  utils::detect_issue(pids[4]==FORK_RESULT_ERROR, "Blad operacji fork");

  // PROCES DYREKTORA
  pids[5] = fork();
  if (pids[5] == 0){
    prctl(PR_SET_NAME, "so_Director", NULL, NULL, NULL);
    director(pids[0], pids[1], pids[2], pids[3], pids[4]);
    std::exit(0);
  }
  utils::detect_issue(pids[4]==FORK_RESULT_ERROR, "Blad operacji fork");

  // obsluga sygnalow pozwalajaca na zwolnienie zasobow
  utils::detect_issue(signal(SIGTERM, cleanup)==SIG_ERR, "Blad ustawiania handlera syngalu");
  utils::detect_issue(signal(SIGINT, cleanup)==SIG_ERR, "Blad ustawiania handlera syngalu"); // ctrl+z
  utils::detect_issue(signal(SIGCHLD, child_terminated)==SIG_ERR, "Blad ustawiania handlera syngalu");

  // czekamy dopolki sa jeszcze dzialajace procesy
  // czekanie na procesy potomne utils::detect_issue(waitpid(pid_x, NULL, 0)==WAIT_ERROR, "Blad czekania na pid_x");
  waitpid(pids[0], NULL, 0) ;
  waitpid(pids[1], NULL, 0);
  waitpid(pids[2], NULL, 0) ;
  waitpid(pids[3], NULL, 0);
  waitpid(pids[4], NULL, 0) ;
  waitpid(pids[5], NULL, 0);

  // usun wszystkie mechanizmy ipc
  cleanup(0);
}

void cleanup(int signal)
{
  key_t key_ipc = ftok("/tmp", 32);
  utils::detect_issue(key_ipc==IPC_RESULT_ERROR, "SPRZATANIE: Blad przy tworzeniu klucza ftok");
  int semid = semget(key_ipc, 0, 0);
  utils::detect_issue(semid==IPC_RESULT_ERROR, "SPRZATANIE: Blad przy pobieraniu id semaforow");

  int memid = shmget(key_ipc, 0, 0);
  utils::detect_issue(memid==IPC_RESULT_ERROR, "SPRZATANIE: Blad przy pobieraniu id pamieci");

  int result1 = semctl(semid, 0, IPC_RMID);
  utils::detect_issue(result1==IPC_RESULT_ERROR, "SPRZATANIE: Blad przy usuwaniu zbioru semaforow");

  int result2 = shmctl(memid, IPC_RMID, NULL);
  utils::detect_issue(result2==IPC_RESULT_ERROR, "SPRZATANIE: Blad przy usuwaniu segmentu pameci");

  exit(0);
}

void child_terminated(int signal)
{
  int status;
  // odbieramy pid zakonczonego procesu, zeby nie byl zombie
  pid_t pid = waitpid(-1, &status, WNOHANG);

  // sprawdzamy ktory proces sie zakonczyl i reagujemy
  if (pid == pids[0])
  {
    std::cout<<"Ktorys dostawca nie zyje. Pozostali dostawcy koncza prace"<<std::endl;
    //kill(pids[1], SIGUSR1);
    //kill(pids[2], SIGUSR1);
  }
  else if (pid == pids[1])
  {
    std::cout<<"Dostawca Y nie zyje. Pozostali dostawcy koncza prace"<<std::endl;
    kill(pids[0], SIGUSR1);
    kill(pids[2], SIGUSR1);
  }
  else if (pid == pids[2])
  {
    std::cout<<"Dostawca Z nie zyje. Pozostali dostawcy koncza prace"<<std::endl;
   kill(pids[0], SIGUSR1);
   kill(pids[1], SIGUSR1);
  }
  else if (pid == pids[3])
  {
    std::cout<<"Maszyna A wylaczona. ALe to nic."<<std::endl;

  }
  else if (pid == pids[4])
  {
    std::cout<<"Maszyna A wylaczona. ALe to nic."<<std::endl;
  }
  else if (pid == pids[5])
  {
    std::cout<<"Dyrektor wylaczony. Cala fabryka sie zamyka"<<std::endl;

   kill(pids[0], SIGUSR1);
   kill(pids[1], SIGUSR1);
   kill(pids[2], SIGUSR1);
   kill(pids[3], SIGUSR1);
   kill(pids[4], SIGUSR1);
  }
}


bool is_pid_active(pid_t pid)
{
  // Ścieżka do katalogu procesu w /proc
  std::string path = "/proc/" + std::to_string(pid);

  // Sprawdzamy, czy katalog dla danego PID istnieje
  DIR* dir = opendir(path.c_str());
  if (dir)
  {
    // Jeśli katalog istnieje, oznacza to, że proces jest aktywny

    return true;
  }
  return false;
}


