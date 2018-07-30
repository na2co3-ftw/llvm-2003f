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

unsigned F2003fInstrInfo::getFiFromFiMalkrz(unsigned Opc) const {
  switch (Opc) {
  default: return F2003f::INSTRUCTION_LIST_END;
  case F2003f::FIrr_MALKRZrr: case F2003f::FIrr_MALKRZrm: case F2003f::FIrr_MALKRZri:
  case F2003f::FIrr_MALKRZx: return F2003f::FIrr;

  case F2003f::FIrm_MALKRZrr: case F2003f::FIrm_MALKRZrm: case F2003f::FIrm_MALKRZri:
  case F2003f::FIrm_MALKRZx: return F2003f::FIrm;

  case F2003f::FIri_MALKRZrr: case F2003f::FIri_MALKRZrm: case F2003f::FIri_MALKRZri:
  case F2003f::FIri_MALKRZx: return F2003f::FIri;

  case F2003f::FImr_MALKRZrr: case F2003f::FImr_MALKRZrm: case F2003f::FImr_MALKRZri:
  case F2003f::FImr_MALKRZx: return F2003f::FImr;

  case F2003f::FImm_MALKRZrr: case F2003f::FImm_MALKRZrm: case F2003f::FImm_MALKRZri:
  case F2003f::FImm_MALKRZx: return F2003f::FImm;

  case F2003f::FImi_MALKRZrr: case F2003f::FImi_MALKRZrm: case F2003f::FImi_MALKRZri:
  case F2003f::FImi_MALKRZx: return F2003f::FImi;

  case F2003f::FIir_MALKRZrr: case F2003f::FIir_MALKRZrm: case F2003f::FIir_MALKRZri:
  case F2003f::FIir_MALKRZx: return F2003f::FIir;

  case F2003f::FIim_MALKRZrr: case F2003f::FIim_MALKRZrm: case F2003f::FIim_MALKRZri:
  case F2003f::FIim_MALKRZx: return F2003f::FIim;

  case F2003f::FIii_MALKRZrr: case F2003f::FIii_MALKRZrm: case F2003f::FIii_MALKRZri:
  case F2003f::FIii_MALKRZx: return F2003f::FIii;

  }
}

unsigned F2003fInstrInfo::getMalkrzFromFiMalkrz(unsigned Opc) const {
  switch (Opc) {
  default: return F2003f::INSTRUCTION_LIST_END;
  case F2003f::FIrr_MALKRZrr: case F2003f::FIrm_MALKRZrr: case F2003f::FIri_MALKRZrr:
  case F2003f::FImr_MALKRZrr: case F2003f::FImm_MALKRZrr: case F2003f::FImi_MALKRZrr:
  case F2003f::FIir_MALKRZrr: case F2003f::FIim_MALKRZrr: case F2003f::FIii_MALKRZrr:
  return F2003f::MALKRZrr;

  case F2003f::FIrr_MALKRZrm: case F2003f::FIrm_MALKRZrm: case F2003f::FIri_MALKRZrm:
  case F2003f::FImr_MALKRZrm: case F2003f::FImm_MALKRZrm: case F2003f::FImi_MALKRZrm:
  case F2003f::FIir_MALKRZrm: case F2003f::FIim_MALKRZrm: case F2003f::FIii_MALKRZrm:
  return F2003f::MALKRZrm;

  case F2003f::FIrr_MALKRZri: case F2003f::FIrm_MALKRZri: case F2003f::FIri_MALKRZri:
  case F2003f::FImr_MALKRZri: case F2003f::FImm_MALKRZri: case F2003f::FImi_MALKRZri:
  case F2003f::FIir_MALKRZri: case F2003f::FIim_MALKRZri: case F2003f::FIii_MALKRZri:
  return F2003f::MALKRZri;

  case F2003f::FIrr_MALKRZx: case F2003f::FIrm_MALKRZx: case F2003f::FIri_MALKRZx:
  case F2003f::FImr_MALKRZx: case F2003f::FImm_MALKRZx: case F2003f::FImi_MALKRZx:
  case F2003f::FIir_MALKRZx: case F2003f::FIim_MALKRZx: case F2003f::FIii_MALKRZx:
  return F2003f::MALKRZx;
  }
}

bool F2003fInstrInfo::
reverseBranchCondition(SmallVectorImpl<MachineOperand> &Cond) const {
  assert(!Cond.empty() && "Invalid 2003f branch condition!");
  F2003fCC::CondCode CC = static_cast<F2003fCC::CondCode>(Cond.back().getImm());
  Cond.back() = MachineOperand::CreateImm(OppositeF2003fCC(CC));
  return false;
}

bool F2003fInstrInfo::isUnpredicatedTerminator(const MachineInstr &MI) const {
  if (!MI.isTerminator())
    return false;

  // Conditional branch is a special case.
  if (MI.isBranch() && !MI.isBarrier())
    return true;
  if (!MI.isPredicable())
    return true;
  return !isPredicated(MI);
}

bool F2003fInstrInfo::analyzeBranch(MachineBasicBlock &MBB,
                                    MachineBasicBlock *&TBB,
                                    MachineBasicBlock *&FBB,
                                    SmallVectorImpl<MachineOperand> &Cond,
                                    bool AllowModify) const {
  // Start from the bottom of the block and work up, examining the
  // terminator instructions.
  MachineBasicBlock::iterator I = MBB.end();
  while (I != MBB.begin()) {
    --I;
    if (I->isDebugValue())
      continue;

    // Working from the bottom, when we see a non-terminator
    // instruction, we're done.
    if (!isUnpredicatedTerminator(*I))
      break;

    // A terminator that isn't a branch can't easily be handled
    // by this analysis.
    if (!I->isBranch())
      return true;

    // Cannot handle indirect branches.
    if (I->isIndirectBranch())
      return true;

    // Handle unconditional branches.
    if (I->getOpcode() == F2003f::KRZx) {
      if (!AllowModify) {
        TBB = I->getOperand(0).getMBB();
        continue;
      }

      // If the block has any instructions after a JMP, delete them.
      while (std::next(I) != MBB.end())
        std::next(I)->eraseFromParent();

      Cond.clear();
      FBB = nullptr;

      // Delete the JMP if it's equivalent to a fall-through.
      if (MBB.isLayoutSuccessor(I->getOperand(0).getMBB())) {
        TBB = nullptr;
        I->eraseFromParent();
        I = MBB.end();
        continue;
      }

      // TBB is used to indicate the unconditinal destination.
      TBB = I->getOperand(0).getMBB();
      continue;
    }

    // Handle conditional branches.
    assert(getMalkrzFromFiMalkrz(I->getOpcode()) == F2003f::MALKRZx && "Invalid conditional branch");

    // Working from the bottom, handle the first conditional branch.
    if (Cond.empty()) {
      FBB = TBB;
      TBB = I->getOperand(0).getMBB();
      Cond.push_back(MachineOperand::CreateImm(I->getOpcode()));
      for (unsigned i = 1; i < I->getNumOperands(); i++) {
        Cond.push_back(I->getOperand(i));
      }
      continue;
    }

    return true;
  }

  return false;
}

unsigned F2003fInstrInfo::insertBranch(MachineBasicBlock &MBB,
                                       MachineBasicBlock *TBB,
                                       MachineBasicBlock *FBB,
                                       ArrayRef<MachineOperand> Cond,
                                       const DebugLoc &DL,
                                       int *BytesAdded) const {
  // Shouldn't be a fall through.
  assert(TBB && "insertBranch must not be told to insert a fallthrough");
  assert((Cond.empty() ||
    (getMalkrzFromFiMalkrz(Cond[0].getImm()) == F2003f::MALKRZx && get(Cond[0].getImm()).getNumOperands() == Cond.size())) &&
         "Invalid 2003f branch condition!");
  assert(!BytesAdded && "code size not handled");

  if (Cond.empty()) {
    // Unconditional branch?
    assert(!FBB && "Unconditional branch with multiple successors!");
    BuildMI(&MBB, DL, get(F2003f::KRZx)).addMBB(TBB);
    return 1;
  }

  // Conditional branch.
  unsigned Count = 0;
  MachineInstrBuilder Malkrz = BuildMI(&MBB, DL, get(Cond[0].getImm())).addMBB(TBB);
  for (unsigned i = 1; i < Cond.size(); i++) {
    Malkrz.add(Cond[i]);
  }
  ++Count;

  if (FBB) {
    // Two-way Conditional branch. Insert the second branch.
    BuildMI(&MBB, DL, get(F2003f::KRZx)).addMBB(FBB);
    ++Count;
  }
  return Count;
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
    if (!I->isBranch())
      break;
    // Remove the branch.
    I->eraseFromParent();
    I = MBB.end();
    ++Count;
  }

  return Count;
}
