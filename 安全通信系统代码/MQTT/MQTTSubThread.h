//
// Created by ruiy on 20-12-31.
//

#ifndef ROI_MQTTSUBTHREAD_H
#define ROI_MQTTSUBTHREAD_H

#include <MQTTClient.h>
#include <boost/any.hpp>
#include <Config/Config.h>
#include <QtCore>
using namespace std;

void delivered(void *context, MQTTClient_deliveryToken dt);
int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message);
void connlost(void *context, char *cause);
void setCallbacks(MQTTClient client);

class MQTTSubThread: public QObject{
    Q_OBJECT

signals:
    void plot_csi_signalBOB(const std::vector<std::complex<float>> &csi);
private:
    bool start_status;

    map<string, boost::any> extends;

public:
    MQTTSubThread():

    start_status(false),
    conn_opts(MQTTClient_connectOptions_initializer),
    client_id(""),
    QOS(1),
    timeout(1000L)
    {
        connect(this, SIGNAL(plot_csi_signalBOB(const std::vector<std::complex<float>> &)), (const QObject*)get_global_widget(), SLOT(plot_csi(const std::vector<std::complex<float>> &)));
    }

public:
    MQTTClient_connectOptions conn_opts;

    //初始化MQTT参数
    string client_id;
    string topic;
    string payload;
    int  QOS;
    unsigned long  timeout;

    string global_current_time;
    string line_num;
    vector<std::string> crc_infos;

    //Key类
    static Key alice_key;//Alice端密钥
    static Key bob_key;//Bob端密钥

    void start();

//订阅端函数部分
    void create_client(MQTTClient* client,const char* clientId);
    void sub_connect(MQTTClient client);
    void subscribe(MQTTClient client, string topic_,Key &key);
    void subscribe(MQTTClient client, string topic_);
    void sub_disconnect(MQTTClient client);

//密钥函数部分
    bool started() {return start_status;}
    std::string do_to_string(bool print);
    void print_mem();
    void fill(std::string global_current_time_, const std::vector<std::string> &crc_codes_);
    void assign(const char* in);
    vector<string> separated(const char* in);
//    void server_process(const char* in);
    static void erase_thiskey();    //

    void set_extends (string key, const boost::any &value){
        extends[key] = value;
    }

    boost::any get_extends (string key){
        return extends[key];
    }


    //发布端的订阅处理部分
//    void client_process(const char* in);

    void stop(){
        start_status = false;
    }
};


#endif //ROI_MQTTSUBTHREAD_H
