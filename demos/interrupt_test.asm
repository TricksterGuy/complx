; Demo showing LC3 interrupts
.orig x180
	.fill INTERRUPTHANDLER
.end

.orig x3000
	LD R0, KBSRIE
	STI R0, KBSR
	
	LOOP BR LOOP
	HALT
	
INTERRUPTHANDLER
	
	LEA R0, MSG
	PUTS
	LDI R0, KBDR
	OUT
	AND R0, R0, 0
	ADD R0, R0, 10
	OUT
	RTI
	




KBSRIE .fill x4000
KBSR .fill xFE00
KBDR .fill xFE02
MSG .stringz "You typed: "

.end
