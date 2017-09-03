#include <stdio.h> // For printf(), fprintf(), perror()
#include <sys/socket.h> // For socket(), connect(), send(), recv()
#include <arpa/inet.h> // For sockaddr_in, inet_addr()
#include <stdlib.h> // For atoi()
#include <string.h> // For exit(), memset()
#include <unistd.h> // For close()

#define RECVBUFSIZE 32 // 受信バッファサイズ
#define DEFAULT_PORT 7 // echo サービスの well-known ポート

void errorHandler (char *msg) {
  perror(msg);
  exit(1);
}

int main (int argc, char *argv[]) {

  int sock; // socket descriptor
  struct sockaddr_in echoServerAddr;
  unsigned short echoServerPort;
  char *serverIP;
  char *echoString;
  char echoBuffer[RECVBUFSIZE];
  unsigned int echoStringLen;
  int recvBytes, totalRecvBytes;

  // arguments validation
  if ((argc < 3) || (argc > 4)) {
    fprintf(stderr, "Usage: %s <Server IP> <Echo Word> [<Server Port>]\n", argv[0]);
    exit(1);
  }

  serverIP = argv[1];
  echoString = argv[2];

  // port validation
  if (argc == 4) {
    echoServerPort = atoi(argv[3]);
  } else {
    echoServerPort = DEFAULT_PORT;
  }

  // create socket
  if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    errorHandler("socket() failed");
  }

  // create address struct of echo server
  memset(&echoServerAddr, 0, sizeof(echoServerAddr)); // echoServerAddr 構造体の領域をゼロクリアしておく
  echoServerAddr.sin_family = AF_INET;
  echoServerAddr.sin_addr.s_addr = inet_addr(serverIP);
  echoServerAddr.sin_port = htons(echoServerPort);

  // connect echo server
  if (connect(sock, (struct sockaddr *) &echoServerAddr, sizeof(echoServerAddr)) < 0) {
    errorHandler("connect() failed");
  }

  echoStringLen = strlen(echoString);

  // send message to echo server
  if (send(sock, echoString, echoStringLen, 0) != echoStringLen) {
    errorHandler("send() failed");
  }

  // recieve message from echo server
  totalRecvBytes = 0;
  printf("Recieved: ");
  while (totalRecvBytes < echoStringLen) {
    if ((recvBytes = recv(sock, echoBuffer, RECVBUFSIZE-1, 0)) <= 0) {
      errorHandler("recv() failed");
    }
    totalRecvBytes += recvBytes;
    echoBuffer[recvBytes] = '\0';
    printf("%s", echoBuffer);
  }
  printf("\n");

  // close socket
  close(sock);
  exit(0);

}
