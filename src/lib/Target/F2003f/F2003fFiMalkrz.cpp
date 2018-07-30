//===-- F2003fFiMalkrz.cpp - 2003f FI MALKRZ Pass  ------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains a pass that converts FI_MALKRZ Pseudo Instruction to
// bundle of FI and MALKRZ
//
//===----------------------------------------------------------------------===//

#include "F2003f.h"
#include "F2003fInstrInfo.h"
#include "F2003fSubtarget.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/IR/DebugLoc.h"
#include "llvm/Support/Debug.h"

using namespace llvm;

#define DEBUG_TYPE "2003f-fi-malkrz"

namespace {

class F2003fFiMalkrz : public MachineFunctionPass {
public:
  static char ID;

  F2003fFiMalkrz() : MachineFunctionPass(ID) {}

  StringRef getPassName() const override { return "2003f Eliminate Pseudo Instr FI_MALKRZ"; }

  bool runOnMachineFunction(MachineFunction &F) override;

  MachineFunctionProperties getRequiredProperties() const override {
    return MachineFunctionProperties().set(
        MachineFunctionProperties::Property::NoVRegs);
  }
};

char F2003fFiMalkrz::ID = 0;

}

bool F2003fFiMalkrz::runOnMachineFunction(MachineFunction &F) {
  const F2003fInstrInfo &TII =
    *static_cast<const F2003fInstrInfo *>(F.getSubtarget().getInstrInfo());

  for (auto &BB : F) {
    for (MachineBasicBlock::iterator I = BB.begin(); I != BB.end(); ) {
      MachineInstr &MI = *I;
      DebugLoc DL = MI.getDebugLoc();
      unsigned Opc = MI.getOpcode();

      unsigned FiOpc = TII.getFiFromFiMalkrz(Opc);
      if (FiOpc != F2003f::INSTRUCTION_LIST_END) {
        DEBUG(dbgs() << "Expand: ");
        DEBUG(MI.dump());

        MIBundleBuilder Bundle(BB, I);

        unsigned MalkrzOpc = TII.getMalkrzFromFiMalkrz(Opc);
        unsigned MalkrzNumOperands = TII.get(MalkrzOpc).getNumOperands();
        MachineInstrBuilder Malkrz = BuildMI(F, DL, TII.get(MalkrzOpc));
        for (unsigned i = 0; i < MalkrzNumOperands; i++) {
          Malkrz.add(MI.getOperand(i));
        }
        Bundle.prepend(Malkrz);

        unsigned FiNumOperands = TII.get(FiOpc).getNumOperands();
        MachineInstrBuilder Fi = BuildMI(F, DL, TII.get(FiOpc));
        for (unsigned i = 0; i < FiNumOperands; i++) {
          Fi.add(MI.getOperand(i + MalkrzNumOperands));
        }
        Bundle.prepend(Fi);

        I = BB.erase(MI);
      } else {
       ++I;
      }
    }
  }
  return true;
}

FunctionPass *llvm::createF2003fFiMalkrzPass() { return new F2003fFiMalkrz(); }
