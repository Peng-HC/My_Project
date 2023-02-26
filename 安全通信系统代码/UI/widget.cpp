#include "widget.h"
#include "ui_widget.h"
#include <QMessageBox>
#include <Network/Command/CrcCheckCmd.h>
#include <QSlider>
#include <QtGui/QSpinBox>
#include <QSettings>
#include <QtGui/QFileDialog>
#include <QLineEdit>
#include <QtGui/QCheckBox>
#include <Util/File/file.h>
#include <Util/Network/networklet.h>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    init_csi_graph();

    {
        QWidget* ptr_widget = this->findChild<QWidget*>("widget_csi");
        QGridLayout* ptr_layout = new QGridLayout();
        ptr_widget->setLayout(ptr_layout);
        ptr_layout->addWidget(q_custom_plot_widget);
    }

    {
        QWidget* ptr_burst_len_widget = this->findChild<QWidget*>("burst_len_widget");
        ptr_burst_len_widget->hide();
    }

    {
        QWidget* band_width_len_widget = this->findChild<QWidget*>("band_width_widget");
        band_width_len_widget->hide();
    }

    {
        QWidget* ptr_antenna_widget = this->findChild<QWidget*>("antenna_widget");
        ptr_antenna_widget->hide();
    }

    ptr_widget_image = this->findChild<QWidget*>("widget_image");
    ptr_label_image = this->findChild<QLabel*>("label_image");
    ptr_label_image->setScaledContents(true);
    ptr_label_image_type = this->findChild<QLabel*>("label_image_type");

    ptr_hour_line_edit = this->findChild<QLabel*>("hour_line_edit");
    ptr_minute_line_edit = this->findChild<QLabel*>("minute_line_edit");
    ptr_second_line_edit = this->findChild<QLabel*>("second_line_edit");
    ptr_key_num_label = this->findChild<QLabel*>("key_num_label");

    ptr_generate_key_time_label = this->findChild<QLabel*>("generate_key_time_label");
    ptr_generate_zuc_init_key_time_label = this->findChild<QLabel*>("generate_zuc_init_key_time_label");
    ptr_msg_handler_time_label = this->findChild<QLabel*>("msg_handler_time_label");

    ptr_generate_key_time1_label = this->findChild<QLabel*>("generate_key_time1_label");
    ptr_msg_handler_time1_label = this->findChild<QLabel*>("msg_handler_time1_label");

    ptr_key_agreement_result_label = this->findChild<QLabel*>("key_agreement_result_label");

    ptr_randomnessTest_sample_num_label = this->findChild<QLabel*>("randomnessTest_sample_num_label");
    ptr_randomnessTest_times_label = this->findChild<QLabel*>("randomnessTest_times_label");
    ptr_randomnessTest_passedNums_label = this->findChild<QLabel*>("randomnessTest_passedNums_label");



    alert_model = new QMessageBox(QMessageBox::Critical, "Title", "Text",
                          QMessageBox::Ok | QMessageBox::Default | QMessageBox::Cancel | QMessageBox::Escape ,0 );
    alert_model->setButtonText(QMessageBox::Ok, "确定");
    alert_model->setButtonText(QMessageBox::Cancel, "取消");

    //start(t)之後，每t毫秒后(即timeout)都會呼叫TimeSlot()
    msTimer = new QTimer(this);
    connect(msTimer,SIGNAL(timeout()),this, SLOT(TimeSlot()));


    /*
     * connect(ui->pushButton, &QPushButton::clicked, this, &Widget::start);
     * 第一个参数：
     * ui是为窗体构建界面的辅助类对象指针，我们在窗体设计界面拖的空间对象都存在这个ui指向的对象里
     * ui->pushButton就指向我们之前拖的按钮对象
     * 因为通过设计模式拖的空间全部是以指针类型访问的，所以以后说到窗体里的空间，一般都是说它的指针名字
     * 第二个参数：
     * 用SIGNAL宏包裹，里面是按钮对象的信号clicked()，信号的声明和成员函数类似，但必须放在signals声明段落
     * 上面没有看到signals声明段落，是因为QPushButton类的对象自带这个信号，不需要自定义
     * 第三个参数：
     * 接收对象的指针，也就服务提供方，是槽函数所在对象的指针，this即主窗体自己
     * 第四个参数是接收对象里的槽函数，并用SLOT宏封装起来
     *
     * connect 函数意义非常清晰，将源头和源头的信号，关联到接收端和接收端的槽函数
     * 注意源头和接收端必须是存在的实体对象指针，不能是野指针
     * connect函数必须放在ui->setupUi之后，否则控件指针是未定义的野指针，那种关联必然失败，会导致程序崩溃
     */

}


void Widget::renderBySetting() {

     /**
     * 获取全局的config
     */

    // 获取 app 部分，用于设置界面
    ptr_role_combo_box = this->findChild<QComboBox*>("role_combo_box"); //设置 角色
    ptr_role_combo_box->setCurrentIndex(Config::role_index);
    ptr_mode_combo_box = this->findChild<QComboBox*>("mode_combo_box"); // 设置 模式
    ptr_mode_combo_box->setCurrentIndex(Config::mode_index);
    ptr_image_path_line_edit = this->findChild<QLineEdit*>("image_path_line_edit"); // 设置所需要加密图片的路径
    ptr_image_path_line_edit->setText(QString::fromStdString(Config::image_path));
    ptr_exist_eve_check_box = this->findChild<QCheckBox*>("exist_eve_check_box"); // 设置是否存在eve
    ptr_exist_eve_check_box->setCheckState(Config::exist_eve? Qt::CheckState::Checked: Qt::CheckState::Unchecked);
    ptr_enable_encodedMsg_check_box = this->findChild<QCheckBox*>("enable_encodedMsg_check_box"); // 设置是否发送文字加密信息
    ptr_enable_encodedMsg_check_box->setCheckState(Config::enable_encodedMsg ? Qt::CheckState::Checked: Qt::CheckState::Unchecked);
    ptr_enable_encodedImg_check_box = this->findChild<QCheckBox*>("enable_encodedImg_check_box"); // 设置是否发送图片加密信息
    ptr_enable_encodedImg_check_box->setCheckState(Config::enable_encodedImg ? Qt::CheckState::Checked: Qt::CheckState::Unchecked);
    ptr_enable_winner_filter_box = this->findChild<QCheckBox*>("enable_winner_filter_box"); // 设置是否使用维纳滤波器
    ptr_enable_winner_filter_box->setCheckState(Config::Config::enable_winner_filter ? Qt::CheckState::Checked: Qt::CheckState::Unchecked);
    ptr_enable_harq_check_box = this->findChild<QCheckBox*>("enable_harq_check_box"); // 设置是否使用harq
    ptr_enable_harq_check_box->setCheckState(Config::enable_harq ? Qt::CheckState::Checked: Qt::CheckState::Unchecked);

    ptr_enable_randomTest_check_box = this->findChild<QCheckBox*>("enable_randomTest_check_box"); // 设置是否进行zuc密钥随机性检测
    ptr_enable_randomTest_check_box->setCheckState(Config::enable_randomTest ? Qt::CheckState::Checked: Qt::CheckState::Unchecked);

    ptr_resend_interval_slider = this->findChild<QSlider*>("resend_interval_slider"); // 设置alice重发间隔
    ptr_resend_interval_slider->setValue(Config::resend_interval); // ms
    ptr_ldpc_rate_combo_box = this->findChild<QComboBox*>("ldpc_rate_combo_box"); // 设置LDPC编码码率
    ptr_ldpc_rate_combo_box->setCurrentIndex(Config::ldpc_rate_index);

    ptr_pss_energe_slider= this->findChild<QSlider*>("pss_energe_slider"); //设置pss能量检测阈值
    ptr_pss_energe_slider->setValue(Config::RECEIVE_PSS_ENERGE[Config::role_index]);
    ptr_pss_xcorr_slider= this->findChild<QSlider*>("pss_xcorr_slider"); //设置pss相关阈值
    ptr_pss_xcorr_slider->setValue(Config::RECEIVE_PSS_THRESHOLD[Config::role_index]);
    ptr_dmrs_energe_slider= this->findChild<QSlider*>("dmrs_energe_slider"); //设置dmrs能量检测阈值
    ptr_dmrs_energe_slider->setValue(Config::RECEIVE_DMRS_ENERGE[Config::role_index]);
    ptr_dmrs_xcorr_slider= this->findChild<QSlider*>("dmrs_xcorr_slider"); //设置dmrs相关阈值
    ptr_dmrs_xcorr_slider->setValue(Config::RECEIVE_DMRS_THRESHOLD[Config::role_index]);


    // 获取 usrp 部分设置，用于设置界面
    ptr_usrp_ip_combo_box = this->findChild<QComboBox*>("usrp_ip_combo_box"); // 设置usrp的ip
    ptr_usrp_ip_combo_box->setCurrentIndex(Config::usrp_ip_index);

    //设置收发增益
    ptr_pub_gain_slider = this->findChild<QSlider*>("pub_gain_slider"); // 设置发射增益
    ptr_pub_gain_slider->setValue(Config::pub_gain);
    ptr_sub_gain_slider = this->findChild<QSlider*>("sub_gain_slider"); // 设置接收增益
    ptr_sub_gain_slider->setValue(Config::sub_gain);

    ptr_samp_rate_slider = this->findChild<QSlider*>("samp_rate_slider"); // 设置采样率
    ptr_samp_rate_slider->setValue(Config::samp_rate); // MHz
    ptr_carrier_freq_slider = this->findChild<QSlider*>("carrier_freq_slider"); // 设置载波
    ptr_carrier_freq_slider->setValue(Config::carrier_freq); // MHz
    ptr_tx_carrier_freq_slider = this->findChild<QSlider*>("tx_carrier_freq_slider"); // 设置发射载波
    ptr_tx_carrier_freq_slider->setValue(Config::tx_carrier_freq); // MHz
    ptr_rx_carrier_freq_slider = this->findChild<QSlider*>("rx_carrier_freq_slider"); // 设置接收载波
    ptr_rx_carrier_freq_slider->setValue(Config::rx_carrier_freq); // MHz
    ptr_latency_slider = this->findChild<QSlider*>("latency_slider"); // 设置收发延迟
    ptr_latency_slider->setValue(Config::latency); // ms


    // 密钥保存
    ptr_store_csi_dir_line_edit = this->findChild<QLineEdit*>("store_csi_dir_line_edit"); // 设置存储csi的目录
    ptr_store_csi_dir_line_edit->setText(QString::fromStdString(Config::csi_dir));
    ptr_store_key_dir_line_edit = this->findChild<QLineEdit*>("store_key_dir_line_edit"); // 设置存储key的目录
    ptr_store_key_dir_line_edit->setText(QString::fromStdString(Config::key_dir));

    ptr_store_zuc_init_key_dir_line_edit = this->findChild<QLineEdit*>("store_zuc_init_key_dir_line_edit"); // 设置存储zuc初始密钥的目录
    ptr_store_zuc_init_key_dir_line_edit->setText(QString::fromStdString(Config::zuc_key_dir));


    ptr_save_append_check_box = this->findChild<QCheckBox*>("save_append_check_box"); // 设置是否需要连续存储
    ptr_save_append_check_box->setCheckState(Config::save_append? Qt::CheckState::Checked: Qt::CheckState::Unchecked);

    // 获取运行按钮控件
    ptr_run_push_button = this->findChild<QPushButton*>("run_push_button");

    // 获取状态窗口
    ptr_status_text_label = this->findChild<QLabel*>("status_text_label");
    ptr_status_icon_label = this->findChild<QLabel*>("status_icon_label");
    ptr_message_line_edit = this->findChild<QLineEdit*>("message_line_edit");

    run_status_pixmap;
    run_status_pixmap.load(":/Resources/images/working.svg");

    stop_status_pixmap;
    stop_status_pixmap.load(":/Resources/images/unavailable.svg");

    ptr_label_image_type->setText(Config::IMAGE_TYPE_TEXTS[Config::role_index].c_str());

    on_role_combo_box_currentIndexChanged(Config::role_index);
}

/**
 * 1.此处处理界面写入的参数,并将其保存在可执行文件目录下作为配置文件roi.ini
 * 2.再更新可能改动的参数跑程序
 */
void Widget::updateConfig() {

    /**
    * app 配置
    */
    int role_index = ptr_role_combo_box->currentIndex(); // 设置角色
    int mode_index = ptr_mode_combo_box->currentIndex(); // 设置模式

    // 设置所需要加密图片的路径
    std::string image_path = (ptr_image_path_line_edit->text()).toStdString();
    std::cout << image_path << std::endl;
    if (role_index == Config::ROLES::ALICE && image_path.empty()) { // 若Alice未选择加密图片路径，则报错
        alert_msg("Input Error", "Please Choose Image Path");
        return;
    }

    // 设置是否存在eve
    bool exist_eve = ptr_exist_eve_check_box->isChecked();
    int resend_interval = ptr_resend_interval_slider->value(); // 获取alice重发的时间间隔

    // 设置是否发送文字加密信息
    bool enable_encodedMsg = ptr_enable_encodedMsg_check_box->isChecked();
    // 设置是否发送图片加密信息
    bool enable_encodedImg = ptr_enable_encodedImg_check_box->isChecked();
    // 设置是否使用维纳滤波器
    bool enable_winner_filter = ptr_enable_winner_filter_box->isChecked();
    // 设置是否使用harq
    bool enable_harq = ptr_enable_harq_check_box->isChecked();
    // 设置是否使用zuc密钥随机性检测
    bool enable_randomTest = ptr_enable_randomTest_check_box->isChecked();


    //阈值配置
    float pss_energe=ptr_pss_energe_slider->value();//获取pss_energe
    float pss_xcorr=ptr_pss_xcorr_slider->value();//获取pss_xcorr
    float dmrs_energe=ptr_dmrs_energe_slider->value();//获取dmrs_energe
    float dmrs_xcorr=ptr_dmrs_xcorr_slider->value();//获取dmrs_xcorr
    /**
     * usrp 配置
     */

    float pub_gain = ptr_pub_gain_slider->value(); // 获取增益
    float sub_gain = ptr_sub_gain_slider->value(); // 获取增益

    float samp_rate = ptr_samp_rate_slider->value(); // 获取采样率 MHz

    float carrier_freq = ptr_carrier_freq_slider->value(); // 获取载波 MHz
    float tx_carrier_freq = ptr_tx_carrier_freq_slider->value(); // 获取发射载波 MHz
    float rx_carrier_freq = ptr_rx_carrier_freq_slider->value(); // 获取接收载波 MHz

    int usrp_ip_index = ptr_usrp_ip_combo_box->currentIndex(); // 获取usrp的ip
    int ldpc_rate_index = ptr_ldpc_rate_combo_box->currentIndex(); // 获取新的码率
    int latency = ptr_latency_slider->value(); // 获取usrp的延迟


    /**
     * Key 存储配置
     */
    bool save_append = ptr_save_append_check_box->isChecked();// 获取是否需要连续存储

    std::string csi_dir = ptr_store_csi_dir_line_edit->text().toStdString(); // 获取存储csi的目录
    if (csi_dir.empty()) { // 若未选择图片路径，则报错
        alert_msg("Input Error", "Please Choose CSI Store Path");
        return;
    }

    if (!is_dir_exist(csi_dir)) {
        alert_msg("Input Error", "CSI store directory does not exist");
        return;
    }

    std::string key_dir = ptr_store_key_dir_line_edit->text().toStdString(); // 获取存储key的目录
    if (key_dir.empty()) { // 若未选择图片路径，则报错
        alert_msg("Input Error", "Please Choose Key Store Path");
        return;
    }

    if (!is_dir_exist(key_dir)) {
        alert_msg("Input Error", "Key store directory does not exist");
        return;
    }

    std::string zuc_init_key_dir = ptr_store_zuc_init_key_dir_line_edit->text().toStdString(); // 获取存储zuc初始key的目录
    if (zuc_init_key_dir.empty()) { // 若未选择zuc_init_key存储路径，则报错
        alert_msg("Input Error", "Please Choose ZUC Init Key Store Path");
        return;
    }

    if (!is_dir_exist(zuc_init_key_dir)) {
        alert_msg("Input Error", "ZUC Init Key store directory does not exist");
        return;
    }

    //若用户勾选了随机性检测，但没有勾选连续存储，则报错提醒！
    if(enable_randomTest&&!save_append) {
        alert_msg("Input Error", "随机性检测要求勾选密钥连续存储控件!");
        return;
    }


    // 更新setting和global variables
    QSettings setting(QString::fromStdString(Config::config_file_path), QSettings::IniFormat); // 选择配置文件，若无则创建一个

    setting.beginGroup("usrp"); // usrp 设备类 QString::fromStdString(usrp_ip)
    setting.setValue("usrp_ip_index", usrp_ip_index);
    setting.setValue("samp_rate", samp_rate);
    setting.setValue("carrier_freq", carrier_freq);
    setting.setValue("tx_carrier_freq", tx_carrier_freq);
    setting.setValue("rx_carrier_freq", rx_carrier_freq);

    setting.setValue("pub_gain", pub_gain);
    setting.setValue("sub_gain", sub_gain);

    setting.setValue("latency", latency);
    setting.endGroup();

    setting.beginGroup("app"); // 应用本身的一些配置
    setting.setValue("role_index", role_index);
    setting.setValue("mode_index", mode_index);
    setting.setValue("image_path", QString::fromStdString(image_path));
    setting.setValue("exist_eve", exist_eve);
    setting.setValue("enable_encodedMsg", enable_encodedMsg);
    setting.setValue("enable_encodedImg", enable_encodedImg);
    setting.setValue("enable_winner_filter", enable_winner_filter);
    setting.setValue("enable_harq", enable_harq);
    setting.setValue("enable_randomTest", enable_randomTest);
    setting.setValue("resend_interval", resend_interval);
    setting.setValue("ldpc_rate_index", ldpc_rate_index);
    setting.setValue("pss_energe",pss_energe);
    setting.setValue("pss_xcorr",pss_xcorr);
    setting.setValue("dmrs_energe",dmrs_energe);
    setting.setValue("dmrs_xcorr",dmrs_xcorr);
    setting.endGroup();

    setting.beginGroup("key"); // 密钥存储的一些配置
    setting.setValue("csi_dir", QString::fromStdString(csi_dir));
    setting.setValue("key_dir", QString::fromStdString(key_dir));
    setting.setValue("zuc_init_key_dir", QString::fromStdString(zuc_init_key_dir)); // 更新zuc初始密钥存储目录，这样每次进入ROI系统就不需要重新设定存储路径了
    setting.setValue("save_append", save_append);
    setting.endGroup();


    //这里需要更新下Config各变量的值，因为Transceiver函数中用的值是Config
    Config::set_config(Config::config_file_path);
}

Widget::~Widget()
{
    delete ui;
    delete msTimer;
}
/* void on_<object name>_<signal name>(<signal parameters>);
 * 该种形式的方法会被uic和moc工具处理，自动将pushButton的clicked函数关联到该函数上
 * 非这种形式的需要手动关联，见构造函数中connect函数的注释
 */
void Widget::on_run_push_button_clicked()
{
    Config::isRun=true;
    updateConfig();
    startTimer();//计时器，用于UI显示计时

    // 停止工作线程
    stop_work_thread();
    create_work_thread(); // 创建work_thread所指涉的对象
    start_work_thread();
}

void Widget::on_stop_push_button_clicked()
{
    Config::isRun=false;
    stop_work_thread();
}

void Widget::create_work_thread() {
    if (work_thread != nullptr) {
        stop_work_thread();
    }
    work_thread = make_shared<WorkThread>();
    connect(work_thread.get(), SIGNAL(plot_csi_signalALICE(const std::vector<std::complex<float>> &)), this, SLOT(plot_csi(const std::vector<std::complex<float>> &)));
    connect(work_thread.get(), SIGNAL(plot_csi_signalBOB(const std::vector<std::complex<float>> &)), this, SLOT(plot_csi(const std::vector<std::complex<float>> &)));
    connect(work_thread.get(), SIGNAL(alert_msg_signal(const QString&, const QString&)), this, SLOT(alert_msg(const QString&, const QString&)));
}

void Widget::start_work_thread() {
    try {
        work_thread->start();
    } catch (std::exception e) {
        stop_work_thread(); // 销毁
        alert_msg("exception", e.what());
        return;
    } catch (std::runtime_error e) {
        stop_work_thread(); // 销毁
        alert_msg("runtime error", e.what());
        return;
    }
    set_run_status_ui();
}

void Widget::restart() {
    stop_work_thread(); // 停止之前工作的线程
    create_work_thread(); // 创建新的工作线程
    start_work_thread(); // 启动新的工作线程
}

void Widget::set_run_status_ui() {
    ptr_status_text_label->setText("运行中");
    ptr_status_icon_label->setPixmap(run_status_pixmap);
}

void Widget::set_stop_status_ui() {
    ptr_status_text_label->setText("未运行");
    ptr_status_icon_label->setPixmap(stop_status_pixmap);
}

void Widget::startTimer() {
    countTemp=0;
    secondTemp=0;
    minuteTemp=0;
    hourTemp=0;
    Config::key_num = 0;
    msTimer->start(10);
}

void Widget::init_csi_graph() {
    q_custom_plot_widget = new QCustomPlot;
    q_custom_plot_widget->addGraph(); //添加曲线，每加入一条曲线需调用一次
    q_custom_plot_widget->setInteractions(QCP::iRangeDrag/*拖拽*/ | QCP::iRangeZoom/*缩小*/ | QCP::iSelectAxes |QCP::iSelectLegend | QCP::iSelectPlottables);
    q_custom_plot_widget->axisRect()->setupFullAxesBox(false);//设置四周全轴框

    //设置颜色、线宽
    QPen graphPen;
    graphPen.setColor(QColor(rand()%245+10, rand()%245+10, rand()%245+10));//QColor::yellow()
    graphPen.setWidthF(rand()/(double)RAND_MAX*2+1);
    q_custom_plot_widget->graph(0)->setPen(graphPen);

    //设置标题, 必须先调用insertRow插入一行才能够显示
    q_custom_plot_widget->plotLayout()->insertRow(0);
    QCPTextElement *title = new QCPTextElement(q_custom_plot_widget, "channel state infomation",   QFont("sans", 17, QFont::Bold));
    q_custom_plot_widget->plotLayout()->addElement(0, 0, title);

    //显示线介绍，默认右上角
    q_custom_plot_widget->legend->setVisible(true);
    QFont legendFont = font();
    legendFont.setPointSize(10);
    q_custom_plot_widget->legend->setFont(legendFont);
    q_custom_plot_widget->legend->setSelectedFont(legendFont);
    q_custom_plot_widget->legend->setSelectableParts(QCPLegend::spItems); // legend box shall not be selectable, only legend items

}
void Widget::on_open_image_push_button_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Open Image"), "/home", tr("Image Files(*bmp)")); // 目前只允许 bmp 文件的加解密
    QLineEdit* ptr_line_edit = this->findChild<QLineEdit*>("image_path_line_edit");
    if (path.length() == 0)
    {
        ptr_line_edit->setPlaceholderText(tr("No Any Image"));
    }
    else
    {
        ptr_line_edit->setText(path);
    }

}

void Widget::on_store_csi_dir_push_button_clicked()
{

    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "/home", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (dir.length() == 0)
    {
        ptr_store_csi_dir_line_edit->setPlaceholderText("No CSI Store Dir");
    }
    else
    {
        ptr_store_csi_dir_line_edit->setText(dir);
    }
}

void Widget::on_store_key_dir_push_button_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "/home", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (dir.length() == 0)
    {
        ptr_store_key_dir_line_edit->setPlaceholderText("No Key Store Dir");
    }
    else
    {
        ptr_store_key_dir_line_edit->setText(dir);
    }
}

// 点击存储zuc初始密钥的button按钮
void Widget::on_store_zuc_init_key_dir_push_button_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "/home", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (dir.length() == 0)
    {
        ptr_store_zuc_init_key_dir_line_edit->setPlaceholderText("No ZUC Init Key Store Dir");
    }
    else
    {
        ptr_store_zuc_init_key_dir_line_edit->setText(dir);
    }
}

void Widget::TimeSlot () {//每10ms触发一次

    countTemp+=1;
    if(countTemp==100)//10*100 = 1000ms = 1s
    {
        countTemp=0;
        secondTemp+=1;
        if(secondTemp==60)
        {
            secondTemp=0;
            minuteTemp+=1;
            if(minuteTemp==60)
            {
                minuteTemp=0;
                hourTemp+=1;
            }
        }

        QString hourstr = QString("%1").arg(hourTemp, std::max(to_string(hourTemp).size() + 0.0, 2.0), 10, QChar('0'));
        QString minutestr = QString("%1").arg(minuteTemp, 2, 10, QChar('0'));
        QString secondstr = QString("%1").arg(secondTemp, 2, 10, QChar('0'));

        ptr_hour_line_edit->setText(hourstr + ":");
        ptr_minute_line_edit->setText(minutestr + ":");
        ptr_second_line_edit->setText(secondstr);

        ptr_key_num_label->setText(QString::number(Config::key_num) + " bits");

        ptr_generate_key_time_label->setText(QString::number(Config::generate_key_time,'f',2) + " ms");
        ptr_generate_zuc_init_key_time_label->setText(QString::number(Config::generate_zuc_init_key_time,'f',2) + " ms");
        ptr_msg_handler_time_label->setText(QString::number(Config::msg_handler_time,'f',2) + " ms");

        ptr_key_agreement_result_label->setText("第" + QString::number(Config::init_zucKey_seq) + "次生成zuc初始密钥"+(Config::isSuccess?"成功":"失败"));

        ptr_randomnessTest_sample_num_label->setText(QString::number(Config::randomnessTest_sample_num) + " bits");
        ptr_randomnessTest_times_label->setText(QString::number(Config::randomnessTest_times) + " 次");
        ptr_randomnessTest_passedNums_label->setText(QString::number(Config::randomnessTest_passedNums) + "/" + QString::number(Config::randomnessTest_totalNums));

        if (alert_model->isVisible()) { // 若窗口打开
            alert_model->setButtonText(QMessageBox::Ok, "立即重启(" + QString::number(--restart_count_down) + ")");
            if (restart_count_down == 0) { // 倒计时结束，重启工作线程
                alert_model->button(QMessageBox::Ok)->click();
            }
        }
    }
}

void Widget::role_or_mode_combo_box_currentIndexChanged(int role_index, int mode_index) {
    QWidget* ptr_open_image_parent_widget = this->findChild<QWidget*>("open_image_parent_widget"); // 所需要加密图片的路径控件
    QWidget* ptr_crcImage_and_enable_encodedMsg_parent_widget_parent_widget = this->findChild<QWidget*>("crcImage_and_enable_encodedMsg_parent_widget"); // 设置是否 发送图片信息 and 发送文字加密信息 的控件

    QWidget* ptr_latency_parent_widget = this->findChild<QWidget*>("latency_parent_widget"); // 设置是否存在延迟的控件
    QWidget* ptr_resend_interval_parent_widget = this->findChild<QWidget*>("resend_interval_parent_widget"); // 设置是否存在延迟的控件
    QWidget* ptr_mode_parent_widget = this->findChild<QWidget*>("mode_parent_widget"); // 设置模式的控件
    QWidget* ptr_carrier_freq_parent_widget = this->findChild<QWidget*>("carrier_freq_parent_widget"); // 设置是否存在载波的控件
    QWidget* ptr_tx_carrier_freq_parent_widget = this->findChild<QWidget*>("tx_carrier_freq_parent_widget"); // 设置是否存在tx载波的控件
    QWidget* ptr_rx_carrier_freq_parent_widget = this->findChild<QWidget*>("rx_carrier_freq_parent_widget"); // 设置是否存在rx载波的控件

    QWidget* ptr_save_append_parent_widget = this->findChild<QWidget*>("save_append_and_exist_eve_parent_widget"); // 设置是否 连续存储 and eve存在 的控件
    QLineEdit* ptr_message_line_edit = this->findChild<QLineEdit*>("message_line_edit"); // 设置消息控件的格式

    if (role_index == Config::ROLES::ALICE) {

        ptr_open_image_parent_widget->show();
        ptr_crcImage_and_enable_encodedMsg_parent_widget_parent_widget->show();
        ptr_latency_parent_widget->hide();
        ptr_resend_interval_parent_widget->show();
        ptr_mode_parent_widget->show();
        ptr_save_append_parent_widget->show();
        ptr_message_line_edit->setPlaceholderText("请输入待发送消息");
        ptr_message_line_edit->setDisabled(false);

        if (mode_index == Config::MODES::TDD) {
            ptr_carrier_freq_parent_widget->show();
            ptr_tx_carrier_freq_parent_widget->hide();
            ptr_rx_carrier_freq_parent_widget->hide();
        } else if (mode_index == Config::MODES::FDD) {
            ptr_carrier_freq_parent_widget->hide();
            ptr_tx_carrier_freq_parent_widget->show();
            ptr_rx_carrier_freq_parent_widget->show();
        }

        ptr_generate_key_time1_label->setText("生成加密密钥时间");
        ptr_msg_handler_time1_label->setText("图片文字加密时间");


    } else if (role_index == Config::ROLES::BOB) {

        ptr_open_image_parent_widget->hide();
        ptr_crcImage_and_enable_encodedMsg_parent_widget_parent_widget->show();
        ptr_latency_parent_widget->show();
        ptr_resend_interval_parent_widget->hide();
        ptr_mode_parent_widget->show();
        ptr_save_append_parent_widget->hide();
        ptr_message_line_edit->setPlaceholderText("收到的消息");

        if (mode_index == Config::MODES::TDD) {
            ptr_carrier_freq_parent_widget->show();
            ptr_tx_carrier_freq_parent_widget->hide();
            ptr_rx_carrier_freq_parent_widget->hide();
        } else if (mode_index == Config::MODES::FDD) {
            ptr_carrier_freq_parent_widget->hide();
            ptr_tx_carrier_freq_parent_widget->show();
            ptr_rx_carrier_freq_parent_widget->show();
        }

        ptr_generate_key_time1_label->setText("生成解密密钥时间");
        ptr_msg_handler_time1_label->setText("图片文字解密时间");

    } else if (role_index == Config::ROLES::EVE) {

        ptr_open_image_parent_widget->hide();
        ptr_crcImage_and_enable_encodedMsg_parent_widget_parent_widget->hide();
        ptr_latency_parent_widget->hide();
        ptr_resend_interval_parent_widget->hide();
        ptr_mode_parent_widget->hide();

        ptr_carrier_freq_parent_widget->show();
        ptr_tx_carrier_freq_parent_widget->hide();
        ptr_rx_carrier_freq_parent_widget->hide();
        ptr_save_append_parent_widget->hide();
        ptr_message_line_edit->setPlaceholderText("收到的消息");

        ptr_generate_key_time1_label->setText("生成解密密钥时间");
        ptr_msg_handler_time1_label->setText("图片文字解密时间");

    }
}

// 当模式改变时，选择性显示控件
void Widget::on_mode_combo_box_currentIndexChanged(int index) {
    role_or_mode_combo_box_currentIndexChanged(Config::role_index, index);
}

// 当 角色改变时，选择性显示空间
void Widget::on_role_combo_box_currentIndexChanged(int index)
{
    role_or_mode_combo_box_currentIndexChanged(index, Config::mode_index);
}


QString Widget::get_message() {
    return ptr_message_line_edit->text();
}

void Widget::set_message(const QString &msg) {
    ptr_message_line_edit->setText(msg);
}