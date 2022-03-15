_start:
    pushc 32767
    xorhc 32767 ; This instruction shouldn't change anything.
    popc  32767
    halt