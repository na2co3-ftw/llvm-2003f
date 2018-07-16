//===-- F2003fInstrInfo.td - 2003f Instruction defs --------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file describes the 2003f instructions in TableGen format.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// 2003f specific DAG Nodes.
//

// Type profiles.
def SDT_F2003fCallSeqStart : SDCallSeqStart<[ SDTCisVT<0, i32>, SDTCisVT<1, i32> ]>;
def SDT_F2003fCallSeqEnd : SDCallSeqEnd<[ SDTCisVT<0, i32>, SDTCisVT<1, i32> ]>;
def SDT_F2003fCall : SDTypeProfile<0, 1, [SDTCisVT<0, iPTR>]>;

// Node definitions.
def F2003fcallseq_start : SDNode<"ISD::CALLSEQ_START", SDT_F2003fCallSeqStart,
                              [SDNPHasChain, SDNPSideEffect, SDNPOutGlue]>;
def F2003fcallseq_end   : SDNode<"ISD::CALLSEQ_END",   SDT_F2003fCallSeqEnd,
                              [SDNPHasChain, SDNPSideEffect,
                               SDNPOptInGlue, SDNPOutGlue]>;
def F2003fFenxeo : SDNode<"F2003fISD::FENXEO", SDT_F2003fCall,
                          [SDNPHasChain, SDNPOutGlue, SDNPOptInGlue, SDNPVariadic]>;
def F2003fDosnud : SDNode<"F2003fISD::DOSNUD", SDTNone,
                          [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;

//===----------------------------------------------------------------------===//
//  2003f specific transformation functions and pattern fragments.
//

def xnor  : PatFrag<(ops node:$a, node:$b), (not (xor node:$a, node:$b))>;

//===----------------------------------------------------------------------===//
// Operand Definitions.
//

def mem : Operand<iPTR> {
  let PrintMethod = "printMem";
  let MIOperandInfo = (ops Reg, Reg, i32imm);
  // let ParserMatchClass = parserMatchClass;
  // let OperandType = "OPERAND_MEMORY";
  // let EncoderMethod = "getMemEncoding";
}

def addr : ComplexPattern<iPTR, 3, "SelectAddr", [], []>;

//===----------------------------------------------------------------------===//
// Instructions
//===----------------------------------------------------------------------===//

// Instruction Formats.
class F2003fInst<dag outs, dag ins, string asmstr, list<dag> pattern> : Instruction {
  field bits<8> Inst;

  let Namespace = "F2003f";
  let OutOperandList = outs;
  let InOperandList = ins;
  let AsmString = asmstr;
  let Pattern = pattern;
}

class F2003fPseudo<dag outs, dag ins, list<dag> pattern>
  : F2003fInst<outs, ins, "", pattern> {
  let isCodeGenOnly = 1;
  let isPseudo = 1;
}

class BinOpR<dag ins, string mnemonic, list<dag> pattern>
  : F2003fInst<(outs Reg:$dst), ins, !strconcat(mnemonic, " $src1 $src2"), pattern> {
  let Constraints = "$src1 = $dst";
}

class BinOpM<dag ins, string mnemonic, list<dag> pattern>
  : F2003fInst<(outs), ins, !strconcat(mnemonic, " $dst $src"), pattern>;

multiclass BinOp<string mnemonic, SDPatternOperator opnode, bit CommutableRR> {
  let isCommutable = CommutableRR in
  def rr : BinOpR<(ins Reg:$src1, Reg:$src2), mnemonic,
                   [(set Reg:$dst, (opnode Reg:$src1, Reg:$src2))]>;

  def rm : BinOpR<(ins Reg:$src1, mem:$src2), mnemonic,
                   [(set Reg:$dst, (opnode Reg:$src1, (load addr:$src2)))]>;

  def ri : BinOpR<(ins Reg:$src1, i32imm:$src2), mnemonic,
                   [(set Reg:$dst, (opnode Reg:$src1, imm:$src2))]>;


  def mr : BinOpM<(ins mem:$dst, Reg:$src), mnemonic,
                   [(store (opnode (load addr:$dst), Reg:$src), addr:$dst)]>;

  def mm : BinOpM<(ins mem:$dst, mem:$src), mnemonic,
                   [(store (opnode (i32 (load addr:$dst)), (load addr:$src)), addr:$dst)]>;

  def mi : BinOpM<(ins mem:$dst, i32imm:$src), mnemonic,
                   [(store (opnode (i32 (load addr:$dst)), imm:$src), addr:$dst)]>;
}

multiclass MonoOp<string mnemonic, SDPatternOperator opnode> {
  let Constraints = "$src = $dst" in
  def r : F2003fInst<(outs Reg:$dst), (ins Reg:$src), !strconcat(mnemonic, " $dst"),
                     [(set Reg:$dst, (opnode Reg:$src))]>;

  def m : F2003fInst<(outs), (ins mem:$dst), !strconcat(mnemonic, " $dst"),
                     [(store (opnode (i32 (load addr:$dst))), addr:$dst)]>;
}

// Instructions.
// let hasSideEffects = 0 in
def KRZrr : F2003fInst<(outs Reg:$dst), (ins Reg:$src), "krz $dst $src", []>;
// let canFoldAsLoad = 1, isReMaterializable = 1 in
def KRZrm : F2003fInst<(outs Reg:$dst), (ins mem:$src), "krz $dst $src",
                       [(set Reg:$dst, (load addr:$src))]>;
// let isReMaterializable = 1, isAsCheapAsAMove = 1 in
def KRZri : F2003fInst<(outs Reg:$dst), (ins i32imm:$src), "krz $dst $src",
                       [(set Reg:$dst, imm:$src)]>; // relocImm?

def KRZmr : F2003fInst<(outs), (ins mem:$dst, Reg:$src), "krz $dst $src",
                       [(store Reg:$src, addr:$dst)]>;
def KRZmm : F2003fInst<(outs), (ins mem:$dst, mem:$src), "krz $dst $src",
                       [(store (i32 (load addr:$src)), addr:$dst)]>;
def KRZmi : F2003fInst<(outs), (ins mem:$dst, i32imm:$src), "krz $dst $src",
                       [(store (i32 imm:$src), addr:$dst)]>;

//def MALKRZ : Instruction;
//def INJ : Instruction;

let hasSideEffects = 0 in
def FEN : F2003fInst<(outs), (ins), "fen", []>;

defm NAC : MonoOp<"nac", not>;

defm ATA    : BinOp<"ata", add, 1>;
defm NTA    : BinOp<"nta", sub, 0>;
defm ADA    : BinOp<"ada", and, 1>;
defm EKC    : BinOp<"ekc", or, 1>;
defm DAL    : BinOp<"dal", xnor, 1>;
defm DTO    : BinOp<"dto", srl, 0>;
defm DRO    : BinOp<"dro", shl, 0>;
defm DTOSNA : BinOp<"dtosna", sra, 0>;

//def LAT : Instruction;
//def LATSNA : Instruction;

//def FI : Instruction;

// F5 is marked as a use to prevent stack-pointer assignments that appear
// immediately before calls from potentially appearing dead.
let isCall = 1, Uses = [F5] in {
  def INJ_FENXEOi : F2003fInst<(outs), (ins i32imm:$func), "inj f5@ xx $func",
                               [(F2003fFenxeo imm:$func)]>;
  def INJ_FENXEOr : F2003fInst<(outs), (ins Reg:$func), "inj f5@ xx $func",
                               [(F2003fFenxeo Reg:$func)]>;
  def INJ_FENXEOm : F2003fInst<(outs), (ins mem:$func), "inj f5@ xx $func",
                               [(F2003fFenxeo (load addr:$func))]>;
}

let isReturn = 1, isTerminator = 1, isBarrier = 1 in {
  def KRZ_DOSNUD : F2003fInst<(outs), (ins), "krz xx f5@",
                              [(F2003fDosnud)]>;
}

// ADJCALLSTACKDOWN/UP implicitly use/def SP because they may be expanded into
// a stack adjustment and the codegen must know that they may modify the stack
// pointer before prolog-epilog rewriting occurs.
// Pessimistically assume ADJCALLSTACKDOWN / ADJCALLSTACKUP will become
// sub / add which can clobber SR.
let Defs = [F5], Uses = [F5] in {
def ADJCALLSTACKDOWN : F2003fPseudo<(outs), (ins i32imm:$amt1, i32imm:$amt2),
                                    [(F2003fcallseq_start timm:$amt1, timm:$amt2)]>;
def ADJCALLSTACKUP : F2003fPseudo<(outs), (ins i32imm:$amt1, i32imm:$amt2),
                                   [(F2003fcallseq_end timm:$amt1, timm:$amt2)]>;
}
