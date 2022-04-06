value:
    .word 0

    start
    pushc [t - @1]
    uncall
    pushm value
    undup
    stop

    nop
    nop
    nop
    nop
    nop

top:
    branch bot
t:
    call
    neg
    pushm value
    inc 10
    popm value
bot:
    branch top