//===-- F2003fMCAsmInfo.h - 2003f asm properties ---------------*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declaration of the F2003fMCAsmInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_F2003F_MCTARGETDESC_F2003FMCASMINFO_H
#define LLVM_LIB_TARGET_F2003F_MCTARGETDESC_F2003FMCASMINFO_H

#include "llvm/MC/MCAsmInfoELF.h"

namespace llvm {
class Triple;

class F2003fMCAsmInfo : public MCAsmInfoELF {
  void anchor() override;

public:
  explicit F2003fMCAsmInfo(const Triple &TT);
};

} // namespace llvm

#endif
