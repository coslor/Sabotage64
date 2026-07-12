<p align="center">
    <h1>SABOTAGE 64</h1>
    <br>
    <img src=sabotage64_clip.png alt="Screenshot of Sabotage gameplay" width="300">
</p>

...is a game for the Commodore 64 computer. It is intended as an homage to Mark Allen's amazing Sabotage for the Apple ][ back in 1981. The original was written in 6502 assembler, but my version is writen in C, for [Oscar64](https://github.com/drmortalwombat/oscar64), a standards-compliant, optimizing C compiler that runs on various modern platforms and generates C64 machine code. 

A note on project structure: the sabotage64/sabotage64 folder is for CBM PRG Studio, which I used for some of the screens. The .sdd files are used by PRG Studio, and the generated .bin files are for Oscar64. The .pe file is for yet another screen editor, this one online at https://petscii.krissz.hu/ and the generated screen values are actually copy & pasted in title_screen.h. I'm hoping to integrate that file into PRG Studio as well, as I can standardize this crud a little bit.