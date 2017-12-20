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
        printf("WSAStartupʧ���ˣ�������Ϣ����: %d\n", dwRet);
        return 0;
    }
    // ����һ��I/O��ɶ˿�.
    if ((hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0)) == NULL)
    {
        printf( "CreateIoCompletionPort ʧ���ˣ�������Ϣ����: %d\n", GetLastError());
        return 0;
    }
    // ����ϵͳ���ж���cpu������
    GetSystemInfo(&SystemInfo);
    printf("���д���������: %d\n",SystemInfo.dwNumberOfProcessors);
    // ����ϵͳ���õĴ��������������̣߳�Ϊÿ�����������������߳�
    for(i = 0; i < (int)SystemInfo.dwNumberOfProcessors * 2; i++)
    {
        HANDLE ThreadHandle;
        // ����һ��������̲߳��Ҵ���һ����ɶ˿ڸ�����߳�.
        if ((ThreadHandle = CreateThread(NULL, 0, ServerWorkerThread, hCompletionPort,
                                         0, &dwThreadID)) == NULL)
        {
            printf("CreateThread() ���� %d\n", GetLastError());
            return 0;
        }
        else
        {
            printf("������һ���߳�.\n");
            CloseHandle(ThreadHandle);
        }
        // �ر� thread���

    }
    // ����һ�������׽���
    if ((sListen =WSASocket(AF_INET, SOCK_STREAM, 0, NULL,0,WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
    {
        printf("WSASocket() ���� %d\n", WSAGetLastError());
        return 0;
    }
    else
    {
        printf("���������׽��ֳɹ�\n");
    }
    sinInternetAddr.sin_family = AF_INET;
    sinInternetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    sinInternetAddr.sin_port = htons(PORT);
    if (bind(sListen, (PSOCKADDR) &sinInternetAddr, sizeof(sinInternetAddr)) == SOCKET_ERROR)
    {
        printf("bind()�˿ڻ�IPʱ���� %d\n", WSAGetLastError());
        return 0;
    }
    else
    {
        printf("�󶨶˿�%d�ɹ�\n",PORT);
    }
    // ׼��socket ��������
    if (listen(sListen, 5) == SOCKET_ERROR)
    {
        printf("listen()  ���� %d\n", WSAGetLastError());
        return 0;
    }
    else
    {
        printf("Ԥ����ɹ�����ʼ�ڶ˿� %d ������...\n",PORT);
    }
    //�������Ӳ��ҽ�����ɶ˿ڴ���
    while(TRUE)
    {
        if ((sAccept = WSAAccept(sListen, NULL, NULL, NULL, 0)) == SOCKET_ERROR)
        {
            printf("WSAAccept()  ���� %d\n", WSAGetLastError());
            return 0;
        }
        // ����һ���׽�����Ϣ�ṹ��ȥ��ϵ����socket
        if ((PerHandleData = (LPPER_HANDLE_DATA) GlobalAlloc(GPTR,sizeof(PER_HANDLE_DATA))) == NULL)
        {
            printf("GlobalAlloc()  ���� %d\n", GetLastError());
            return 0;
        }
        // �����ܵ����׽�����ԭʼ����ɶ˿���ϵ����.
        printf("����Ϊ %d ��socket��������\n", sAccept);
        PerHandleData->Socket = sAccept;
        if (CreateIoCompletionPort((HANDLE) sAccept, hCompletionPort, (DWORD) PerHandleData,0) == NULL)
        {
            printf("CreateIoCompletionPort  ���� %d\n", GetLastError());
            return 0;
        }
        // ����ÿһ��I/O �׽�����Ϣ�ṹ��ȥ�����汻���õ� to associate with the
        // WSARecv ����.
        if ((PerIoData = (LPPER_IO_OPERATION_DATA) GlobalAlloc(GPTR,       sizeof(PER_IO_OPERATION_DATA))) == NULL)
        {
            printf("GlobalAlloc()  ���� %d\n", GetLastError());
            return 0;
        }
        else{
            printf("������һ������\n");
        }
        ZeroMemory(&(PerIoData->overlapped), sizeof(OVERLAPPED));

        PerIoData->recvBuffer.len = DATA_BUFSIZE;
        PerIoData->recvBuffer.buf = szRecvBuffer;
        dwFlags = 0;

        if (WSARecv(sAccept, &(PerIoData->recvBuffer),  1, &dwRecvBytes, &dwFlags,&(PerIoData->overlapped), NULL) == SOCKET_ERROR)
        {
            if (WSAGetLastError() != ERROR_IO_PENDING)
            {
                printf("WSARecv()  ���� %d\n", WSAGetLastError());
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
            printf("GetQueuedCompletionStatus  ���� %d\n", GetLastError());
            return 0;
        }
        if (dwBytesTransferred == 0)
        {
            printf("���ڹر�socket %d\n", PerHandleData->Socket);
            if (closesocket(PerHandleData->Socket) == SOCKET_ERROR)
            {
                printf("closesocket()  ���� %d\n", WSAGetLastError());
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
                    printf("WSASend()  ���� %d\n", WSAGetLastError());
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
                    printf("WSARecv()  ���� %d\n", WSAGetLastError());
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
                    printf("WSASend()  ���� %d\n", WSAGetLastError());
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
