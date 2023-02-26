//
// Created by ruiy on 20-12-6.
//

#ifndef ROI_MQTTTHREAD_H
#define ROI_MQTTTHREAD_H

#include <queue>
#include <complex>
#include <Config/Config.h>
#include <vector.h>
#include <Util/mqtt/MQTTClient.h>
#include <QtCore>

using namespace std;
class MQTTThread {

private:
    bool start_status;

    map<string, boost::any> extends;

    Key pub_key = Key({});

public:
    MQTTThread():

    start_status(false),
    conn_opts(MQTTClient_connectOptions_initializer),
    pubmsg(MQTTClient_message_initializer),
    client_id(""),
    QOS(1),
    timeout(1000L)
    {

    }

public:

    MQTTClient_connectOptions conn_opts;
    MQTTClient_message pubmsg;
    MQTTClient_deliveryToken token;

    //初始化MQTT参数
    string client_id;
    string payload;
    int  QOS;
    unsigned long  timeout;


    string content;
    string size;

    string global_current_time;
    string line_num;
    vector<std::string> crc_infos;


    void start();
    void create_client(MQTTClient* client,const char* clientId);
    void pub_connect(MQTTClient client);
    void publish_message(MQTTClient client,string topic,MQTTClient_message* pubmsg_);
    void disconnect(MQTTClient client);


//    void crc_check(MQTTClient client,string global_current_time_, const vector <complex<float>> &csi, Key &key);
//    void polar_compute_uncheck_key(const vector <complex<float>> &csi, Key &key);
    void send_imgSecret(MQTTClient client, string send_file, string recv_file);
    void send_polarmsg(MQTTClient client, std::string& code_bits);
    void send_ldpcmsg(MQTTClient client, std::string& code_bits, bool);
    void send_harq_mark();

    void alice_send_originmsg(MQTTClient client);
    void bob_send_originmsg(MQTTClient client);

    void alice_send_zucSuccessSeq(MQTTClient client);

    void fill(string global_current_time_, const vector <string> &crc_marks);
    bool started() {return start_status;}

    void set_extends (string key, const boost::any &value){
        extends[key] = value;
    }

    boost::any get_extends (string key){
        return extends[key];
    }

    std::string do_to_string(bool print);

    void print_mem();

    //订阅部分处理逻辑

    void sub_init(Key &key);
    std::vector<std::string> crc_marks(Key &key);

    void stop(){
        start_status = false;
        std::cout<<"---------------客户端与服务器停止连接(stop())---------------"<<std::endl;
    }

};


#endif //ROI_MQTTTHREAD_H
