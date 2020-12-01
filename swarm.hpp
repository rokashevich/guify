#pragma once
#include <pthread.h>
#include <semaphore.h>

#include <functional>
#include <iostream>
#include <map>
#include <set>
#include <tuple>
#include <vector>
class Swarm {
  const pid_t program_pid_;
  const std::string program_name_;
  sem_t semaphore_;
  int server_socket_;
  std::vector<pid_t> instances_pids_;
  std::function<void(int, int)> callback_;
  void RunServer();  // Сервер, к которому будут коннектиться другие инстансы.
  pthread_t server_thread_id_;
  void Callback(int num_instances, int own_num);
  void Reconnect();
  std::map<pid_t, std::tuple<pthread_t, std::string>> connections_;

  std::tuple<sem_t, void (*)(std::pair<int, int>)>
      reconnecter_notifier_parameters_;
  void RunClient();

  std::string SockPath(pid_t pid);
  Swarm();
  Swarm(const Swarm&) = delete;
  Swarm& operator=(const Swarm&) = delete;

 public:
  void Start(std::function<void(int, int)> callback);
  void Stop();
  sem_t* Semaphore();
  static Swarm& Singleton();
  ~Swarm();
};
