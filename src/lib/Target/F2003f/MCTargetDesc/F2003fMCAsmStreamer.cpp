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
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstPrinter.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/Format.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/LEB128.h"
#include "llvm/Support/MathExtras.h"
#include <string>

using namespace llvm;

F2003fMCAsmStreamer::F2003fMCAsmStreamer(MCContext &Ctx, std::unique_ptr<formatted_raw_ostream> os,
                                         bool isVerboseAsm, MCInstPrinter *printer, bool showInst)
    : MCStreamer(Ctx), OSOwner(std::move(os)), OS(*OSOwner),
      InstPrinter(printer), CommentStream(CommentToEmit), IsVerboseAsm(isVerboseAsm), ShowInst(showInst) {
  assert(InstPrinter);
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
    OS.PadToColumn(40);
    size_t Position = Comments.find('\n');
    OS << "; " << Comments.substr(0, Position) <<'\n';

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
  OS << ';' << T;
  EmitEOL();
}

void F2003fMCAsmStreamer::addExplicitComment(const Twine &T) {
  StringRef c = T.getSingleStringRef();
  if (c.startswith(StringRef(";"))) {
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

void F2003fMCAsmStreamer::ChangeSection(MCSection *Section, const MCExpr *) {}

static bool isValidUnquotedName(StringRef Name) {
  return true;
}

void F2003fMCAsmStreamer::EmitLabel(MCSymbol *Symbol, SMLoc Loc) {
  MCStreamer::EmitLabel(Symbol, Loc);

  StringRef Name = Symbol->getName();
  if (!isValidUnquotedName(Name)) {
    report_fatal_error("Symbol name with unsupported characters");
  }

  LabelToEmit.append(" l' ");
  LabelToEmit.append(Name.str());
}

void F2003fMCAsmStreamer::EmitInstruction(const MCInst &Inst, const MCSubtargetInfo &STI, bool PrintSchedInfo) {
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

  StringRef label = LabelToEmit;
  if (!label.empty())
    OS << label;
  LabelToEmit.clear();

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

void F2003fMCAsmStreamer::FinishImpl() {
  StringRef label = LabelToEmit;
  if (!label.empty()) {
    OS << "fen" << label;
    EmitEOL();
  }
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

// .type .globl .hidden .indirect_symbol .internal .lazy_reference .local .no_dead_strip .symbol_resolver
// .alt_entry .private_extern .protected .reference .weak .weak_definition .weak_def_can_be_hidden
bool F2003fMCAsmStreamer::EmitSymbolAttribute(MCSymbol *Symbol, MCSymbolAttr Attribute) {
  return false;
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

void F2003fMCAsmStreamer::EmitBytes(StringRef Data) { // .byte
  llvm_unreachable("2003lk doesn't support this directive");
}

void F2003fMCAsmStreamer::EmitValueImpl(const MCExpr *Value, unsigned Size, SMLoc Loc) { // .byte .short .long .quad
  llvm_unreachable("2003lk doesn't support this directive");
}

void F2003fMCAsmStreamer::emitFill(const MCExpr &NumBytes, uint64_t FillValue, SMLoc Loc) { // .zero
  llvm_unreachable("2003lk doesn't support this directive");
}

void F2003fMCAsmStreamer::emitFill(const MCExpr &NumValues, int64_t Size, int64_t Expr, SMLoc Loc) { // .fill
  llvm_unreachable("2003lk doesn't support this directive");
}

void F2003fMCAsmStreamer::EmitValueToAlignment(unsigned ByteAlignment, int64_t Value,
                                               unsigned ValueSize, unsigned MaxBytesToEmit) { // .p2align .p2alignw .p2alignl
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

