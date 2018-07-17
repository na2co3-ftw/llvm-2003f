//===-- F2003fInstrInfo.cpp - 2003f Instruction Information ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the 2003f implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#include "F2003fInstrInfo.h"
#include "F2003f.h"
#include "F2003fMachineFunctionInfo.h"
#include "F2003fTargetMachine.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

#define GET_INSTRINFO_CTOR_DTOR
#include "F2003fGenInstrInfo.inc"

// Pin the vtable to this file.
void F2003fInstrInfo::anchor() {}

void F2003fInstrInfo::copyPhysReg(MachineBasicBlock &MBB,
                                  MachineBasicBlock::iterator I,
                                  const DebugLoc &DL, unsigned DestReg,
                                  unsigned SrcReg, bool KillSrc) const {
  BuildMI(MBB, I, DL, get(F2003f::KRZrr), DestReg)
    .addReg(SrcReg, getKillRegState(KillSrc));
}

F2003fInstrInfo::F2003fInstrInfo(F2003fSubtarget &STI)
  : F2003fGenInstrInfo(F2003f::ADJCALLSTACKDOWN, F2003f::ADJCALLSTACKUP),
    RI() {}

void F2003fInstrInfo::storeRegToStackSlot(MachineBasicBlock &MBB,
                                          MachineBasicBlock::iterator MI,
                                    unsigned SrcReg, bool isKill, int FrameIdx,
                                          const TargetRegisterClass *RC,
                                          const TargetRegisterInfo *TRI) const {
  DebugLoc DL;
  if (MI != MBB.end()) DL = MI->getDebugLoc();
  MachineFunction &MF = *MBB.getParent();
  MachineFrameInfo &MFI = MF.getFrameInfo();

  MachineMemOperand *MMO = MF.getMachineMemOperand(
      MachinePointerInfo::getFixedStack(MF, FrameIdx),
      MachineMemOperand::MOStore, MFI.getObjectSize(FrameIdx),
      MFI.getObjectAlignment(FrameIdx));

  BuildMI(MBB, MI, DL, get(F2003f::KRZmr))
    .addFrameIndex(FrameIdx).addReg(0).addImm(0)
    .addReg(SrcReg, getKillRegState(isKill)).addMemOperand(MMO);
}

void F2003fInstrInfo::loadRegFromStackSlot(MachineBasicBlock &MBB,
                                           MachineBasicBlock::iterator MI,
                                           unsigned DestReg, int FrameIdx,
                                           const TargetRegisterClass *RC,
                                           const TargetRegisterInfo *TRI) const{
  DebugLoc DL;
  if (MI != MBB.end()) DL = MI->getDebugLoc();
  MachineFunction &MF = *MBB.getParent();
  MachineFrameInfo &MFI = MF.getFrameInfo();

  MachineMemOperand *MMO = MF.getMachineMemOperand(
      MachinePointerInfo::getFixedStack(MF, FrameIdx),
      MachineMemOperand::MOLoad, MFI.getObjectSize(FrameIdx),
      MFI.getObjectAlignment(FrameIdx));

  BuildMI(MBB, MI, DL, get(F2003f::KRZrm))
    .addReg(DestReg, getDefRegState(true))
    .addFrameIndex(FrameIdx).addReg(0).addImm(0).addMemOperand(MMO);
}

void F2003fInstrInfo::getNoop(MCInst &NopInst) const {
  NopInst.setOpcode(F2003f::FEN);
}

bool F2003fInstrInfo::analyzeBranch(MachineBasicBlock &MBB,
                                    MachineBasicBlock *&TBB,
                                    MachineBasicBlock *&FBB,
                                    SmallVectorImpl<MachineOperand> &Cond,
                                    bool AllowModify) const {
  return true;
}

unsigned F2003fInstrInfo::insertBranch(MachineBasicBlock &MBB,
                                       MachineBasicBlock *TBB,
                                       MachineBasicBlock *FBB,
                                       ArrayRef<MachineOperand> Cond,
                                       const DebugLoc &DL,
                                       int *BytesAdded) const {
  // Shouldn't be a fall through.
  assert(TBB && "insertBranch must not be told to insert a fallthrough");
  // assert((Cond.size() == 1 || Cond.size() == 0) &&
  //        "F2003f branch conditions have one component!");
  assert(!BytesAdded && "code size not handled");

  if (Cond.empty()) {
    // Unconditional branch?
    assert(!FBB && "Unconditional branch with multiple successors!");
    BuildMI(&MBB, DL, get(F2003f::KRZri)).addReg(F2003f::XX).addMBB(TBB);
    return 1;
  }

  // Conditional branch.
  llvm_unreachable("Conditional brunch is unimplemented");
  return 0;
}

unsigned F2003fInstrInfo::removeBranch(MachineBasicBlock &MBB,
                                       int *BytesRemoved) const {
  assert(!BytesRemoved && "code size not handled");

  MachineBasicBlock::iterator I = MBB.end();
  unsigned Count = 0;

  while (I != MBB.begin()) {
    --I;
    if (I->isDebugValue())
      continue;
    if ((I->getOpcode() != F2003f::KRZrr &&
         I->getOpcode() != F2003f::KRZri &&
         I->getOpcode() != F2003f::KRZrm) ||
        I->getOperand(0).getReg() != F2003f::XX)
      break;
    // Remove the branch.
    I->eraseFromParent();
    I = MBB.end();
    ++Count;
  }

  return Count;
}
