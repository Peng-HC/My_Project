//
// Created by ruiy on 21-1-24.
//

#ifndef ROI_SENDENCRYPTEDIMAGEMSG_H
#define ROI_SENDENCRYPTEDIMAGEMSG_H


#include <Config/Config.h>
#include <QtCore>

class SendEncryptedImageMsg: public QObject {
    Q_OBJECT
signals:
    void plot_decrypted_image_signal();
    void plot_crypted_image_signal();

public:

    SendEncryptedImageMsg():
    send_file(""),
    recv_file("")
    {
        connect(this, SIGNAL(plot_decrypted_image_signal()), (const QObject*)get_global_widget(), SLOT(plot_decrypted_image()));
        connect(this, SIGNAL(plot_crypted_image_signal()),                                                                        (const QObject*)get_global_widget(), SLOT(plot_crypted_image()));
    }

public:

    std::string send_file;
    std::string recv_file;
    std::string size;
    std::string content;

    void fill_file(std::string send_file_, std::string recv_file_);
    std::string file_do_to_string (bool print);
    void print_mem();
    std::vector<std::string> separated(const char* in);
    void assign(const char* in);

    void server_process(const char* in);
    void client_process(std::string &key_code);

    void EncryptionImage(const char *in_fname, const char *out_fname, std::string &pwd);

    void DecryptionImage(const char *in_fname, const char *out_fname, std::string &pwd);
};


#endif //ROI_SENDENCRYPTEDIMAGEMSG_H
