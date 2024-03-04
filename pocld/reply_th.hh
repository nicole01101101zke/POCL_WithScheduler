/* reply_th.hh - pocld thread that sends command results back to the client

   Copyright (c) 2018 Michal Babej / Tampere University of Technology
   Copyright (c) 2019-2023 Jan Solanti / Tampere University

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to
   deal in the Software without restriction, including without limitation the
   rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
   sell copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
   IN THE SOFTWARE.
*/

#ifndef POCL_REMOTE_REPLY_TH_HH
#define POCL_REMOTE_REPLY_TH_HH

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "common.hh"
#include "traffic_monitor.hh"
#include "virtual_cl_context.hh"

#ifdef __GNUC__
#pragma GCC visibility push(hidden)
#endif

class ReplyQueueThread {
  std::atomic_int *fd;
  std::string id_str;
  VirtualContextBase *virtualContext;
  std::vector<Reply *> io_inflight;
  std::mutex io_mutex;
  std::condition_variable io_cond;
  std::thread io_thread;
  ExitHelper *eh;
  TrafficMonitor *netstat;

public:
  ReplyQueueThread(std::atomic_int *f, VirtualContextBase *c, ExitHelper *eh,
                   TrafficMonitor *tm, const char *id_str);

  ~ReplyQueueThread();

  void pushReply(Reply *reply);

  void writeThread();
};

typedef std::unique_ptr<ReplyQueueThread> ReplyQueueThreadUPtr;

#ifdef __GNUC__
#pragma GCC visibility pop
#endif

#endif
