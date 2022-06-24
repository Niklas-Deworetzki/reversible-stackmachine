; Example program computing Euler's number.
; Requires around 8 MB of data memory to build a table of digits
; And stores 5k digits on the stack.
.set how_many_digits 5000
.set aux_number      1776
.set print_count     5000

		start
		allocpar 4
		pushc [main - @+1]
		call
		popc [@-1 - main]   ; Clean computation data from stack.
        popc aux_number
		popc how_many_digits

        inc print_count     ; Move ptr to digits up so they can be fetched from end.
        swap                ; < size ptr
        pushtrue            ; < true size ptr
enter_print_bot:
        branch enter_print_top

enter_print_top:
        branch enter_print_bot
loop_print_top:
        brf loop_print_bot
        pushc [how_many_digits + 1] ; Includes leading 2 digit
        swap
        cmpopeq
        popc [how_many_digits + 1]

        dec 1   ; Decrement counter
        swap    ; Place ptr on top
        load    ; Load from ptr
        bury    ; Place digit on stack

        dec 1   ; Decrement ptr
        swap    ; And restore order
                ; < size ptr

        pushc [5000 - print_count]  ; Loop until print_count digits
        cmpusheq                    ; have been placed on stack.
        swap
        popc [5000 - print_count]
loop_print_bot:
        brf loop_print_top
exit_print_bot:
        branch exit_print_top

exit_print_top:
        branch exit_print_bot
        poptrue                     ; Remove condition and
        popc [5000 - print_count]   ; size value.
        popm 0                      ; ptr can be arbitrary, but M[0] is clear.
		stop

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; AUTO GENERATED CODE
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

main_top:
		branch main_bot
		neg
main:
		call
		asf 4
		allocpar 1
		pushc how_many_digits
		pushc aux_number
		pushc [spigot - @+1]
		call
		popc [@-1 - spigot]
		popl 1
		popl 2
		popl 3
		pushl 2
		pushc 1
		add
		popl 4
		popl 2
		pushl 1
		popl -2
		pushl 2
		popl -3
		pushl 3
		popl -4
		pushl 4
		popl -5
		rsf 4
main_bot:
		branch main_top
spigot_top:
		branch spigot_bot
		neg
spigot:
		call
		asf 7
		pushl -3
		popl 2
		pushl -2
		popl 1
		pushl 1
		pushc 1
		add
		popl 3
		popl 1
		pushc 1
		popl 4
		pushl 3
		pushl 4
		pushtrue
enter_init_bot:
		branch enter_init_top
loop_init_top:
		branch loop_init_bot
enter_init_top:
		brt enter_init_bot
		swap
		popl 3
		pushc 1
		pushl 3
		dig
		cmpopeq
		popl 3
		popc 1
		popl 4
		pushc 1
		pushl 3
		add
		dup
		bury
		sub
		popl 3
		popc 1
		pushl 3
		load
		swap
		popl 3
		xor
		pushl 3
		swap
		store
		popl 3
		pushc 1
		pushl 3
		add
		dig
		undup
		sub
		popl 3
		popc 1
		pushc 1
		pushl 4
		load
		swap
		popl 4
		xor
		pushl 4
		swap
		store
		popl 4
		popc 1
		pushl 3
		inc 1
		popl 3
		pushl 4
		inc 1
		popl 4
		pushl 4
		pushl 1
		pushl 3
		cmpusheq
		bury
		popl 3
		popl 1
		pushl 3
		swap
exit_init_bot:
		brt exit_init_top
loop_init_bot:
		branch loop_init_top
exit_init_top:
		branch exit_init_bot
		poptrue
		popl 3
		popl 4
		pushl 1
		pushl 3
		undup
		popl 1
		pushl 1
		dup
		add
		pushl 4
		undup
		sub
		undup
		popl 1
		pushc 2
		pushl 1
		load
		swap
		popl 1
		xor
		pushl 1
		swap
		store
		popl 1
		popc 2
		pushc 1
		pushfalse
enter_iterate_bot:
		branch enter_iterate_top
loop_iterate_top:
		branch loop_iterate_bot
enter_iterate_top:
		brf enter_iterate_bot
		swap
		popl 3
		pushc 1
		pushl 3
		dig
		cmpopne
		popl 3
		popc 1
		pushc 2
		pushl 1
		arpushmul
		dup
		popl 4
		arpopmul
		popl 1
		popc 2
		pushl 3
		pushl 1
		arpushmul
		pushl 4
		add
		popl 4
		arpopmul
		popl 1
		popl 3
		pushl 1
		dup
		popl 5
		popl 1
		pushc 0
		popl 6
		pushtrue
enter_row_bot:
		branch enter_row_top
loop_row_top:
		branch loop_row_bot
enter_row_top:
		brt enter_row_bot
		pushl 1
		pushl 5
		dig
		cmpopeq
		popl 5
		popl 1
		pushl 5
		dec 1
		popl 5
		pushl 1
		inc 1
		pushl 4
		sub
		popl 4
		dec 1
		popl 1
		pushc 10
		pushl 4
		load
		swap
		popl 4
		arpushmul
		pushl 6
		add
		popl 6
		arpopmul
		pushl 4
		swap
		store
		popl 4
		popc 10
		pushl 1
		pushl 4
		add
		popl 4
		popl 1
		pushl 5
		load
		swap
		popl 5
		pushl 6
		arpushdiv
		dup
		popl 7
		arpopdiv
		popl 6
		pushl 5
		swap
		store
		popl 5
		pushl 5
		load
		swap
		popl 5
		pushl 7
		arpushmul
		pushl 6
		sub
		popl 6
		arpopmul
		popl 7
		pushl 5
		swap
		store
		popl 5
		pushl 4
		load
		swap
		popl 4
		popc 0
		pushl 6
		pushl 4
		swap
		store
		popl 4
		pushl 7
		popl 6
		pushc 1
		pushl 5
		cmpusheq
		bury
		popl 5
		popc 1
exit_row_bot:
		brt exit_row_top
loop_row_bot:
		branch loop_row_top
exit_row_top:
		branch exit_row_bot
		poptrue
		pushl 5
		popc 1
		pushl 4
		dec 1
		popl 4
		pushl 4
		load
		swap
		popl 4
		popc 0
		pushl 6
		pushl 4
		swap
		store
		popl 4
		pushl 3
		pushl 1
		arpushmul
		pushl 4
		sub
		popl 4
		arpopmul
		popl 1
		popl 3
		pushl 1
		pushl 4
		undup
		popl 1
		pushl 3
		inc 1
		popl 3
		pushl 2
		pushl 3
		cmpushle
		bury
		popl 3
		popl 2
		pushl 3
		swap
exit_iterate_bot:
		brf exit_iterate_top
loop_iterate_bot:
		branch loop_iterate_top
exit_iterate_top:
		branch exit_iterate_bot
		popfalse
		popl 3
		pushl 2
		inc 1
		pushl 3
		undup
		dec 1
		popl 2
		pushl 1
		dup
		popl 3
		popl 1
		pushl 2
		pushl 1
		arpushmul
		dup
		popl 4
		arpopmul
		popl 1
		popl 2
		pushc 2
		pushl 1
		arpushmul
		pushl 4
		add
		popl 4
		arpopmul
		popl 1
		popc 2
		pushl 2
		pushc 1
		add
		popl 5
		popl 2
		pushl 5
		pushfalse
enter_fill_bot:
		branch enter_fill_top
loop_fill_top:
		branch loop_fill_bot
enter_fill_top:
		brf enter_fill_bot
		pushl 1
		pushl 3
		dig
		cmpopne
		popl 3
		popl 1
		popl 5
		pushl 4
		pushl 3
		memswap
		popl 3
		popl 4
		pushl 4
		inc 1
		popl 4
		pushl 1
		pushl 3
		add
		popl 3
		popl 1
		pushl 5
		dec 1
		popl 5
		pushc 0
		pushl 5
		cmpushne
		bury
		popl 5
		popc 0
		pushl 5
		swap
exit_fill_bot:
		brf exit_fill_top
loop_fill_bot:
		branch loop_fill_top
exit_fill_top:
		branch exit_fill_bot
		popfalse
		popc 0
		pushl 2
		inc 1
		pushl 4
		sub
		popl 4
		dec 1
		popl 2
		pushl 4
		pushl 3
		undup
		popl 4
		pushl 1
		popl -2
		pushl 2
		popl -3
		pushl 4
		popl -4
		rsf 7
spigot_bot:
		branch spigot_top
