//===-- F2003fSubtarget.cpp - 2003f Subtarget Information -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the 2003f specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#include "F2003fSubtarget.h"
#include "F2003f.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

#define DEBUG_TYPE "2003f-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "F2003fGenSubtargetInfo.inc"

void F2003fSubtarget::anchor() { }

F2003fSubtarget &
F2003fSubtarget::initializeSubtargetDependencies(StringRef CPU, StringRef FS) {
  std::string CPUName = CPU;
  if (CPUName.empty())
    CPUName = "2003f";

  ParseSubtargetFeatures(CPUName, FS);

  return *this;
}

F2003fSubtarget::F2003fSubtarget(const Triple &TT, const std::string &CPU,
                                 const std::string &FS, const TargetMachine &TM)
    : F2003fGenSubtargetInfo(TT, CPU, FS), FrameLowering(),
      InstrInfo(initializeSubtargetDependencies(CPU, FS)), TLInfo(TM, *this) {}
