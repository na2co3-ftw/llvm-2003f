define i32 @main(i32 %idx) nounwind {
  %arr = alloca i32, i32 4
  %ptr = getelementptr inbounds i32, i32* %arr, i32 %idx
  store i32 10, i32* %ptr
  %b = call i32 @xok_cersva(i32* %arr)
  ret i32 %b
}

declare i32 @xok_cersva(i32* nocapture) nounwind
