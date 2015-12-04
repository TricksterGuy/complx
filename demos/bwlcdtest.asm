;@plugin filename=lc3_bwlcd width=8 height=8 initaddr=x4000 startaddr=x6000

; Demo that initializes a black and white LCD and draws a smiley
.orig x3000
	; initialize display
	LD R0, DISPLAYINITVALUE
	STI R0, DISPLAYINITADDR
	
	; display starting address
	LD R0, DISPLAYSTART
	
	; Value to write to display
	AND R1, R1, 0
	ADD R1, R1, 1

	STR R1, R0, 0
	STR R1, R0, 1
	STR R1, R0, 8
	STR R1, R0, 9

	STR R1, R0, 6
	STR R1, R0, 7
	STR R1, R0, 14
	STR R1, R0, 15

	ADD R0, R0, 15
	ADD R0, R0, 15
	ADD R0, R0, 2

	STR R1, R0, 0
	STR R1, R0, 1
	STR R1, R0, 8
	STR R1, R0, 9

	STR R1, R0, 6
	STR R1, R0, 7
	STR R1, R0, 14
	STR R1, R0, 15

	AND R3, R3, 0
	ADD R3, R3, -16
	ADD R0, R0, 15
	ADD R0, R0, 1

stloop
	STR R1, R0, 0
	ADD R0, R0, 1
	ADD R3, R3, 1
	BRN stloop	

	HALT

DISPLAYINITVALUE .fill x8000
DISPLAYINITADDR .fill x4000
DISPLAYSTART .fill x6000
.end
