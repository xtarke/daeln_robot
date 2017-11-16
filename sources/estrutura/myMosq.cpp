#include "myMosq.h"

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <time.h>

myMosq::myMosq(const char * _id, const char * _topic, const char * _host, int _port) : mosquittopp(_id){

	mosqpp::lib_init();

	this->keepalive = 60;

	this->id = _id;
	this->port = _port;
	this->host = _host;
	this->topic = _topic;

	//connect_async(host, port, keepalive);
	connect(host, port, keepalive);
	loop_start();

	dado.a = 1;
	dado.b = 2;
	dado.c = 3;

};

myMosq::~myMosq(){
	loop_stop();
	mosqpp::lib_cleanup();
}

void myMosq::on_connect(int rc){

	if ( rc == 0 ) {
		std::cout << ">> myMosq - connected with server (" << rc << ")" << std::endl;
		subscribe(NULL, "node");
	} else {
		std::cout << ">> myMosq - Impossible to connect with server(" << rc << ")" << std::endl;
	}

}

bool myMosq::send_message(const char * _message){

    dado.a++;
    dado.b++;
    dado.c++;

	int ret = publish(NULL, this->topic, sizeof(foo_t), &dado);

	return ( ret == MOSQ_ERR_SUCCESS );

}

void myMosq::on_disconnect(int rc){

	std::cout << ">> myMosq - disconnection(" << rc << ")" << std::endl;

}

void myMosq::on_publish(int mid){

	std::cout << ">> myMosq - Message (" << mid << ") succeed to be published " << std::endl;

}

/* USAR PARA CONTAR O TEMPO */

void myMosq::on_subscribe(int mid, int qos_count, const int *granted_qos){

	printf("Subscription succeeded.\n");

}

void myMosq::on_message(const struct mosquitto_message *message){
    publish(NULL, "pc", 8, "retorno");
    std::cout << "enviado!" << std::endl;

}
