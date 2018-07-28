//===-- F2003fInstPrinter.cpp - Convert 2003f MCInst to assembly syntax ---===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This class prints an 2003f MCInst to a 2003lk.
//
//===----------------------------------------------------------------------===//

#include "F2003fInstPrinter.h"
#include "F2003f.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FormattedStream.h"
using namespace llvm;

#define DEBUG_TYPE "asm-printer"


// Include the auto-generated portion of the assembly writer.
#include "F2003fGenAsmWriter.inc"

void F2003fInstPrinter::printRegName(raw_ostream &OS, unsigned RegNo) const {
  OS << getRegisterName(RegNo);
}

void F2003fInstPrinter::printInst(const MCInst *MI, raw_ostream &O,
                                  StringRef Annot, const MCSubtargetInfo &STI) {
  printInstruction(MI, O);
  printAnnotation(O, Annot);
}

void F2003fInstPrinter::printOperand(const MCInst *MI, unsigned OpNo,
                                     raw_ostream &O) {
  const MCOperand &Op = MI->getOperand(OpNo);
  if (Op.isReg()) {
    printRegName(O, Op.getReg());
  } else if (Op.isImm()) {
    O << (uint32_t)Op.getImm();
  } else {
    assert(Op.isExpr() && "unknown operand kind in printOperand");
    Op.getExpr()->print(O, &MAI);
  }
}

void F2003fInstPrinter::printMem(const MCInst *MI, unsigned OpNo, raw_ostream &O) {
  const MCOperand &base = MI->getOperand(OpNo);
  const MCOperand &dispReg = MI->getOperand(OpNo+1);
  const MCOperand &dispImm = MI->getOperand(OpNo+2);

  assert(base.getReg() && "Expected register");
  printOperand(MI, OpNo, O);

  if (dispReg.getReg()) {
    O << '+';
    printOperand(MI, OpNo+1, O);
  }

  if (dispImm.isImm()) {
    if (dispImm.getImm()) {
      O << '+';
      printOperand(MI, OpNo+2, O);
    }
  } else {
    O << '+';
    assert(dispImm.isExpr() && "Expected immediate");
    dispImm.getExpr()->print(O, &MAI);
  }

  O << '@';
}

void F2003fInstPrinter::printCC(const MCInst *MI, unsigned OpNo, raw_ostream &O) {
  F2003fCC::CondCode CC = (F2003fCC::CondCode)MI->getOperand(OpNo).getImm();
  O << F2003fCCToString(CC);
}
