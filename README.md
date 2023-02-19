# C++联网对战五子棋

---

## 一、项目部署

部署环境：`Linux16.04`	`Qt 5.14.0` 	`xmake 2.7.6`

1. 开放本机端口`10032`

   > （1）查看防火墙状态，查看开放的端口
   >
   > ```bash
   > sudo ufw status
   > ```
   >
   > （2）开放端口
   >
   > ```bash
   > # sudo ufw allow 端口号
   > sudo ufw allow 10032
   > ```

2. 使用`wget`安装`xmake`，并编译

   ```C++
   bash <(wget https://xmake.io/shget.text -O -)
   ```

   ```bash
   cd Gobang\GobangServer
   # 执行xmake
   xmake
   ```

3. 启动`GobangServer`服务端

   ```bash
   Gobang\GobangServer$ ./gobang_server
   ```

4. 用Qt打开`Gobang\Gobang\Gobang.pro`，并构建、运行
