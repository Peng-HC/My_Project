//
// Created by ruiy on 18-5-17.
//


#include "Config.h"
#include <UI/widget.h>
#include <Util/Time/time.h>
#include <QtCore/QSettings>
#include <QtCore/QUrl>
#include <fstream>


std::string Config::config_file_path;
// appl config
int Config::role_index;
int Config::mode_index;
std::string Config::image_path;
bool Config::exist_eve;
bool Config::enable_encodedMsg;
bool Config::enable_encodedImg;
int Config::ldpc_rate_index;
bool Config::enable_winner_filter;

std::string Config::tmp_encrypted_image; // 临时加密文件名
std::string Config::tmp_decrypted_image; // 临时解密文件名

// sub config variables
std::string Config::tx_file;
std::string Config::rx_file;
float Config::sine_freq;
int Config::cell_id;
float Config::threshold;//PSS阈值
float Config::threshold_DMRS;//DMRS阈值
float Config::energe;
float Config::energe_DMRS;
int Config::time_slot;
bool Config::alice_or_not;

bool Config::rx_tx_connect;
std::string Config::tmp_key_file;
std::string Config::key_dir;
std::string Config::csi_dir;
std::string Config::tmp_zuc_init_key_file;    // 临时zuc初始密钥存储目录
std::string Config::tmp_zuc_stream_key_file;    // 临时zuc流密钥存储目录
std::string Config::zuc_key_dir; // zuc密钥存储目录(初始密钥+流密钥)
bool Config::save_append;
// USRP X310
std::array<std::string, 5> Config::USRP_IPS = {"192.168.40.1", "192.168.40.2", "192.168.40.3", "192.168.40.4", ""};
// USRP N210
//std::array<std::string, 5> Config::USRP_IPS = {"192.168.10.1", "192.168.10.2", "192.168.10.3", "192.168.10.4", ""};

//std::array<std::string, 3> Config::TX_FILES = {"/home/pln/ROI-V2X-ZUC/UI/Resources/tx_data/Zeros1000_PSBCH_20frames_PSSCH", "/home/ruiy/ROI/ROI-V2X-ZUC/UI/Resources/tx_data/Zeros1000_PSSCH", ""};
//这里需要修改发送的文件 采用相对路径(以bin目录为根路径)
char s_path[MAX_PATH];
std::string source_path=getcwd(s_path,MAX_PATH); //获得当前项目的绝对地址 bin/
std::string alice_tx_path=source_path+"/Zeros1000_PSBCH_20frames_PSSCH";
std::string bob_tx_path=source_path+"/Zeros1000_PSSCH";
std::array<std::string, 3> Config::TX_FILES = {"Zeros1000_PSBCH_20frames_PSSCH","Zeros1000_PSSCH", ""};

std::string Config::tmp_filter_matrix_filepath = "/tmp/111.bin";
std::array<float, 3> Config::RECEIVE_SINGAL_SINE_FREQS = {16, 8, 16};
//新增
std::array<int, 3> Config::RECEIVE_SINGAL_CELL_ID = {301, 301, 301};
std::array<float, 3> Config::RECEIVE_PSS_THRESHOLD = {10, 5, 40};
std::array<float, 3> Config::RECEIVE_DMRS_THRESHOLD = {40,40,40};
std::array<float, 3> Config::RECEIVE_PSS_ENERGE={150,40,250};
std::array<float, 3> Config::RECEIVE_DMRS_ENERGE={15,15,15};
std::array<float, 5> Config::LDPC_RATE= {2/3.0,1/2.0,1/3.0,1/4.0,1/5.0}; // 控件对应的码率
std::array<int,3> Config::PSSCH_ARRIVE_TIME_SLOT={19,18,19};
std::array<bool,3> Config::ALICE_OR_NOT={true,false,false};

std::array<float, 3> Config::RECEIVE_SINGAL_SINE_FREQ_THRESHOLDS = {8, 8, 8};
std::array<bool, 3> Config::RX_TX_CONNECTIONS = {false, true, false};
std::array<std::string, 3> Config::IMAGE_TYPE_TEXTS = {"加密图片", "解密图片", "解密图片"};
std::array<double, 5> Config::LDPC_RATE_TABLE = {0.66667, 0.5, 0.33333, 0.25, 0.2}; //

int Config::usrp_ip_index;
float Config::samp_rate;
double Config::carrier_freq;
double Config::tx_carrier_freq;
double Config::rx_carrier_freq;
float Config::gain;
float Config::sub_gain;
float Config::pub_gain;
int Config::burst_len = 20000;
std::string Config::antenna = "TX/RX";
double Config::band_width = 20e6;

int Config::key_num = 0;

std::string Config::result;

float Config::right_rate_threshold = 0.1;

QVector<QRgb> Config::gray_color_table = get_gray_color_table();

const int Config::restart_count_down = 10;

int Config::latency = 0;
int Config::resend_interval = 1000; // 默认重发间隔为1s

// session
Key Config::key = Key({});

bool Config::workThreadStatus = false;

Key Config::alice_uncheck_key = Key({});
//Key Config::uncheck_key = Key({});  // 未校验的Ka或Kb
//vector<complex<float>> Config::uncheck_csi = {0.0}; // 未校验的CSI
vector<complex<float>> Config::alice_csi = {0.0};
vector<std::string> Config::bob_crc_marks = {""};

// 码字统计，密钥循环缓冲区
int Config::totalcws = 0, Config::correctcws = 0;
int Config::alice_key_buffer_idx = 0;
int Config::bob_key_buffer_idx = 0;
std::string Config::alice_key_buffer, Config::bob_key_buffer;
std::string Config::bob_lastkey, Config::alice_lastkey;

// 重传
bool Config::bob_send_harq = false;
bool Config::alice_send_parity = false;
int Config::msg_seq = 0;
std::string Config::redundant_parity;
std::string Config::bob_last_msg;
int Config::bob_last_msg_len = 0;
bool Config::enable_harq;    // 是否使用harq控件
bool Config::enable_randomTest;    // 是否使用zuc密钥流随机性检测控件

bool Config::secret_status = false;

/* 密钥生成步骤的处理时间 */
// 1.从USRP获得信号到处理生成初始的密钥Ka或kb
double_t Config::generate_key_time = 0; //初始时设置为0
// 2.基于LDPC前向纠错最终协商出送入ZUC的加密密钥ma，并验证了双方密钥的一致性
double_t Config::generate_zuc_init_key_time = 0;
clock_t Config::alice_zuc_stage1=0,Config::alice_zuc_stage2=0;
// 3.基于ZUC加密密钥完成信息的加密或者解密（Alice端显示加密时间，Bob端显示解密时间）
double_t Config::msg_handler_time = 0;
clock_t Config::msg_time=0,Config::img_time=0;

int Config::randomnessTest_sample_num = 0;
int Config::randomnessTest_times = 0;
int Config::randomnessTest_passedNums = 0;
int Config::randomnessTest_totalNums = 0;

// 成功协商出初始zuc密钥的次数
int Config::last_init_zucKey_seq = 0;
int Config::init_zucKey_seq = 0;
// 保存生成协商的zuc初始密钥
std::vector<std::string> Config::init_zuc_key;
// 保存生成的二进制zuc密钥流
std::vector<std::string> Config::zuc_generate_key_bin;
// 初始zuc密钥是否协商成功
bool Config::isSuccess=false;
// zuc密钥协商成功的次数
int Config::successTimes = 0;

// 存储bob端接收到的明文消息
std::string Config::bob_recv_originmsg="";
// 存储alice端接收到的明文消息
std::string Config::alice_recv_originmsg="";

//Alice端的zuc协商成功的序号
std::vector<int> Config::alice_zucSuccessSeq;
//ABob端的zuc协商成功的序号
std::map<int,std::string> Config::bob_zucSuccessSeq;


bool Config::isRun=false;


void Config::set_config(std::string config_file_path_) {
    config_file_path = config_file_path_;

    /**
     * 获取全局的config
     */

    QSettings setting(QString::fromStdString(Config::config_file_path), QSettings::IniFormat); // 选择配置文件，若无则创建一个

    setting.beginGroup("usrp"); // usrp 设备类 QString::fromStdString(usrp_ip)
    //默认是192.168.10.2
    usrp_ip_index = setting.value("usrp_ip_index", 1).toInt();
    samp_rate = setting.value("samp_rate", 30720).toFloat(); // 单位k
    carrier_freq = setting.value("carrier_freq", 5200).toDouble(); // 单位兆
    tx_carrier_freq = setting.value("tx_carrier_freq", 5200).toDouble(); // 单位兆
    rx_carrier_freq = setting.value("rx_carrier_freq", 5200).toDouble(); // 单位兆
    sub_gain = setting.value("sub_gain", 30).toFloat();
    pub_gain = setting.value("pub_gain", 30).toFloat();
    latency = setting.value("latency", 0).toInt(); // 单位ms
    setting.endGroup();

    setting.beginGroup("app"); // 应用本身的一些配置
    role_index = setting.value("role_index", 0).toInt();
    mode_index = setting.value("mode_index", 0).toInt();
    image_path = setting.value("image_path", "").toString().toStdString(); // 默认为空，需要用户手动填入
    exist_eve = setting.value("exist_eve", false).toBool();
    enable_encodedMsg = setting.value("enable_encodedMsg", false).toBool();
    enable_encodedImg = setting.value("enable_encodedImg", false).toBool();
    enable_winner_filter=setting.value("enable_winner_filter",false).toBool();

    enable_harq=setting.value("enable_harq",false).toBool(); // 默认不使用harq控件
    enable_randomTest=setting.value("enable_randomTest",false).toBool(); // 默认不使用zuc密钥流随机性检测控件

    resend_interval = setting.value("resend_interval", 1000).toInt();
    ////////////////////////////////////
    ldpc_rate_index = setting.value("ldpc_rate_index",1).toInt();
    Config::RECEIVE_PSS_ENERGE[role_index]=setting.value("pss_energe", 250).toFloat();
    Config::RECEIVE_PSS_THRESHOLD[role_index]=setting.value("pss_xcorr", 250).toFloat();
    Config::RECEIVE_DMRS_ENERGE[role_index]=setting.value("dmrs_energe", 250).toFloat();
    Config::RECEIVE_DMRS_THRESHOLD[role_index]=setting.value("dmrs_xcorr", 250).toFloat();
    setting.endGroup();

    // 图像数据处理的配置（因为这两个都是临时文件，所以直接写到/tmp文件夹下)）
    tmp_encrypted_image = "/tmp/encryption.bmp";
    tmp_decrypted_image = "/tmp/decryption.bmp";

    tx_file = Config::TX_FILES[role_index];
    rx_file = "/tmp/rx_data";

    // 接收到数据之后是否立刻发送
    if (is_tdd()) {
        rx_tx_connect = RX_TX_CONNECTIONS[role_index];
    } else if (is_fdd()) { // 如果是fdd，则rx和tx之间应该无相互作用
        rx_tx_connect = RX_TX_CONNECTIONS[role_index];
    }

    // 接收到的帧信号中首尾两端正弦波的频率
    sine_freq = Config::RECEIVE_SINGAL_SINE_FREQS[role_index];
    // 粗同步时，用fft判断是否为指定频率正弦波的阈值
    threshold = Config::RECEIVE_SINGAL_SINE_FREQ_THRESHOLDS[role_index];
    cell_id=Config::RECEIVE_SINGAL_CELL_ID[role_index];
    threshold=Config::RECEIVE_PSS_THRESHOLD[role_index];
    threshold_DMRS=Config::RECEIVE_DMRS_THRESHOLD[role_index];
    energe=Config::RECEIVE_PSS_ENERGE[role_index];
    energe_DMRS=Config::RECEIVE_DMRS_ENERGE[role_index];
    time_slot=Config::PSSCH_ARRIVE_TIME_SLOT[role_index];
    alice_or_not=Config::ALICE_OR_NOT[role_index];


    // 密钥保存
    tmp_key_file = "/tmp/key.txt"; // 临时保存密钥所使用，最终会转存到其他用户指定目录
    tmp_zuc_init_key_file = "/tmp/zuc_init_key.txt";   // 临时保存zuc初始密钥所使用，最终会转存到其他用户指定目录
    tmp_zuc_stream_key_file = "/tmp/zuc_key.txt";   // 临时保存zuc密钥流所使用，最终会转存到其他用户指定目录

    setting.beginGroup("key"); // usrp 设备类 QString::fromStdString(usrp_ip)
    csi_dir = setting.value("csi_dir", "").toString().toStdString(); // 存储csi的目录，子目录是按时间进行分类
    key_dir = setting.value("key_dir", "").toString().toStdString(); // 存储key的目录，子目录是按时间进行分类
    zuc_key_dir = setting.value("zuc_init_key_dir", "").toString().toStdString(); // 存储zuc初始key的目录，子目录是按时间进行分类
    save_append = setting.value("save_append", true).toBool(); // 是否连续存储，默认为是
    setting.endGroup();
}

QVector<QRgb> get_gray_color_table () {
    QVector<QRgb> gray_color_table;
    unsigned int rgb = 0;
    for(int i = 0; i < 256; i++) {
        gray_color_table.append(rgb);
        rgb += 0x00010101;
    }
    return gray_color_table;
}

Widget* get_global_widget() {
    static Widget* w = new Widget;
    return w;
}

QString get_message () {
    Widget* w = get_global_widget();
    return w->get_message();
}

void set_message (const QString& msg) {
    Widget* w = get_global_widget();
    w->set_message(msg);
}

std::string get_global_current_time(std::string timestr) {
    static std::string current_time = get_current_time();
    if (!timestr.empty()) current_time = timestr;
    return current_time;
}

std::string read_key (const std::string& key_path) {
    std::ifstream is(key_path, std::ios::in | std::ios::binary);
    char buf[512];
    std::string key;
    while (is.read(buf, sizeof(buf)).gcount() > 0) {
        key.append(buf, is.gcount());
    }
    is.close();
    return key;
}
