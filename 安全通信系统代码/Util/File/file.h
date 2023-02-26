//
// Created by calt on 18-7-21.
//

#ifndef ROI_FILE_H
#define ROI_FILE_H

#include <string>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <iostream>
#include <Util/File/file.h>
#include <zconf.h>
#include <dirent.h>

void copyfile(std::string src, std::string dst, bool append = false);

bool is_file_exist(const std::string &file_path);

bool is_dir_exist(const std::string &dir_path);

void save_key(std::string key, std::string key_path, std::string key_dir, std::string current_time, bool append);
void save_csi(std::string csi_path, std::string csi_dir, std::string current_time, bool append);
void save_calculated_csi (std::vector<std::complex<float>> calculated_csi, std::string calculated_csi_dir, std::string current_time, bool append);
// 存储zuc初始密钥
void save_zucInitKey(std::string key, std::string key_path, std::string key_dir, std::string current_time, bool isZucInitKey,bool append);

#endif //ROI_FILE_H
