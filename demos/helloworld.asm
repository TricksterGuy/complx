.orig x3000
LEA R0, HELLOWORLD_STR ; Load address of "Hello world!"
PUTS ; Display Hello world!
HALT ; The end
; Hello world
HELLOWORLD_STR .stringz "Hello world!"
.end
