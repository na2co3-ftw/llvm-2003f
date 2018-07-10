//===-- F2003fTargetMachine.h - Define TargetMachine for 2003f --*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the 2003f specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_F2003F_F2003FTARGETMACHINE_H
#define LLVM_LIB_TARGET_F2003F_F2003FTARGETMACHINE_H

#include "F2003fSubtarget.h"
#include "llvm/Target/TargetFrameLowering.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {

class F2003fTargetMachine : public LLVMTargetMachine {
  std::unique_ptr<TargetLoweringObjectFile> TLOF;
  F2003fSubtarget Subtarget;

public:
  F2003fTargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                      StringRef FS, const TargetOptions &Options,
                      Optional<Reloc::Model> RM, CodeModel::Model CM,
                      CodeGenOpt::Level OL);
  ~F2003fTargetMachine() override;

  const F2003fSubtarget *getSubtargetImpl(const Function &F) const override {
    return &Subtarget;
  }
  TargetPassConfig *createPassConfig(PassManagerBase &PM) override;

  TargetLoweringObjectFile *getObjFileLowering() const override {
    return TLOF.get();
  }

  MCStreamer *createMCStreamerToAsm(MCContext &Context, std::unique_ptr<formatted_raw_ostream>FOut,
                                    MCInstPrinter *InstPrinter, MCCodeEmitter *MCE, MCAsmBackend *MAB) override;
};

}

#endif
