.orig x3000
	BR #1
	ADD R0, R1, R2
	ADD R0, R1, #2
	LD R0, #-1
	ST R0, #-1
	JSR #-1
	JSRR R0
	AND R0, R1, R2
	AND R0, R1, #2
	LDR R0, R1, #2
	STR R0, R1, #2
	NOT R0, R1
	LDI R0, #-1
	STI R0, #-1
	RET
	JMP R0
	LEA R0, #-1
	TRAP x25
.end
