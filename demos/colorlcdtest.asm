;@plugin filename=lc3_colorlcd width=8 height=8 initaddr=x4000 startaddr=x6000
;@plugin filename=lc3_random address=xFEED

; Simple demo that initializes a color display and then fills it with random colors.
.orig x3000
	; initialize display
	LD R3, DISPLAYINITVALUE
	STI R3, DISPLAYINITADDR
	
	; display starting address
	LD R4, DISPLAYSTART
	
	AND R2, R2, 0
	ADD R2, R2, 15	
	ADD R2, R2, 15
	ADD R2, R2, 15
	ADD R2, R2, 15
	ADD R2, R2, 4

COLORLOOP
	LDI R0, RANDOMADDR
	LD R1, COLORMAX
	AND R0, R0, R1
	STR R0, R4, #0
	ADD R4, R4, 1
	ADD R2, R2, -1
	BRP COLORLOOP

	HALT

DISPLAYINITADDR .fill x4000
DISPLAYINITVALUE .fill x8000
RANDOMADDR .fill xFEED
DISPLAYSTART .fill x6000
COLORMAX .fill x7FFF ; x7FFF is while
; colors are 15 bits rrrrrgggggbbbbb 5 bits per component.

.end
