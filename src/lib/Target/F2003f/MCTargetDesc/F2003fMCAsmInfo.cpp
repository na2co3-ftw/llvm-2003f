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
#include "llvm/ADT/StringExtras.h"
#include "llvm/MC/MCStreamer.h"
using namespace llvm;

void F2003fMCAsmInfo::anchor() { }

F2003fMCAsmInfo::F2003fMCAsmInfo(const Triple &TT) {
  IsLittleEndian = false;
  SeparatorString = " ";
  CommentString = ";";
  // LabelSuffix = "l'";
  PrivateGlobalPrefix = "";
  PrivateLabelPrefix = "";
  LinkerPrivateGlobalPrefix = "";
  InlineAsmStart = "";
  InlineAsmEnd = "";
  SupportsQuotedNames = true;
  // ZeroDirective = "\t.zero\t";
  // AsciiDirective = "\t.ascii\t";
  // AscizDirective = "\t.asciz\t";
  // Data8bitsDirective = "\t.byte\t";
  // Data16bitsDirective = "\t.short\t";
  // Data32bitsDirective = "\t.long\t";
  // Data64bitsDirective = "\t.quad\t";
  GlobalDirective = "kue ";
  HasFunctionAlignment = false;
  HasDotTypeDotSizeDirective = false;
  HasSingleParameterDotFile = false;
  // WeakDirective = "\t.weak\t";
}

static bool isAcceptableChar(char C) {
   return (C >= 'a' && C <= 'z') || C == 'F' || C == 'R' || C == 'V' || C == 'X' ||
          (C >= '0' && C <= '9') || C == '\'' || C == '_' || C == '-';
}

bool F2003fMCAsmInfo::isValidUnquotedName(StringRef Name) const {
  if (Name.empty())
    return false;
  if (Name.size() == 2) {
    if (Name[0] == 'f' && Name[1] >= '0' && Name[1] <= '7')
      return false;
    if (Name.equals("xx"))
      return false;
  }
  if (Name[0] == '\'') {
    return false;
  }

  bool hasNonNumericChar = false;

  for (char C : Name) {
    if (!hasNonNumericChar && (C <= '0' || C >= '9')) {
      hasNonNumericChar = true;
    }
    if (!isAcceptableChar(C))
      return false;
  }

  return hasNonNumericChar;
}

void F2003fMCAsmInfo::printQuotedName(raw_ostream &OS, StringRef Name) const {
  OS << '\'';
  for (char C : Name) {
    if (C == '\'') {
      OS << "''";
    } else if (isAcceptableChar(C)) {
      OS << C;
    } else if (C >= 'A' && C <= 'Z') {
      OS << '\'' << (char)(C - 'A' + 'a');
    } else {
      OS << "'x";
      OS << hexdigit((C >> 4 & 0xF), true);
      OS << hexdigit((C >> 0) & 0xF, true);
    }
  }
}
