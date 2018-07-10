//===-- F2003fTargetInfo.cpp - F2003f Target Implementation ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "F2003f.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/TargetRegistry.h"
using namespace llvm;

Target &llvm::getTheF2003fTarget() {
  static Target TheF2003fTarget;
  return TheF2003fTarget;
}

extern "C" void LLVMInitializeF2003fTargetInfo() {
  RegisterTarget<Triple::f2003f> X(getTheF2003fTarget(), "2003f",
                                   "2003'd ferlesyl");
}
