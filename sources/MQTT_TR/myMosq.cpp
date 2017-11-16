#include "myMosq.h"

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <time.h>

struct timespec t1;
struct timespec t2;

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

	memset(&t1, 0, sizeof(struct timespec));
	memset(&t2, 0, sizeof(struct timespec));

};

myMosq::~myMosq(){
	loop_stop();
	mosqpp::lib_cleanup();
}

void myMosq::on_connect(int rc){

	std::cout << "RC: "<< rc << std::endl;
	if ( rc == 0 ) {
		std::cout << ">> myMosq - connected with server" << std::endl;
		subscribe(NULL, "pc");
	} else {
		std::cout << ">> myMosq - Impossible to connect with server(" << rc << ")" << std::endl;
	}

}

bool myMosq::send_message(const char * _message){

    memset(&t1, 0, sizeof(struct timespec));
    memset(&t2, 0, sizeof(struct timespec));
    clock_gettime(CLOCK_REALTIME ,&t1);

	int ret = publish(NULL,this->topic, strlen(_message),_message);

	std::cout << "T1:" << t1.tv_nsec << std::endl; //ticks[0] * 1000
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

    clock_gettime(CLOCK_REALTIME ,&t2);
    std::cout << "T2:" << t2.tv_nsec << std::endl; //ticks[1] * 1000
    //std::cout << "TS1: " << t1.tv_sec << std::endl;
    //std::cout << "TS2: " << t2.tv_sec << std::endl;

    //std::cout << "T1: " << t1.tv_nsec << " | T2: " << t2.tv_nsec << std::endl;
    std::cout << "Tempo: " << ((t2.tv_sec - t1.tv_sec)*1000 + (double)(t2.tv_nsec - t1.tv_nsec)/ 1000000)  << "ms." << std::endl;
    std::cout << "Tempo: " << ((t2.tv_sec - t1.tv_sec)*1000 + (t2.tv_nsec - t1.tv_nsec)/ 1000000)  << "ms." << std::endl;


    std::cout << "\tT1: " << t1.tv_sec << "." << t1.tv_nsec << std::endl;
    std::cout << "\tT2: " << t2.tv_sec << "." << t2.tv_nsec << std::endl;


    long s = (t2.tv_sec - t1.tv_sec);
    long ns = (t2.tv_nsec - t1.tv_nsec);

    if (ns < 0){
        s = s - 1;
        ns = 1000000000 + ns;
    }




    std::cout << "\t\tS: " << s << " ns: " << ns << std::endl;


}
