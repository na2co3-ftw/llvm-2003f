//==- F2003fFrameLowering.h - Define frame lowering for 2003f ---*- C++ -*--==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the 2003f implementation of TargetFrameLowering class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_F2003F_F2003FFRAMELOWERING_H
#define LLVM_LIB_TARGET_F2003F_F2003FFRAMELOWERING_H

#include "F2003f.h"
#include "llvm/Target/TargetFrameLowering.h"

namespace llvm {
class F2003fFrameLowering : public TargetFrameLowering {
protected:

public:
  explicit F2003fFrameLowering()
      : TargetFrameLowering(TargetFrameLowering::StackGrowsDown, 4, 0) {}

  /// emitProlog/emitEpilog - These methods insert prolog and epilog code into
  /// the function.
  void emitPrologue(MachineFunction &MF, MachineBasicBlock &MBB) const override;
  void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const override;

  MachineBasicBlock::iterator eliminateCallFramePseudoInstr(
    MachineFunction &MF, MachineBasicBlock &MBB,
    MachineBasicBlock::iterator I) const override;

  bool canSimplifyCallFramePseudos(const MachineFunction &MF) const override;

  bool hasFP(const MachineFunction &MF) const override;
  bool hasReservedCallFrame(const MachineFunction &MF) const override;
};

} // End llvm namespace

#endif
