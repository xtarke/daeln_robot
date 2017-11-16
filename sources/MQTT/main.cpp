#undef DEBUG
#include <iostream>
#include <string.h>

#include "myMosq.h"

using namespace std;

int main() {

	char estado[2];

    myMosq *my_Mosq = new myMosq(NULL, "node", "192.168.0.100", 1883);

	while(1){

        std::cout << "L (Ligado) ou D (Desligado): ";
        std::cin >> estado;

        if( strcmp (estado, "l") == 0 )
            strcpy(estado, "L");
        else if( strcmp (estado, "d") == 0 )
            strcpy(estado, "D");

        #ifdef DEBUG
            std::cout << "estado: " << estado << std::endl;
        #endif

        if( (strcmp (estado, "L") != 0) and (strcmp (estado, "D") != 0) ){
            std::cout << "Informação invalida!" << std::endl;
        }else{
            my_Mosq->send_message(estado);
        }

	}

	return 0;
}
