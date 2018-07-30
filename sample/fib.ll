define i32 @pom(i32 %n) nounwind {
fasal:
  %0 = icmp sge i32 %n, 2
  br i1 %0, label %xeu, label %halt
xeu:
  %1 = phi i32 [ %n, %fasal ]
  %2 = sub nsw i32 %1, 1
  %3 = call i32 @pom(i32 %2)
  %4 = sub nsw i32 %2, 1
  %5 = call i32 @pom(i32 %4)
  %6 = add nsw i32 %3, %5
  br label %halt
halt:
  %7 = phi i32 [ %6, %xeu ], [ %n, %fasal ]
  ret i32 %7
}
