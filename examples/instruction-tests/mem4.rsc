data:
.word 0

_start:
    pushc data
    pushc 42
    store
    popc data
    pushm data
    halt