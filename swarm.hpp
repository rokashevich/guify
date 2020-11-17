#pragma once
#include <pthread.h>

#include <iostream>
#include <map>
#include <set>
#include <tuple>
#include <vector>
class Swarm {
  pid_t program_pid_;
  std::string program_name_;
  std::vector<pid_t> instances_pids_;
  std::map<pid_t, std::tuple<pthread_t, std::string>> connections_;

  pthread_t server_thread_id_;
  int listen_socket_;

  void RunServer();  // Сервер, к которому будут коннектиться чужие клиенты.
  void SetupConnections();
  std::string SockPath(pid_t pid);

 public:
  Swarm();
  ~Swarm();
};
