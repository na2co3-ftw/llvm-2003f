//==-- F2003f.h - Top-level interface for 2003f representation ---*- C++ -*-==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the entry points for global functions defined in
// the LLVM 2003f backend.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_F2003F_F2003F_H
#define LLVM_LIB_TARGET_F2003F_F2003F_H

#include "MCTargetDesc/F2003fMCTargetDesc.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
  class F2003fTargetMachine;
  class FunctionPass;
  class formatted_raw_ostream;

  FunctionPass *createF2003fISelDag(F2003fTargetMachine &TM,
                                    CodeGenOpt::Level OptLevel);

} // end namespace llvm;

#endif
