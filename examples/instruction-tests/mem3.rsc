data:
.word 42

_start:
    pushc data
    load
    swap
    popc data
    halt