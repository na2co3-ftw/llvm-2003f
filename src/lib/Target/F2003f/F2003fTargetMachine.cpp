//===-- F2003fTargetMachine.cpp - Define TargetMachine for 2003f ----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Top-level implementation for the 2003f target.
//
//===----------------------------------------------------------------------===//

#include "F2003fTargetMachine.h"
#include "F2003f.h"
#include "MCTargetDesc/F2003fMCAsmStreamer.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/Support/TargetRegistry.h"
using namespace llvm;

extern "C" void LLVMInitializeF2003fTarget() {
  // Register the target.
  RegisterTargetMachine<F2003fTargetMachine> X(getTheF2003fTarget());
}

static Reloc::Model getEffectiveRelocModel(Optional<Reloc::Model> RM) {
  if (!RM.hasValue())
    return Reloc::Static;
  return *RM;
}

static std::string computeDataLayout(const Triple &TT, StringRef CPU,
                                     const TargetOptions &Options) {
  return "E-p:32:32-i32:32-n32";
}

F2003fTargetMachine::F2003fTargetMachine(const Target &T, const Triple &TT,
                                         StringRef CPU, StringRef FS,
                                         const TargetOptions &Options,
                                         Optional<Reloc::Model> RM,
                                         CodeModel::Model CM,
                                         CodeGenOpt::Level OL)
    : LLVMTargetMachine(T, computeDataLayout(TT, CPU, Options), TT, CPU, FS,
                        Options, getEffectiveRelocModel(RM), CM, OL),
      TLOF(make_unique<TargetLoweringObjectFileELF>()),
      Subtarget(TT, CPU, FS, *this) {
  initAsmInfo();
}

F2003fTargetMachine::~F2003fTargetMachine() {}

namespace {
/// F2003fPassConfig Code Generator Pass Configuration Options.
class F2003fPassConfig : public TargetPassConfig {
public:
  F2003fPassConfig(F2003fTargetMachine &TM, PassManagerBase &PM)
    : TargetPassConfig(TM, PM) {}

  F2003fTargetMachine &getF2003fTargetMachine() const {
    return getTM<F2003fTargetMachine>();
  }

  bool addInstSelector() override;
  void addPreEmitPass() override;
};
} // namespace

TargetPassConfig *F2003fTargetMachine::createPassConfig(PassManagerBase &PM) {
  return new F2003fPassConfig(*this, PM);
}

bool F2003fPassConfig::addInstSelector() {
  // Install an instruction selector.
  addPass(createF2003fISelDag(getF2003fTargetMachine(), getOptLevel()));
  return false;
}

// This pass may be implemented by targets that want to run passes
// immediately before machine code is emitted.
void F2003fPassConfig::addPreEmitPass() {
  addPass(createF2003fFiMalkrzPass());
}

MCStreamer *F2003fTargetMachine::createMCStreamerToAsm(MCContext &Context, std::unique_ptr<formatted_raw_ostream>FOut,
                                                    MCInstPrinter *InstPrinter, MCCodeEmitter *MCE, MCAsmBackend *MAB) {
  MCStreamer *S = new F2003fMCAsmStreamer(Context, std::move(FOut), Options.MCOptions.AsmVerbose,
                                          InstPrinter, Options.MCOptions.ShowMCInst);
  return S;
}