//
// Created by calt on 18-7-21.
//

#include <cstring>
#include <complex>
#include <vector>
#include "file.h"

bool is_file_exist(const std::string &file_path) {
    if (file_path.empty()) return false;
    if (access(file_path.c_str(), F_OK)) return 0;
    return false;
}

bool is_dir_exist(const std::string &dir_path) {
    if (dir_path.empty()) return false;
    DIR *dp;
    if ((dp = opendir(dir_path.c_str())) == NULL) return false;
    closedir(dp);
    return true;
}


void copyfile(std::string src, std::string dst, bool append)
{
    using namespace std;
    std::ifstream in(src,ios::binary);
    ofstream out;
    if (append == false) {
        out.open(dst,ios::binary);
    } else {
        out.open(dst,ios::binary | ios::app);
    }
    if (!in.is_open()) {
        cout << "error open file " << src << endl;
        exit(EXIT_FAILURE);
    }
    if (!out.is_open()) {
        cout << "error open file " << dst << endl;
        exit(EXIT_FAILURE);
    }
    if (src == dst) {
        cout << "the src file can't be same with dst file" << endl;
        exit(EXIT_FAILURE);
    }
    char buf[2048];
    long long totalBytes = 0;
    while(in)
    {
        //read从in流中读取2048字节，放入buf数组中，同时文件指针向后移动2048字节
        //若不足2048字节遇到文件结尾，则以实际提取字节读取。
        in.read(buf, 2048);
        //gcount()用来提取读取的字节数，write将buf中的内容写入out流。
        out.write(buf, in.gcount());
        totalBytes += in.gcount();
    }
    in.close();
    out.close();
}

// 存储由原始的接收到的csi数据再计算之后的使用csi
void save_calculated_csi (std::vector<std::complex<float>> calculated_csi, std::string calculated_csi_dir, std::string current_time, bool append) {

    std::string save_dir = calculated_csi_dir + "/" + current_time;
    std::string save_path = save_dir + "/" + "calculated_csi.bin";

    if (is_dir_exist(save_dir) == false) {
        int is_create = mkdir(save_dir.c_str(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRWXG | S_IRWXO);
        if( !is_create )
            std::cout << "calculated csi create path: " << save_dir << std::endl;
        else
            std::cout << "calculated csi create path: " << save_dir << " failed! error code: " << is_create << std::endl;
    }


    // 写入计算过后的csi
    FILE *file_id;
    if (append) {
        file_id = fopen(save_path.c_str(), "ab");
    } else {
        file_id = fopen(save_path.c_str(), "wb");
    }

    for (auto elem: calculated_csi) {
        float real = elem.real();
        float imag = elem.imag();
        fwrite(&real, sizeof(float), 1, file_id);
        fwrite(&imag, sizeof(float), 1, file_id);
    }
    fclose(file_id);
}

//默认就是在后面添加密钥
void save_key(std::string key, std::string key_path, std::string key_dir, std::string current_time, bool append) {
    std::ofstream out(key_path, std::ios::out | std::ios::binary);
    out.write(key.c_str(), key.size());
    out.close();

    std::string key_save_dir = key_dir + "/" + current_time;
    std::string key_save_path = key_save_dir + "/" + "key.txt";

    if (is_dir_exist(key_save_dir) == false) {
        int is_create = mkdir(key_save_dir.c_str(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRWXG | S_IRWXO);
        if( !is_create )
            std::cout << "key create path: " << key_save_dir << std::endl;
        else
            std::cout << "key create path: " << key_save_dir << " failed! error code: " << is_create << std::endl;
    }

    copyfile(key_path, key_save_path, append);
}

void save_csi(std::string csi_path, std::string csi_dir, std::string current_time, bool append) {
        std::string csi_save_dir = csi_dir + "/" + current_time;
        std::string csi_save_path = csi_save_dir + "/" + "csi";

        std::cout << is_dir_exist(csi_save_dir) << std::endl;
        if (is_dir_exist(csi_save_dir) == false) {
            int is_create = mkdir(csi_save_dir.c_str(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRWXG | S_IRWXO);
            if( !is_create )
                std::cout << "csi create path: " << csi_save_dir << std::endl;
            else
                std::cout << "csi creat path: " << csi_save_dir << " failed! error code: " << is_create << std::endl;
        }

        copyfile(csi_path, csi_save_path, append);
}

// 存储zuc初始密钥
//默认就是在后面添加密钥
void save_zucInitKey(std::string key, std::string key_path, std::string key_dir, std::string current_time, bool isZucInitKey,bool append) {
    //换行操作
    key+="\n";
    // key_path:为临时存储zuc密钥的路径
    std::ofstream out(key_path, std::ios::out | std::ios::binary);
    out.write(key.c_str(), key.size());
    out.close();

    std::string key_save_dir = key_dir + "/" + current_time;
    std::string key_save_path;
    if(isZucInitKey) {
        key_save_path = key_save_dir + "/" + "zuc_init_key.txt";
    } else {
        key_save_path = key_save_dir + "/" + "zuc_key.txt";
    }
    if (is_dir_exist(key_save_dir) == false) {
        int is_create = mkdir(key_save_dir.c_str(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRWXG | S_IRWXO);
        if(!is_create ) {
            if(isZucInitKey) {
                std::cout << "zuc init key create path: " << key_save_dir << std::endl;
            } else {
                std::cout << "zuc key create path: " << key_save_dir << std::endl;
            }
        }
        else {
            if(isZucInitKey) {
                std::cout << "zuc init key create path: " << key_save_dir;
            } else {
                std::cout << "zuc key create path: " << key_save_dir;
            }
            std::cout << " failed! error code: " << is_create << std::endl;
        }
    }
    copyfile(key_path, key_save_path, append);
}


