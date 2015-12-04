.orig x3000
	JSR SUBR
	.fill 0
	HALT

SUBR
	LD R0, A
	LD R1, B
	ADD R2, R0, R1
	RET
	
A .fill 60
B .fill 6
.end
