//============================================================================
// Name        : mosquittto.cpp
// Author      : Juca
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "myMosq.h"

using namespace std;

int main() {

	myMosq *my_Mosq = new myMosq(NULL, "temperature/celsius", "localhost", 1883);
	my_Mosq->send_message("22.32");
	//std::cout << "\n--Send_mensage--" << std::endl;

	return 0;
}
