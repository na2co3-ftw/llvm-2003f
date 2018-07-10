; RUN: opt -S -instsimplify < %s | FileCheck %s
target datalayout = "e-m:w-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"
%S = type { i16, i32 }

define <2 x i16> @test1() {
entry:
  %b = insertelement <2 x i16> <i16 undef, i16 0>, i16 extractvalue (%S select (i1 icmp eq (i16 extractelement (<2 x i16> bitcast (<1 x i32> <i32 1> to <2 x i16>), i32 0), i16 0), %S zeroinitializer, %S { i16 0, i32 1 }), 0), i32 0
  ret <2 x i16> %b
}

; CHECK-LABEL: @test1(
; CHECK: ret <2 x i16> zeroinitializer
