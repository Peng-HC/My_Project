//
// Created by pc on 2022/4/11.
//

#include "header.h"
#include <Eigen/Dense>
#include <Eigen/LU>
#include <Eigen/QR>
#include <Eigen/SVD>
vector<float> winner_filter(int m, vector<float>x,vector<float>y){//m=338
    if(x.size()!=y.size()){
        perror("x.size()!=y.size()\n");
        return {};
    }
    vector<float> r=xcorr_delay(x,x);
    vector<float> p=xcorr_delay(y,x);
    int n=x.size();

    Eigen::Matrix<float,Eigen::Dynamic,Eigen::Dynamic> Rxx(m,m);
    for(int i=0;i<m;++i){
        Rxx(i,0)=r[1121+i]/float(n);
        Rxx(0,i)=r[1121+i]/float(n);
    }
    /***生成toeplitz矩阵***/
    for(int i=1;i<m;++i){
        for(int j=1;j<m;++j){
            Rxx(i,j)=Rxx(i-1,j-1);
        }
    }

    Eigen::Matrix<float,Eigen::Dynamic,1> Rxs(m);
    for(int i=0;i<m;++i){
        Rxs(i,0)=p[1121+i]/float(n);
    }

    cout<<Rxx.size()<<endl;
    Eigen::Matrix<float,1,Eigen::Dynamic> H(m);
    H= Rxx.llt().solve(Rxs);

//    cout<<"--------------"<<endl;
//    for(int i=0;i<H.size();++i){
//        cout<<H[i]<<endl;
//    }

    vector<float> ans;
    ans.resize(m);
    for(int i=0;i<m;++i){
        ans[i]=H(0,i);
    }
    return ans;

}
