//===-- F2003fFrameLowering.cpp - 2003f Frame Information -----------------===//
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

#include "F2003fFrameLowering.h"
#include "F2003fInstrInfo.h"
#include "F2003fMachineFunctionInfo.h"
#include "F2003fSubtarget.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Function.h"
#include "llvm/Target/TargetOptions.h"

using namespace llvm;

bool F2003fFrameLowering::hasFP(const MachineFunction &MF) const {
  return false;
}

bool F2003fFrameLowering::hasReservedCallFrame(const MachineFunction &MF) const {
  return false;
}

void F2003fFrameLowering::emitPrologue(MachineFunction &MF,
                                       MachineBasicBlock &MBB) const {
  assert(&MF.front() == &MBB && "Shrink-wrapping not yet supported");
  MachineFrameInfo &MFI = MF.getFrameInfo();
  const F2003fInstrInfo &TII =
      *static_cast<const F2003fInstrInfo *>(MF.getSubtarget().getInstrInfo());

  MachineBasicBlock::iterator MBBI = MBB.begin();
  DebugLoc DL = MBBI != MBB.end() ? MBBI->getDebugLoc() : DebugLoc();

  // Get the number of bytes to allocate from the FrameInfo.
  uint64_t StackSize = MFI.getStackSize();

  if (StackSize) {
    BuildMI(MBB, MBBI, DL, TII.get(F2003f::NTAri), F2003f::F5)
    .addImm(StackSize);
  }
}

void F2003fFrameLowering::emitEpilogue(MachineFunction &MF,
                                       MachineBasicBlock &MBB) const {
  const MachineFrameInfo &MFI = MF.getFrameInfo();
  const F2003fInstrInfo &TII =
      *static_cast<const F2003fInstrInfo *>(MF.getSubtarget().getInstrInfo());

  MachineBasicBlock::iterator MBBI = MBB.getLastNonDebugInstr();
  DebugLoc DL = MBBI->getDebugLoc();

  // Get the number of bytes to allocate from the FrameInfo
  uint64_t StackSize = MFI.getStackSize();

  if (StackSize) {
    BuildMI(MBB, MBBI, DL, TII.get(F2003f::ADAri), F2003f::F5)
    .addImm(StackSize);
  }
}