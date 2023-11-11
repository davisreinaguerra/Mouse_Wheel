from tkinter import *
import tkinter_GUI_functions as tg


class TreadmillGUI:

    def __init__(self):

        # Set up Root Window
        self.root = Tk()
        self.root.title("Treadmill")
        self.root.geometry("600x400")

        # Set up Menu Bar
        self.menubar = Menu(self.root)
        # __File Menu
        self.filemenu = Menu(self.menubar, tearoff=0)
        self.menubar.add_cascade(label="File", menu=self.filemenu)
        self.filemenu.add_command(label="Save Parameters")
        self.filemenu.add_command(label="Load Parameters")

        # __Help Menu
        self.helpmenu = Menu(self.menubar, tearoff=0)
        self.menubar.add_cascade(label="Help", menu=self.helpmenu)
        self.helpmenu.add_command(label="Session")
        self.helpmenu.add_command(label="Photometry")

        # Frames
        # __Title Frame
        self.GUI_titleframe = Frame(
            master=self.root,
            width=600,
            height=50,
            bg="#D6D2E3"
        )
        self.GUI_titleframe.pack(fill=BOTH, expand=True)

        # __Background
        self.GUI_background = Frame(
            master=self.root,
            width=600,
            height=300,
            bg="#EAE6F6"
        )
        self.GUI_background.pack(fill=BOTH, expand=True)

        # Title
        self.title = Label(
            master=self.GUI_titleframe,
            text="Treadmill V1.0",
            background="#D6D2E3",
            font=("Times New Roman", 35)
        )
        self.title.place(x=10,y=10)

        # Place Watermark
        self.lisa = Canvas(
            self.GUI_titleframe,
            bg="#D6D2E3",
            height=75,
            width=100,
            highlightthickness=0)
        file_handle = PhotoImage(file="lisa.png")
        self.lisa.create_image(0, 0, anchor="nw", image=file_handle)
        self.lisa.place(x=500, y=0)

        #Configure Menu Bar
        self.root.config(menu=self.menubar)

        # Display and Pause Code
        self.root.mainloop()

        # Monitor On-Closed functionality
        print("Treadmill GUI Closed")


TreadmillGUI()