#include "server.hpp"

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

Server::Server() {
  ConfigureConst();
  StartListener();
  SetupConnections();
}

Server::~Server() {
  pthread_cancel(server_thread_id_);
  pthread_join(server_thread_id_, nullptr);
}

void Server::ConfigureConst() {
  program_pid_ = getpid();
  program_name_ = helpers::GetProgramNameByPid(program_pid_);
  helpers::MessageDebug("program_name_ = " + program_name_);
  helpers::MessageDebug("program_pid_ = " + std::to_string(program_pid_));
}

void Server::StartListener() {
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
    int* s = (int*)parameter;
    int s2;
    unsigned t;
    char buf[1];
    struct sockaddr_un remote;
    for (;;) {
      t = sizeof(remote);
      if ((s2 = accept(*s, (struct sockaddr*)&remote, &t)) == -1) {
        std::cerr << "accept" << std::endl;
        return nullptr;
      }
      if (recv(s2, buf, sizeof(buf), 0) == 0) {
        std::cout << "client disconnected" << std::endl;
        // Клиент отсоединился.
      }
      close(s2);
    }
  };
  pthread_create(&server_thread_id_, nullptr, server_thread_function,
                 &listen_socket_);
  std::cout << "started server s=" << sock_path << std::endl;
}

void Server::SetupConnections() {
  // Получаем список пидов всех инстансов программы.
  actual_pids_ = helpers::PidOf(program_name_);
  helpers::MessageDebug(actual_pids_, "all_pids_ = ");
  for (const auto& pid : actual_pids_) {
    if (pid == program_pid_) continue;
  }
  // Удаляем из словаря пиды, которые отсутствуют в полученном списке.
  //  helpers::erase_if(
  //      connections_, [this](const std::pair<pid_t, pthread_t>& item) {
  //        pid_t connected_pid = item.first;
  //        if (!std::binary_search(actual_pids_.begin(), actual_pids_.end(),
  //                                connected_pid)) {
  //          pthread_t thread_id = item.second;
  //          pthread_cancel(thread_id);
  //          pthread_join(thread_id, nullptr);
  //          return true;
  //        }
  //        return false;
  //      });
  // Добавляем новые пиды в словарь и сразу же коннектимся к ним.
  auto client_thread_function = [](void* parameter) -> void* {
    int s, len;
    if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
      std::cerr << "socket" << std::endl;
      return nullptr;
    }
    ClientThreadParameter* p = (ClientThreadParameter*)parameter;
    std::string sock_path =
        "/tmp/" + p->program_name + "_" + std::to_string(p->pid);
    struct sockaddr_un remote;
    remote.sun_family = AF_UNIX;
    std::strcpy(remote.sun_path, sock_path.data());
    len = strlen(remote.sun_path) + sizeof(remote.sun_family);
    if (connect(s, (struct sockaddr*)&remote, len) == -1) {
      std::cerr << "error connect to st=" << sock_path << std::endl;
      return nullptr;
    }
    std::cout << "connected to client" << std::endl;
    return nullptr;
  };
  for (auto pid : actual_pids_) {
    if (pid == program_pid_) continue;
    if (connections_.find(pid) == connections_.end()) {
      ClientThreadParameter* p = new ClientThreadParameter;
      p->pid = pid;
      p->program_name = program_name_;
      pthread_create(&p->thead_id, nullptr, client_thread_function, p);
      connections_[pid] = p;
    }
  }
}
