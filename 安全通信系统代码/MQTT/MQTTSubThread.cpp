//
// Created by ruiy on 20-12-31.
//

#include "MQTTSubThread.h"
#include "MQTTThread.h"
#include "SendEncryptedImageMsg.h"
#include <MQTTClient.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <Lib/SP++3.0/include/vector.h>
#include <CSI/header.h>
#include <Util/Math/8b10b/8b10b.h>
#include <Util/Digest/Sha512/Sha512.h>
#include "Util/File/file.h"
#include <QtCore>
#include <QtCore/qobject.h>
#include <QtGui/QImage>
#include <Util/Image/toolLet.h>
#include "SendPolarEncodedMsgCmd.h"
#include "SendLdpcEncodedMsgCmd.h"
#include "SendOriginMsg.h"

using namespace std;

//#define ADDRESS "broker-cn.emqx.io:1883"

#define ADDRESS "47.122.3.201:1883"

volatile MQTTClient_deliveryToken deliveredtoken;
int rc2;
bool process_finish_status = false;

Key MQTTSubThread::bob_key = Key({});//之所以是静态变量是因为pub_key被两个实例使用
Key MQTTSubThread::alice_key = Key({});

clock_t startTime, endTime;


/**
 * 工具函数
 */
std::string MQTTSubThread::do_to_string(bool print) {
    std::string res;
    res += global_current_time + "\n";
    res += line_num + "\n";
    for (const auto& info: crc_infos) {
        res += info + "\n";
    } // 每一行以\n结尾

    if (print) {
        std::cout << res << std::endl;
    }

    return res;
}

void MQTTSubThread::print_mem() {
    std::cout << "global_current_time: " << global_current_time << std::endl;
    std::cout << "line_num:" << line_num << std::endl;
    std::cout << "crc_infos:" << std::endl;
    for (int i=0;i<crc_infos.size();i++) {
        //当后面紧跟着的输出字段长度小于n的时候，在该字段前面用空格补齐；当输出字段长度大于n时，全部整体输出。
        std::cout << std::setw(3) << i << ": " <<crc_infos[i] << std::endl;
    }
}

void MQTTSubThread::fill(std::string global_current_time_, const std::vector<std::string> &crc_codes_) {
    // 命令 - 具体命令 - crc_codes
    global_current_time = global_current_time_;
    crc_infos = crc_codes_;
    line_num = boost::lexical_cast<std::string>(crc_infos.size());
}

vector<string> MQTTSubThread::separated(const char* in){
    const char* delim = "\n";//分隔符(换行符)
    char* in2 = const_cast<char*>(in);//转换类型，const char* -> char*

    vector<string> res;
    char* s = strtok(in2,delim);
    while (s != NULL){//按照分隔符将
        res.push_back(s);
        s = strtok(NULL,delim);
    }
    return res;
}

void MQTTSubThread::assign(const char* in) {

    vector<string> res;

    std::string jf = in;

    if (jf == "low_right_rate")
        global_current_time = jf;
    else{
        vector<string> res = separated(in);
        global_current_time = res[0];
        line_num = res[1];
        int line_num_int = boost::lexical_cast<int>(line_num);
        crc_infos.clear();


        for (int i=2;i<line_num_int + 2;i++) {
            std::string line = res[i];
            crc_infos.push_back(line);
        }
    }
}

/**
 * "订阅"相关函数
 */
void delivered(void *context, MQTTClient_deliveryToken dt)
{
    printf("Message with token value %d delivery confirmed\n", dt);
    deliveredtoken = dt;
}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{


    MQTTThread pub_harq;
    MQTTSubThread sub_mqttSubThread2;
    SendEncryptedImageMsg sendEncryptedMsg2;
    SendPolarEncodedMsgCmd sendPolarEncodedMsgCmd2;
    SendLdpcEncodedMsgCmd sendLdpcEncodedMsgCmd2;
    SendOriginMsg sendOriginMsg;
    SendOriginMsg sendZucSuccessSeq;
    bool harqflag = false;

    const char* payloadptr;

    printf("Message arrived\n");
    printf("     topic: %s\n", topicName);
    printf("     message: \n");

    payloadptr = message->payload;

    std::string tmp_topicName = topicName;
    if (tmp_topicName == "alice_crc")
        std::cout<<"---------------------------------------------错误!我们已经舍弃crc机制,bob端不可能订阅得到alice_crc---------------------------------------------"<<std::endl;
//        sub_mqttSubThread2.server_process(payloadptr);
    else if (tmp_topicName == "bob_crc_marks")
        std::cout<<"---------------------------------------------错误!我们已经舍弃crc机制,alice端不可能订阅得到bob_crc_marks---------------------------------------------"<<std::endl;
        //bob发送校验结果来了，alice启动处理程序
//        sub_mqttSubThread2.client_process(payloadptr);
    else if (tmp_topicName == "send_encrypted_msg"){
        clock_t img_decrypted_start_time=clock();

        sendEncryptedMsg2.server_process(payloadptr);

        clock_t img_decrypted_end_time=clock();
        Config::img_time=img_decrypted_end_time-img_decrypted_start_time;

        std::cout<<"解密图片所花费的时间"<<(double)(1000.0*(Config::img_time)/CLOCKS_PER_SEC)<<std::endl;
        std::cout<<"解密图片所花费的时间"<<(double)(1000.0*(Config::img_time)/CLOCKS_PER_SEC)<<std::endl;
        std::cout<<"解密图片所花费的时间"<<(double)(1000.0*(Config::img_time)/CLOCKS_PER_SEC)<<std::endl;
//        if(Config::enable_encodedImg) {
//            // 发送图片,所以还需要算上解密图片的时间
//            total_decrypted_time+=img_decrypted_end_time-img_decrypted_start_time;
//            Config::msg_handler_time=total_decrypted_time;
//            total_decrypted_time=0;
//        }
    }
    else if (tmp_topicName == "send_polar_msg"){
        sendPolarEncodedMsgCmd2.server_process(payloadptr);
    }
    else if (tmp_topicName == "send_ldpc_msg"){
        //bob端文字解密时间
        clock_t msg_decrypted_start_time=clock();

        sendLdpcEncodedMsgCmd2.server_process(payloadptr, Config::bob_send_harq);

        clock_t msg_decrypted_end_time=clock();
        Config::msg_time=msg_decrypted_end_time-msg_decrypted_start_time;
        std::cout<<"解密文字所花费的时间"<<(double)(1000.0*(Config::msg_time)/CLOCKS_PER_SEC)<<std::endl;
        std::cout<<"解密文字所花费的时间"<<(double)(1000.0*(Config::msg_time)/CLOCKS_PER_SEC)<<std::endl;
        std::cout<<"解密文字所花费的时间"<<(double)(1000.0*(Config::msg_time)/CLOCKS_PER_SEC)<<std::endl;

        if(Config::enable_encodedImg) {
            Config::msg_handler_time=(double)(1000.0*(Config::img_time+Config::msg_time)/CLOCKS_PER_SEC); //ms
            std::cout<<"解密图片和文字所花费的时间"<<Config::msg_handler_time<<std::endl;
            std::cout<<"解密图片和文字所花费的时间"<<Config::msg_handler_time<<std::endl;
            std::cout<<"解密图片和文字所花费的时间"<<Config::msg_handler_time<<std::endl;
        } else {
            Config::msg_handler_time=(double)(1000.0*(Config::msg_time)/CLOCKS_PER_SEC); //ms
        }
        Config::img_time=0;
        Config::msg_time=0;
    }
    else if (tmp_topicName == "bob_send_harq") {
        if (Config::role_index == Config::ROLES::ALICE) {
            //   MQTTSubThread::erase_thiskey();
            Config::alice_send_parity = true;
        }
    }
    else if (tmp_topicName=="alice_send_originmsg") {
//        std::cout<<"========================================================================="<<endl;
//        std::cout<<"Config::bob_recv_originmsg="<<Config::bob_recv_originmsg<<endl;
        clock_t generate_zuc_init_key_start_time=clock();

        sendOriginMsg.bob_msg_handler(payloadptr);

        clock_t generate_zuc_init_key_end_time=clock();
        Config::generate_zuc_init_key_time=(double)(1000.0*(generate_zuc_init_key_end_time-generate_zuc_init_key_start_time)/CLOCKS_PER_SEC); //ms
//        Config::bob_recv_originmsg=payloadptr;
//        std::cout<<"Config::bob_recv_originmsg="<<Config::bob_recv_originmsg<<endl;
//        std::cout<<"========================================================================="<<endl;
//        WorkThread workThread;
//        workThread.bob_msg_handler(payloadptr);
    }
    else if (tmp_topicName=="bob_send_originmsg") {
        clock_t generate_zuc_init_key_start_time=clock();

        sendOriginMsg.alice_msg_handler(payloadptr);

        clock_t generate_zuc_init_key_end_time=clock();
        Config::alice_zuc_stage2=generate_zuc_init_key_end_time-generate_zuc_init_key_start_time;

        std::cout<<"Alice生成zuc初始密钥的第2阶段所花费的时间："<<(double)(1000.0*(Config::alice_zuc_stage2)/CLOCKS_PER_SEC)<<"ms"<<std::endl;
        std::cout<<"Alice生成zuc初始密钥的第2阶段所花费的时间："<<(double)(1000.0*(Config::alice_zuc_stage2)/CLOCKS_PER_SEC)<<"ms"<<std::endl;
        std::cout<<"Alice生成zuc初始密钥的第2阶段所花费的时间："<<(double)(1000.0*(Config::alice_zuc_stage2)/CLOCKS_PER_SEC)<<"ms"<<std::endl;

        Config::generate_zuc_init_key_time=(double)(1000.0*(Config::alice_zuc_stage1+Config::alice_zuc_stage2)/CLOCKS_PER_SEC); //ms
        Config::alice_zuc_stage1=0;
        Config::alice_zuc_stage2=0;
    } else if(tmp_topicName=="alice_send_zucSuccessSeq") {
        sendZucSuccessSeq.bob_zucSuccessSeq_handler(payloadptr);
    }


    MQTTClient_freeMessage(&message);
//    MQTTClient_free(topicName);

    //各process处理完之后,代表订阅的循环等待就可以跳出了
    process_finish_status = true;

    return 1;
}

void connlost(void *context, char *cause)
{
    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause);
}

////Bob接收冗余码并进行处理:得到BOB端秘钥,以及正确率大于阈值就发送给Alice CRC校验结果
//void MQTTSubThread::server_process(const char* in) {
//    // 从USRP获得信号到处理生成初始的密钥Ka或Kb
//    clock_t generate_key_start_time,generate_key_end_time;
//
//    // 记录bob端收到alice端发过来的crc的时间(start)
//    generate_key_start_time=clock();
//
//    assign(in);
//
////    splab::Vector<std::complex<float>> splab_type_csi = RX_OUT(Config::rx_file.c_str()).getColumn(2);
//    splab::Vector<std::complex<float>> splab_type_csi = RX_OUT_V2X(Config::rx_file.c_str());
//    std::vector<complex<float>> csi(splab_type_csi.begin(), splab_type_csi.end());
////    std::vector<complex<float>> csi_plot(csi.end()-(1152-30),csi.end());
//    std::vector<complex<float>> csi_plot;
//
//    if(Config::is_alice()){
//        csi_plot.assign(csi.begin(),csi.begin()+1152-30);
//    }else{
//        if(Config::enable_winner_filter) {
//            cout << "i'm bob couting winner filter" << endl;
//            vector<float> csi_11(1122);
//            vector<float> csi_8(1122);
//            for (int i = 0; i < csi_11.size(); ++i) {
//                csi_11[i] = abs(csi[i + 1122 * 3]);
//                csi_8[i] = abs(csi[i + 1122 * 2]);
//            }
//            vector<float> w_filter = winner_filter(338, csi_8, csi_11);
//            vector<float> csi_predict(1122);
//            for (int i = 0; i < csi_predict.size(); ++i) {
//                if (i > w_filter.size() - 2) {
//                    for (int j = 0; j < w_filter.size(); ++j)
//                        csi_predict[i] += w_filter[j] * csi_11[i - j];
//                } else {
//                    for (int j = 0; j < w_filter.size(); ++j) {
//                        if (i - j >= 0) {
//                            csi_predict[i] += w_filter[j] * csi_11[i - j];
//                        } else {
//                            break;
//                        }
//                    }
//                }
//            }
////            for (int i = 0; i < csi_predict.size(); ++i) {
////                cout << csi_predict[i] << endl;
////            }
//            for (int i = 30; i < csi_predict.size(); ++i) {
//                csi_plot.push_back(complex<float>{csi_predict[i], 0.0});
//            }
//        }else{
//            csi_plot.assign(csi.end()-(1152-30),csi.end());
//        }
//    }
//    std::vector<float> csi_abs;
//    for(int i=0;i<csi_plot.size();++i){
//        csi_abs.push_back(abs(csi_plot[i]));
//    }
//
//    emit plot_csi_signalBOB(csi_plot);
//
//    Key key1(csi_abs);
//    bob_key = key1;
//    bob_key.calc_uncheck_key();//没有经过校验的key
//    bob_key.calc_uncheck_key2();//没有经过校验的key
//
//    // 记录bob端生成初始密钥的时间(end)
//    generate_key_end_time=clock();
//    Config::generate_key_time=(double)(1000.0*(generate_key_end_time-generate_key_start_time)/CLOCKS_PER_SEC); //ms
//
//    float right_rate = bob_key.check_by_redundance(crc_infos);
//
//    // 填写具体命令
//    fill(global_current_time, bob_key.get_crc_marks());//传的校验结果
//    set_extends("bob_key", bob_key);//Bob端自己的key,协商密钥
//    std::vector<std::string> key10b = encode_8b_to_10b(bob_key.get_keys());//已校验的key进行8B/10B编码
//
//    std::string sha512_code= sha512(key10b, 8);//散列函數，4096位,512 * 8
//
//    cout << "right_rate = -------------------------------------------" << right_rate << endl;//0.1
//
//    if (right_rate >= Config::right_rate_threshold) { // 如果正确率过小，则不保存该次过程协商的结果，使用上次的秘钥
//        cout << "--------------------密钥生成111111111111111111组---------------------" << endl;
//        save_key(sha512_code, Config::tmp_key_file, Config::key_dir, get_global_current_time(global_current_time), true);
//        save_csi(Config::rx_file, Config::csi_dir, get_global_current_time(), true);//接收到的未计算的信号,就是接收信号
//        save_calculated_csi(csi, Config::csi_dir, get_global_current_time(), true);//真正的经过计算的csi信号
//        Config::key = bob_key;
//        Config::key_num += 512 * 8;
//        Config::result = do_to_string(false);
//        Config::secret_status = true;
//cout << "---------------------------------------------------------------------get_global_current_time-----------" << get_global_current_time() << endl;
//    } else{
//        cout << "--------------------low_right_rate,saving wrong key---------------------" << endl;
//        save_key(sha512_code, Config::tmp_key_file, Config::key_dir, get_global_current_time(global_current_time)+"_low_right_rate", true);
//        save_csi(Config::rx_file, Config::csi_dir, get_global_current_time()+"_low_right_rate", true);//接收到的未计算的信号,就是接收信号
//        save_calculated_csi(csi, Config::csi_dir, get_global_current_time()+"_low_right_rate", true);//真正的经过计算的csi信号
//        //正确率低就置false,跳过这次调和结果
//        Config::secret_status = false;
//        Config::result = "low_right_rate";
//    }
//}
//
//void MQTTSubThread::client_process(const char* in) {
////    SendEncryptedImageMsg sendEncryptedMsg3;
//
//    assign(in);
//
//    if (global_current_time == "low_right_rate"){
//        cout << "正确率太低了,舍弃此次秘钥------------------------" << endl;
//        save_csi(Config::rx_file, Config::csi_dir, get_global_current_time()+"_low_right_rate", true);
//        std::vector<std::complex<float>> csi = Config::alice_csi;
//        save_calculated_csi(csi, Config::csi_dir, get_global_current_time()+"_low_right_rate", true);
//    }
//    else{
//        int cnt = 0;
//        for (const auto& info: crc_infos) if (info == "1") cnt++;
//
//        Config::bob_crc_marks = crc_infos;//接收到的信息是校验结果  原始版本
//
//        float right_rate = static_cast<float>(cnt) / crc_infos.size();
//        std::cout << std::endl << "## right rate: " << cnt << "/" << crc_infos.size() << "=" << right_rate << " ##" << std::endl;
//
//        if (right_rate < Config::right_rate_threshold) return; // 如果正确率过小，则使用上次的秘钥
//
//        Key key = Config::alice_uncheck_key;
//
//        key.check_by_marks(crc_infos);//得到Alice端的协商密钥
//
//        std::vector<std::string> key10b = encode_8b_to_10b(key.get_keys());
//
//        std::string sha512_code= sha512(key10b, 8);
//
//        save_key(sha512_code, Config::tmp_key_file, Config::key_dir, get_global_current_time(), true);
//        save_csi(Config::rx_file, Config::csi_dir, get_global_current_time(), true);
//        std::vector<std::complex<float>> csi = Config::alice_csi;
//        save_calculated_csi(csi, Config::csi_dir, get_global_current_time(), true);
//        Config::key = key;
//        Config::key_num += 512 * 8;
//        Config::result = sha512_code;//alice存储编码后的密钥
//
//        cout << "----------------------密钥生成111111111111111111组---------------------------------" << endl;
//        cout << "---------------------------------------------------------------------get_global_current_time-----------" << get_global_current_time() << endl;
//
////        if (Config::enable_encodedImg == true) {
////            sendEncryptedMsg3.client_process(sha512_code);//alice端绘制图形
////        }
//    }
//}

void MQTTSubThread::start() {

    if (start_status) return;

    start_status = true;
}


void MQTTSubThread::create_client(MQTTClient* client,const char* clientId){

    start();

    process_finish_status = false;

    client_id = clientId;

    if ((rc2 = MQTTClient_create(client, ADDRESS, clientId,
                                MQTTCLIENT_PERSISTENCE_NONE,NULL)) != MQTTCLIENT_SUCCESS){
        printf("Failed to create client, return code %d\n",rc2);
        exit(EXIT_FAILURE);
    }

    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

}

void setCallbacks(MQTTClient client){

    MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);
}

void MQTTSubThread::sub_connect(MQTTClient client)
{

    if (!start_status) return;

    if ((rc2 = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS){
        printf("Failed to connect, return code %d\n",rc2);
        exit(EXIT_FAILURE);
    }
}

void MQTTSubThread::subscribe(MQTTClient client, string topic_,Key &key){

    if (!start_status) return;

    process_finish_status = false;//现在订阅连接都是一直开启的，每次订阅释放掉message之后，为了避免下次订阅还没有收到消息就结束订阅

    topic = topic_;

    if (topic == "alice_crc")//alice发送给bob crc冗余码 bob端订阅收到的
        bob_key = key;
    else if (topic == "bob_crc_marks")//bob发送给alice 校验结果 Alice端订阅收到的
        alice_key = key;

    const char* topicName = topic.c_str();

    cout << "Subscribing to topic: " << topic << endl;
    cout << "for client: " << client_id << " using Qos: " << QOS << endl;
    cout << endl;

    MQTTClient_subscribe(client, topicName, QOS);

    double time = 0;
    startTime = clock();

    while (true){
        if (process_finish_status){
            break;
        }
        endTime = clock();
        time = endTime - startTime;
        if ((double)(time / CLOCKS_PER_SEC) > 20.0){
            cout << "我订阅超时啦-----------111-------" << endl;
            break;
        }
        //ui停止键
        if (!Config::workThreadStatus)
            break;
    }
}
//用来订阅加密信息
void MQTTSubThread::subscribe(MQTTClient client, string topic_){
    if (!start_status) return;

    process_finish_status = false;//现在订阅连接都是一直开启的，每次订阅释放掉message之后，为了避免下次订阅还没有收到消息就结束订阅

    topic = topic_;

    const char* topicName = topic.c_str();

    cout << "Subscribing to topic: " << topic << endl;
    cout << "for client: " << client_id << " using Qos: " << QOS << endl;
    cout << endl;

    MQTTClient_subscribe(client, topicName, QOS);

    double time = 0;
    startTime = clock();

    while (true){
        if (process_finish_status){
            break;
        }
        endTime = clock();
        time = endTime - startTime;
        if ((double)(time / CLOCKS_PER_SEC) > 20.0){
            cout << "我订阅超时啦-----------222-------" << endl;
            break;
        }
        if (!Config::workThreadStatus)
            break;
    }
}

void MQTTSubThread::sub_disconnect(MQTTClient client){

    start_status = false;

    if ((rc2 = MQTTClient_disconnect(client, timeout)) != MQTTCLIENT_SUCCESS)
        printf("Failed to disconnect, return code %d\n", rc2);
    MQTTClient_destroy(&client);

}

//alice端从密钥缓冲区中删除上一组密钥
void MQTTSubThread::erase_thiskey() {
    auto buffer_end = Config::alice_key_buffer.end();
    auto buffer_begin = Config::alice_key_buffer.begin();
    auto ptr = buffer_begin + Config::alice_key_buffer_idx;
    std::string temp_key = Config::alice_lastkey;
    for (int i = temp_key.size()-1; i >= 0; i--) {
        *ptr = temp_key[i];
        if (ptr == buffer_begin) ptr = buffer_end;
        ptr--;
    }
    Config::alice_key_buffer_idx = ptr - buffer_begin;
}
