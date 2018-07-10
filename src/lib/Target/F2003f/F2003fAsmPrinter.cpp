//===-- F2003fAsmPrinter.cpp - 2003f LLVM assembly writer -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains a printer that converts from our internal representation
// of machine-dependent LLVM code to the 2003lk assembly language.
//
//===----------------------------------------------------------------------===//

#include "InstPrinter/F2003fInstPrinter.h"
#include "F2003f.h"
// #include "F2003fInstrInfo.h"
#include "F2003fMCInstLower.h"
#include "F2003fTargetMachine.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Mangler.h"
#include "llvm/IR/Module.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/ErrorHandling.h"
using namespace llvm;

#define DEBUG_TYPE "asm-printer"

namespace {
  class F2003fAsmPrinter : public AsmPrinter {
  public:
    F2003fAsmPrinter(TargetMachine &TM, std::unique_ptr<MCStreamer> Streamer)
        : AsmPrinter(TM, std::move(Streamer)) {}

    StringRef getPassName() const override { return "2003f Assembly Printer"; }

    void EmitInstruction(const MachineInstr *MI) override;

    void EmitStartOfAsmFile(Module &) override;
  };
} // end of anonymous namespace

//===----------------------------------------------------------------------===//
void F2003fAsmPrinter::EmitInstruction(const MachineInstr *MI) {
  F2003fMCInstLower MCInstLowering(OutContext, *this);

  MCInst TmpInst;
  MCInstLowering.Lower(MI, TmpInst);
  EmitToStreamer(*OutStreamer, TmpInst);
}

void F2003fAsmPrinter::EmitStartOfAsmFile(Module &module) {
  OutStreamer->EmitRawText("'c'i\n");
}

// Force static initialization.
extern "C" void LLVMInitializeF2003fAsmPrinter() {
  RegisterAsmPrinter<F2003fAsmPrinter> X(getTheF2003fTarget());
}
