;@plugin filename=lc3_multiply

.orig x3000
	; The error instruction is now multiply
	MUL R0, R0, 1
	MUL R0, R1, 1
	MUL R0, R0, 0
	MUL R0, R0, 2
	MUL R0, R1, 2
	MUL R0, R0, R1
	MUL R0, R1, R2

	MUL R0, R0, R0
.end
