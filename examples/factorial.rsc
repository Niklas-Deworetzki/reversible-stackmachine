; An implementation of the factorial function.
;
; (C) 2022, Niklas Deworetzki

; Set the value of a symbol without allocating memory.
.set input 5

        pushc input ; Initialize counter.
        pushc 1     ; Initialize accumulator.

        pushtrue
loop_top:
        brf loop_bot

; Pop (accumulator == 1) as the entry condition.
        pushc 1
        swap
        cmpopeq
        popc 1

; accumulator = accumulator * counter
        arpushmul
        swap
        arpopdiv

; Decrement counter by 1.
        swap
        dec 1
        swap

; Push (counter == 0) as the exit condition.
        swap
        pushc 0
        cmpusheq
        bury
        popc 0
        dig
        dig

loop_bot:
        brf loop_top
        poptrue

        swap
        popc 0      ; Counter must be 0 when exiting loop.

        halt
