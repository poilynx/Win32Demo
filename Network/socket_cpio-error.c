#define _WIN32_WINNT 0x0501
#include <winsock2.h>
//#include <windows.h>
#include <stdio.h>
#define PORT 5150
#define DATA_BUFSIZE 8192
#define ioRead 0
#define ioWrite 1
typedef struct
{
    OVERLAPPED overlapped;
    WSABUF sendBuffer;
    WSABUF recvBuffer;
    DWORD dwOperationType;
    WSABUF buf;
    char  buffer[DATA_BUFSIZE];
    DWORD dwBytesSend;
    DWORD dwBytesRecv;
    LPVOID ioQueue;
} PER_IO_OPERATION_DATA, * LPPER_IO_OPERATION_DATA;
typedef struct
{
    LPVOID sendQueue;
    LPVOID recvQueue;
}IO_BUFFER_QUEUE;
typedef struct
{
    SOCKET Socket;
} PER_HANDLE_DATA, * LPPER_HANDLE_DATA;

typedef struct
{
    SOCKET sClient;
    HANDLE hCompletionPortID;
}SENDTHREAD_DATA;
DWORD WINAPI ServerWorkerThread(LPVOID CompletionPortID);
DWORD WINAPI ProcessQueueThread(LPVOID sClient);

int main(void)
{
    SOCKADDR_IN sinInternetAddr;
    SOCKET sListen;
    SOCKET sAccept;
    HANDLE hCompletionPort;
    SYSTEM_INFO SystemInfo;
    LPPER_HANDLE_DATA PerHandleData;
    LPPER_IO_OPERATION_DATA PerIoData;
    int i;
    DWORD dwRecvBytes;
    DWORD dwFlags;
    DWORD dwThreadID;
    WSADATA wsaData;
    DWORD dwRet;

    if ((dwRet = WSAStartup(0x0202, &wsaData)) != 0)
    {
        printf("WSAStartup失败了，错误信息如下: %d\n", dwRet);
        return 0;
    }
    // 设置一个I/O完成端口.
    if ((hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0)) == NULL)
    {
        printf( "CreateIoCompletionPort 失败了，错误信息如下: %d\n", GetLastError());
        return 0;
    }
    // 测试系统中有多少cpu处理器
    GetSystemInfo(&SystemInfo);
    printf("共有处理器数量: %d\n",SystemInfo.dwNumberOfProcessors);
    // 基于系统可用的处理器创建工作线程，为每个处理器创建连个线程
    for(i = 0; i < (int)SystemInfo.dwNumberOfProcessors * 2; i++)
    {
        HANDLE ThreadHandle;
        // 创建一个服务端线程并且传递一个完成端口给这个线程.
        if ((ThreadHandle = CreateThread(NULL, 0, ServerWorkerThread, hCompletionPort,
                                         0, &dwThreadID)) == NULL)
        {
            printf("CreateThread() 错误： %d\n", GetLastError());
            return 0;
        }
        else
        {
            printf("创建了一个线程.\n");
            CloseHandle(ThreadHandle);
        }
        // 关闭 thread句柄

    }
    // 创建一个监听套接字
    if ((sListen =WSASocket(AF_INET, SOCK_STREAM, 0, NULL,0,WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
    {
        printf("WSASocket() 错误： %d\n", WSAGetLastError());
        return 0;
    }
    else
    {
        printf("创建监听套接字成功\n");
    }
    sinInternetAddr.sin_family = AF_INET;
    sinInternetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    sinInternetAddr.sin_port = htons(PORT);
    if (bind(sListen, (PSOCKADDR) &sinInternetAddr, sizeof(sinInternetAddr)) == SOCKET_ERROR)
    {
        printf("bind()端口或IP时错误： %d\n", WSAGetLastError());
        return 0;
    }
    else
    {
        printf("绑定端口%d成功\n",PORT);
    }
    // 准备socket 用来监听
    if (listen(sListen, 5) == SOCKET_ERROR)
    {
        printf("listen()  错误 %d\n", WSAGetLastError());
        return 0;
    }
    else
    {
        printf("预处理成功，开始在端口 %d 处监听...\n",PORT);
    }
    //接受连接并且交给完成端口处理
    while(TRUE)
    {
        if ((sAccept = WSAAccept(sListen, NULL, NULL, NULL, 0)) == SOCKET_ERROR)
        {
            printf("WSAAccept()  错误： %d\n", WSAGetLastError());
            return 0;
        }
        // 创建一个套接字信息结构体去联系起来socket
        if ((PerHandleData = (LPPER_HANDLE_DATA) GlobalAlloc(GPTR,sizeof(PER_HANDLE_DATA))) == NULL)
        {
            printf("GlobalAlloc()  错误： %d\n", GetLastError());
            return 0;
        }
        // 将接受到的套接字与原始的完成端口联系起来.
        printf("号码为 %d 的socket连接上了\n", sAccept);
        PerHandleData->Socket = sAccept;
        if (CreateIoCompletionPort((HANDLE) sAccept, hCompletionPort, (DWORD) PerHandleData,0) == NULL)
        {
            printf("CreateIoCompletionPort  错误： %d\n", GetLastError());
            return 0;
        }
        // 创建每一个I/O 套接字信息结构体去和下面被调用的 to associate with the
        // WSARecv 连接.
        if ((PerIoData = (LPPER_IO_OPERATION_DATA) GlobalAlloc(GPTR,       sizeof(PER_IO_OPERATION_DATA))) == NULL)
        {
            printf("GlobalAlloc()  错误： %d\n", GetLastError());
            return 0;
        }
        else{
            printf("接收了一个连接\n");
        }
        ZeroMemory(&(PerIoData->overlapped), sizeof(OVERLAPPED));

        PerIoData->recvBuffer.len = DATA_BUFSIZE;
        PerIoData->recvBuffer.buf = szRecvBuffer;
        dwFlags = 0;

        if (WSARecv(sAccept, &(PerIoData->recvBuffer),  1, &dwRecvBytes, &dwFlags,&(PerIoData->overlapped), NULL) == SOCKET_ERROR)
        {
            if (WSAGetLastError() != ERROR_IO_PENDING)
            {
                printf("WSARecv()  错误： %d\n", WSAGetLastError());
                return 0;
            }
        }

    }
}

DWORD WINAPI ServerWorkerThread(LPVOID hCompletionPortID)
{
    HANDLE hCompletionPort = (HANDLE) hCompletionPortID;
    DWORD dwBytesTransferred;
    //LPOVERLAPPED Overlapped;
    LPPER_HANDLE_DATA PerHandleData;
    LPPER_IO_OPERATION_DATA PerIoData;
    //DWORD dwRealSend, dwRecvBytes;
    DWORD dwFlags;
    //CHAR szsendBuffer[DATA_BUFSIZE];
    while(TRUE)
    {
        if (GetQueuedCompletionStatus(hCompletionPort,
                                      &dwBytesTransferred,
                                      (LPDWORD)&PerHandleData,
                                      (LPOVERLAPPED *) &PerIoData,
                                      INFINITE) == 0)
        {
            printf("GetQueuedCompletionStatus  错误： %d\n", GetLastError());
            return 0;
        }
        if (dwBytesTransferred == 0)
        {
            printf("正在关闭socket %d\n", PerHandleData->Socket);
            if (closesocket(PerHandleData->Socket) == SOCKET_ERROR)
            {
                printf("closesocket()  错误： %d\n", WSAGetLastError());
                return 0;
            }
            GlobalFree(PerHandleData);
            GlobalFree(PerIoData);
            continue;
        }

        switch(PerIoData->dwOperationType)
        {
        case ioWrite:
        {
            puts("\nioWrite.");
            PerIoData->dwOperationType=ioRead;
            ZeroMemory(&(PerIoData->overlapped), sizeof(OVERLAPPED));
            PerIoData->recvBuffer.buf = szsendBuffer;
            PerIoData->recvBuffer.len = strlen(szsendBuffer)+1;
            //puts("WSASend() in");
            if (WSASend(PerHandleData->Socket, &(PerIoData->recvBuffer), 1 , &dwRealSend, 0,
                        &(PerIoData->overlapped), NULL) == SOCKET_ERROR)
            {
                if (WSAGetLastError() != ERROR_IO_PENDING)
                {
                    printf("WSASend()  错误： %d\n", WSAGetLastError());
                    return 0;
                }
            }
        }
        case ioRead:
        {
            CHAR szrecvBuffer[DATA_BUFSIZE];
            puts("\nioRead.");
            PerIoData->recvBuffer.buf[PerIoData->recvBuffer.len]=0;
            fputs(PerIoData->recvBuffer.buf,stdout);
            dwFlags = 0;

            ZeroMemory(&(PerIoData->overlapped), sizeof(OVERLAPPED));
            PerIoData->recvBuffer.len = DATA_BUFSIZE;
            PerIoData->recvBuffer.buf = szrecvBuffer;
            PerIoData->dwOperationType=ioRead;
            if (WSARecv(PerHandleData->Socket, &(PerIoData->recvBuffer), 1, &PerIoData->recvBuffer.len, &dwFlags,
                        &(PerIoData->overlapped), NULL) == SOCKET_ERROR)
            {
                if (WSAGetLastError() != ERROR_IO_PENDING)
                {
                    printf("WSARecv()  错误： %d\n", WSAGetLastError());
                    return 0;
                }
            }
        }
        }


        /*
            ZeroMemory(&(PerIoData->overlapped), sizeof(OVERLAPPED));
            strcpy(szsendBuffer,"@.");
            PerIoData->recvBuffer.buf = szsendBuffer;
            PerIoData->recvBuffer.len = strlen(szsendBuffer)+1;
            //puts("WSASend() in");
            if (WSASend(PerHandleData->Socket, &(PerIoData->recvBuffer), 1 , &dwSendBytes, 0,
                        &(PerIoData->overlapped), NULL) == SOCKET_ERROR)
            {
                if (WSAGetLastError() != ERROR_IO_PENDING)
                {
                    printf("WSASend()  错误： %d\n", WSAGetLastError());
                    return 0;
                }
            }
            //puts("WSASend() out");



        */
            //puts("WSARecv() out");

    }
}

DWORD WINAPI ProcessQueueThread(LPVOID sClient)
{
    return 0;
}
