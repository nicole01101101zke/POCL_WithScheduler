/* TTASimDevice.hh - basic way of accessing accelerator memory.
 *                 as a memory mapped region

   Copyright (c) 2022 Topi Leppänen / Tampere University

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

#ifndef TTASIMDEVICE_H
#define TTASIMDEVICE_H

#include "../AlmaIFDevice.hh"

class SimpleSimulatorFrontend;
class SimulatorCLI;

class TTASimDevice : public AlmaIFDevice {
public:
  TTASimDevice(const std::string &adf_name);
  ~TTASimDevice() override;

  virtual void loadProgramToDevice(almaif_kernel_data_s *kd, cl_kernel kernel,
                                   _cl_command_node *cmd) override;

  pocl_thread_t ttasim_thread;
  pocl_cond_t simulation_start_cond;
  pocl_lock_t lock;
  bool shutdownRequested = false;
  bool debuggerRequested = false;

  SimpleSimulatorFrontend *simulator_;
  SimulatorCLI *simulatorCLI_;

  void restartProgram();
  void stopProgram();

private:
  void loadProgram(const std::string &loadProgram);
};

#endif
