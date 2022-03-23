; A simple procedure, given a single argument (5 in this case) it will
; increment a local variable until it reaches the value of the given
; argument. Both are returned.
;
; (C) 2022, Niklas Deworetzki

                start
                allocpar 1  ; Allocate dummy argument as placeholder for result.
                pushc 5     ; Push input argument for call.

                pushc [countToN - L_Call]
L_Call:         call
                popc [L_Call - countToN]

                stop


countToN_top:   branch countToN_bot
countToN:       call
                neg

                asf 1       ; Allocate frame for 1 variable.

                pushtrue
loop:           brf exit

; Local variable 1 must be equal to 0 when entering the loop.
                pushl 1
                pushc 0
                dig
                cmpopeq
                popc 0
                popl 1

                pushl 1
                inc 1       ; Increment local variable by 1.
                popl 1

; Loop until local variable (at offset 1) is equal to input argument (at offset -2).
                pushl 1
                pushl -2
                cmpusheq
                bury
                popl -2
                popl 1
exit:           brf loop
                poptrue

                pushl 1     ; Get value of local variable 1
                popl -3     ; and store it as return value.
                rsf 1       ; Release stack frame of size 1.
countToN_bot:   branch countToN_top
