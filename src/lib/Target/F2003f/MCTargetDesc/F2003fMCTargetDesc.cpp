//===-- F2003fMCTargetDesc.cpp - 2003f Target Descriptions ----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides 2003f specific target descriptions.
//
//===----------------------------------------------------------------------===//

#include "F2003fMCTargetDesc.h"
#include "InstPrinter/F2003fInstPrinter.h"
#include "F2003fMCAsmInfo.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

#define GET_INSTRINFO_MC_DESC
#include "F2003fGenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "F2003fGenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
#include "F2003fGenRegisterInfo.inc"

// static MCInstrInfo *createF2003fMCInstrInfo() {
//   MCInstrInfo *X = new MCInstrInfo();
//   InitF2003fMCInstrInfo(X);
//   return X;
// }

// static MCRegisterInfo *createF2003fMCRegisterInfo(const Triple &TT) {
//   MCRegisterInfo *X = new MCRegisterInfo();
//   InitF2003fMCRegisterInfo(X, F2003f::XX);
//   return X;
// }

// static MCSubtargetInfo *
// createF2003fMCSubtargetInfo(const Triple &TT, StringRef CPU, StringRef FS) {
//   return createF2003fMCSubtargetInfoImpl(TT, CPU, FS);
// }

static MCInstPrinter *createF2003fMCInstPrinter(const Triple &T,
                                                unsigned SyntaxVariant,
                                                const MCAsmInfo &MAI,
                                                const MCInstrInfo &MII,
                                                const MCRegisterInfo &MRI) {
  if (SyntaxVariant == 0)
    return new F2003fInstPrinter(MAI, MII, MRI);
  return nullptr;
}

extern "C" void LLVMInitializeF2003fTargetMC() {
  // Register the MC asm info.
  RegisterMCAsmInfo<F2003fMCAsmInfo> X(getTheF2003fTarget());

  // // Register the MC instruction info.
  // TargetRegistry::RegisterMCInstrInfo(getTheF2003fTarget(),
  //                                     createF2003fMCInstrInfo);

  // // Register the MC register info.
  // TargetRegistry::RegisterMCRegInfo(getTheF2003fTarget(),
  //                                   createF2003fMCRegisterInfo);

  // // Register the MC subtarget info.
  // TargetRegistry::RegisterMCSubtargetInfo(getTheF2003fTarget(),
  //                                         createF2003fMCSubtargetInfo);

  // Register the MCInstPrinter.
  TargetRegistry::RegisterMCInstPrinter(getTheF2003fTarget(),
                                        createF2003fMCInstPrinter);
}
