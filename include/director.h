//
// Created by andrzej on 12/21/24.
//

#ifndef DIRECTOR_H
#define DIRECTOR_H


class director
{
    public:
        director();
        ~director();

    // klucz ipc
    key_t key_ipc;
    int semid;
    int memid;

    void main_loop();

};


#endif //DIRECTOR_H
