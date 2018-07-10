define i32 @add(i32 %a, i32 %b) nounwind readnone {
entry:
  %add = add nsw i32 %b, %a
  ret i32 %add
}

define i32 @call() nounwind readnone {
entry:
  %call = tail call i32 @add(i32 1, i32 2)
  ret i32 %call
}
