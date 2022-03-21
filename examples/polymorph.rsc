; Virtual Function Table definition for nil.
; All nil instances are empty objects.
nil_vtable:
nil_method_size:
    .word size_nil  ; Virtual method size is implemented as size_nil.

; Virtual Function Table definition for cons.
; All cons instances have two members "head" and "tail".
cons_vtable:
cons_method_size:
    .word size_cons  ; Virtual method size is implemented as size_cons.

.set cons_off_head   1  ; Offset of "head" field in cons instances.
.set cons_off_tail   2  ; Offset of "tail" field in cons instances.


obj1:
    .word nil_vtable

obj2:
    .word cons_vtable 111 obj1

obj3:
    .word cons_vtable 222 obj2

obj4:
    .word cons_vtable 333 obj3

obj5:
    .word cons_vtable 333 obj4

; Program entry point.
    allocpar 1          ; Allocate result slot.
    pushc obj5          ; Push reference to List object.
    pushc [size - @1]
    call
    popc [@-1 - size]
    popc obj5           ; Pop reference to List object.
    halt

size_top:
    branch size_bot
    neg
; Polymorphic function uses 1st argument as "this" pointer to load
; vtable and implementation address.
size:
    call
    asf 3
    pushl -2
    popl   1

; Load call address from this pointer into local variable 3.
    pushl  1
    load
    load
    dup
    popl 3
    store
    store
    popl 1

; Forward the call to the concrete implementation.
    pushl 2
    pushl 1
    pushl 3
    dec   @+1
    call
    dec   @-1
    neg
    popl 3
    popl 1
    popl 2

; Clean up call address using this pointer.
    pushl  1
    load
    load
    pushl 3
    undup
    store
    store
    popl 1

    pushl  2
    popl  -3
    pushl  1
    popl  -2
    rsf 3
size_bot:
    branch size_top
size_cons_top:
    branch size_cons_bot
    neg
; Implementation of size method for cons objects.
; Will call size for the tail and increment the result by 1.
size_cons:
    call
    asf 0

    pushl -2            ; Push this pointer.
    load cons_off_tail  ; Load tail pointer.

    allocpar 1          ;
    swap                ; Allocate result parameter.

    pushc [size - @+1]
    call
    popc  [@-1 - size]

    swap
    inc 1               ; Increment length by 1.
    popl -3             ; Return result.

    store cons_off_tail ; Restore tail pointer.
    popl -2             ; Restore this pointer.

    rsf 0
size_cons_bot:
    branch size_cons_top


size_nil_top:
    branch size_nil_bot
    neg
; Implementation of size method for nil objects.
; Will always return 0.
size_nil:
    call
    asf 0

    pushc 0         ; Push constant 0 as length.
    popl -3         ; Return result.

    rsf 0
size_nil_bot:
    branch size_nil_top
