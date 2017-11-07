#include <iostream>

#include "myMosq.h"

using namespace std;

int main() {

	//char estado;

	//std::cout << "L (Ligado) ou D (Desligado): ";
	//std::cin >> estado;

	myMosq *my_Mosq = new myMosq(NULL, "temperature/celsius", "localhost", 1883);
	my_Mosq->send_message("22.32");
	//std::cout << "\n--Send_mensage--" << std::endl;

	return 0;
}
