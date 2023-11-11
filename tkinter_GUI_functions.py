from tkinter import *


def fancy_button(master_frame, name, xpos, ypos):
    Button(
        master=master_frame,
        text=name,
        bg="#514C64",
        fg="#EFEDF6",
        relief=RAISED,
        width=10,
        borderwidth=5,
        cursor="dotbox"
    ).place(x=xpos, y=ypos)