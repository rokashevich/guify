#pragma once
#include <pthread.h>

#include <iostream>
#include <map>
#include <set>
#include <vector>
class Server {
  struct ClientThreadParameter {
    pid_t pid;
    std::string program_name;
    pthread_t thead_id;
  };
  std::string program_name_;
  pid_t program_pid_;
  pthread_t server_thread_id_;
  int listen_socket_;
  std::vector<pid_t> actual_pids_;
  std::map<pid_t, ClientThreadParameter*> connections_;
  void ConfigureConst();
  void StartListener();
  void SetupConnections();

 public:
  Server();
  ~Server();
};
