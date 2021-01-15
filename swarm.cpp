#include "swarm.hpp"

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#include <algorithm>
#include <cassert>
#include <cstring>
#include <map>
#include <string>

#include "helpers.hpp"
Swarm::Swarm()
    : program_pid_(getpid()),
      program_name_(helpers::GetProgramNameByPid(program_pid_)) {
  sem_init(&semaphore_, 0, 1);
}
Swarm::~Swarm() { Stop(); }
void Swarm::Stop() { pthread_cancel(server_thread_id_); }
Swarm& Swarm::Singleton() {
  static Swarm* singleton = new Swarm();
  return *singleton;
}
void Swarm::RunServer() {
  const std::string sock_path = SockPath(program_pid_);
  if ((server_socket_ = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    std::cerr << "socket" << std::endl;
    return;
  }
  struct sockaddr_un local;
  local.sun_family = AF_UNIX;
  std::strcpy(local.sun_path, sock_path.data());
  unlink(local.sun_path);
  int len = strlen(local.sun_path) + sizeof(local.sun_family);
  if (bind(server_socket_, (struct sockaddr*)&local, len) == -1) {
    std::cerr << "bind" << std::endl;
    return;
  }
  if (listen(server_socket_, 32) == -1) {
    std::cerr << "listen" << std::endl;
    return;
  }
  auto server_thread_function = [](void* parameter) -> void* {
    while (true) {
      int* s = (int*)parameter;
      int listener_socker;
      struct sockaddr_un remote;
      unsigned t = sizeof(remote);
      if ((listener_socker = accept(*s, (struct sockaddr*)&remote, &t)) == -1) {
        std::cerr << "accept!" << std::endl;
        continue;
      }
      auto recv_thread_function = [](void* parameter) -> void* {
        int s = *(int*)parameter;
        std::cerr << "someone connected " << s << std::endl;
        sem_post(Swarm::Singleton().Semaphore());
        char buf[1];
        recv(s, buf, sizeof(buf), 0);
        close(s);
        std::cerr << "someone disconnected " << s << std::endl;
        sem_post(Swarm::Singleton().Semaphore());
        return nullptr;
      };
      pthread_t listener_thread_id;
      pthread_create(&listener_thread_id, nullptr, recv_thread_function,
                     &listener_socker);
    }
    return nullptr;
  };
  pthread_create(&server_thread_id_, nullptr, server_thread_function,
                 &server_socket_);
  pthread_detach(server_thread_id_);
}

void Swarm::Reconnect() {
  // Получаем обновлённый список PIDов всех инстансов программы.
  instances_pids_ = helpers::PidOf(program_name_);
  helpers::MessageDebug(instances_pids_, "instances_pids_=");
  //  instances_pids_ = helpers::PidOf(program_name_);
  //  helpers::MessageDebug(instances_pids_, "2 instances_pids_=");
  //  instances_pids_ = helpers::PidOf(program_name_);
  //  helpers::MessageDebug(instances_pids_, "3 instances_pids_=");
  // exit(0);
  // Удаляем соединения к PIDам, которые отсутствуют в обновлённом списке.
  helpers::erase_if(
      connections_,
      [this](const std::pair<pid_t, std::tuple<pthread_t, std::string>>& item) {
        pthread_t thread_id = std::get<0>(item.second);
        pid_t connected_pid = item.first;
        if (!std::binary_search(instances_pids_.begin(), instances_pids_.end(),
                                connected_pid)) {
          pthread_cancel(thread_id);
          pthread_join(thread_id, nullptr);
          return true;
        }
        return false;
      });
  // Удаляем соединения к PIDам, которые есть, но потоковые функции которых
  // завершеились.
  helpers::erase_if(
      connections_,
      [](const std::pair<pid_t, std::tuple<pthread_t, std::string>> item) {
        pthread_t thread_id = std::get<0>(item.second);
        return !pthread_tryjoin_np(thread_id, nullptr);
      });
  // Добавляем новые пиды в словарь и сразу же коннектимся к ним.
  auto client_thread_function = [](void* parameter) -> void* {
    auto sock_path = ((std::string*)parameter)->data();
    int s, len;
    if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
      std::cerr << "socket" << std::endl;
      return nullptr;
    }
    struct sockaddr_un remote;
    remote.sun_family = AF_UNIX;
    std::strcpy(remote.sun_path, sock_path);
    len = strlen(remote.sun_path) + sizeof(remote.sun_family);
    if (connect(s, (struct sockaddr*)&remote, len) == -1) {
      std::cerr << "error connect to st=" << sock_path << std::endl;
      return nullptr;
    }
    char buf[1];
    recv(s, buf, sizeof(buf), 0);
    close(s);
    std::cerr << "disconnected from client" << std::endl;
    return nullptr;
  };
  for (auto pid : instances_pids_) {
    if (pid == program_pid_) continue;
    if (connections_.find(pid) == connections_.end()) {
      pthread_t thread_id;
      std::string sock_path = SockPath(pid);
      connections_[pid] = std::make_tuple(thread_id, sock_path);
      pthread_create(&std::get<0>(connections_[pid]), nullptr,
                     client_thread_function, &std::get<1>(connections_[pid]));
    }
  }
}
std::string Swarm::SockPath(pid_t pid) {
  return "/tmp/" + program_name_ + "_" + std::to_string(pid);
}
void Swarm::RunClient() {
  auto thread_function = [](void*) -> void* {
    Swarm& swarm = Swarm::Singleton();
    sem_t* semaphore = swarm.Semaphore();
    while (true) {
      sem_wait(semaphore);
      swarm.Reconnect();

      swarm.Callback(444, 333);
    }
  };
  pthread_t thread_id;
  pthread_create(&thread_id, nullptr, thread_function, nullptr);
  pthread_detach(server_thread_id_);
}
sem_t* Swarm::Semaphore() { return &semaphore_; };
void Swarm::Start(std::function<void(int, int)> callback) {
  callback_ = callback;
  RunServer();
  RunClient();
}
void Swarm::Callback(int num_instances, int own_num) {
  callback_(num_instances, own_num);
}

std::tuple<int, int> Swarm::Order() { return std::make_tuple<int, int>(1, 1); }
