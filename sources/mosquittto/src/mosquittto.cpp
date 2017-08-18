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

	int x;

	//myMosq juca(&x, "sensor", "localhost", 1883);

	//juca.publish()
	myMosq *my_Mosq = new myMosq(“BlaBla”, “test”, “remotehost”, 1883);
	my_Mosq->send_message(“Hello world!”);


	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!




	return 0;
}
