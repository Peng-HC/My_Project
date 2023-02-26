//#define BOOST_ASIO_ENABLE_HANDLER_TRACKING
#include <UI/widget.h>
#include <Util/Thread/Thread.h>
#include <glog/logging.h>


namespace po = boost::program_options;

std::mutex mu;
semaphore sema_resend_receive_to_compute(0);
semaphore sema_compute_to_network(0);


/**
 * config variables
 */


int main(int argc, char *argv[])
{
    //先注册自己的MsgHandleIr
    //qInstallMsgHandler(customMessageHandler);
    //自定义类型
    qRegisterMetaType<std::vector<std::complex<float>>> ("std::vector<std::complex<float>>");
    qRegisterMetaType<std::vector<std::string>> ("std::vector<std::string>");
    qRegisterMetaType<std::string> ("std::string");

    // 准备需要发射的文件
    QFile file;
    // 这里的资源文件统一放到UI文件夹下，否则会出错，这个需要之后再研究下，现在没有时间看了
    file.copy(":/Resources/tx_data/tx_data1", QString::fromStdString(Config::TX_FILES[Config::ROLES::ALICE]));
    file.copy(":/Resources/tx_data/tx_data2", QString::fromStdString(Config::TX_FILES[Config::ROLES::BOB]));
    file.copy(":/Resources/filter/111.bin", QString::fromStdString(Config::tmp_filter_matrix_filepath));

    /**
     * 日志
     */
    //初始化
    google::InitGoogleLogging(argv[0]);

    // create logpath
    std::string str_des;
    str_des.append("mkdir -p ");
    str_des.append("log");
    system(str_des.c_str());
    // >= INFO
    std::string str_info;
    str_info.append("./log/");//   ..../bin/log/

//    FLAGS_log_dir = str_info;
    google::SetLogDestination(google::INFO, str_info.c_str());

    //也打印到终端
    FLAGS_alsologtostderr = 1;
    FLAGS_colorlogtostderr = true;  //log为彩色

    LOG(INFO) << "I am INFO111111111111!";

    /**
     * Qt Gui
     */
    QApplication a(argc, argv);
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    //1.程序第一次运行时初始化各控件的值
    //2.程序以后运行时读取已有的配置文件中的值
    Config::set_config("roi.ini"); // 设置global变量
    Widget *w = get_global_widget();

    w->renderBySetting();
    w->show();


    qDebug() << "main thread Id: " << a.thread()->currentThreadId();

    int ret = a.exec();

    return ret;
}
