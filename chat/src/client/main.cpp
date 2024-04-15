#include <iostream>
#include <string>
#include "json.hpp"
#include <thread>
#include <chrono>
#include <ctime>
#include <functional>
#include <semaphore.h>

// socket网络编程
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <atomic>

// 我自己写的库
#include "user.hpp"
#include "group.hpp"
#include "public.hpp"
#include "socketconn.hpp"
#include "menufunction.hpp"

using namespace std;
using json = nlohmann ::json;

// main作为发送线程，子线程用作接收线程
int main(int argc, char **argv)
{
    if (argc < 3)
    {
        // 向标准错误流 (cerr) 输出一条错误消息,它的输出会立即显示在控制台上
        cerr << "input error! should: ./Client 127.0.0.1 8000" << endl;
        exit(-1);
    }
    // 解析命令行穿来的ip+port
    char *ip = argv[1];
    uint16_t port = atoi(argv[2]);

    // 建立socket连接,用封装好的函数
    int clientfd = socketconn(ip, port);

    // 初始化读写线程通信用的信号量
    sem_init(&rwsem, 0, 0);

    // 连接服务器成功，启动接收子线程
    std::thread readTask(readTaskHandler, clientfd);
    // 创建pthread
    readTask.detach(); // 线程分离

    // main线程用于接受用户输入，负责发送数据
    while (1)
    {
        // 设计登录，注册，退出界面
        cout << "--------------" << endl;
        cout << "---welcome!---" << endl;
        cout << "1.login" << endl;
        cout << "2.register" << endl;
        cout << "3.quit" << endl;
        cout << "--------------" << endl;
        // 养成cin后读取缓冲区的回车习惯
        cout << "choice: ";
        int choice = 0;
        cin >> choice;
        cin.get();

        switch (choice)
        {
            // 1.login业务
        case 1:
        {
            if (login(clientfd, rwsem))
            {
                // 进入聊天主界面
                isMainMenRunning = true;
                Menu(clientfd, isMainMenRunning);
            }
        }
        break;
        case 2:
            Register(clientfd,rwsem);
            break;
        case 3:
            quit(clientfd, rwsem);
            break;
        default:
            cerr << "invalid input!" << endl;
            break;
        }
    }
    return 0;
}




