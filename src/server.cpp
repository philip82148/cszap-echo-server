#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <thread>

#include "../debug.hpp"

using namespace std;

void ConnectionThread(int socket_fd) {
  while (true) {
    sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    int new_socket_fd =
        accept(socket_fd, (struct sockaddr *)&client_addr, &addr_len);
    clog << "Accepted a connection from " << inet_ntoa(client_addr.sin_addr)
         << ":" << ntohs(client_addr.sin_port) << endl;

    while (true) {
      // リード
      char buffer[256];
      ssize_t size = read(new_socket_fd, buffer, 255);

      if (size <= 0) break;

      // ライト
      write(new_socket_fd, buffer, size);
    }

    // クローズ
    close(new_socket_fd);
  }
}

int main() {
  // ソケット(エンドポイント)作成
  // domain: AF_INET=IPv4、type: SOCK_STREAM=TCP、protocol: 0=自動選択
  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_fd == -1) {
    clog << "Failed to create a socket" << endl;
    return 1;
  }
  clog << "Created a socket" << endl;

  // バインド
  sockaddr_in server_addr = {
      .sin_family = AF_INET,
      .sin_port = htons(12345),
      .sin_addr = {.s_addr = INADDR_ANY},
      .sin_zero = {0},
  };
  if (bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr))) {
    clog << "Failed to bind an address to a socket" << endl;
    return 1;
  }
  clog << "Bound an address to a socket" << endl;

  // リッスン
  if (listen(socket_fd, SOMAXCONN)) {
    clog << "Failed to listen" << endl;
    return 1;
  }
  clog << "Started to listend" << endl;

  vector<thread> connection_threads;
  for (int i = 0; i < SOMAXCONN; ++i) {
    connection_threads.emplace_back(ConnectionThread, socket_fd);
  }
  clog << "Made " << SOMAXCONN << " connections" << endl;

  for (int i = 0; i < SOMAXCONN; ++i) {
    connection_threads[i].join();
  }

  close(socket_fd);

  return 0;
}
