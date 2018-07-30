//==-- F2003f.h - Top-level interface for 2003f representation ---*- C++ -*-==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the entry points for global functions defined in
// the LLVM 2003f backend.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_F2003F_F2003F_H
#define LLVM_LIB_TARGET_F2003F_F2003F_H

#include "MCTargetDesc/F2003fMCTargetDesc.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {

namespace F2003fCC {
  enum CondCode {
    LLONYS  = 0, // > (unsigned)
    XTLONYS = 1, // <= (unsigned)
    XOLONYS = 2, // >= (unsigned)
    XYLONYS = 3, // < (unsigned)
    CLO  = 6,  // ==
    NIV  = 7,  // !=
    LLO  = 8,  // >
    XTLO = 9,  // <=
    XOLO = 10, // >=
    XYLO = 11  // <
  };
}

inline static const char *F2003fCCToString(F2003fCC::CondCode CC) {
  switch (CC) {
  case F2003fCC::LLONYS:  return "llonys";
  case F2003fCC::XTLONYS: return "xtlonys";
  case F2003fCC::XOLONYS: return "xolonys";
  case F2003fCC::XYLONYS: return "xylonys";
  case F2003fCC::CLO:  return "clo";
  case F2003fCC::NIV:  return "niv";
  case F2003fCC::LLO:  return "llo";
  case F2003fCC::XTLO: return "xtlo";
  case F2003fCC::XOLO: return "xolo";
  case F2003fCC::XYLO: return "xylo";
  }
  llvm_unreachable("Unknown condition code");
}

inline F2003fCC::CondCode OppositeF2003fCC(F2003fCC::CondCode CC) {
  return static_cast<F2003fCC::CondCode>(CC ^ 1);
}

class F2003fTargetMachine;
class FunctionPass;
class formatted_raw_ostream;

FunctionPass *createF2003fISelDag(F2003fTargetMachine &TM,
                                  CodeGenOpt::Level OptLevel);

FunctionPass *createF2003fFiMalkrzPass();

} // end namespace llvm;

#endif
