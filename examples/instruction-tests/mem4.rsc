data:
.word 0

    start
    pushc data
    pushc 42
    store
    popc data
    pushm data
    stop