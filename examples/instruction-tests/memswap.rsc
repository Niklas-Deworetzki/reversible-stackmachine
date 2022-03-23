data:
.word 0 42

    start
    pushc [data + 0]
    pushc [data + 1]
    memswap
    popc [data + 1]
    popc [data + 0]

    pushm [data + 0]
    stop