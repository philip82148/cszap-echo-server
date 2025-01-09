#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

void *ConnectionThread(void *socket_fd_ptr) {
  int socket_fd = *(int *)socket_fd_ptr;
  while (1) {
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    int new_socket_fd =
        accept(socket_fd, (struct sockaddr *)&client_addr, &addr_len);
    printf(
        "Accepted a connection from %s:%d\n",
        inet_ntoa(client_addr.sin_addr),
        ntohs(client_addr.sin_port)
    );

    while (1) {
      // リード
      char buffer[256];
      ssize_t size = read(new_socket_fd, buffer, 255);

      if (size <= 0) break;

      // ライト
      write(new_socket_fd, buffer, size);
    }

    // クローズ
    close(new_socket_fd);
    printf(
        "Closed a connection from %s:%d\n",
        inet_ntoa(client_addr.sin_addr),
        ntohs(client_addr.sin_port)
    );
  }
  return NULL;
}

int main() {
  // ソケット(エンドポイント)作成
  // domain: AF_INET=IPv4、type: SOCK_STREAM=TCP、protocol: 0=自動選択
  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_fd == -1) {
    printf("Failed to create a socket. errno=%d\n", errno);
    return 1;
  }
  printf("Created a socket\n");

  // バインド
  struct sockaddr_in server_addr = {
      .sin_family = AF_INET,
      .sin_port = htons(12345),
      .sin_addr = {.s_addr = INADDR_ANY},
      .sin_zero = {0},
  };
  if (bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr))) {
    printf("Failed to bind an address to a socket. errno=%d\n", errno);
    return 1;
  }
  printf("Bound an address to a socket\n");

  // リッスン
  if (listen(socket_fd, SOMAXCONN)) {
    printf("Failed to listen. errno=%d\n", errno);
    return 1;
  }
  printf("Started to listend\n");

  pthread_t connection_threads[SOMAXCONN];
  for (int i = 0; i < SOMAXCONN; i++) {
    if (pthread_create(
            &connection_threads[i], NULL, ConnectionThread, &socket_fd
        )) {
      printf("Failed to create a thread. errno=%d\n", errno);
      return 1;
    }
  }

  for (int i = 0; i < SOMAXCONN; i++) {
    if (pthread_join(connection_threads[i], NULL)) {
      printf("Failed to join a thread. errno=%d\n", errno);
      return 1;
    }
  }

  close(socket_fd);

  return 0;
}
