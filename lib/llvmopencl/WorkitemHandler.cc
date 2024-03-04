// Base class for passes that generate work-group functions out of a bunch
// of work-items.
//
// Copyright (c) 2011-2012 Carlos Sánchez de La Lama / URJC and
//               2012-2019 Pekka Jääskeläinen
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "CompilerWarnings.h"
IGNORE_COMPILER_WARNING("-Wmaybe-uninitialized")
#include <llvm/ADT/Twine.h>
POP_COMPILER_DIAGS
IGNORE_COMPILER_WARNING("-Wunused-parameter")
#include "llvm/IR/Metadata.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/ValueSymbolTable.h"
#include "llvm/Support/CommandLine.h"

#include "WorkitemHandler.h"
#include "Kernel.h"
#include "DebugHelpers.h"
POP_COMPILER_DIAGS

#include "pocl_llvm_api.h"

#include <iostream>
#include <sstream>

POP_COMPILER_DIAGS

namespace pocl {

using namespace llvm;

cl::opt<bool> AddWIMetadata(
    "add-wi-metadata", cl::init(false), cl::Hidden,
    cl::desc("Adds a work item identifier to each of the instruction in "
             "work items."));

void
WorkitemHandler::Initialize(Kernel *K) {

  llvm::Module *M = K->getParent();

  getModuleIntMetadata(*M, "device_address_bits", AddressBits);

  getModuleStringMetadata(*M, "KernelName", KernelName);
  getModuleIntMetadata(*M, "WGMaxGridDimWidth", WGMaxGridDimWidth);
  getModuleIntMetadata(*M, "WGLocalSizeX", WGLocalSizeX);
  getModuleIntMetadata(*M, "WGLocalSizeY", WGLocalSizeY);
  getModuleIntMetadata(*M, "WGLocalSizeZ", WGLocalSizeZ);
  getModuleBoolMetadata(*M, "WGDynamicLocalSize", WGDynamicLocalSize);
  getModuleBoolMetadata(*M, "WGAssumeZeroGlobalOffset",
                        WGAssumeZeroGlobalOffset);

  if (WGLocalSizeX == 0)
    WGLocalSizeX = 1;
  if (WGLocalSizeY == 0)
    WGLocalSizeY = 1;
  if (WGLocalSizeZ == 0)
    WGLocalSizeZ = 1;

  SizeTWidth = AddressBits;
  SizeT = IntegerType::get(M->getContext(), SizeTWidth);

  assert ((SizeTWidth == 32 || SizeTWidth == 64) &&
          "Only 32 and 64 bit size_t widths supported.");

  llvm::Type *LocalIdType = SizeT;
  LocalIdZGlobal = M->getOrInsertGlobal(POCL_LOCAL_ID_Z_GLOBAL, LocalIdType);
  LocalIdYGlobal = M->getOrInsertGlobal(POCL_LOCAL_ID_Y_GLOBAL, LocalIdType);
  LocalIdXGlobal = M->getOrInsertGlobal(POCL_LOCAL_ID_X_GLOBAL, LocalIdType);
}

bool WorkitemHandler::dominatesUse(llvm::DominatorTree &DT, Instruction &Inst,
                                   unsigned OpNum) {

  Instruction *Op = cast<Instruction>(Inst.getOperand(OpNum));
  BasicBlock *OpBlock = Op->getParent();
  PHINode *PN = dyn_cast<PHINode>(&Inst);

  // DT can handle non phi instructions for us.
  if (!PN) 
    {
      // Definition must dominate use unless use is unreachable!
      return Op->getParent() == Inst.getParent() || DT.dominates(Op, &Inst);
    }

  // PHI nodes are more difficult than other nodes because they actually
  // "use" the value in the predecessor basic blocks they correspond to.
  unsigned Val = PHINode::getIncomingValueNumForOperand(OpNum);
  BasicBlock *PredBB = PN->getIncomingBlock(Val);
  return (PredBB && DT.dominates(OpBlock, PredBB));
}

/* Fixes the undominated variable uses.

   These appear when a conditional barrier kernel is replicated to
   form a copy of the *same basic block* in the alternative 
   "barrier path".

   E.g., from

   A -> [exit], A -> B -> [exit]

   a replicated CFG as follows, is created:

   A1 -> (T) A2 -> [exit1],  A1 -> (F) A2' -> B1, B2 -> [exit2] 

   The regions are correct because of the barrier semantics
   of "all or none". In case any barrier enters the [exit1]
   from A1, all must (because there's a barrier in the else
   branch).

   Here at A2 and A2' one creates the same variables. 
   However, B2 does not know which copy
   to refer to, the ones created in A2 or ones in A2' (correct).
   The mapping data contains only one possibility, the
   one that was placed there last. Thus, the instructions in B2 
   might end up referring to the variables defined in A2 
   which do not nominate them.

   The variable references are fixed by exploiting the knowledge
   of the naming convention of the cloned variables. 

   One potential alternative way would be to collect the refmaps per BB,
   not globally. Then as a final phase traverse through the 
   basic blocks starting from the beginning and propagating the
   reference data downwards, the data from the new BB overwriting
   the old one. This should ensure the reachability without 
   the costly dominance analysis.
*/
bool WorkitemHandler::fixUndominatedVariableUses(llvm::DominatorTree &DT,
                                                 llvm::Function &F) {
  bool changed = false;
#if LLVM_VERSION_MAJOR < 11
  DT.releaseMemory();
#else
  DT.reset();
#endif
  DT.recalculate(F);

  for (Function::iterator i = F.begin(), e = F.end(); i != e; ++i) 
    {
      llvm::BasicBlock *bb = &*i;
      for (llvm::BasicBlock::iterator ins = bb->begin(), inse = bb->end();
           ins != inse; ++ins)
        {
          for (unsigned opr = 0; opr < ins->getNumOperands(); ++opr)
            {
              if (!isa<Instruction>(ins->getOperand(opr))) continue;
              Instruction *operand = cast<Instruction>(ins->getOperand(opr));
              if (dominatesUse(DT, *ins, opr)) 
                  continue;
#ifdef DEBUG_REFERENCE_FIXING
              std::cout << "### dominance error!" << std::endl;
              operand->dump();
              std::cout << "### does not dominate:" << std::endl;
              ins->dump();
#endif
              StringRef baseName;
              std::pair< StringRef, StringRef > pieces = 
                operand->getName().rsplit('.');
              if (pieces.second.startswith("pocl_"))
                baseName = pieces.first;
              else
                baseName = operand->getName();
              
              Value *alternative = NULL;

              unsigned int copy_i = 0;
              do {
                std::ostringstream alternativeName;
                alternativeName << baseName.str();
                if (copy_i > 0)
                  alternativeName << ".pocl_" << copy_i;

                alternative = 
                  F.getValueSymbolTable()->lookup(alternativeName.str());

                if (alternative != NULL)
                  {
                    ins->setOperand(opr, alternative);
                    if (dominatesUse(DT, *ins, opr))
                      break;
                  }
                     
                if (copy_i > 10000 && alternative == NULL)
                  break; /* ran out of possibilities */
                ++copy_i;
              } while (true);

              if (alternative != NULL) {
#ifdef DEBUG_REFERENCE_FIXING
                  std::cout << "### found the alternative:" << std::endl;
                  alternative->dump();
#endif                      
                  changed |= true;
                } else {
#ifdef DEBUG_REFERENCE_FIXING
                  std::cout << "### didn't find an alternative for" << std::endl;
                  operand->dump();
                  std::cerr << "### BB:" << std::endl;
                  operand->getParent()->dump();
                  std::cerr << "### the user BB:" << std::endl;
                  ins->getParent()->dump();
#endif
                  std::cerr << "Could not find a dominating alternative variable." << std::endl;
                  dumpCFG(F, "broken.dot");
                  abort();
              }
            }
        }
    }
  return changed;
}

/**
 * Moves the phi nodes in the beginning of the src to the beginning of
 * the dst. 
 *
 * MergeBlockIntoPredecessor function from llvm discards the phi nodes
 * of the replicated BB because it has only one entry.
 */
void
WorkitemHandler::movePhiNodes(llvm::BasicBlock* Src, llvm::BasicBlock* Dst) {
  while (PHINode *PN = dyn_cast<PHINode>(Src->begin()))
    PN->moveBefore(Dst->getFirstNonPHI());
}


} // namespace pocl
