#undef DEBUG
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include "myMosq.h"

using namespace std;

int main() {

    myMosq *my_Mosq = new myMosq("1550", "pc", "192.168.0.100", 1883);

	while(1){

        usleep(3000000);

	}

	return 0;
}
