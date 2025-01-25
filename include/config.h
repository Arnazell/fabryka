#ifndef CONFIG_H
#define CONFIG_H

// sciezka do pliku z magazynem
#define WAREHOUSE_PATH "warehouse_data"

// rodzaje bledow & wynikow
#define IPC_RESULT_ERROR -1
#define FORK_RESULT_ERROR -1
#define INVALID_USER_INPUT -1
#define WAIT_ERROR -1

// zwracane przez warehouse
#define MACHINE_RECIEVED_PRODUCT 0
#define WAREHOUSE_SUCCESFUL_INSERT 0
#define WAREHOUSE_CLOSED 1

// definicja semaforow
#define sem_dostepne_x 0
#define sem_dostepne_y 1
#define sem_dostepne_z 2

#define sem_wolne_miejsca_x 3
#define sem_wolne_miejsca_y 4
#define sem_wolne_miejsca_z 5

#define sem_shelf_x 6
#define sem_shelf_y 7
#define sem_shelf_z 8

#define sem_wareohuse_working 9
#define sem_factory_working 9

// rodzaje komend dyrektora
#define COMMAND_STOP_WAREHOUSE 1
#define COMMAND_STOP_FACTORY 2
#define COMMAND_STOP_WAREHOUSE_FACTORY_AND_SAVE 3
#define COMMAND_STOP_WAREHOUSE_FACTORY_NO_SAVE 4

// czasy wykonywania czynnosci -> w milisekundach
#define speed_machine_a 100//10000
#define speed_machine_b 100//10000
#define speed_supply_x 100000000//1000
#define speed_supply_y 100000000//1000
#define speed_supply_z 100000000//1000

// maksymalna zajetosc pamieci ram przez magazyn w %
#define MAX_RAM 0.005






#endif //CONFIG_H
