//
// Created by ruiy on 20-12-6.
//

#include "MQTTThread.h"
#include "SendEncryptedImageMsg.h"
#include <Util/mqtt/MQTTClient.h>
#include <iostream>
#include <fstream>
#include <Key/Key.h>
#include <QtCore/QThread>
#include <iomanip>
#include "SendPolarEncodedMsgCmd.h"
#include "SendLdpcEncodedMsgCmd.h"
#include "SendOriginMsg.h"
using namespace std;


//#define ADDRESS "broker-cn.emqx.io:1883"

#define ADDRESS "47.122.3.201:1883"


//broker返回的状态码
int rc;

/**
 * 工具函数
 */
void MQTTThread::fill(string global_current_time_, const vector <string> &crc_marks) {
    global_current_time = global_current_time_;
    crc_infos = crc_marks;
    line_num = to_string(crc_infos.size());
}

// 用于格式化要写入sock管道的字符串
std::string MQTTThread::do_to_string(bool print) {
    std::string res;
    res += global_current_time + "\n";
    res += line_num + "\n";
    for (const auto& info: crc_infos) {
        res += info + "\n";
    } // 每一行以\n结尾

    if (print) {
        std::cout << "发送的消息是： " << res << std::endl;
    }

    return res;
}

void MQTTThread::print_mem() {//打印发送的消息

    std::cout << "global_current_time: " << global_current_time << std::endl;
    std::cout << "line_num:" << line_num << std::endl;
    std::cout << "crc_infos:" << std::endl;
    for (int i=0; i<crc_infos.size(); i++) {
        //当后面紧跟着的输出字段长度小于n的时候，在该字段前面用空格补齐；当输出字段长度大于n时，全部整体输出。
        std::cout << setw(3) << i << ": " <<crc_infos[i] << std::endl;
    }
}

/**
 * "发布"相关函数
 */
void MQTTThread::start() {

    if (start_status) {
        cout<<"发布客户端已启动,此时start_status=1"<<endl;
        return;
    }
    start_status = true;
    cout<<"发布客户端没有启动,start()将start_status置为1"<<endl;

}

void MQTTThread::create_client(MQTTClient* client,const char* clientId){
    start();

    client_id = clientId;//保存到本地
    cout << "***************创建客户端进程***************"<<std::endl;
    cout << "client_id : " << client_id <<"    start_status : "<< start_status<< endl;

    if ((rc = MQTTClient_create(client, ADDRESS, clientId,
                                MQTTCLIENT_PERSISTENCE_NONE,NULL)) != MQTTCLIENT_SUCCESS){
        printf("Failed to create client, return code %d\n",rc);
        exit(EXIT_FAILURE);
    }

    conn_opts.keepAliveInterval = 20;   // 在这段时间内没有数据相关的消息时，客户端发送一个非常小的MQTT ping消息，服务器将会确认这个
    conn_opts.cleansession = 1;
//    conn_opts.reliable = true;  // 必须完成发布的消息（已经收到确认），才能发送下一个消息
}


void MQTTThread::pub_connect(MQTTClient client){
    cout << "***************创线程与客户端连接***************"<<std::endl;
    std::cout<<"start_status="<<start_status<<"    client="<<client<<std::endl;
    std::cout<<"start_status="<<start_status<<"    client="<<client<<std::endl;
    std::cout<<"start_status="<<start_status<<"    client="<<client<<std::endl;
    if (!start_status) return;

    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS){
        printf("Failed to connect, return code %d\n",rc);
        exit(EXIT_FAILURE);
    }

}

void MQTTThread::publish_message(MQTTClient client,string topic,MQTTClient_message* pubmsg_)
{
    /***********************************************************************************************
     * 这里找到了Bob端发布失败的原因:当bob端发布zuc初始密钥协商结果的消息时，标志位start_status被置为0了，导致发布函数跳出循环，所以alice端无法收到bob端发送的明文协商信息.
     * 这里的疑惑点是：什么时候在哪里把标志位start_status置为0了，目前还没有找到.
     * 先暂时手动置start_status=1，以便bob端发出协商结果.
     *
     * */

    if (start_status==0) {
        std::cout<<"************主题为 "<<topic.c_str()<<" 的发布端未开启,MQTT客户端无法向服务器发布消息!************"<<std::endl;
    }

    if (!start_status) return;

    const char* topicName = topic.c_str();
    if ((rc = MQTTClient_publishMessage(client, topicName, pubmsg_, &token)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to publish message, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }

    cout << "Waiting for up to " << (int)(timeout/1000) << " seconds for publication" << endl;
    cout << "on topic " << topic << " for client with ClientID: " << client_id << endl;

    rc = MQTTClient_waitForCompletion(client, token, timeout);
    printf("Message with delivery token %d delivered\n", token);
}

//void MQTTThread::crc_check(MQTTClient client,string global_current_time_, const vector <complex<float>> &csi, Key &key) {
//        if (!started()) return;
//
//        //CRC
//        key.calc_uncheck_key();
//
//        if (Config::enable_encodedMsg == true) {
//            //Polar
//            key.calc_uncheck_key2();
//        }
//
//        std::vector<std::string> crc_redundances = key.get_crc_redundances();
//        // 填写具体命令
//        fill(global_current_time_, crc_redundances);
//        Config::alice_uncheck_key = key;
//        Config::alice_csi = csi;
//
//        std::string out = do_to_string(false);//打印发送的消息
//
//        const char* out1 = out.c_str();
//
//        pubmsg.payload = out1;
//        pubmsg.payloadlen = (int)strlen(out1);
//        pubmsg.qos = QOS;
//        pubmsg.retained = 0;
//
//        publish_message(client,"alice_crc",&pubmsg);//Alice端生成本端初始密钥和冗余码，发送到“alice_crc”topic的订阅端
//}

void MQTTThread::disconnect(MQTTClient client){

    start_status = false;

    if ((rc = MQTTClient_disconnect(client, 10000)) != MQTTCLIENT_SUCCESS)
        printf("Failed to disconnect, return code %d\n", rc);
    MQTTClient_destroy(&client);
    std::cout<<"---------------发布端"<<client<<"与服务器终止连接(disconnect)---------------"<<std::endl;
}


void MQTTThread::send_imgSecret(MQTTClient client, string send_file, string recv_file) {
    if (!started()) return;

    SendEncryptedImageMsg sendEncryptedMsg;

    sendEncryptedMsg.fill_file(send_file,recv_file);

//    sendEncryptedMsg.print_mem();

    std::string out = sendEncryptedMsg.file_do_to_string(false);

    char out2[out.size()];
    for (int i = 0; i < out.size(); i++){
        if (out[i] == '\0')
            out2[i] = (char)(65);
        else
            out2[i] = out[i];
    }

    pubmsg.payload = out2;
    pubmsg.payloadlen = (int)strlen(out2);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;
    publish_message(client,"send_encrypted_msg",&pubmsg);
}

void MQTTThread::send_polarmsg(MQTTClient client, std::string& msg_binstr) {
    if (!started()) return;

    SendPolarEncodedMsgCmd sendPolarEncodedMsgCmd;

    sendPolarEncodedMsgCmd.client_process(msg_binstr);

//    sendPolarEncodedMsgCmd.print_mem();

    std::string out = sendPolarEncodedMsgCmd.fill_do_to_string(false);

    char out2[out.size()];
    for (int i = 0; i < out.size(); i++){
        if (out[i] == '\0')
            out2[i] = (char)(18);
        else
            out2[i] = out[i];
    }


    pubmsg.payload = out2;
    pubmsg.payloadlen = (int)strlen(out2);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;

    publish_message(client,"send_polar_msg",&pubmsg);
}

void MQTTThread::send_ldpcmsg(MQTTClient client, std::string& msg_binstr, bool isharq) {
    if (!started()) return;

    SendLdpcEncodedMsgCmd sendLdpcEncodedMsgCmd;

    clock_t msg_encrypted_start_time=clock();

    sendLdpcEncodedMsgCmd.client_process(msg_binstr, isharq);

    clock_t msg_encrypted_end_time=clock();
    Config::msg_time=msg_encrypted_end_time-msg_encrypted_start_time;

    if(Config::enable_encodedImg) {
        Config::msg_handler_time=(double)(1000.0*(Config::img_time+Config::msg_time)/CLOCKS_PER_SEC); //ms
    } else {
        Config::msg_handler_time=(double)(1000.0*(Config::msg_time)/CLOCKS_PER_SEC); //ms
    }
    Config::img_time=0;
    Config::msg_time=0;

//    sendLdpcEncodedMsgCmd.print_mem();

    std::string out = sendLdpcEncodedMsgCmd.fill_do_to_string(false);

    char out2[out.size()];
    for (int i = 0; i < out.size(); i++){
        if (out[i] == '\0')
            out2[i] = (char)(18);
        else
            out2[i] = out[i];
    }

    pubmsg.payload = out2;
    pubmsg.payloadlen = (int)strlen(out2);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;

    publish_message(client,"send_ldpc_msg",&pubmsg);
}

void MQTTThread::alice_send_originmsg(MQTTClient client) {
    if (!started()) return;

    SendOriginMsg sendOriginMsg;

    clock_t generate_zuc_init_key_start_time=clock();

    sendOriginMsg.alice_sender_originmsg();

    clock_t generate_zuc_init_key_end_time=clock();
    Config::alice_zuc_stage1=generate_zuc_init_key_end_time-generate_zuc_init_key_start_time;
    std::cout<<"Alice生成zuc初始密钥的第1阶段所花费的时间："<<(double)(1000.0*(Config::alice_zuc_stage1)/CLOCKS_PER_SEC)<<"ms"<<std::endl;
    std::cout<<"Alice生成zuc初始密钥的第1阶段所花费的时间："<<(double)(1000.0*(Config::alice_zuc_stage1)/CLOCKS_PER_SEC)<<"ms"<<std::endl;
    std::cout<<"Alice生成zuc初始密钥的第1阶段所花费的时间："<<(double)(1000.0*(Config::alice_zuc_stage1)/CLOCKS_PER_SEC)<<"ms"<<std::endl;

    std::string out = sendOriginMsg.fill_do_to_string(false);

    char out2[out.size()];
    for (int i = 0; i < out.size(); i++){
        if (out[i] == '\0')
            out2[i] = (char)(18);
        else
            out2[i] = out[i];
    }

    pubmsg.payload = out2;
    pubmsg.payloadlen = (int)strlen(out2);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;
    publish_message(client,"alice_send_originmsg",&pubmsg);
}

void MQTTThread::bob_send_originmsg(MQTTClient client) {
    if (!started()) return;

    SendOriginMsg sendOriginMsg;

    sendOriginMsg.bob_sender_originmsg();

    std::string out = sendOriginMsg.fill_do_to_string(false);

    char out2[out.size()];
    for (int i = 0; i < out.size(); i++){
        if (out[i] == '\0')
            out2[i] = (char)(18);
        else
            out2[i] = out[i];
    }

    pubmsg.payload = out2;
    pubmsg.payloadlen = (int)strlen(out2);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;

    std::cout<<"Bob发送的zuc协商消息:"<<pubmsg.payload<<std::endl;
    std::cout<<"Bob发送的zuc协商消息:"<<pubmsg.payload<<std::endl;
    std::cout<<"Bob发送的zuc协商消息:"<<pubmsg.payload<<std::endl;

    publish_message(client,"bob_send_originmsg",&pubmsg);
//    if(Config::isSuccess) {
//        if(Config::enable_encodedImg) {
//            // 图片较大,需要等待图片发送过来,这里设置等待2s
//            std::cout<<"正在接收加密的图片..."<<std::endl;
//            usleep(2e6);
//        }
//    }

}

void MQTTThread::alice_send_zucSuccessSeq(MQTTClient client) {
    if (!started()) return;
    SendOriginMsg sendZucSucessSeq;
    sendZucSucessSeq.alice_sender_zucSucessSeq();
    std::string out = sendZucSucessSeq.fill_do_to_string(false);

    char out2[out.size()];
    for (int i = 0; i < out.size(); i++){
        if (out[i] == '\0')
            out2[i] = (char)(18);
        else
            out2[i] = out[i];
    }

    pubmsg.payload = out2;
    pubmsg.payloadlen = (int)strlen(out2);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;
    publish_message(client,"alice_send_zucSuccessSeq",&pubmsg);
}


