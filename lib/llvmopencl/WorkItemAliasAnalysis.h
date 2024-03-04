// Header for AllocasToEntry, an LLVM pass to move allocas to the function
// entry node.
//
// Copyright (c) 2013 Pekka Jääskeläinen / TUT
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

#ifndef POCL_WORKITEM_ALIAS_ANALYSIS_H
#define POCL_WORKITEM_ALIAS_ANALYSIS_H

#include "config.h"

#include <llvm/IR/Function.h>
#include <llvm/IR/PassManager.h>
#include <llvm/Pass.h>
#include <llvm/Passes/PassBuilder.h>

namespace pocl {

class WorkItemAAResult;

#if LLVM_MAJOR < MIN_LLVM_NEW_PASSMANAGER

/// Legacy wrapper pass to provide the (WorkItemAAWrapperPass) object.
class WorkItemAliasAnalysis : public llvm::FunctionPass {
  std::unique_ptr<WorkItemAAResult> Result;

public:
  static char ID;

  WorkItemAliasAnalysis();
  virtual ~WorkItemAliasAnalysis();

  WorkItemAAResult &getResult();

  virtual bool runOnFunction(llvm::Function &F) override;
  virtual void getAnalysisUsage(llvm::AnalysisUsage &AU) const override;
};

#else

class WorkItemAliasAnalysis
    : public llvm::AnalysisInfoMixin<WorkItemAliasAnalysis> {
public:
  static llvm::AnalysisKey Key;
  using Result = WorkItemAAResult;
  static void registerWithPB(llvm::PassBuilder &PB);

  Result run(llvm::Function &F, llvm::FunctionAnalysisManager &AM);
  static bool isRequired() { return true; }
};

#endif

} // namespace pocl

#endif
