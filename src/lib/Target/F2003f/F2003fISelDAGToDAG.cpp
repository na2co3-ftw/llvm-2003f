//===-- F2003fISelDAGToDAG.cpp - A dag to dag inst selector for 2003f -----===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines an instruction selector for the 2003f target.
//
//===----------------------------------------------------------------------===//

#include "F2003f.h"
#include "F2003fTargetMachine.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/IR/CallingConv.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetLowering.h"
using namespace llvm;

#define DEBUG_TYPE "2003f-isel"

//===----------------------------------------------------------------------===//
//                      Pattern Matcher Implementation
//===----------------------------------------------------------------------===//

/// F2003fDAGToDAGISel - 2003f specific code to select 2003f machine
/// instructions for SelectionDAG operations.
///
namespace {
  class F2003fDAGToDAGISel : public SelectionDAGISel {
  public:
    F2003fDAGToDAGISel(F2003fTargetMachine &TM, CodeGenOpt::Level OptLevel)
        : SelectionDAGISel(TM, OptLevel) {}

    StringRef getPassName() const override {
      return "2003f DAG->DAG Pattern Instruction Selection";
    }

    // Include the pieces autogenerated from the target description.
  #include "F2003fGenDAGISel.inc"

  private:
    void Select(SDNode *N) override;
    bool SelectAddr(SDValue N, SDValue &Base, SDValue &DispReg, SDValue &DispImm);
  };
}  // end anonymous namespace

/// createF2003fISelDag - This pass converts a legalized DAG into a
/// 2003f-specific DAG, ready for instruction scheduling.
///
FunctionPass *llvm::createF2003fISelDag(F2003fTargetMachine &TM,
                                        CodeGenOpt::Level OptLevel) {
  return new F2003fDAGToDAGISel(TM, OptLevel);
}

void F2003fDAGToDAGISel::Select(SDNode *Node) {
  // Select the default instruction
  SelectCode(Node);
}

/// SelectAddr - returns true if it is able pattern match an addressing mode.
/// It returns the operands which make up the maximal addressing mode it can
/// match by reference.
bool F2003fDAGToDAGISel::SelectAddr(SDValue N,
                                    SDValue &Base, SDValue &DispReg, SDValue &DispImm) {
  EVT VT = N.getValueType();

  if (FrameIndexSDNode *FIN = dyn_cast<FrameIndexSDNode>(N)) {
    Base = CurDAG->getTargetFrameIndex(FIN->getIndex(), VT);
    DispReg = CurDAG->getRegister(0, VT);
    DispImm = CurDAG->getTargetConstant(0, SDLoc(N), VT);
    return true;
  }

  llvm_unreachable("Unknown pattern");
  return true;
}