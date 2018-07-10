//=== F2003fMachineFunctionInfo.h - 2003f machine function info --*- C++ -*-==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares 2003f-specific per-machine-function information.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_F2003F_F2003FMACHINEFUNCTIONINFO_H
#define LLVM_LIB_TARGET_F2003F_F2003FMACHINEFUNCTIONINFO_H

#include "llvm/CodeGen/MachineFunction.h"

namespace llvm {

/// F2003fMachineFunctionInfo - This class is derived from MachineFunction and
/// contains private F2003f target-specific information for each MachineFunction.
class F2003fMachineFunctionInfo : public MachineFunctionInfo {
  virtual void anchor();

public:
  explicit F2003fMachineFunctionInfo(MachineFunction &MF) {}
};

} // End llvm namespace

#endif
