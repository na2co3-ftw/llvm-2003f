//===- F2003fMCAsmStreamer.cpp - 2003lk Text Assembly Output ----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides the 2003lk specific subclass of MCStreamer instead of
// MCAsmStreamer.
//
//===----------------------------------------------------------------------===//

#include "F2003fMCAsmStreamer.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/Twine.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstPrinter.h"
#include "llvm/MC/MCObjectFileInfo.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/Format.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/LEB128.h"
#include "llvm/Support/MathExtras.h"
#include <string>

using namespace llvm;

F2003fMCAsmStreamer::F2003fMCAsmStreamer(MCContext &Ctx, std::unique_ptr<formatted_raw_ostream> os,
                                         bool isVerboseAsm, MCInstPrinter *printer, bool showInst)
    : MCStreamer(Ctx), OSOwner(std::move(os)), OS(*OSOwner), MAI(Ctx.getAsmInfo()),
      InstPrinter(printer), CommentStream(CommentToEmit), IsVerboseAsm(isVerboseAsm), ShowInst(showInst) {
  assert(InstPrinter);
}

void F2003fMCAsmStreamer::reset() {
  LabelToEmit.clear();
  ExplicitCommentToEmit.clear();
  CommentToEmit.clear();
  Symbols.clear();
  CurrentAlignmentToValue = 1;
  ValueBuffer.clear();
  MCStreamer::reset();
}

void F2003fMCAsmStreamer::EmitEOL() {
  // Dump Explicit Comments here.
  emitExplicitComments();
  // If we don't have any comments, just emit a \n.
  if (!IsVerboseAsm) {
    OS << '\n';
    return;
  }
  EmitCommentsAndEOL();
}

void F2003fMCAsmStreamer::EmitCommentsAndEOL() {
  if (CommentToEmit.empty() && CommentStream.GetNumBytesInBuffer() == 0) {
    OS << '\n';
    return;
  }

  StringRef Comments = CommentToEmit;

  assert(Comments.back() == '\n' &&
         "Comment array not newline terminated");
  do {
    // Emit a line of comments.
    OS.PadToColumn(MAI->getCommentColumn());
    size_t Position = Comments.find('\n');
    OS << MAI->getCommentString() << ' ' << Comments.substr(0, Position) <<'\n';

    Comments = Comments.substr(Position+1);
  } while (!Comments.empty());

  CommentToEmit.clear();
}

void F2003fMCAsmStreamer::AddComment(const Twine &T, bool EOL) {
  if (!IsVerboseAsm) return;

  T.toVector(CommentToEmit);

  if (EOL)
    CommentToEmit.push_back('\n'); // Place comment in a new line.
}

raw_ostream &F2003fMCAsmStreamer::GetCommentOS() {
  if (!IsVerboseAsm)
    return nulls();  // Discard comments unless in verbose asm mode.
  return CommentStream;
}

void F2003fMCAsmStreamer::emitRawComment(const Twine &T, bool TabPrefix) {
  OS << MAI->getCommentString() << T;
  EmitEOL();
}

void F2003fMCAsmStreamer::addExplicitComment(const Twine &T) {
  StringRef c = T.getSingleStringRef();
  if (c.startswith(StringRef(MAI->getCommentString()))) {
    ExplicitCommentToEmit.append(c.str());
  } else {
    assert(false && "Unexpected Assembly Comment");
  }
  if (c.back() == '\n')
    emitExplicitComments();
}

void F2003fMCAsmStreamer::emitExplicitComments() {
  StringRef Comments = ExplicitCommentToEmit;
  if (!Comments.empty())
    OS << Comments;
  ExplicitCommentToEmit.clear();
}

void F2003fMCAsmStreamer::AddBlankLine() {
  EmitEOL();
}

void F2003fMCAsmStreamer::ChangeSection(MCSection *Section, const MCExpr *Subsection) {
  flushValues();
  // 2003lk does not support explicit section
}

void F2003fMCAsmStreamer::EmitLabel(MCSymbol *Symbol, SMLoc Loc) {
  assert(ValueBuffer.empty() && "Label is not aligned!");

  MCStreamer::EmitLabel(Symbol, Loc);
  visitUsedSymbol(*Symbol);

  std::string Name;
  {
    raw_string_ostream TempStream(Name);
    Symbol->print(TempStream, MAI);
  }

  LabelToEmit.append(" l' ");
  LabelToEmit.append(Name);
}

void F2003fMCAsmStreamer::flushLabel(bool addFen) {
  StringRef label = LabelToEmit;
  if (!label.empty()) {
    if (addFen) {
      OS << "fen";
    }
    OS << label;
  }
  LabelToEmit.clear();
}

void F2003fMCAsmStreamer::EmitInstruction(const MCInst &Inst, const MCSubtargetInfo &STI, bool PrintSchedInfo) {
  MCStreamer::EmitInstruction(Inst, STI);

  // Show the MCInst if enabled.
  if (ShowInst) {
    if (PrintSchedInfo)
      GetCommentOS() << "\n";
    Inst.dump_pretty(GetCommentOS(), InstPrinter.get(), "\n ");
    GetCommentOS() << "\n";
  }

  // drop initial tab
  std::string Buffer;
  {
    raw_string_ostream TempStream(Buffer);
    InstPrinter->printInst(&Inst, TempStream, "", STI);
  }
  OS << Buffer.substr(1);

  flushLabel(false);

  if (PrintSchedInfo) {
    std::string SI = STI.getSchedInfoStr(Inst);
    if (!SI.empty())
      GetCommentOS() << SI;
  }

  StringRef Comments = CommentToEmit;
  if (Comments.size() && Comments.back() != '\n')
    GetCommentOS() << "\n";

  EmitEOL();
}

void F2003fMCAsmStreamer::EmitRawTextImpl(StringRef String) {
  if (!String.empty() && String.back() == '\n')
    String = String.substr(0, String.size()-1);
  OS << String;
  EmitEOL();
}

void F2003fMCAsmStreamer::visitUsedSymbol(const MCSymbol &Symbol) {
  if (!Symbol.isRegistered()) {
    Symbol.setIsRegistered(true);
    Symbols.push_back(&Symbol);
  }
}

void F2003fMCAsmStreamer::FinishImpl() {
  flushValues();
  flushLabel(true);

  for (auto *Symbol : Symbols) {
    if (Symbol->isUndefined()) {
      OS << "xok ";
      Symbol->print(OS, MAI);
      EmitEOL();
    }
  }
}

bool F2003fMCAsmStreamer::EmitSymbolAttribute(MCSymbol *Symbol, MCSymbolAttr Attribute) {
  visitUsedSymbol(*Symbol);

  switch (Attribute) {
  default: return false;
  case MCSA_Global: // kue
    OS << MAI->getGlobalDirective();
    break;
  }

  Symbol->print(OS, MAI);
  EmitEOL();

  return true;
}

void F2003fMCAsmStreamer::EmitBytes(StringRef Data) {
  for (unsigned i = 0, e = Data.size(); i != e; ++i) {
    EmitValue(MCConstantExpr::create((unsigned char)Data[i], getContext()), 1);
  }
}

void F2003fMCAsmStreamer::EmitIntValue(uint64_t Value, unsigned Size) {
  EmitValue(MCConstantExpr::create(Value, getContext()), Size);
}

void F2003fMCAsmStreamer::EmitValueImpl(const MCExpr *Value, unsigned Size, SMLoc Loc) {
  assert(Size <= 8 && "Invalid size");
  assert(getCurrentSectionOnly() &&
    "Cannot emit contents before setting section!");

  if (CurrentAlignmentToValue == Size && ValueBuffer.empty()) {
    EmitValueWithAlignment(Value, Size, Loc);
    return;
  }

  int64_t IntValue;
  if (!Value->evaluateAsAbsolute(IntValue))
    report_fatal_error("Don't know how to emit this value.");

  unsigned i = Size;
  while (i != 0) {
    i--;
    ValueBuffer.push_back((unsigned char)(IntValue >> (i * 8)));

    if (ValueBuffer.size() == CurrentAlignmentToValue) {
      uint64_t ValueToEmit = 0;
      for (unsigned j = 0; j < CurrentAlignmentToValue; ++j) {
        ValueToEmit = (ValueToEmit << 8) | ValueBuffer[j];
      }
      EmitValueWithAlignment(MCConstantExpr::create(ValueToEmit, getContext()), CurrentAlignmentToValue, Loc);
      ValueBuffer.clear();
    }
  }
}

void F2003fMCAsmStreamer::EmitValueWithAlignment(const MCExpr *Value, unsigned Size, SMLoc Loc) {
  const char *Directive = nullptr;
  switch (Size) {
  default: break;
  case 1: Directive = MAI->getData8bitsDirective();  break;
  case 2: Directive = MAI->getData16bitsDirective(); break;
  case 4: Directive = MAI->getData32bitsDirective(); break;
  case 8: Directive = MAI->getData64bitsDirective(); break;
  }

  assert(Directive && "Invalid size for machine code value!");
  OS << Directive;
  Value->print(OS, MAI);
  flushLabel(false);
  EmitEOL();
}

void F2003fMCAsmStreamer::EmitValueToAlignment(unsigned ByteAlignment, int64_t Value,
  unsigned ValueSize, unsigned MaxBytesToEmit) {
  if (ByteAlignment == 1 || ByteAlignment == 2 || ByteAlignment == 4) {
    CurrentAlignmentToValue = ByteAlignment;
    return;
  }
  if (ByteAlignment % 4 == 0) {
    CurrentAlignmentToValue = 4;
    return;
  }
  CurrentAlignmentToValue = 1;
}

void F2003fMCAsmStreamer::flushValues() {
  if (!ValueBuffer.empty()) {
    uint64_t ValueToEmit = 0;
    for (unsigned i = 0, e = ValueBuffer.size(); i != e; ++i) {
      ValueToEmit = (ValueToEmit << 8) | ValueBuffer[i];
    }
    ValueToEmit <<= (CurrentAlignmentToValue - ValueBuffer.size()) * 8;

    EmitValueWithAlignment(MCConstantExpr::create(ValueToEmit, getContext()), CurrentAlignmentToValue);
    ValueBuffer.clear();
  }

  CurrentAlignmentToValue = 1;
}


void F2003fMCAsmStreamer::EmitThumbFunc(MCSymbol *Func) { // .thumb_func
  llvm_unreachable("2003lk doesn't support this directive");
}

void F2003fMCAsmStreamer::EmitAssignment(MCSymbol *Symbol, const MCExpr *Value) { // sybmol = value
  llvm_unreachable("2003lk doesn't support this directive");
}

void F2003fMCAsmStreamer::EmitWeakReference(MCSymbol *Alias, const MCSymbol *Symbol) { // .weakref
  llvm_unreachable("2003lk doesn't support this directive");
}

void F2003fMCAsmStreamer::EmitSymbolDesc(MCSymbol *Symbol, unsigned DescValue) { // .desc
  llvm_unreachable("2003lk doesn't support this directive");
}

void F2003fMCAsmStreamer::EmitCommonSymbol(MCSymbol *Symbol, uint64_t Size, unsigned ByteAlignment) { // .comm
  llvm_unreachable("2003lk doesn't support this directive");
}

void F2003fMCAsmStreamer::EmitLocalCommonSymbol(MCSymbol *Symbol, uint64_t Size, unsigned ByteAlignment) { // .lcomm
  llvm_unreachable("2003lk doesn't support this directive");
}

void F2003fMCAsmStreamer::EmitZerofill(MCSection *Section, MCSymbol *Symbol,
                                       uint64_t Size, unsigned ByteAlignment) { // .zerofill
  llvm_unreachable("2003lk doesn't support this directive");
}

void F2003fMCAsmStreamer::EmitTBSSSymbol(MCSection *Section, MCSymbol *Symbol, uint64_t Size,
                                         unsigned ByteAlignment) { // .tbss
  llvm_unreachable("2003lk doesn't support this directive");
}

void F2003fMCAsmStreamer::emitFill(const MCExpr &NumBytes, uint64_t FillValue, SMLoc Loc) { // .zero
  llvm_unreachable("2003lk doesn't support this directive");
}

void F2003fMCAsmStreamer::emitFill(const MCExpr &NumValues, int64_t Size, int64_t Expr, SMLoc Loc) { // .fill
  llvm_unreachable("2003lk doesn't support this directive");
}

void F2003fMCAsmStreamer::EmitCodeAlignment(unsigned ByteAlignment, unsigned MaxBytesToEmit) {
  llvm_unreachable("2003lk doesn't support this directive");
}

void F2003fMCAsmStreamer::emitValueToOffset(const MCExpr *Offset, unsigned char Value, SMLoc Loc) { // .org
  llvm_unreachable("2003lk doesn't support this directive");
}

void F2003fMCAsmStreamer::EmitBundleAlignMode(unsigned AlignPow2) { // .bundle_align_mode
  llvm_unreachable("2003lk doesn't support this directive");
}

void F2003fMCAsmStreamer::EmitBundleLock(bool AlignToEnd) { // .bundle_lock
  llvm_unreachable("2003lk doesn't support this directive");
}

void F2003fMCAsmStreamer::EmitBundleUnlock() { // .bundle_unlock
  llvm_unreachable("2003lk doesn't support this directive");
}

