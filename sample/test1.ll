define i32 @addsub(i32 %a, i32 %b, i32 %c) nounwind readnone {
entry:
  %add = add nsw i32 %a, %b
  %sub = sub nsw i32 %add, %c
  ret i32 %sub
}
