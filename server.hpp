#pragma once
#include <iostream>
class Server {
  std::string program_name_;
  pid_t current_pid_;
  pthread_t server_thread_id_;
  int listen_socket_;

 public:
  Server();
  ~Server();
};
