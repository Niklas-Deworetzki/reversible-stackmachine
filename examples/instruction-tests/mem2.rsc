data:
.word 0

_start:
    pushc 42
    popm data
    pushm data
    halt