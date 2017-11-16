#undef DEBUG
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include "myMosq.h"

using namespace std;

int main() {

	char estado[2];

    myMosq *my_Mosq = new myMosq(NULL, "node", "192.168.0.100", 1883);

	while(1){
        usleep(3000000);
        my_Mosq->send_message("teste");
/*
        std::cout << "Medir (s/n)? ";
        std::cin >> estado;

        if( (strcmp(estado, "s") == 0 ) or (strcmp(estado, "S") == 0 ) ){

            my_Mosq->send_message("teste");

        }else if( (strcmp(estado, "n") == 0 ) or (strcmp(estado, "N") == 0 ) ){

            break;

        }else{

            std::cout << "Informação invalida!" << std::endl;

        }
*/
	}

	return 0;
}
