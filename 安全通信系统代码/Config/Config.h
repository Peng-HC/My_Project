//
// Created by ruiy on 18-5-17.
//

#ifndef ROI_CONFIG_H
#define ROI_CONFIG_H

#include <string>
#include <boost/program_options.hpp>
#include <QtCore/QString>
#include <QtGui/QRgb>
#include <QtCore/QVector>
#include <Key/Key.h>

#define MAX_PATH 1000
class Widget;

namespace po = boost::program_options;

Widget* get_global_widget();
std::string get_global_current_time(std::string timestr="");
QVector<QRgb> get_gray_color_table ();
QString get_message ();
std::string read_key(const std::string& key_path);

void set_message (const QString& msg);

class Config {

public:
    // 配置文件路径
    static std::string config_file_path;

    // 已生成密钥数目

    // 应用程序的配置
    static int role_index;
    static int mode_index;
    static std::string peer_ip;
    static std::string image_path;
    static std::string eve_ip;
    static bool exist_eve;
    static int port;
    static int resend_interval; // 重发间隔
    static bool enable_encodedMsg; // 是否发送文字加密信息
    static bool enable_encodedImg; // 是否发送图片加密信息
    static bool enable_winner_filter;//是否使用维纳滤波器
    static int ldpc_rate_index;
    static std::array<float, 5> LDPC_RATE; // 控件对应的码率

    // usrp 配置
    static int usrp_ip_index;
    static float samp_rate;
    static double carrier_freq;
    static double tx_carrier_freq;
    static double rx_carrier_freq;
    static float gain;
    static float  sub_gain;
    static float pub_gain;

    static int burst_len;
    static std::string antenna;
    static double band_width;

    // 图片存储配置(写死在临时文件夹)
    static std::string tmp_encrypted_image; // 临时加密文件名
    static std::string tmp_decrypted_image; // 临时解密文件名

    static std::string tx_file; // 此文件应当由程序生成。但由于时间仓促，现在直接以文件形式提供
    static std::string rx_file; // 此文件为程序接收到的导频数据

    static float sine_freq;
    static int cell_id;
    static float threshold;//主分支
    static float threshold_DMRS;
    static float energe;
    static float energe_DMRS;
    static int time_slot;
    static bool alice_or_not;
    static bool rx_tx_connect;
    static std::string tmp_key_file;

    // 密钥存储
    static std::string key_dir;
    static std::string csi_dir;
    static std::string tmp_zuc_init_key_file;    // 临时zuc初始密钥存储目录
    static std::string tmp_zuc_stream_key_file;    // 临时zuc流密钥存储目录
    static std::string zuc_key_dir; // zuc密钥存储目录(初始密钥+流密钥)
    static bool save_append;

    // 密钥数
    static int key_num;
    static float right_rate_threshold;

    // 收发延迟
    static int latency;

    //bob端生成的校验结果
    static std::string result;

    //是否要发送此次调和(crc)之后的加密信息:true为发送,false为不发送,即使用上次的秘钥信息
    static bool secret_status;

    static void set_config(std::string config_file_path_);

    enum ROLES {ALICE, BOB, EVE};
    enum MODES {TDD, FDD};

    static std::array<std::string, 5> USRP_IPS;
    static std::array<std::string, 3> TX_FILES;
    static std::array<float, 3> RECEIVE_SINGAL_SINE_FREQS;
    //V2X新增
    static std::array<int,3> RECEIVE_SINGAL_CELL_ID;
    static std::array<float, 3> RECEIVE_PSS_THRESHOLD;
    static std::array<float, 3> RECEIVE_DMRS_THRESHOLD;
    static std::array<float, 3> RECEIVE_PSS_ENERGE;
    static std::array<float, 3> RECEIVE_DMRS_ENERGE;
    static std::array<int,3> PSSCH_ARRIVE_TIME_SLOT;
    static std::array<bool,3> ALICE_OR_NOT;

    static std::array<float, 3> RECEIVE_SINGAL_SINE_FREQ_THRESHOLDS;
    static std::array<bool, 3> RX_TX_CONNECTIONS;
    static std::array<std::string, 3> IMAGE_TYPE_TEXTS; // 加解密图像上方显示的文字
    static std::array<double,5> LDPC_RATE_TABLE;    // ldpc selected code rate table

    static QVector<QRgb> gray_color_table;

    static std::string tmp_filter_matrix_filepath; // 计算csi时滤波器的系数矩阵从该文件中读取

    const static int restart_count_down;

    static bool is_alice() {return role_index == ROLES::ALICE;}
    static bool is_bob() {return role_index == ROLES::BOB;}
    static bool is_eve() {return role_index == ROLES::EVE;}
    static bool is_role(int role) {return role_index == role;}
    static bool not_role(int role) {return role_index != role;}

    static bool is_tdd () {return mode_index == MODES::TDD;}
    static bool is_fdd () {return mode_index == MODES::FDD;}
    static bool is_mode (int mode) {return mode_index == mode;}
    static bool not_mode (int mode) {return mode_index != mode;}

    // session
    static Key key;
    static bool workThreadStatus;

    static Key alice_uncheck_key;
//    static Key uncheck_key; // 未校验的Ka或Kb
//    static std::vector<std::complex<float>> uncheck_csi; // 未校验的CSI
    static std::vector<std::complex<float>> alice_csi;
    static std::vector<std::string> bob_crc_marks;

    /* new codes for ldpc */
    // 记录码字是否正确
    static int correctcws;
    static int totalcws;

    // 循环密钥缓冲区
    static std::string alice_key_buffer, bob_key_buffer;
    static int alice_key_buffer_idx, bob_key_buffer_idx;

    // 保存待发送结果,以备重传
    static int alice_msg_len;
    static std::string alice_msg_stored;
    static std::string alice_lastkey, bob_lastkey;
    static bool alice_send_parity, bob_send_harq;
    static int msg_seq;
    static std::string redundant_parity;
    static std::string bob_last_msg;
    static int bob_last_msg_len;
    static bool enable_harq;    // 是否使用harq控件
    static bool enable_randomTest;    // 是否使用zuc密钥流随机性检测控件

    /* 密钥生成步骤的处理时间 */
    // 1.从USRP获得信号到处理生成初始的密钥Ka或kb
    static double_t generate_key_time;
    // 2.基于LDPC前向纠错最终协商出送入ZUC的加密密钥ma，并验证了双方密钥的一致性
    static double_t generate_zuc_init_key_time;
    /*alice有两个阶段：一是发布ma和验证消息，二是对订阅的反馈信息进行处理阶段*/
    static clock_t alice_zuc_stage1,alice_zuc_stage2;
    // 3.基于ZUC加密密钥完成信息的加密或者解密(Alice端显示加密时间，Bob端显示解密时间)
    static double_t msg_handler_time;
    static clock_t msg_time,img_time;

    /*随机性检测结果*/
    static int randomnessTest_sample_num;
    static int randomnessTest_times;
    static int randomnessTest_passedNums;
    static int randomnessTest_totalNums;



    // 成功协商出初始zuc密钥的次数
    static int last_init_zucKey_seq;
    static int init_zucKey_seq;
    // 保存生成协商的zuc初始密钥
    static std::vector<std::string> init_zuc_key;
    // 保存生成的2进制zuc密钥流
    static std::vector<std::string> zuc_generate_key_bin;
    // 初始zuc密钥是否协商成功
    static bool isSuccess;
    // zuc密钥协商成功的次数
    static int successTimes;

    // 存储bob端接收到的明文消息
    static std::string bob_recv_originmsg;
    // 存储alice端接收到的明文消息
    static std::string alice_recv_originmsg;

    static bool isRun;

    //Alice端的zuc协商成功的序号
    static std::vector<int> alice_zucSuccessSeq;
    //ABob端的zuc协商成功的序号
    static std::map<int,std::string> bob_zucSuccessSeq;
};


#endif //ROI_CONFIG_H
