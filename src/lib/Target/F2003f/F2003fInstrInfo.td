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

def SDT_F2003fSelectCC : SDTypeProfile<1, 5, [
  SDTCisSameAs<0, 1>, SDTCisSameAs<1, 2>, SDTCisSameAs<3, 4>,
  SDTCisVT<5, i8>
]>;

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

def F2003fselectcc : SDNode<"F2003fISD::SELECT_CC" , SDT_F2003fSelectCC>;
// def F2003fbrcc     : SDNode<"F2003fISD::BR_CC"     , SDTBrCC, [SDNPHasChain]>;

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

def cc : Operand<i8> {
  let PrintMethod = "printCC";
}

//===----------------------------------------------------------------------===//
// Instructions
//===----------------------------------------------------------------------===//

// Instruction Formats.
class I<dag outs, dag ins, string asmstr, list<dag> pattern> : Instruction {
  field bits<8> Inst;

  let Namespace = "F2003f";
  let OutOperandList = outs;
  let InOperandList = ins;
  let AsmString = asmstr;
  let Pattern = pattern;
}

class PseudoI<dag outs, dag ins, list<dag> pattern>
  : I<outs, ins, "", pattern> {
  let isCodeGenOnly = 1;
  let isPseudo = 1;
}

// Move Instructions.
let hasSideEffects = 0 in
def KRZrr : I<(outs Reg:$dst), (ins Reg:$src), "krz $dst $src", []>;

let canFoldAsLoad = 1, isReMaterializable = 1 in
def KRZrm : I<(outs Reg:$dst), (ins mem:$src), "krz $dst $src",
              [(set Reg:$dst, (load addr:$src))]>;

let isReMaterializable = 1, isAsCheapAsAMove = 1 in
def KRZri : I<(outs Reg:$dst), (ins i32imm:$src), "krz $dst $src",
              [(set Reg:$dst, imm:$src)]>;

def KRZmr : I<(outs), (ins mem:$dst, Reg:$src), "krz $dst $src",
              [(store Reg:$src, addr:$dst)]>;
def KRZmm : I<(outs), (ins mem:$dst, mem:$src), "krz $dst $src",
              [(store (i32 (load addr:$src)), addr:$dst)]>;
def KRZmi : I<(outs), (ins mem:$dst, i32imm:$src), "krz $dst $src",
              [(store (i32 imm:$src), addr:$dst)]>;

let hasSideEffects = 0 in {
  let Constraints = "$src1 = $dst" in {
    def MALKRZrr : I<(outs Reg:$dst), (ins Reg:$src1, Reg:$src2), "malkrz $dst $src2", []>;
    def MALKRZrm : I<(outs Reg:$dst), (ins Reg:$src1, mem:$src2), "malkrz $dst $src2", []>;
    def MALKRZri : I<(outs Reg:$dst), (ins Reg:$src1, i32imm:$src2), "malkrz $dst $src2", []>;
  }
  def MALKRZmr : I<(outs), (ins mem:$dst, Reg:$src), "malkrz $dst $src", []>;
  def MALKRZmm : I<(outs), (ins mem:$dst, mem:$src), "malkrz $dst $src", []>;
  def MALKRZmi : I<(outs), (ins mem:$dst, i32imm:$src), "malkrz $dst $src", []>;
}

//def INJ : Instruction;

let hasSideEffects = 0 in
def FEN : I<(outs), (ins), "fen", []>;

// Unary Arithmetic Instructions.
multiclass MonoOp<string mnemonic, SDPatternOperator opnode> {
  let Constraints = "$src = $dst" in
  def r : I<(outs Reg:$dst), (ins Reg:$src), !strconcat(mnemonic, " $dst"),
            [(set Reg:$dst, (opnode Reg:$src))]>;

  def m : I<(outs), (ins mem:$dst), !strconcat(mnemonic, " $dst"),
            [(store (opnode (i32 (load addr:$dst))), addr:$dst)]>;
}

defm NAC : MonoOp<"nac", not>;

// Binary Arithmetic Instructions.
multiclass BinOp<string mnemonic, SDPatternOperator opnode, bit CommutableRR> {
  let Constraints = "$src1 = $dst" in {
    let isCommutable = CommutableRR in
    def rr : I<(outs Reg:$dst), (ins Reg:$src1, Reg:$src2), !strconcat(mnemonic, " $src1 $src2"),
               [(set Reg:$dst, (opnode Reg:$src1, Reg:$src2))]>;

    def rm : I<(outs Reg:$dst), (ins Reg:$src1, mem:$src2), !strconcat(mnemonic, " $src1 $src2"),
               [(set Reg:$dst, (opnode Reg:$src1, (load addr:$src2)))]>;

    def ri : I<(outs Reg:$dst), (ins Reg:$src1, i32imm:$src2), !strconcat(mnemonic, " $src1 $src2"),
               [(set Reg:$dst, (opnode Reg:$src1, imm:$src2))]>;
  }


  def mr : I<(outs), (ins mem:$dst, Reg:$src), !strconcat(mnemonic, " $dst $src"),
             [(store (opnode (load addr:$dst), Reg:$src), addr:$dst)]>;

  def mm : I<(outs), (ins mem:$dst, mem:$src), !strconcat(mnemonic, " $dst $src"),
             [(store (opnode (i32 (load addr:$dst)), (load addr:$src)), addr:$dst)]>;

  def mi : I<(outs), (ins mem:$dst, i32imm:$src), !strconcat(mnemonic, " $dst $src"),
             [(store (opnode (i32 (load addr:$dst)), imm:$src), addr:$dst)]>;
}

defm ATA    : BinOp<"ata", add, 1>;
defm NTA    : BinOp<"nta", sub, 0>;
defm ADA    : BinOp<"ada", and, 1>;
defm EKC    : BinOp<"ekc", or, 1>;
defm DAL    : BinOp<"dal", xnor, 1>;
defm DTO    : BinOp<"dto", srl, 0>;
defm DRO    : BinOp<"dro", shl, 0>;
defm DTOSNA : BinOp<"dtosna", sra, 0>;

// Multiplication Instructions.
multiclass MulOp<string mnemonic, SDPatternOperator OpNode, SDPatternOperator OpNodeLoHi> {
  let Constraints = "$src1 = $dst" in {
    let isCommutable = 1 in
    def _LOrr : I<(outs Reg:$dst), (ins Reg:$src1, Reg:$src2), !strconcat(mnemonic, " $src1 $src1 $src2"),
                  [(set Reg:$dst, (OpNode Reg:$src1, Reg:$src2))]>;

    def _LOrm : I<(outs Reg:$dst), (ins Reg:$src1, mem:$src2), !strconcat(mnemonic, " $src1 $src1 $src2"),
                  [(set Reg:$dst, (OpNode Reg:$src1, (load addr:$src2)))]>;

    def _LOri : I<(outs Reg:$dst), (ins Reg:$src1, i32imm:$src2), !strconcat(mnemonic, " $src1 $src1 $src2"),
                  [(set Reg:$dst, (OpNode Reg:$src1, imm:$src2))]>;
  }

  def _LOmr : I<(outs), (ins mem:$dst, Reg:$src), !strconcat(mnemonic, " $dst $dst $src"),
                [(store (OpNode (load addr:$dst), Reg:$src), addr:$dst)]>;

  def _LOmm : I<(outs), (ins mem:$dst, mem:$src), !strconcat(mnemonic, " $dst $dst $src"),
                [(store (OpNode (i32 (load addr:$dst)), (load addr:$src)), addr:$dst)]>;

  def _LOmi : I<(outs), (ins mem:$dst, i32imm:$src), !strconcat(mnemonic, " $dst $dst $src"),
                [(store (OpNode (i32 (load addr:$dst)), imm:$src), addr:$dst)]>;


  let Constraints = "$src1 = $dstl" in {
    let isCommutable = 1 in
    def rrr : I<(outs Reg:$dstl, Reg:$dsth), (ins Reg:$src1, Reg:$src2), !strconcat(mnemonic, " $dstl $dsth $src2"),
                [(set Reg:$dstl, Reg:$dsth, (OpNodeLoHi Reg:$src1, Reg:$src2))]>;

    def rrm : I<(outs Reg:$dstl, Reg:$dsth), (ins Reg:$src1, mem:$src2), !strconcat(mnemonic, " $dstl $dsth $src2"),
                [(set Reg:$dstl, Reg:$dsth, (OpNodeLoHi Reg:$src1, (load addr:$src2)))]>;

    def rri : I<(outs Reg:$dstl, Reg:$dsth), (ins Reg:$src1, i32imm:$src2), !strconcat(mnemonic, " $dstl $dsth $src2"),
                [(set Reg:$dstl, Reg:$dsth, (OpNodeLoHi Reg:$src1, imm:$src2))]>;

    let mayStore = 1, hasSideEffects = 0 in {
      def rmr : I<(outs Reg:$dstl), (ins Reg:$src1, mem:$dsth, Reg:$src2), !strconcat(mnemonic, " $dstl $dsth $src2"), []>;

      let mayLoad = 1 in
      def rmm : I<(outs Reg:$dstl), (ins Reg:$src1, mem:$dsth, mem:$src2), !strconcat(mnemonic, " $dstl $dsth $src2"), []>;

      def rmi : I<(outs Reg:$dstl), (ins Reg:$src1, mem:$dsth, i32imm:$src2), !strconcat(mnemonic, " $dstl $dsth $src2"), []>;
    }
  }

  let mayLoad = 1, mayStore = 1, hasSideEffects = 0 in {
    def mrr : I<(outs Reg:$dsth), (ins mem:$dstl, Reg:$src2), !strconcat(mnemonic, " $dstl $dsth $src2"), []>;
    def mrm : I<(outs Reg:$dsth), (ins mem:$dstl, mem:$src2), !strconcat(mnemonic, " $dstl $dsth $src2"), []>;
    def mri : I<(outs Reg:$dsth), (ins mem:$dstl, i32imm:$src2), !strconcat(mnemonic, " $dstl $dsth $src2"), []>;

    def mmr : I<(outs), (ins mem:$dstl, mem:$dsth, Reg:$src2), !strconcat(mnemonic, " $dstl $dsth $src2"), []>;
    def mmm : I<(outs), (ins mem:$dstl, mem:$dsth, mem:$src2), !strconcat(mnemonic, " $dstl $dsth $src2"), []>;
    def mmi : I<(outs), (ins mem:$dstl, mem:$dsth, i32imm:$src2), !strconcat(mnemonic, " $dstl $dsth $src2"), []>;
  }
}

defm LAT    : MulOp<"lat", mul, umullohi>;
defm LATSNA : MulOp<"latsna", null_frag, smullohi>;

// Comparison Instructions.
let hasSideEffects = 0 in {
  def FIrr : I<(outs), (ins Reg:$lhs,    Reg:$rhs,    cc:$cc), "fi $lhs $rhs $cc", []>;
  def FIrm : I<(outs), (ins Reg:$lhs,    mem:$rhs,    cc:$cc), "fi $lhs $rhs $cc", []>;
  def FIri : I<(outs), (ins Reg:$lhs,    i32imm:$rhs, cc:$cc), "fi $lhs $rhs $cc", []>;
  def FImr : I<(outs), (ins mem:$lhs,    Reg:$rhs,    cc:$cc), "fi $lhs $rhs $cc", []>;
  def FImm : I<(outs), (ins mem:$lhs,    mem:$rhs,    cc:$cc), "fi $lhs $rhs $cc", []>;
  def FImi : I<(outs), (ins mem:$lhs,    i32imm:$rhs, cc:$cc), "fi $lhs $rhs $cc", []>;
  def FIir : I<(outs), (ins i32imm:$lhs, Reg:$rhs,    cc:$cc), "fi $lhs $rhs $cc", []>;
  def FIim : I<(outs), (ins i32imm:$lhs, mem:$rhs,    cc:$cc), "fi $lhs $rhs $cc", []>;
  def FIii : I<(outs), (ins i32imm:$lhs, i32imm:$rhs, cc:$cc), "fi $lhs $rhs $cc", []>;
}

let Constraints = "$src1 = $dst" in {
  def FIrr_MALKRZrr : PseudoI<(outs Reg:$dst), (ins Reg:$src1, Reg:$src2, Reg:$lhs, Reg:$rhs, cc:$cc),
                              [(set Reg:$dst, (F2003fselectcc Reg:$src1, Reg:$src2, Reg:$lhs, Reg:$rhs, timm:$cc))]>;
  def FIrm_MALKRZrr : PseudoI<(outs Reg:$dst), (ins Reg:$src1, Reg:$src2, Reg:$lhs, mem:$rhs, cc:$cc),
                              [(set Reg:$dst, (F2003fselectcc Reg:$src1, Reg:$src2, Reg:$lhs, (load addr:$rhs), timm:$cc))]>;
  def FIri_MALKRZrr : PseudoI<(outs Reg:$dst), (ins Reg:$src1, Reg:$src2, Reg:$lhs, i32imm:$rhs, cc:$cc),
                              [(set Reg:$dst, (F2003fselectcc Reg:$src1, Reg:$src2, Reg:$lhs, imm:$rhs, timm:$cc))]>;

  def FImr_MALKRZrr : PseudoI<(outs Reg:$dst), (ins Reg:$src1, Reg:$src2, mem:$lhs, Reg:$rhs, cc:$cc),
                              [(set Reg:$dst, (F2003fselectcc Reg:$src1, Reg:$src2, (load addr:$lhs), Reg:$rhs, timm:$cc))]>;
  def FImm_MALKRZrr : PseudoI<(outs Reg:$dst), (ins Reg:$src1, Reg:$src2, mem:$lhs, mem:$rhs, cc:$cc),
                              [(set Reg:$dst, (F2003fselectcc Reg:$src1, Reg:$src2, (load addr:$lhs), (i32 (load addr:$rhs)), timm:$cc))]>;
  def FImi_MALKRZrr : PseudoI<(outs Reg:$dst), (ins Reg:$src1, Reg:$src2, mem:$lhs, i32imm:$rhs, cc:$cc),
                              [(set Reg:$dst, (F2003fselectcc Reg:$src1, Reg:$src2, (load addr:$lhs), imm:$rhs, timm:$cc))]>;

  def FIir_MALKRZrr : PseudoI<(outs Reg:$dst), (ins Reg:$src1, Reg:$src2, i32imm:$lhs, Reg:$rhs, cc:$cc),
                              [(set Reg:$dst, (F2003fselectcc Reg:$src1, Reg:$src2, imm:$lhs, Reg:$rhs, timm:$cc))]>;
  def FIim_MALKRZrr : PseudoI<(outs Reg:$dst), (ins Reg:$src1, Reg:$src2, i32imm:$lhs, mem:$rhs, cc:$cc),
                              [(set Reg:$dst, (F2003fselectcc Reg:$src1, Reg:$src2, imm:$lhs, (load addr:$rhs), timm:$cc))]>;
  def FIii_MALKRZrr : PseudoI<(outs Reg:$dst), (ins Reg:$src1, Reg:$src2, i32imm:$lhs, i32imm:$rhs, cc:$cc),
                              [(set Reg:$dst, (F2003fselectcc Reg:$src1, Reg:$src2, imm:$lhs, imm:$rhs, timm:$cc))]>;

  def FIrr_MALKRZrm : PseudoI<(outs Reg:$dst), (ins Reg:$src1, mem:$src2, Reg:$lhs, Reg:$rhs, cc:$cc),
                              [(set Reg:$dst, (F2003fselectcc Reg:$src1, (load addr:$src2), Reg:$lhs, Reg:$rhs, timm:$cc))]>;
  def FIrm_MALKRZrm : PseudoI<(outs Reg:$dst), (ins Reg:$src1, mem:$src2, Reg:$lhs, mem:$rhs, cc:$cc),
                              [(set Reg:$dst, (F2003fselectcc Reg:$src1, (load addr:$src2), Reg:$lhs, (load addr:$rhs), timm:$cc))]>;
  def FIri_MALKRZrm : PseudoI<(outs Reg:$dst), (ins Reg:$src1, mem:$src2, Reg:$lhs, i32imm:$rhs, cc:$cc),
                              [(set Reg:$dst, (F2003fselectcc Reg:$src1, (load addr:$src2), Reg:$lhs, imm:$rhs, timm:$cc))]>;

  def FImr_MALKRZrm : PseudoI<(outs Reg:$dst), (ins Reg:$src1, mem:$src2, mem:$lhs, Reg:$rhs, cc:$cc),
                              [(set Reg:$dst, (F2003fselectcc Reg:$src1, (load addr:$src2), (load addr:$lhs), Reg:$rhs, timm:$cc))]>;
  def FImm_MALKRZrm : PseudoI<(outs Reg:$dst), (ins Reg:$src1, mem:$src2, mem:$lhs, mem:$rhs, cc:$cc),
                              [(set Reg:$dst, (F2003fselectcc Reg:$src1, (load addr:$src2), (load addr:$lhs), (i32 (load addr:$rhs)), timm:$cc))]>;
  def FImi_MALKRZrm : PseudoI<(outs Reg:$dst), (ins Reg:$src1, mem:$src2, mem:$lhs, i32imm:$rhs, cc:$cc),
                              [(set Reg:$dst, (F2003fselectcc Reg:$src1, (load addr:$src2), (load addr:$lhs), imm:$rhs, timm:$cc))]>;

  def FIir_MALKRZrm : PseudoI<(outs Reg:$dst), (ins Reg:$src1, mem:$src2, i32imm:$lhs, Reg:$rhs, cc:$cc),
                              [(set Reg:$dst, (F2003fselectcc Reg:$src1, (load addr:$src2), imm:$lhs, Reg:$rhs, timm:$cc))]>;
  def FIim_MALKRZrm : PseudoI<(outs Reg:$dst), (ins Reg:$src1, mem:$src2, i32imm:$lhs, mem:$rhs, cc:$cc),
                              [(set Reg:$dst, (F2003fselectcc Reg:$src1, (load addr:$src2), imm:$lhs, (load addr:$rhs), timm:$cc))]>;
  def FIii_MALKRZrm : PseudoI<(outs Reg:$dst), (ins Reg:$src1, mem:$src2, i32imm:$lhs, i32imm:$rhs, cc:$cc),
                              [(set Reg:$dst, (F2003fselectcc Reg:$src1, (load addr:$src2), imm:$lhs, imm:$rhs, timm:$cc))]>;

  def FIrr_MALKRZri : PseudoI<(outs Reg:$dst), (ins Reg:$src1, i32imm:$src2, Reg:$lhs, Reg:$rhs, cc:$cc),
                              [(set Reg:$dst, (F2003fselectcc Reg:$src1, imm:$src2, Reg:$lhs, Reg:$rhs, timm:$cc))]>;
  def FIrm_MALKRZri : PseudoI<(outs Reg:$dst), (ins Reg:$src1, i32imm:$src2, Reg:$lhs, mem:$rhs, cc:$cc),
                              [(set Reg:$dst, (F2003fselectcc Reg:$src1, imm:$src2, Reg:$lhs, (load addr:$rhs), timm:$cc))]>;
  def FIri_MALKRZri : PseudoI<(outs Reg:$dst), (ins Reg:$src1, i32imm:$src2, Reg:$lhs, i32imm:$rhs, cc:$cc),
                              [(set Reg:$dst, (F2003fselectcc Reg:$src1, imm:$src2, Reg:$lhs, imm:$rhs, timm:$cc))]>;

  def FImr_MALKRZri : PseudoI<(outs Reg:$dst), (ins Reg:$src1, i32imm:$src2, mem:$lhs, Reg:$rhs, cc:$cc),
                              [(set Reg:$dst, (F2003fselectcc Reg:$src1, imm:$src2, (load addr:$lhs), Reg:$rhs, timm:$cc))]>;
  def FImm_MALKRZri : PseudoI<(outs Reg:$dst), (ins Reg:$src1, i32imm:$src2, mem:$lhs, mem:$rhs, cc:$cc),
                              [(set Reg:$dst, (F2003fselectcc Reg:$src1, imm:$src2, (load addr:$lhs), (i32 (load addr:$rhs)), timm:$cc))]>;
  def FImi_MALKRZri : PseudoI<(outs Reg:$dst), (ins Reg:$src1, i32imm:$src2, mem:$lhs, i32imm:$rhs, cc:$cc),
                              [(set Reg:$dst, (F2003fselectcc Reg:$src1, imm:$src2, (load addr:$lhs), imm:$rhs, timm:$cc))]>;

  def FIir_MALKRZri : PseudoI<(outs Reg:$dst), (ins Reg:$src1, i32imm:$src2, i32imm:$lhs, Reg:$rhs, cc:$cc),
                              [(set Reg:$dst, (F2003fselectcc Reg:$src1, imm:$src2, imm:$lhs, Reg:$rhs, timm:$cc))]>;
  def FIim_MALKRZri : PseudoI<(outs Reg:$dst), (ins Reg:$src1, i32imm:$src2, i32imm:$lhs, mem:$rhs, cc:$cc),
                              [(set Reg:$dst, (F2003fselectcc Reg:$src1, imm:$src2, imm:$lhs, (load addr:$rhs), timm:$cc))]>;
  def FIii_MALKRZri : PseudoI<(outs Reg:$dst), (ins Reg:$src1, i32imm:$src2, i32imm:$lhs, i32imm:$rhs, cc:$cc),
                              [(set Reg:$dst, (F2003fselectcc Reg:$src1, imm:$src2, imm:$lhs, imm:$rhs, timm:$cc))]>;

}

// Control Flow Instructions.
// F5 is marked as a use to prevent stack-pointer assignments that appear
// immediately before calls from potentially appearing dead.
let isCall = 1, Uses = [F5] in {
  def INJ_FENXEOi : I<(outs), (ins i32imm:$func), "inj f5@ xx $func",
                      [(F2003fFenxeo imm:$func)]>;
  def INJ_FENXEOr : I<(outs), (ins Reg:$func), "inj f5@ xx $func",
                      [(F2003fFenxeo Reg:$func)]>;
  def INJ_FENXEOm : I<(outs), (ins mem:$func), "inj f5@ xx $func",
                      [(F2003fFenxeo (load addr:$func))]>;
}

let isReturn = 1, isTerminator = 1, isBarrier = 1 in {
  def KRZ_DOSNUD : I<(outs), (ins), "krz xx f5@",
                     [(F2003fDosnud)]>;
}

// ADJCALLSTACKDOWN/UP implicitly use/def SP because they may be expanded into
// a stack adjustment and the codegen must know that they may modify the stack
// pointer before prolog-epilog rewriting occurs.
// Pessimistically assume ADJCALLSTACKDOWN / ADJCALLSTACKUP will become
// sub / add which can clobber SR.
let Defs = [F5], Uses = [F5] in {
def ADJCALLSTACKDOWN : PseudoI<(outs), (ins i32imm:$amt1, i32imm:$amt2),
                               [(F2003fcallseq_start timm:$amt1, timm:$amt2)]>;
def ADJCALLSTACKUP   : PseudoI<(outs), (ins i32imm:$amt1, i32imm:$amt2),
                               [(F2003fcallseq_end timm:$amt1, timm:$amt2)]>;
}

def ATAframe : PseudoI<(outs Reg:$dst), (ins i32imm:$base, i32imm:$offset), []>;

//===----------------------------------------------------------------------===//
// DAG Pattern Matching Rules
//===----------------------------------------------------------------------===//

def : Pat<(xor Reg:$src1, Reg:$src2), (NACr (DALrr Reg:$src1, Reg:$src2))>;
def : Pat<(xor Reg:$src1, (load addr:$src2)), (NACr (DALrm Reg:$src1, addr:$src2))>;
def : Pat<(xor Reg:$src1, imm:$src2), (NACr (DALri Reg:$src1, imm:$src2))>;
