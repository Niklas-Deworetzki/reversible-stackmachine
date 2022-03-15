; A simple program implementing the computation steps for
; the collatz conjecture. Even numbers are divided by 2.
; Odd numbers are multiplied by 3 and incremented by 1.
; The resulting trace of numbers generated is placed on the stack.
;
; (C) 2022, Niklas Deworetzki

.set input 19

        pushc input     ; Start with input number

        pushtrue
step:
        brf repeat      ; Entry condition for loop is (number != input)
        pushc input
        swap
        cmpopeq
        popc input

        pushc 1
        arpushand       ; Bitwise and with last bit of number.
        cmpushne        ; Is number even?
if_even:
        brt to_even
if_odd:
        branch to_odd

to_even:
        branch if_even
; Entered true branch, clean up condition and arguments.
        poptrue
        popc 0
        popc 1

; Divide by 2.
        pushc 2
        swap
        arpushdiv
        dig
        popc 2
        swap

; Prepare condition for joined paths.
        pushc 1
        pushc 0
        pushtrue
from_even:
        branch fi_even


to_odd:
        branch if_odd
; Entered false branch, clean up condition and arguments.
        popfalse
        popc 1
        popc 1

; Multiply with 3.
        pushc 3
        arpushmul
; Increment by 1.
        inc 1
        bury
        popc 3

; Prepare condition for joined paths.
        pushc 1
        pushc 1
        pushfalse
from_odd:
        branch fi_odd


fi_odd:
        branch from_odd
fi_even:
        brt from_even
; Verify condition and arguments.
        cmpopne
        arpopand

; Shuffle numbers on stack and check if we reached 1.
        dig
        cmpusheq
        dig
        popc 1
repeat:
        brf step
; We reached 1, remove condition from stack.
        poptrue

        halt
