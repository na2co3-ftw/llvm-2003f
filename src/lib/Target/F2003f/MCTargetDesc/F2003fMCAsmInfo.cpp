//===-- F2003fMCAsmInfo.cpp - 2003f asm properties ------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declarations of the F2003fMCAsmInfo properties.
//
//===----------------------------------------------------------------------===//

#include "F2003fMCAsmInfo.h"
using namespace llvm;

void F2003fMCAsmInfo::anchor() { }

F2003fMCAsmInfo::F2003fMCAsmInfo(const Triple &TT) {
  IsLittleEndian = false;
  // MaxInstLength = 4;
  HasFunctionAlignment = false;
}
