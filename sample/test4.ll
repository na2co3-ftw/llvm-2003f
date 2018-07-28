define i32 @select_test(i32 %a, i32 %b, i32 %c) nounwind {
  %A = icmp eq i32 %a, %b
  %B = zext i1 %A to i32
  %C = select i1 %A, i32 %c, i32 10
  %D = add nsw i32 %B, %C
  ret i32 %D
}
