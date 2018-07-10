//===- F2003fMCAsmStreamer.h - 2003lk Text Assembly Output ------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the 2003lk specific subclass of MCStreamer instead of
// MCAsmStreamer.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_F2003F_MCTARGETDESC_F2003FMCASMSTREAMER_H
#define LLVM_LIB_TARGET_F2003F_MCTARGETDESC_F2003FMCASMSTREAMER_H

#include "llvm/MC/MCStreamer.h"

namespace llvm {

class F2003fMCAsmStreamer final : public MCStreamer {
  std::unique_ptr<formatted_raw_ostream> OSOwner;
  formatted_raw_ostream &OS;
  std::unique_ptr<MCInstPrinter> InstPrinter;

  SmallString<256> LabelToEmit;
  SmallString<128> ExplicitCommentToEmit;
  SmallString<128> CommentToEmit;
  raw_svector_ostream CommentStream;

  unsigned IsVerboseAsm : 1;
  unsigned ShowInst : 1;

public:
  F2003fMCAsmStreamer(MCContext &Ctx, std::unique_ptr<formatted_raw_ostream> os,
                      bool isVerboseAsm, MCInstPrinter *printer, bool showInst);

  bool isVerboseAsm() const override { return IsVerboseAsm; }
  bool hasRawTextSupport() const override { return true; }

  void AddComment(const Twine &T, bool EOL = true) override;
  raw_ostream &GetCommentOS() override;
  void emitRawComment(const Twine &T, bool TabPrefix = true) override;
  void addExplicitComment(const Twine &T) override;
  void emitExplicitComments() override;
  void AddBlankLine() override;
  void ChangeSection(MCSection *, const MCExpr *) override;

  void EmitLabel(MCSymbol *Symbol, SMLoc Loc = SMLoc()) override;
  void EmitThumbFunc(MCSymbol *Func) override;
  void EmitAssignment(MCSymbol *Symbol, const MCExpr *Value) override;
  void EmitWeakReference(MCSymbol *Alias, const MCSymbol *Symbol) override;
  bool EmitSymbolAttribute(MCSymbol *Symbol, MCSymbolAttr Attribute) override;
  void EmitSymbolDesc(MCSymbol *Symbol, unsigned DescValue) override;
  void EmitCommonSymbol(MCSymbol *Symbol, uint64_t Size, unsigned ByteAlignment) override;
  void EmitLocalCommonSymbol(MCSymbol *Symbol, uint64_t Size, unsigned ByteAlignment) override;
  void EmitZerofill(MCSection *Section, MCSymbol *Symbol = nullptr, uint64_t Size = 0, unsigned ByteAlignment = 0) override;
  void EmitTBSSSymbol(MCSection *Section, MCSymbol *Symbol, uint64_t Size, unsigned ByteAlignment = 0) override;

  void EmitBytes(StringRef Data) override;
  void EmitValueImpl(const MCExpr *Value, unsigned Size, SMLoc Loc = SMLoc()) override;
  void emitFill(const MCExpr &NumBytes, uint64_t FillValue, SMLoc Loc = SMLoc()) override;
  void emitFill(const MCExpr &NumValues, int64_t Size, int64_t Expr, SMLoc Loc = SMLoc()) override;
  void EmitValueToAlignment(unsigned ByteAlignment, int64_t Value = 0,
                            unsigned ValueSize = 1, unsigned MaxBytesToEmit = 0) override;
  void EmitCodeAlignment(unsigned ByteAlignment, unsigned MaxBytesToEmit = 0) override;
  void emitValueToOffset(const MCExpr *Offset, unsigned char Value, SMLoc Loc) override;

  void EmitInstruction(const MCInst &Inst, const MCSubtargetInfo &STI, bool PrintSchedInfo = false) override;
  void EmitBundleAlignMode(unsigned AlignPow2) override;
  void EmitBundleLock(bool AlignToEnd) override;
  void EmitBundleUnlock() override;

  void EmitRawTextImpl(StringRef String) override;

  void FinishImpl() override;

  inline void EmitEOL();
  void EmitCommentsAndEOL();

};

}

#endif
