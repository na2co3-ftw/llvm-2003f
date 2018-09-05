define void @i8_test(i8 %a, i8 %b, i8* %ret) {
	%A = add nsw i8 %a, %b
	store i8 %A, i8* %ret
	ret void
}

define void @i16_test(i16 %a, i16 %b, i16* %ret) {
	%A = add nsw i16 %a, %b
	store i16 %A, i16* %ret
	ret void
}

define i32 @ashr16(i32 %a) {
	%A = ashr i32 %a, 16
	ret i32 %A
}

define i32 @compose(i16 %hi, i16 %lo) {
	%hi_ = zext i16 %hi to i32
	%lo_ = zext i16 %lo to i32
	%A = shl i32 %hi_, 16
	%B = or i32 %A, %lo_
	ret i32 %B
}
