#include "myMosq.h"

#include <iostream>
#include <string.h>
#include <stdio.h>

myMosq::myMosq(const char * _id, const char * _topic, const char * _host, int _port) : mosquittopp(_id)
{

	mosqpp::lib_init();

	this->keepalive = 60;

	this->id = _id;
	this->port = _port;
	this->host = _host;
	this->topic = _topic;

	//connect_async(host, port, keepalive);
	//loop_start();
	connect(host, port, keepalive);

};

myMosq::~myMosq() {
	loop_stop();
	mosqpp::lib_cleanup();
}

void myMosq::on_connect(int rc)
{
	std::cout << "RC: "<< rc << std::endl;
	if ( rc == 0 ) {
		std::cout << ">> myMosq - connected with server" << std::endl;
	} else {
		std::cout << ">> myMosq - Impossible to connect with server(" << rc << ")" << std::endl;
	}
}

bool myMosq::send_message(const char * _message)
{
	// publish(ID, TOPICO, TAMANHO DA MENS., MENSAGEM, QOS, RETORNO(?));    Should return MOSQ_ERR_SUCCESS
	//int ret = publish(NULL,this->topic, strlen(_message),_message,1,false);
	int ret = publish(NULL,this->topic, strlen(_message),_message);
	return ( ret == MOSQ_ERR_SUCCESS );
}

void myMosq::on_disconnect(int rc) {
	std::cout << ">> myMosq - disconnection(" << rc << ")" << std::endl;
}

void myMosq::on_publish(int mid)
{
	std::cout << ">> myMosq - Message (" << mid << ") succeed to be published " << std::endl;
}
