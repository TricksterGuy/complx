.orig x3000
	; R0 will be set to 'A' before here by the test
	OUT
	IN
	GETC
	LEA R0, HELLOWORLD
	PUTS
	LEA R0, PUTSPSTR
	PUTSP
	HALT
	
HELLOWORLD .stringz "HELLO WORLD"
PUTSPSTR .fill x3130
	 .fill x3332
	 .fill x3534
	 .fill x3736
	 .fill x3938
	 .fill x0030
	 .fill x0000
.end