; A simple program adding all numbers defined after the 'numbers' label.
;
; (C) 2022, Niklas Deworetzki

numbers:                    ; Define a label and
    .word 0 1 2 3 4         ; place values in memory behind it.
    .word 5 6 7 8 9         ; Every line with values must start with .word directive.
numbers_end:                ; Another label marks the end of memory region.

accumulator:                ; Initialize an in-memory accumulator with
    .word 0                 ; the value of 0.

            start
; Push the range bounds on the stack.
            pushc numbers_end
            pushc numbers

; Enter the loop, without triggering the branch.
            pushfalse
loop_top:   brt loop_bot
            pushc numbers   ; Push numbers for comparison.
            swap
; Inverted comparison, verifies that the branch was triggered
; except the first time, when numbers and the range counter are equal.
            cmpopne
            popc numbers    ; Clean up numbers after comparison.


            load            ; The range counter is on top, load from it.
            pushm accumulator
            add             ; Update accumulator with loaded value.
            popm accumulator
            store           ; Store at range counter.

            inc 1           ; Increment range counter.

            cmpushne        ; Loop if range counter != numbers_end
loop_bot:   brt loop_top
            popfalse        ; Clean up branching condition.

; Clean up range counter and bounds. Both are now at numbers_end.
            popc numbers_end
            popc numbers_end

; Load the accumulator to display as a result.
            pushm accumulator

; Stop execution.
            stop
