#ifndef MYMOSQ_H_
#define MYMOSQ_H_

#include <mosquittopp.h>

class myMosq : public mosqpp::mosquittopp
{
private:
	const char    *     host;
	const char    *     id;
	const char    *     topic;
	int                 port;
	int                 keepalive;

	void on_connect(int rc);
	void on_disconnect(int rc);
	void on_publish(int mid);

    struct foo_t {
        int a;
        int b;
        int c;
	} dado;

public:
	myMosq(const char *id, const char * _topic, const char *host, int port);
	~myMosq();
	bool send_message(const char * _message);
	void on_message(const struct mosquitto_message *message);
	void on_subscribe(int mid, int qos_count, const int *granted_qos);
};

#endif /* MYMOSQ_H_ */
