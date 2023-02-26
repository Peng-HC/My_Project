#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <TopBlock/Transceiver.h>
#include <Thread/WorkThread.h>
#include <Lib/qcustomplot/qcustomplot.h>
#include <QImage>
#include <QtGui/QLabel>
#include <Util/Math/Convert/Convert.h>
#include <QtGui/QImage>
#include <QtGui/QLineEdit>
#include <QtGui/QComboBox>
#include <QtGui/QCheckBox>
#include <Util/Image/toolLet.h>
#include <QtGui/QMessageBox>
#include <QPushButton>
#include <algorithm>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT
    QWidget* ptr_widget_image;
    QLabel* ptr_label_image;
    QLabel* ptr_label_image_type;
    std::shared_ptr<WorkThread> work_thread;
    QCustomPlot *q_custom_plot_widget;

    // 计时器 所需要变量
    QTimer* msTimer;
    int hourTemp;           //Hour
    int minuteTemp;         //Minute
    int secondTemp;         //Second
    int countTemp;
    QLabel* ptr_hour_line_edit;
    QLabel* ptr_minute_line_edit;
    QLabel* ptr_second_line_edit;

    // 生成密钥数
    QLabel* ptr_key_num_label;

    // 各阶段时间
    QLabel* ptr_generate_key_time_label;
    QLabel* ptr_generate_zuc_init_key_time_label;
    QLabel* ptr_msg_handler_time_label;

    QLabel* ptr_generate_key_time1_label;
    QLabel* ptr_msg_handler_time1_label;

    // zuc初始密钥协商结果
    QLabel* ptr_key_agreement_result_label;

    // 随机性检测结果
    QLabel* ptr_randomnessTest_sample_num_label;
    QLabel* ptr_randomnessTest_times_label;
    QLabel* ptr_randomnessTest_passedNums_label;


    // 控件
    QComboBox* ptr_role_combo_box;
    QComboBox* ptr_mode_combo_box;
    QLineEdit* ptr_image_path_line_edit;
    QLineEdit* ptr_message_line_edit;
    QCheckBox* ptr_exist_eve_check_box;
    QCheckBox* ptr_enable_encodedMsg_check_box;
    QCheckBox* ptr_enable_encodedImg_check_box;
    QCheckBox* ptr_enable_winner_filter_box;
    QCheckBox* ptr_enable_harq_check_box;   //harq控件

    QCheckBox* ptr_enable_randomTest_check_box; //zuc密钥流随机性检测控件

    QSlider* ptr_resend_interval_slider;


    QComboBox* ptr_usrp_ip_combo_box;
    QComboBox* ptr_ldpc_rate_combo_box;   //码率控件
    QSlider* ptr_sub_gain_slider;
    QSlider* ptr_pub_gain_slider;

    QSlider* ptr_pss_energe_slider;
    QSlider* ptr_pss_xcorr_slider;
    QSlider* ptr_dmrs_energe_slider;
    QSlider* ptr_dmrs_xcorr_slider;

    QSlider* ptr_samp_rate_slider;
    QSlider* ptr_carrier_freq_slider;
    QSlider* ptr_tx_carrier_freq_slider;
    QSlider* ptr_rx_carrier_freq_slider;
    QSlider* ptr_latency_slider;

    QLineEdit* ptr_store_csi_dir_line_edit;
    QLineEdit* ptr_store_key_dir_line_edit;
    QLineEdit* ptr_store_zuc_init_key_dir_line_edit; // ZUC初始密钥存储目录
    QCheckBox* ptr_save_append_check_box;

    QPushButton* ptr_run_push_button;
    QLabel* ptr_status_text_label;
    QLabel* ptr_status_icon_label;

    QPixmap run_status_pixmap;
    QPixmap stop_status_pixmap;

    QMessageBox *alert_model;
    std::atomic<int> restart_count_down;

signals:
    void send_secret_signal(const std::string &file_path);
public:
    explicit Widget(QWidget *parent = 0);
    void renderBySetting();
    void set_run_status_ui();
    void set_stop_status_ui();
    QString get_message();
    void set_message(const QString &msg);
    ~Widget();

private slots:
    void on_run_push_button_clicked();
    void on_stop_push_button_clicked();
    void create_work_thread();
    void start_work_thread();
    void updateConfig();
    void restart();
    void on_open_image_push_button_clicked();
    void plot_csi(const std::vector<std::complex<float>> &csi) {
        std::cout<<"-------------ploting csi------------time="<<get_global_current_time()<<std::endl;

        //设置样式，拖拽、缩小、是否可选中
        QVector<double> vecX, vecY;//横坐标值、纵坐标值
        for (int i=0;i<csi.size();i++) {
            vecX << i;
            vecY << std::abs(csi[i]);
        }

        q_custom_plot_widget->graph(0)->setData(vecX, vecY);//在第一个图层中添加数据点

        //重新调节轴、调用后坐标会根据实际情况增加
        q_custom_plot_widget->rescaleAxes();

        //重新生成图表
        q_custom_plot_widget->replot();
    }

    void plot_crypted_image () {
        QImage image(Config::tmp_encrypted_image.c_str());
        image.scaled(ptr_widget_image->size(), Qt::KeepAspectRatio);
        ptr_label_image->setPixmap(QPixmap::fromImage(image));
    }

    void plot_decrypted_image () {
        QImage image(Config::tmp_decrypted_image.c_str());
        image.scaled(ptr_widget_image->size(), Qt::KeepAspectRatio);
        ptr_label_image->setPixmap(QPixmap::fromImage(image));
    }

    void on_store_csi_dir_push_button_clicked();

    void on_store_key_dir_push_button_clicked();

    void on_store_zuc_init_key_dir_push_button_clicked();

    // 计时器相关函数
    void startTimer();
    void TimeSlot ();

    void role_or_mode_combo_box_currentIndexChanged(int role_index, int mode_index);
    void on_mode_combo_box_currentIndexChanged(int index);
    void on_role_combo_box_currentIndexChanged(int index);

    void alert_msg(const QString& title, const QString& text) {
        stop_work_thread();
        if (!alert_model->isVisible()) {
            restart_count_down = Config::restart_count_down;
            alert_model->setWindowTitle(title);
            alert_model->setText(text);
            alert_model->setButtonText(QMessageBox::Ok, "立即重启(" + QString::number(restart_count_down) + ")");

            if (alert_model->exec() == QMessageBox::Ok) { // 立即重启按钮
                restart();
            }
        }
    }
    void enableRunBtn (bool b) {
        ptr_run_push_button->setDisabled(b); // 禁用或者启用运行按钮控件
    }

    void stop_work_thread() {
        set_stop_status_ui();
        if (work_thread != nullptr && work_thread->started()) {
            std::cout << "stop work thread" << std::endl;
            work_thread->stop_server_thread();
            work_thread->stop_transceiver();
            work_thread->terminate();
            work_thread->wait();
            work_thread->set_started_status(false);
        }
        std::cout << "work thread has been stopped" << std::endl;
        work_thread = nullptr;
        // stop glog
//        google::ShutdownGoogleLogging();
    }

private:
    Ui::Widget *ui;
    void init_csi_graph();
};

#endif // WIDGET_H
