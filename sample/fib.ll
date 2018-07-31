define i32 @pom(i32 %n) nounwind {
fasal:
  %0 = icmp sge i32 %n, 2
  br i1 %0, label %xeu, label %halt
xeu:
  %1 = sub nsw i32 %n, 1
  %2 = call i32 @pom(i32 %1)
  %3 = sub nsw i32 %1, 1
  %4 = call i32 @pom(i32 %3)
  %5 = add nsw i32 %2, %4
  br label %halt
halt:
  %6 = phi i32 [ %5, %xeu ], [ %n, %fasal ]
  ret i32 %6
}
