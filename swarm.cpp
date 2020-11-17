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
  RunServer();
  SetupConnections();
}

Swarm::~Swarm() { pthread_cancel(server_thread_id_); }

void Swarm::RunServer() {
  int len;
  struct sockaddr_un local;
  const std::string sock_path =
      "/tmp/" + program_name_ + "_" + std::to_string(program_pid_);
  if ((listen_socket_ = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    std::cerr << "socket" << std::endl;
    return;
  }
  local.sun_family = AF_UNIX;
  std::strcpy(local.sun_path, sock_path.data());
  unlink(local.sun_path);
  len = strlen(local.sun_path) + sizeof(local.sun_family);
  if (bind(listen_socket_, (struct sockaddr*)&local, len) == -1) {
    std::cerr << "bind" << std::endl;
    return;
  }
  if (listen(listen_socket_, 32) == -1) {
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
        std::cerr << "accept" << std::endl;
        continue;
      }
      auto recv_thread_function = [](void* parameter) -> void* {
        int s = *(int*)parameter;
        std::cout << "someone connected " << s << std::endl;
        char buf[1];
        recv(s, buf, sizeof(buf), 0);
        close(s);
        std::cout << "someone disconnected " << s << std::endl;
        return nullptr;
      };
      pthread_t listener_thread_id;
      pthread_create(&listener_thread_id, nullptr, recv_thread_function,
                     &listener_socker);
    }
    return nullptr;
  };
  pthread_create(&server_thread_id_, nullptr, server_thread_function,
                 &listen_socket_);
  pthread_detach(server_thread_id_);
}

void Swarm::SetupConnections() {
  // Получаем обновлённый список PIDов всех инстансов программы.
  instances_pids_ = helpers::PidOf(program_name_);

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
    std::cout << "disconnected from client" << std::endl;
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
