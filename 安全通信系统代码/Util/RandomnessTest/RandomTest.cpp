//
// Created by phc on 23-1-1.
//

#include "RandomTest.h"
//#include "../../Config/Config.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <pthread.h>
#include <sys/wait.h>
#define MAX_PATH 1000

//int main() {
//    std::string zucInitKey_save_dir = "/home/pln/ROI-V2X-ZUC/Util/RandomnessTest/gbt-1.0/data/data.e";
//    std::cout<<zucInitKey_save_dir<<std::endl;
//    const char *args[]={"./assess","256","0",
//                        zucInitKey_save_dir.c_str(),
//                        "1","0","1","0"};
////                        const char *arg0="./assess";
////                        const char *arg1="1000";
////                        const char *arg2="0";
////                        const char *arg3="/home/pln/ROI-V2X-ZUC/Util/RandomnessTest/gbt-1.0/data/data.e";
////                        const char *arg4="1";
////                        const char *arg5="0";
////                        const char *arg6="1";
////                        const char *arg7="0";
//    RandomTest randomTest;
//    randomTest.process(args);
//}

//主程序
void RandomTest::process(const char * args[]) {
    int status;
//    pid_t wpid;
    char s_path[MAX_PATH];
    std::string source_path=getcwd(s_path, MAX_PATH); //获得当前项目的绝对地址 bin/
    std::cout<<"source_path:"<<source_path<<std::endl;
    const std::string exe_file_path=source_path+"/../Util/RandomnessTest/gbt-1.0/assess";
//            "/home/pln/ROI-V2X-ZUC/Util/RandomnessTest/gbt-1.0/assess";
//    const char *arg[3]={"./assess","1000",0};
//    const char *arg0="./assess";
//    const char *arg1="1000";
//    const char *arg2="0";
//    const char *arg3="/home/pln/ROI-V2X-ZUC/Util/RandomnessTest/gbt-1.0/data/data.e";
//    const char *arg4="1";
//    const char *arg5="0";
//    const char *arg6="1";
//    const char *arg7="0";
    int iret = 0;
    /***************************************************************/
    int fd[2];//文件句柄，用于父子进程通信
    pipe(fd);//管道，0代表读，1代表写

    /***************************************************************/
    //创建子进程,否则执行完execl()函数后，整个程序会全部结束
    pid_t pid = fork();
    if(pid<0) {
        perror("子进程创建失败!!!");
    }
    else if(pid>0) {
        // 父进程等待子进程正常中止
//        while ((wpid = wait(&status)) > 0);
//        sleep(3);
        wait(&status);
        std::cout<<"我是父进程"<<std::endl;
//        std::cout<<"iret="<<iret<<std::endl;
//        std::cout<<"args[9]="<<args[9]<<std::endl;
        std::cout << "Hello, World!" << std::endl;
    } else if (pid==0) {
        std::cout<<"子进程创建成功"<<std::endl;
        close(fd[0]);//关闭读通道
//        std::cout<<"args[1]="<<args[1]<<std::endl;
//        std::cout<<"args[6]="<<args[6]<<std::endl;
        iret = execl(exe_file_path.c_str(),args[0],args[1],args[2],args[3],args[4],args[5],args[6],args[7],args[8],args[9],NULL);
//        std::cout<<"args[9]="<<args[9]<<std::endl;
//        std::cout<<"iret="<<iret<<std::endl;
        std::cout<<"-----------------------0----------------------------------"<<std::endl;
        exit(1);
    }
}

// txt文件转成2进制文件
void RandomTest::txt2bin(const char *in_fname, const char *out_fname) {
    std::cout<<"txt文件地址:"<<in_fname<<std::endl;
    std::cout<<"bin文件地址:"<<out_fname<<std::endl;
    FILE *in_file,*out_file;
    char ch;   // register修饰符暗示编译程序相应的变量将被频繁地使用，如果可能的话，应将其保存在CPU的寄存器中
    in_file=fopen(in_fname,"rb");//以读的方式打开二进制文件
    if(in_file==NULL)//如果打开失败
    {
        std::cout<<"打开文件 "<<in_fname<<" 失败"<<std::endl;
        exit(1);
    }
    out_file=fopen(out_fname,"wb");//以写的方式打开二进制文件
    if(out_file==NULL)//如果创建失败
    {
        std::cout<<"创建文件 "<<out_file<<" 失败"<<std::endl;
        exit(1);
    }
    while(!feof(in_file))
    {
        ch=fgetc(in_file);
        fputc(ch,out_file);
    }
    fclose(in_file);
    fclose(out_file);
}

void RandomTest::read_binFile(const char *in_fname) {
    FILE *in_file;
    char ch;   // register修饰符暗示编译程序相应的变量将被频繁地使用，如果可能的话，应将其保存在CPU的寄存器中
    in_file=fopen(in_fname,"rb");//以读的方式打开二进制文件
    if(in_file==NULL)//如果打开失败
    {
        std::cout<<"打开文件 "<<in_fname<<" 失败"<<std::endl;
        exit(1);
    }

    while(!feof(in_file))
    {
        ch=fgetc(in_file);
        std::cout<<ch<<" ";
    }
    fclose(in_file);
}

