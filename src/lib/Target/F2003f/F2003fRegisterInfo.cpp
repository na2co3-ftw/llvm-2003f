//===-- F2003fRegisterInfo.cpp - 2003f Register Information --------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the 2003f implementation of the TargetRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#include "F2003fRegisterInfo.h"
#include "F2003f.h"
#include "F2003fMachineFunctionInfo.h"
#include "F2003fTargetMachine.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"

using namespace llvm;

#define DEBUG_TYPE "2003f-reg-info"

#define GET_REGINFO_TARGET_DESC
#include "F2003fGenRegisterInfo.inc"

F2003fRegisterInfo::F2003fRegisterInfo()
  : F2003fGenRegisterInfo(F2003f::XX, 0, 0, F2003f::XX) {} // ?

const MCPhysReg*
F2003fRegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
  return CSR_NoRegs_SaveList;
}

const uint32_t *
F2003fRegisterInfo::getCallPreservedMask(const MachineFunction &MF,
                                          CallingConv::ID CC) const {
  return CSR_NoRegs_RegMask;
}

BitVector F2003fRegisterInfo::getReservedRegs(const MachineFunction &MF) const {
  BitVector Reserved(getNumRegs());
  Reserved.set(F2003f::F5);
  Reserved.set(F2003f::XX);
  return Reserved;
}

const TargetRegisterClass *
F2003fRegisterInfo::getPointerRegClass(const MachineFunction &MF, unsigned Kind)
                                                                         const {
  return &F2003f::RegRegClass;
}

void
F2003fRegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II,
                                        int SPAdj, unsigned FIOperandNum,
                                        RegScavenger *RS) const {
  assert(SPAdj == 0 && "Unexpected");

  MachineInstr &MI = *II;
  MachineBasicBlock &MBB = *MI.getParent();
  MachineFunction &MF = *MBB.getParent();
  int FrameIndex = MI.getOperand(FIOperandNum).getIndex();

  int Offset = MF.getFrameInfo().getObjectOffset(FrameIndex);
  Offset += MF.getFrameInfo().getStackSize();

  // Fold imm into offset
  if (MI.getOperand(FIOperandNum + 2).isImm()) {
    Offset += (int32_t)MI.getOperand(FIOperandNum + 2).getImm();
  }

  MI.getOperand(FIOperandNum).ChangeToRegister(F2003f::F5, false);
  MI.getOperand(FIOperandNum + 2).ChangeToImmediate(Offset);
}

unsigned F2003fRegisterInfo::getFrameRegister(const MachineFunction &MF) const {
  return F2003f::F5;
}
