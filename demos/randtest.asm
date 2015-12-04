;@plugin filename=lc3_random address=xFEED

.orig x3000
	; Set the random seed by storing to the device register
	AND R0, R0, 0
	ADD R0, R0, 10
	STI R0, RANDOM_DEVREG
	; Reading from the device register gives you a random number
	LDI R0, RANDOM_DEVREG
	HALT

RANDOM_DEVREG .fill xFEED
.end
