//
// Created by calt on 18-7-3.
//

#ifndef ROI_WORKTHREAD_H
#define ROI_WORKTHREAD_H

#include <QThread>
#include <TopBlock/Transceiver.h>
#include <Network/SocketClientThread.h>
#include <Network/SocketServerThread.h>
#include <Util/Semaphore/Semaphore.h>
#include <Util/Time/time.h>
#include <CSI/header.h>
#include <Util/Math/CalcLet/calc_let.h>
#include <Util/Math/Convert/Convert.h>
#include <Util/Encode/bch/bch_encode.h>
#include <Util/Encode/bch/bch_decode.h>
#include <Util/Encode/turbo/tur_encoder.h>
#include <Util/Encode/turbo/tur_decoder.h>

#include <MQTT/MQTTThread.h>
#include <MQTTClient.h>
#include <MQTT/MQTTSubThread.h>
#include <cstring>
#include <glog/logging.h>
#include "MQTT/SendEncryptedImageMsg.h"
#include "Util/Digest/Sha512/Sha512.h"
#include "Util/File/file.h"
#include "Util/RandomnessTest/RandomTest.h"

using namespace std;


class WorkThread : public QThread {
Q_OBJECT

    typedef boost::exception_detail::clone_impl<boost::exception_detail::error_info_injector<boost::bad_any_cast> > AnyCastException;
    typedef boost::exception_detail::clone_impl<boost::exception_detail::error_info_injector<boost::system::system_error>> ConnectionException;

signals:

    void plot_csi_signalALICE(const std::vector<std::complex<float>> &csi);

    void plot_csi_signalBOB(const std::vector<std::complex<float>> &csi);///////////////////////////////////////
    void alert_msg_signal(const QString &title, const QString &text);

private slots:

    /**
     * 加密图片部分
     */
    //alice加密图片发布
    void alice_send_imgSecret(const std::string &file_path) {
        //alice发布端(加密图片)(2阶段)
        startTime = clock();
        alice_pub_imageThread.send_imgSecret(alice_encrypted_image_client, Config::tmp_encrypted_image,
                                             Config::tmp_encrypted_image);
        endTime = clock();
        cout
                << "alice777----alice发布端(加密图片)(2阶段)运行时间是:------------------------------------------------------------------------"
                << (double) (endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
    };

    //bob订阅加密图片信息 or eve
    void sub_recv_imgSecret() {
        //bob订阅端(解密图片)(2阶段)
        startTime = clock();
        bob_sub_imageThead.subscribe(bob_decrypted_image_client, "send_encrypted_msg");
        endTime = clock();
        cout
                << "bob777----bob订阅端(解密图片)(2阶段)运行时间是:------------------------------------------------------------------------"
                << (double) (endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
    }

    /**
     * polar加密信息部分
     */
    //alice发布polar加密信息
    void pub_send_polar_encoded_msg(const std::string &msg) {
        //把msg转化为01字符串
        Convert converter;
        std::string msg_binstr = converter.str2binstr(msg);
        //alice发布端(加密信息)(2阶段)
        startTime = clock();
        alice_pub_polarmsgThread.send_polarmsg(alice_polarmsg_client, msg_binstr);
        endTime = clock();
        cout << "alice888-----alice发布端(加密信息)(2阶段)运行时间是:----------------------------------------------"
             << (double) (endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
    };

    //bob接收Polar加密信息进行解密 or eve
    void sub_recv_polar_decoded_msg() {
        //bob订阅端(解密信息)(2阶段)
        startTime = clock();
        bob_sub_polarmsgThead.subscribe(bob_polarmsg_client, "send_polar_msg");
        endTime = clock();
        cout
                << "bob888----bob订阅端(解密信息)(2阶段)运行时间是:------------------------------------------------------------------------"
                << (double) (endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
    }

    /**
     * ldpc加密信息部分
     */
    //alice发布ldpc加密信息
    void pub_send_ldpc_encoded_msg(const std::string &msg) {
        //把msg转化为01字符串
        Convert converter;
        std::string msg_binstr = converter.str2binstr(msg);

        //alice发布端(加密信息)(2阶段)
        startTime = clock();
        alice_pub_ldpcmsgThread.send_ldpcmsg(alice_ldpcmsg_client, msg_binstr, false);
        endTime = clock();
        cout << "alice888-----alice发布端(加密信息)(2阶段)运行时间是:----------------------------------------------"
             << (double) (endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
    };

    void pub_send_parity(const std::string &parity) {
        // 冗余比特直接发送
        std::string parity_str(parity);
        startTime = clock();
        alice_pub_ldpcmsgThread.send_ldpcmsg(alice_ldpcmsg_client, parity_str, true);
        endTime = clock();
        cout << "alice发布端(冗余比特)运行时间是:----------------------------------------------"
             << (double) (endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;

    }

    //bob接收ldpc加密信息进行解密 or eve
    void sub_recv_ldpc_decoded_msg() {
        //bob订阅端(解密信息)(2阶段)
        startTime = clock();
        bob_sub_ldpcmsgThead.subscribe(bob_ldpcmsg_client, "send_ldpc_msg");
        endTime = clock();
        cout
                << "bob888----bob订阅端(解密信息)(2阶段)运行时间是:------------------------------------------------------------------------"
                << (double) (endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
    }

    /*
     * 明文消息部分(zuc密钥协商过程)
     */
    void alice_pub_send_origin_msg() {
        //alice发布明文消息(2阶段)
        startTime = clock();
        alice_pub_originmsgThread.alice_send_originmsg(alice_pub_originmsg_client);
        endTime = clock();
        cout << "alice888.111-----alice发布端(明文消息)(2阶段)运行时间是:----------------------------------------------"
             << (double) (endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
    };

    // bob接收alice发送过来的明文信息
    void bob_sub_recv_origin_msg() {
        //bob订阅端(明文信息)(2阶段)
        startTime = clock();
        bob_sub_originmsgThead.subscribe(bob_sub_originmsg_client, "alice_send_originmsg");
        endTime = clock();
        cout
                << "bob888.111----bob订阅端(明文信息)(2阶段)运行时间是:------------------------------------------------------------------------"
                << (double) (endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
    }

    void bob_pub_send_origin_msg() {
        //bob发布明文消息(2阶段)
        startTime = clock();
        bob_pub_originmsgThread.bob_send_originmsg(bob_pub_originmsg_client);
        endTime = clock();
        cout << "alice888.222-----bobe发布端(明文消息)(2阶段)运行时间是:----------------------------------------------"
             << (double) (endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
    };

    // alice接收bob发送过来的明文信息(第n次密钥协商成功或失败)
    void alice_sub_recv_origin_msg() {
        //alice订阅端(明文信息)(2阶段)
        startTime = clock();
        alice_sub_originmsgThead.subscribe(alice_sub_originmsg_client, "bob_send_originmsg");
        endTime = clock();
        cout
                << "alice----alice订阅端(明文信息)(2阶段)运行时间是:------------------------------------------------------------------------"
                << (double) (endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
    }

    // Alice端发送zuc协商成功的序号
    void alice_pub_send_zucSuccess_seq() {
        startTime = clock();
        alice_pub_zucSuccessSeqThread.alice_send_zucSuccessSeq(alice_pub_zucSuccessSeq_client);
        endTime = clock();
        cout << "alice发布端(发送zuc协商成功的序号)运行时间是:----------------------------------------------"
             << (double) (endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
    }

    // bob接收alice发送过来的zuc协商成功序号
    void bob_sub_recv_zucSuccess_seq() {
        startTime = clock();
        bob_sub_zucSuccessSeqThead.subscribe(bob_sub_zucSuccessSeq_client, "alice_send_zucSuccessSeq");
        endTime = clock();
        cout
                << "bob订阅端(订阅zuc协商成功的序号)运行时间是:------------------------------------------------------------------------"
                << (double) (endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
    }


public:
    Transceiver transceiver;
private:
    semaphore sema_loop;
    bool status_start;

    //加密图片的发送
    MQTTThread alice_pub_imageThread;
    MQTTClient alice_encrypted_image_client;
    MQTTSubThread bob_sub_imageThead;
    MQTTClient bob_decrypted_image_client;

    //加密信息的发送
    MQTTThread alice_pub_polarmsgThread, alice_pub_ldpcmsgThread;
    MQTTClient alice_polarmsg_client, alice_ldpcmsg_client;
    MQTTSubThread bob_sub_polarmsgThead, bob_sub_ldpcmsgThead;
    MQTTClient bob_polarmsg_client, bob_ldpcmsg_client;

    //HARQ相关
    MQTTThread bob_pub_harq;
    MQTTSubThread alice_sub_harq;
    MQTTClient alice_sub_harq_client;
    MQTTClient bob_pub_harq_client;

    // alice端明文信息的发送
    MQTTThread alice_pub_originmsgThread,alice_pub_zucSuccessSeqThread;
    MQTTClient alice_pub_originmsg_client,alice_pub_zucSuccessSeq_client;
    MQTTSubThread bob_sub_originmsgThead,bob_sub_zucSuccessSeqThead;
    MQTTClient bob_sub_originmsg_client,bob_sub_zucSuccessSeq_client;

    // bob端明文信息的发送
    MQTTThread bob_pub_originmsgThread;
    MQTTClient bob_pub_originmsg_client;
    MQTTSubThread alice_sub_originmsgThead;
    MQTTClient alice_sub_originmsg_client;

//    // zuc密钥流随机性检测
//    RandomTest randomTest;

    std::vector<std::complex<float>> csi;
    std::vector<float> csi_abs;
    std::vector<std::complex<float>> csi_plot;
    Key key = Key({});
    std::mutex mu;
    clock_t startTime, endTime;

    // 与时间相关
    // 1.从USRP获得信号到处理生成初始的密钥Ka或kb
    clock_t generate_key_start_time, generate_key_end_time;

public:
//    WorkThread (): alice_MqttThread(), alice_MqttSubThread(), bob_MqttThread(), bob_MqttSubThread(), alice_pub_imageThread(), bob_sub_imageThead(), status_start(false), alice_pub_ldpcmsgThread(), bob_sub_ldpcmsgThead(), alice_sub_harq(), bob_pub_harq(),bob_pub_originmsgThread(),alice_sub_originmsgThead(),alice_pub_originmsgThread(),bob_sub_originmsgThead(), sema_loop(1){};
    WorkThread()
            : alice_pub_imageThread(), bob_sub_imageThead(), status_start(false), alice_pub_ldpcmsgThread(),
              bob_sub_ldpcmsgThead(), alice_sub_harq(), bob_pub_harq(), bob_pub_originmsgThread(),
              alice_sub_originmsgThead(), alice_pub_originmsgThread(), bob_sub_originmsgThead(),
              alice_pub_zucSuccessSeqThread(),bob_sub_zucSuccessSeqThead(),sema_loop(1) {};

    void stop_transceiver() {
        std::lock_guard<std::mutex> guard(mu); // 在停止时，不运行send_frame发送数据
        transceiver.stop();
    }

    // 控制USRP发送信号
    bool send_frame() {
        //todo：file_status是啥
        //每一轮开始时使文件状态位为false
        transceiver.set_file_status(false);

        while (true) {
            //alice的导频数据发送模块每隔1s发送导频信号,当bob导频检测模块检测到导频信号后，将tx_file置true，就触发自己的导频数据发送模块.alice导频检测模块一旦检测到信号就会break
            if (transceiver.check_file_status()) {
                // 记录alice端从usrp接收到有用的信号的开始时间
                generate_key_start_time = clock();
                break;
            }

            bool check_usrp_connection = false;
            {
                std::lock_guard<std::mutex> guard(mu); // 在send_frame发送数据时，不允许stop_transceiver的行为发生
                check_usrp_connection = transceiver.check_usrp_connection(); // 判断usrp是否连接
                if (check_usrp_connection) {
                    transceiver.transmit_file(); // 若是连接的，则发送数据
//                    if (Config::is_fdd()) transceiver.set_file_status(false); // 如果是fdd，则应该同时打开收，防止在发射之前已经收到上一次的数据3
                }
            }

            if (!check_usrp_connection) { // 如果usrp未连接，则报错，并停止当前线程
                emit alert_msg_signal("Usrp Connection Error", "connect usrp fail before work");
                wait(); // 挂起当前线程，等待被杀死
            }
            //10^6us == 1s
            usleep(Config::resend_interval * 1000);
        }
        return true;
    }

    void compute_csi() {
        splab::Vector<std::complex<float>> splab_type_csi = RX_OUT_V2X(Config::rx_file.c_str());
        csi.assign(splab_type_csi.begin(), splab_type_csi.end());
        if (Config::is_alice()) {
            csi_plot.assign(csi.begin(), csi.begin() + 1152 - 30);
        } else {
            cout << "i'm not alice\n" << endl;
            if (Config::enable_winner_filter) {
                cout << "i'm bob couting winner filter" << endl;
                vector<float> csi_11(1122);
                vector<float> csi_8(1122);
                for (int i = 0; i < csi_11.size(); ++i) {
                    csi_11[i] = abs(csi[i + 1122 * 3]);
                    csi_8[i] = abs(csi[i + 1122 * 2]);
                }
                vector<float> w_filter = winner_filter(338, csi_8, csi_11);
                vector<float> csi_predict(1122);
                for (int i = 0; i < csi_predict.size(); ++i) {
                    if (i > w_filter.size() - 2) {
                        for (int j = 0; j < w_filter.size(); ++j)
                            csi_predict[i] += w_filter[j] * csi_11[i - j];
                    } else {
                        for (int j = 0; j < w_filter.size(); ++j) {
                            if (i - j >= 0) {
                                csi_predict[i] += w_filter[j] * csi_11[i - j];
                            } else {
                                break;
                            }
                        }
                    }
                }
                for (int i = 30; i < csi_predict.size(); ++i) {
                    csi_plot.push_back(complex<float>{csi_predict[i], 0.0});
                }
            } else {
                csi_plot.assign(csi.end() - (1152 - 30), csi.end());
            }
        }
        csi_abs.resize(csi_plot.size());
        for (int i = 0; i < csi_plot.size(); ++i) {
            csi_abs[i] = abs(csi_plot[i]);
        }
        csi_plot.erase(csi_plot.begin() + 550, csi_plot.begin() + 580);
    }

    void alice_ports_init() {
        //alice发布端(发布明文信息)(1阶段)
        startTime = clock();
        alice_pub_originmsgThread.create_client(&alice_pub_originmsg_client, "Alice_pub_originmsg_clientId");
        alice_pub_originmsgThread.pub_connect(alice_pub_originmsg_client);
        endTime = clock();
        cout
                << "alice111.111----alice发布端(发布明文信息)(1阶段)运行时间是:------------------------------------------------------------------------"
                << (double) (endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;

        startTime = clock();
        alice_pub_zucSuccessSeqThread.create_client(&alice_pub_zucSuccessSeq_client, "Alice_pub_zucSuccessSeq_clientId");
        alice_pub_zucSuccessSeqThread.pub_connect(alice_pub_zucSuccessSeq_client);
        endTime = clock();
        cout
                << "alice发布端(发布zuc协商成功的序号)(1阶段)运行时间是:------------------------------------------------------------------------"
                << (double) (endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;


        //alice订阅端(订阅明文消息)(1阶段)
        startTime = clock();
        alice_sub_originmsgThead.create_client(&alice_sub_originmsg_client, "Alice_sub_originmsg_clientId");
        setCallbacks(alice_sub_originmsg_client);
        alice_sub_originmsgThead.sub_connect(alice_sub_originmsg_client);
        endTime = clock();
        cout
                << "alice----alice订阅端(订阅明文消息)(1阶段)运行时间是:------------------------------------------------------------------------"
                << (double) (endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;

        //alice订阅端(订阅重传请求)
        if (Config::enable_harq) {
            startTime = clock();
            alice_sub_harq.create_client(&alice_sub_harq_client, "Alice_sub_harq_clientId");
            setCallbacks(alice_sub_harq_client);
            alice_sub_harq.sub_connect(alice_sub_harq_client);
            alice_sub_harq.subscribe(alice_sub_harq_client, "bob_send_harq");
            endTime = clock();
            cout
                    << "alice222.555----alice订阅端(订阅重传请求)(1阶段)运行时间是:------------------------------------------------------------------------"
                    << (double) (endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;

        }

        //是否发送图片信息
        if (Config::enable_encodedImg == true) {
            //alice发布端(加密图片)(1阶段)
            startTime = clock();
            alice_pub_imageThread.create_client(&alice_encrypted_image_client, "Alice_encrypted_image_clientId");
            alice_pub_imageThread.pub_connect(alice_encrypted_image_client);
            endTime = clock();
            cout
                    << "alice333----alice发布端(加密图片)(1阶段)运行时间是:------------------------------------------------------------------------"
                    << (double) (endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
        }
        //是否发送文字信息
        if (Config::enable_encodedMsg == true) {
            //alice发布端(加密信息)(1阶段)
            startTime = clock();
            alice_pub_ldpcmsgThread.create_client(&alice_ldpcmsg_client, "Alice_ldpcmsg_clientId");
            alice_pub_ldpcmsgThread.pub_connect(alice_ldpcmsg_client);
            endTime = clock();
            cout
                    << "alice444----alice发布端(加密文字)(1阶段)运行时间是:------------------------------------------------------------------------"
                    << (double) (endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
        }
    }

    void bob_ports_init() {

        //bob订阅端(接收明文信息)(1阶段)
        startTime = clock();
        if (Config::role_index == Config::ROLES::BOB)
            bob_sub_originmsgThead.create_client(&bob_sub_originmsg_client, "Bob_sub_originmsg_clientId");
        else
            bob_sub_originmsgThead.create_client(&bob_sub_originmsg_client, "Eve_sub_originmsg_clientId");

        setCallbacks(bob_sub_originmsg_client);
        bob_sub_originmsgThead.sub_connect(bob_sub_originmsg_client);
        endTime = clock();
        cout
                << "bob111.111----bob订阅端(接收明文信息)(1阶段)运行时间是:------------------------------------------------------------------------"
                << (double) (endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;

        //bob订阅端(接收zuc协商成功的序号)(1阶段)
        startTime = clock();
        if (Config::role_index == Config::ROLES::BOB)
            bob_sub_zucSuccessSeqThead.create_client(&bob_sub_zucSuccessSeq_client, "Bob_sub_zucSuccessSeq_clientId");
        else
            bob_sub_zucSuccessSeqThead.create_client(&bob_sub_zucSuccessSeq_client, "Eve_sub_zucSuccessSeq_clientId");

        setCallbacks(bob_sub_zucSuccessSeq_client);
        bob_sub_zucSuccessSeqThead.sub_connect(bob_sub_zucSuccessSeq_client);
        endTime = clock();
        cout
                << "bob订阅端(接收zuc协商成功的序号)(1阶段)运行时间是:------------------------------------------------------------------------"
                << (double) (endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;


        //bob发布端(发送zuc初始密钥协商结果)(1阶段)
        startTime = clock();
        if (Config::role_index == Config::ROLES::BOB) {
            bob_pub_originmsgThread.create_client(&bob_pub_originmsg_client, "bob_pub_originmsg_clientId");
            if (Config::enable_harq) {
                bob_pub_harq.create_client(&bob_pub_harq_client, "bob_pub_harq_clientId"); //harq协议
            }
        } else
            bob_pub_originmsgThread.create_client(&bob_pub_originmsg_client, "Eve_pub_originmsg_clientId");
        if (Config::enable_harq) {
            bob_pub_harq.pub_connect(bob_pub_harq_client);
        }

        bob_pub_originmsgThread.pub_connect(bob_pub_originmsg_client);
        endTime = clock();
        cout
                << "bob----bob发布端(发送zuc初始密钥协商结果,HARQ)(1阶段)运行时间是:------------------------------------------------------------------------"
                << (double) (endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;

        //是否发送图片信息
        if (Config::enable_encodedImg == true) {
            //bob订阅端(解密图片)(1阶段)
            startTime = clock();

            if (Config::role_index == Config::ROLES::BOB)
                bob_sub_imageThead.create_client(&bob_decrypted_image_client, "Bob_decrypted_image_clientId");
            else
                bob_sub_imageThead.create_client(&bob_decrypted_image_client, "Eve_decrypted_image_clientId");

            setCallbacks(bob_decrypted_image_client);
            bob_sub_imageThead.sub_connect(bob_decrypted_image_client);
            endTime = clock();
            cout
                    << "bob333----bob订阅端(接收解密图片)(1阶段)运行时间是:------------------------------------------------------------------------"
                    << (double) (endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
        }

        //是否发送文字信息
        if (Config::enable_encodedMsg == true) {
            //bob订阅端(解密信息)(1阶段)
            startTime = clock();
            if (Config::role_index == Config::ROLES::BOB)
                bob_sub_ldpcmsgThead.create_client(&bob_ldpcmsg_client, "Bob_ldpcmsg_clientId");
            else
                bob_sub_ldpcmsgThead.create_client(&bob_ldpcmsg_client, "Eve_ldpcmsg_clientId");

            setCallbacks(bob_ldpcmsg_client);
            bob_sub_ldpcmsgThead.sub_connect(bob_ldpcmsg_client);
            endTime = clock();
            cout
                    << "bob444----bob订阅d端(接收解密信息)(1阶段)运行时间是:------------------------------------------------------------------------"
                    << (double) (endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
        }
    }


    // 根据csi计算未经过调和的密钥Ka或Kb
    void calculate_unchecked_key() {
        //提取key
        Key key1(csi_abs);//将key提前到workthread中，即网络部分的交集，然后引用处理
        key = key1;

//        key.calc_uncheck_key();
//        if (Config::enable_encodedMsg == true) {
        key.calc_uncheck_key2();
//        }

        Config::key = key;
        // Alice 独有
        if (Config::role_index == Config::ROLES::ALICE) {
            Config::alice_uncheck_key = key;
            Config::alice_csi = csi;
        }

        // 存储CSI和未经过校验的Key
//        save_key(sha512_code, Config::tmp_key_file, Config::key_dir, get_global_current_time(), true);
        save_csi(Config::rx_file, Config::csi_dir, get_global_current_time(), false);
//        std::vector<std::complex<float>> csi = Config::alice_csi;
//        save_calculated_csi(csi, Config::csi_dir, get_global_current_time(), true);
        save_calculated_csi(csi, Config::csi_dir, get_global_current_time(), false);
//        Config::key = key;
//        Config::key_num += 512 * 8;
//        Config::result = sha512_code;//alice存储编码后的密钥

    }

    // Bob发送HARQ重传请求
    void send_harq() {
        std::string harq = "1";
        harq += '\n' + '1' + '\n';
        char out2[harq.size()];
        for (int i = 0; i < harq.size(); i++) {
            if (harq[i] == '\0')
                out2[i] = (char) (18);
            else
                out2[i] = harq[i];
        }
        bob_pub_harq.pubmsg.payload = out2;
        bob_pub_harq.pubmsg.payloadlen = (int) strlen(out2);
        bob_pub_harq.pubmsg.qos = 2;
        bob_pub_harq.pubmsg.retained = 0;
//        bob_pub_harq.publish_message(bob_pub_client,"bob_send_harq",&bob_pub_harq.pubmsg);
        bob_pub_harq.publish_message(bob_pub_harq_client, "bob_send_harq", &bob_pub_harq.pubmsg);
        std::cout << "Bob发送HARQ请求........." << std::endl;
    }

    void stop_server_thread() {
        cout << "我要结束所有战斗-------------" << endl;
        //使用状态位退出循环
        Config::workThreadStatus = false;

        //alice
        if (Config::role_index == Config::ROLES::ALICE) {
            if (Config::enable_encodedImg == true) {
//                alice_MqttThread.stop();
//                alice_MqttSubThread.stop();
                alice_pub_imageThread.stop();

//                alice_MqttThread.disconnect(alice_pub_client);
//                alice_MqttSubThread.sub_disconnect(alice_sub_client);
                alice_pub_imageThread.disconnect(alice_encrypted_image_client);
            }

            if (Config::enable_encodedMsg == true) {
                alice_pub_ldpcmsgThread.stop();
                alice_pub_ldpcmsgThread.disconnect(alice_ldpcmsg_client);
                if (Config::enable_harq) {
                    alice_sub_harq.stop();
                    alice_sub_harq.sub_disconnect(alice_sub_harq_client);
                }
                alice_pub_originmsgThread.stop();
                alice_pub_originmsgThread.disconnect(alice_pub_originmsg_client);

                alice_sub_originmsgThead.stop(); // 关闭线程
                alice_sub_originmsgThead.sub_disconnect(alice_sub_originmsg_client);    //线程与客户端断开连接

                alice_pub_zucSuccessSeqThread.stop();
                alice_pub_zucSuccessSeqThread.disconnect(alice_pub_zucSuccessSeq_client);

            }
        } else {//bob or eve

            if (Config::enable_encodedImg == true) {
//                bob_MqttSubThread.stop();
//                bob_MqttThread.stop();
                bob_sub_imageThead.stop();
                if (Config::enable_harq) {
                    bob_pub_harq.stop();
                }


//                bob_MqttSubThread.sub_disconnect(bob_pub_client);
//                bob_MqttThread.disconnect(bob_sub_client);
                bob_sub_imageThead.sub_disconnect(bob_decrypted_image_client);
                if (Config::enable_harq) {
                    bob_pub_harq.disconnect(bob_pub_harq_client);
                }

            }

            if (Config::enable_encodedMsg == true) {
                bob_sub_ldpcmsgThead.stop();
                bob_sub_ldpcmsgThead.sub_disconnect(bob_ldpcmsg_client);

                // bob端中止订阅消息
                bob_sub_originmsgThead.stop();
                bob_sub_originmsgThead.sub_disconnect(bob_sub_originmsg_client);

                // bob端中止发布消息
                bob_pub_originmsgThread.stop();
                bob_pub_originmsgThread.disconnect(bob_pub_originmsg_client);

                // bob端中止订阅消息
                bob_sub_zucSuccessSeqThead.stop();
                bob_sub_zucSuccessSeqThead.sub_disconnect(bob_sub_zucSuccessSeq_client);
            }
        }
    }//销毁客户端，断开连接

    bool started() { return status_start; }

    bool set_started_status(bool status) {
        status_start = status;
    }

    void run() {

        if (Config::role_index == Config::ROLES::ALICE) {
            alice_ports_init();
        } else {//bob or eve
            bob_ports_init();
        }

        startTime = clock();
        if (!started()) {
            status_start = true;
            Config::workThreadStatus = true;
            try {
                std::lock_guard<std::mutex> guard(mu); // 凡是在调用usrp模块相关的时候，都应该加锁，防止stop_transceiver出现数据竞险的情况
                transceiver.start();
                endTime = clock();
                cout
                        << "transceiver.start();运行时间是:-------------------------------------------------------------------------"
                        << (double) (endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
            } catch (QString text) {
                emit alert_msg_signal(QString("usrp connection error"), text);
                return;
            }
        }

        if (Config::role_index == Config::ROLES::ALICE) {

            LOG(INFO) << "I am Alice!";

            while (true) {
                try {
                    if (!Config::workThreadStatus) {
                        std::cout << "当前程序运行状态：" << Config::isRun << std::endl;
                        std::cout << "是否勾选了随机性检测控件:" << Config::enable_randomTest << std::endl;
                        std::cout << "zuc初始密钥协商成功次数:" << Config::successTimes << std::endl;
                        break;
                    }

                    if (Config::save_append == true) {
                        get_global_current_time();
                    } else {
                        get_global_current_time(get_current_time()); // 更新全局的时钟,标注本次交互开始的时间
                    }
                    //usrp发送数据
                    if (!send_frame()) break;


                    generate_key_start_time = clock();

                    compute_csi();
                    emit plot_csi_signalALICE(csi_plot);

                    // 计算未经过调和的密钥Ka
                    calculate_unchecked_key();

                    generate_key_end_time = clock();
                    Config::generate_key_time = (double) (1000.0 * (generate_key_end_time - generate_key_start_time) /
                                                          CLOCKS_PER_SEC); //ms

                    alice_pub_send_origin_msg();

                    alice_sub_recv_origin_msg(); // 订阅bob端发送的明文消息
                    // 如果协商的zuc初始密钥正确,生成了zuc密钥流
                    if (Config::isSuccess) {
                        //已生成密钥数改成zuc初始密钥生成的比特数256bit
                        Config::key_num += 256;
                        Config::successTimes++;

                        /*************************************************/
                        std::cout << "zuc初始密钥协商成功次数:" << Config::successTimes << std::endl;
                        //如果用户zuc协商成功的次数每40次,且zuc密钥流的随机性检测开启了,则执行随机性检测
                        const int times = 40; //test
                        if ((Config::successTimes != 0) && (Config::successTimes) % times == 0 &&
                            Config::enable_randomTest) {
                            std::string zucInitKey_save_dir =
                                    Config::zuc_key_dir + "/" + get_global_current_time() + "/zuc_init_key.txt";
                            std::cout << zucInitKey_save_dir << std::endl;
                            char *passedRandomnessTestCount = "";
                            int sample_size_int = times * 256;
                            std::string sample_size = std::to_string(sample_size_int);
                            int sample_count_int = (Config::successTimes * 256) / sample_size_int;
                            std::string sample_count = std::to_string(sample_count_int);
                            const char *choose_randomness_algorithm = "111111111111101";
                            int total_randomness_algorithm = 0;
                            const char *assess_data_path = "/tmp/assess_data.txt";

                            for (int i = 0; i < strlen(choose_randomness_algorithm); i++) {
                                if (choose_randomness_algorithm[i] == '1')
                                    total_randomness_algorithm++;
                            }
                            const char *args[] = {"./assess", sample_size.c_str(), "0",
                                                  zucInitKey_save_dir.c_str(),
                                                  "0", choose_randomness_algorithm, "0", sample_count.c_str(), "0",
                                                  assess_data_path};
                            RandomTest randomTest;
                            randomTest.process(args);
                            FILE *fpRead = NULL;
                            fpRead = fopen(assess_data_path, "r");
                            if (fpRead == NULL) {
                                printf("文件%s不存在\n", assess_data_path);
                                exit(-1);
                            }


                            Config::randomnessTest_sample_num = sample_size_int;
                            Config::randomnessTest_times = sample_count_int;
//                                        Config::randomnessTest_passedNums = atoi(args[9]);
                            fscanf(fpRead, "%d", &Config::randomnessTest_passedNums);
                            Config::randomnessTest_totalNums = total_randomness_algorithm;
                            fclose(fpRead);
                        }
                        /*************************************************/

                        //是否发送图片信息
                        if (Config::enable_encodedImg == true) {
                            clock_t img_encrypted_start_time = clock();
                            SendEncryptedImageMsg sendEncryptedMsg3;
                            std::string encrypted_key = Config::zuc_generate_key_bin[
                                    Config::zuc_generate_key_bin.size() - 1];
                            sendEncryptedMsg3.client_process(encrypted_key);//alice端绘制图形

                            clock_t img_encrypted_end_time = clock();
                            Config::img_time = img_encrypted_end_time - img_encrypted_start_time;

                            // usleep(2e5);
                            // 图片解密失败的原因：应该修改完加密的图片之后再画图！！！
                            alice_send_imgSecret(Config::tmp_encrypted_image.c_str());

                        }
                        //是否发送文字信息
                        if (Config::enable_encodedMsg == true) {
                            if (Config::enable_encodedImg == true)
                                usleep(10e5);   // 先等图片发送完毕(1s)
                            if (Config::alice_send_parity) {
                                pub_send_parity(Config::redundant_parity);
                            } else {
                                std::string seq = to_string(++Config::msg_seq) + ':';
                                // 从ROI系统界面获取用户输入
//                                pub_send_ldpc_encoded_msg(seq + get_message().toStdString());
                                /******************************************************************************/
                                pub_send_ldpc_encoded_msg(seq + "1234567890abcdefghijklmnopqrstuvwxyz!");
                            }
                        }
                        alice_pub_send_zucSuccess_seq();
                        // 记得将本次zuc密钥协商的会话关闭
//                        Config::isSuccess=false;
                    }
                } catch (AnyCastException e) {
                    emit alert_msg_signal(QString("Network Connection Error(AnyCast): Send Crc Error"),
                                          QString(e.what()));
                    wait();
                } catch (ConnectionException e) {
                    emit alert_msg_signal(QString("Network Connection Error: Send Crc Error"), QString(e.what()));
                    wait();
                }
            }

        } else if (Config::role_index == Config::ROLES::BOB) {

            LOG(INFO) << "I am Bob!";

            if (Config::is_tdd()) {
                while (true) {
                    //UI停止
                    if (!Config::workThreadStatus) {
                        std::cout << "当前程序运行状态：" << Config::isRun << std::endl;
                        std::cout << "是否勾选了随机性检测控件:" << Config::enable_randomTest << std::endl;
                        std::cout << "zuc初始密钥协商成功次数:" << Config::successTimes << std::endl;
                        break;
                    }
                    //todo:延时是为了啥
                    //todo:bob检波之后怎么设tx_file
                    usleep(1e5);

                    if (transceiver.check_file_status()) {
                        generate_key_start_time = clock();
                        compute_csi();
                        emit plot_csi_signalBOB(csi_plot);

                        // 计算未经过调和的密钥Kb
                        calculate_unchecked_key();
                        generate_key_end_time = clock();
                        Config::generate_key_time = (double) (1000.0 *
                                                              (generate_key_end_time - generate_key_start_time) /
                                                              CLOCKS_PER_SEC); //ms


//                        todo:bob在收到有效信号后什么时候生成的密钥？为什么这里检测到信号有效后就直接开始处理订阅到的crc了？
//                        bob_process_crc();


                        if (Config::enable_harq && Config::bob_send_harq) send_harq();

                        bob_sub_recv_origin_msg();

                        bob_pub_send_origin_msg();

                        if (Config::isSuccess) {
                            //已生成密钥数改成zuc初始密钥生成的比特数256bit
                            Config::key_num += 256;
                            Config::successTimes++;

                            /*************************************************/
                            std::cout << "zuc初始密钥协商成功次数:" << Config::successTimes << std::endl;
                            //如果用户zuc协商成功的次数每40次,且zuc密钥流的随机性检测开启了,则执行随机性检测
                            const int times = 40; //test
                            if ((Config::successTimes != 0) && (Config::successTimes) % times == 0 &&
                                Config::enable_randomTest) {
                                std::string zucInitKey_save_dir =
                                        Config::zuc_key_dir + "/" + get_global_current_time() + "/zuc_init_key.txt";
                                std::cout << zucInitKey_save_dir << std::endl;
                                char *passedRandomnessTestCount = "";
                                int sample_size_int = times * 256;
                                std::string sample_size = std::to_string(sample_size_int);
                                int sample_count_int = (Config::successTimes * 256) / sample_size_int;
                                std::string sample_count = std::to_string(sample_count_int);
                                const char *choose_randomness_algorithm = "111111111111101";
                                int total_randomness_algorithm = 0;
                                const char *assess_data_path = "/tmp/assess_data.txt";

                                for (int i = 0; i < strlen(choose_randomness_algorithm); i++) {
                                    if (choose_randomness_algorithm[i] == '1')
                                        total_randomness_algorithm++;
                                }
                                const char *args[] = {"./assess", sample_size.c_str(), "0",
                                                      zucInitKey_save_dir.c_str(),
                                                      "0", choose_randomness_algorithm, "0", sample_count.c_str(),
                                                      "0", assess_data_path};
                                RandomTest randomTest;
                                randomTest.process(args);
                                FILE *fpRead = NULL;
                                fpRead = fopen(assess_data_path, "r");
                                if (fpRead == NULL) {
                                    printf("文件%s不存在\n", assess_data_path);
                                    exit(-1);
                                }

                                Config::randomnessTest_sample_num = sample_size_int;
                                Config::randomnessTest_times = sample_count_int;
                                fscanf(fpRead, "%d", &Config::randomnessTest_passedNums);
                                Config::randomnessTest_totalNums = total_randomness_algorithm;
                                fclose(fpRead);
                                /*************************************************/
                            }
                            //是否发送图片信息
                            if (Config::enable_encodedImg == true) {
                                sub_recv_imgSecret();//要在pub_send_secret()运行之前运行
                            }
                            //是否发送文字信息
                            if (Config::enable_encodedMsg == true) {
                                sub_recv_ldpc_decoded_msg();
                            }
                            bob_sub_recv_zucSuccess_seq();
                        }
//                        Config::isSuccess = false;
                    }
                    transceiver.set_file_status(false);
                }
            } else if (Config::is_fdd()) {
                while (true) {
                    usleep(1e5); // 以100ms的频率来实现slave机一直在收发的状态
                    transceiver.set_file_status(false); // 设置接收文件
                }
            }
        } else if (Config::role_index == Config::ROLES::EVE) {
            while (true) {
                if (!Config::workThreadStatus)
                    break;

                usleep(1e5);
                if (transceiver.check_file_status()) {
//                    bob_process_crc();
                }
                transceiver.set_file_status(false);

//                bob_process_crc();

                //当server_process计算完之后发现正确率大于阈值 -> 能够接收到alice发送过来的加密信息
                if (Config::secret_status) {
                    //是否发送图片信息
                    if (Config::enable_encodedImg == true) {
                        sub_recv_imgSecret();//要在pub_send_secret()运行之前运行
                    }
                    //是否发送文字信息
                    if (Config::enable_encodedMsg == true) {
                        sub_recv_ldpc_decoded_msg();
                        if (Config::enable_harq && Config::bob_send_harq) send_harq();
                        bob_sub_recv_origin_msg();
                    }
                    Config::secret_status = false;
                }
                /*
                if (Config::secret_status){
                    //是否发送图片信息
                    if (Config::enable_encodedImg == true) {
                        sub_recv_imgSecret();//要在pub_send_secret()运行之前运行
                    }
                    //是否发送文字信息
                    if (Config::enable_encodedMsg == true) {
                        sub_recv_ldpc_decoded_msg();
                    }

                    Config::secret_status = false;
                }
                */
            }
        }
    }
};


/******************************************************************************/
//    // Alice发布明文信息
//    void alice_pub_send_origin_msg(std::string Alice_Sender_Msg) {
//        std::string msg_binstr = Alice_Sender_Msg;
//        //alice发布端(明文信息)(2阶段)
//        startTime = clock();
//        char out2[Alice_Sender_Msg.size()];
//        for (int i = 0; i < Alice_Sender_Msg.size(); i++){
////            if (Alice_Sender_Msg[i] == '\0')
////                out2[i] = (char)(18);
////            else
//            out2[i] = Alice_Sender_Msg[i];
//        }
//
//        alice_pub_originmsgThread.pubmsg.payload = out2;
//        alice_pub_originmsgThread.pubmsg.payloadlen = (int)strlen(out2);
//        alice_pub_originmsgThread.pubmsg.qos = 1;
//        alice_pub_originmsgThread.pubmsg.retained = 0;
//        alice_pub_originmsgThread.publish_message(alice_pub_originmsg_client,"alice_send_originmsg",&alice_pub_originmsgThread.pubmsg);
//        std::cout << "Alice发送明文信息........." << std::endl;
////      alice_pub_ldpcmsgThread.send_ldpcmsg(alice_ldpcmsg_client,msg_binstr,false);
//        endTime = clock();
//        cout << "alice888-111-----alice发布端(明文信息)(2阶段)运行时间是:----------------------------------------------" << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
//    };
//
//    // Bob端发布明文信息
//    void bob_pub_send_origin_msg(std::string Bob_Sender_Msg) {
//        std::string msg_binstr = Bob_Sender_Msg;
//        //bob发布端(明文信息)(2阶段)
//        startTime = clock();
//        char out2[Bob_Sender_Msg.size()];
//        for (int i = 0; i < Bob_Sender_Msg.size(); i++){
//            //            if (Alice_Sender_Msg[i] == '\0')
//            //                out2[i] = (char)(18);
//            //            else
//            out2[i] = Bob_Sender_Msg[i];
//        }
//        bob_pub_originmsgThread.pubmsg.payload = out2;
//        bob_pub_originmsgThread.pubmsg.payloadlen = (int)strlen(out2);
//        bob_pub_originmsgThread.pubmsg.qos = 1;
//        bob_pub_originmsgThread.pubmsg.retained = 0;
//
//        bob_pub_originmsgThread.publish_message(bob_pub_originmsg_client,"bob_send_originmsg",&bob_pub_originmsgThread.pubmsg);
//        std::cout << "Bob发送明文信息........." << std::endl;
//        endTime = clock();
//        cout << "bob-----bob发布端(明文信息)(2阶段)运行时间是:----------------------------------------------" << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
//    };


#endif //ROI_WORKTHREAD_H
