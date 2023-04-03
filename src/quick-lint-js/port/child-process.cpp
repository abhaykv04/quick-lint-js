// Copyright (C) 2020  Matthew "strager" Glazar
// See end of file for extended copyright information.

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <quick-lint-js/io/file.h>
#include <quick-lint-js/io/pipe.h>
#include <quick-lint-js/io/temporary-directory.h>
#include <quick-lint-js/port/child-process.h>
#include <quick-lint-js/port/have.h>
#include <quick-lint-js/port/thread.h>
#include <quick-lint-js/util/narrow-cast.h>
#include <string>
#include <vector>

#if QLJS_HAVE_CRT_EXTERNS_H
#include <crt_externs.h>
#endif

#if QLJS_HAVE_POSIX_SPAWN
#include <spawn.h>
#endif

#if QLJS_HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif

#if QLJS_HAVE_UNISTD_H
#include <unistd.h>
#endif

#if QLJS_HAVE_WINDOWS_H
#include <quick-lint-js/port/windows.h>
#endif

namespace quick_lint_js {
run_program_result run_program(std::initializer_list<std::string> command) {
  return run_program(span<const std::string>(command.begin(), command.end()));
}

run_program_result run_program(std::initializer_list<const char*> command) {
  return run_program(span<const char* const>(command.begin(), command.end()));
}

run_program_result run_program(std::initializer_list<std::string> command,
                               run_program_options options) {
  return run_program(span<const std::string>(command.begin(), command.end()),
                     options);
}

run_program_result run_program(std::initializer_list<const char*> command,
                               run_program_options options) {
  return run_program(span<const char* const>(command.begin(), command.end()),
                     options);
}

run_program_result run_program(span<const std::string> command) {
  return run_program(command, run_program_options());
}

run_program_result run_program(span<const char* const> command) {
  return run_program(command, run_program_options());
}

run_program_result run_program(span<const std::string> command,
                               run_program_options options) {
  std::vector<const char*> command_raw;
  for (const std::string& arg : command) {
    command_raw.push_back(arg.c_str());
  }
  return run_program(span<const char* const>(command_raw), options);
}

#if QLJS_HAVE_POSIX_SPAWN
run_program_result run_program(span<const char* const> command,
                               run_program_options options) {
  QLJS_ASSERT(command.size() >= 1 && "expected at least path to .exe");

  pipe_fds program_input = make_pipe();
  pipe_fds program_output = make_pipe();
  ::posix_spawn_file_actions_t file_actions;
  ::posix_spawn_file_actions_init(&file_actions);
  ::posix_spawn_file_actions_adddup2(&file_actions, program_input.reader.get(),
                                     STDIN_FILENO);
  ::posix_spawn_file_actions_adddup2(&file_actions, program_output.writer.get(),
                                     STDOUT_FILENO);
  ::posix_spawn_file_actions_adddup2(&file_actions, program_output.writer.get(),
                                     STDERR_FILENO);

  result<std::string, platform_file_io_error> old_cwd;
  if (options.current_directory != nullptr) {
    old_cwd = get_current_working_directory();
    if (!old_cwd.ok()) {
      std::fprintf(stderr, "error: failed to save current directory: %s\n",
                   old_cwd.error_to_string().c_str());
      std::exit(1);
    }

    set_current_working_directory_or_exit(options.current_directory);
  }

  std::vector<char*> argv;
  for (const char* arg : command) {
    argv.push_back(const_cast<char*>(arg));
  }
  argv.push_back(nullptr);
  const char* exe_file = command[0];

  ::pid_t pid;
#if QLJS_HAVE_NS_GET_ENVIRON
  char**& environ = *::_NSGetEnviron();
#endif
  int rc = ::posix_spawnp(/*pid=*/&pid, /*file=*/exe_file,
                          /*file_actions=*/&file_actions,
                          /*attrp=*/nullptr,
                          /*argv=*/argv.data(),
                          /*envp=*/environ);
  if (rc != 0) {
    std::fprintf(stderr, "error: failed to spawn %s: %s\n", exe_file,
                 std::strerror(errno));
    std::exit(1);
  }
  program_input.reader.close();
  program_output.writer.close();

  if (options.current_directory != nullptr) {
    set_current_working_directory_or_exit(old_cwd->c_str());
  }

  ::posix_spawn_file_actions_destroy(&file_actions);

  thread input_writer_thread;
  if (options.input.empty()) {
    program_input.writer.close();
  } else {
    input_writer_thread = thread([&]() -> void {
      program_input.writer.write_full(options.input.data(),
                                      options.input.size());
      program_input.writer.close();
    });
  }

  run_program_result result;

  auto output = read_file(program_output.reader.ref());
  if (!output.ok()) {
    std::fprintf(stderr, "error: %s\n", output.error_to_string().c_str());
    std::exit(1);
  }
  result.output = std::move(*output);

  result.exit_status = wait_for_process_exit(pid);

  if (!options.input.empty()) {
    input_writer_thread.join();
  }

  return result;
}
#endif

#if QLJS_HAVE_SYS_WAIT_H
int wait_for_process_exit(::pid_t pid) {
retry:
  int status;
  ::pid_t rc = ::waitpid(pid, &status, /*options=*/0);
  if (rc == -1) {
    if (errno == EINTR) {
      goto retry;
    }
    std::fprintf(stderr, "error: failed to wait for process %lld: %s\n",
                 narrow_cast<long long>(pid), std::strerror(errno));
    std::exit(1);
  }

  if (WIFEXITED(status)) {
    return WEXITSTATUS(status);
  }
  if (WIFSIGNALED(status)) {
    QLJS_ASSERT(WTERMSIG(status) > 0);
    return -WTERMSIG(status);  // Arbitrary.
  }
  QLJS_ASSERT(false);
  return -1;  // Arbitrary.
}
#endif
}

// quick-lint-js finds bugs in JavaScript programs.
// Copyright (C) 2020  Matthew "strager" Glazar
//
// This file is part of quick-lint-js.
//
// quick-lint-js is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// quick-lint-js is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with quick-lint-js.  If not, see <https://www.gnu.org/licenses/>.
