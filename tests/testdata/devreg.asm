.orig x3000
	; R1 will be set to 65
POLLOUT	 LDI R0, DSR
	 BRZP POLLOUT
	 STI R1, DDR
	 ; At this time 65 will be printed.
	HALT 
DSR  .fill xFE04 ; Read Only
DDR  .fill xFE06 ; Write Only
.end

.orig x3020
POLLIN	 LDI R0, KBSR
	 BRZP POLLIN
	 LDI R1, KBDR	; R1 = 66
POLLIN2	 LDI R0, KBSR
	 BRZP POLLIN2
	 LDI R2, KBDR	; R2 = 67
	 HALT
	 
KBSR .fill xFE00 ; Read Only
KBDR .fill xFE02 ; Read Only
.end

.orig x500
	AND R0, R0, #0
	LDI R1, MCR ; Just to read it
	STI R0, MCR ; HALT
MCR  .fill xFFFE ; Write Only
.end
