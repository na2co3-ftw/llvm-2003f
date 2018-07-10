//===-- F2003fMCTargetDesc.h - 2003f Target Descriptions --------*- C++ -*-===//
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

#ifndef LLVM_LIB_TARGET_F2003F_MCTARGETDESC_F2003FMCTARGETDESC_H
#define LLVM_LIB_TARGET_F2003F_MCTARGETDESC_F2003FMCTARGETDESC_H

#include "llvm/Support/DataTypes.h"

namespace llvm {
class Target;

Target &getTheF2003fTarget();

} // End llvm namespace

// Defines symbolic names for 2003lk registers.
// This defines a mapping from register name to register number.
#define GET_REGINFO_ENUM
#include "F2003fGenRegisterInfo.inc"

// Defines symbolic names for the 2003lk instructions.
#define GET_INSTRINFO_ENUM
#include "F2003fGenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "F2003fGenSubtargetInfo.inc"

#endif
