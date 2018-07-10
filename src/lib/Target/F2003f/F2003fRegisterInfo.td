//===-- F2003fRegisterInfo.td - 2003f Register defs --------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  Declarations that describe the 2003f register file
//===----------------------------------------------------------------------===//

// Registers are identified with 3-bit ID numbers.
class F2003fReg<bits<16> Enc, string n> : Register<n> {
  let HWEncoding = Enc;
  let Namespace = "F2003f";
}

// Integer registers
def F0 : F2003fReg<0, "f0">, DwarfRegNum<[0]>;
def F1 : F2003fReg<1, "f1">, DwarfRegNum<[1]>;
def F2 : F2003fReg<2, "f2">, DwarfRegNum<[2]>;
def F3 : F2003fReg<3, "f3">, DwarfRegNum<[3]>;
def F5 : F2003fReg<5, "f5">, DwarfRegNum<[5]>;
def XX : F2003fReg<7, "xx">, DwarfRegNum<[7]>;

// Register classes.
def Reg : RegisterClass<"F2003f", [i32], 32, (add F0, F1, F2, F3, F5, XX)>;
