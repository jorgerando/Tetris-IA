import gym
import random , time
from nes_py import NESEnv

rom_path = "./Tetris.nes"

class TetrisEnv(NESEnv):

    def __init__(self, rom_file):

        super().__init__(rom_path)

        self.firt_input = True
        self.end_cicle = False
        self.i = 0 # secuence indice
        self.secuence = []

        self.types = [
        'Tu','Tr','Td','Tl',
        'Jl','Ju','Jr','Jd',
        'Zh','Zv',
        'O',
        'Sh','Sv',
        'Lr','Ld','Ll','Lu',
        'Iv',
        'Ih',
         None
        ]

        self.button_map = {
         'right':  0b10000000,
         'left':   0b01000000,
         'down':   0b00100000,
         'up':     0b00010000,
         'start':  0b00001000,
         'select': 0b00000100,
         'B':      0b00000010,
         'A':      0b00000001,
         'NOOP':   0b00000000,
        }

        # board dimensions
        self.cols = 10
        self.rows = 20

    def _did_reset(self):

         # set random seed
         self.ram[0x0017] = random.randint(0, 255)
         self.ram[0x0018] = random.randint(0, 255)

         # Skip the legal notice, the home screen and the level selection
         for _ in range(150):
          self.ram[0x00C3] = 0x00
          self.ram[0x00C0] = 0x04
          self._frame_advance(4)

         return None

    def _get_done(self):
        # check game state
        return self.ram[0x0058] != 0x00

    def get_tetris_state(self):

        # type pieces
        actual = self.types[self.ram[0x0042]]
        next = self.types[self.ram[0x00BF]]
        #print( "Actual :" + str(actual) + " Next :" + str(next) )

        # board state
        start_address = 0x0400
        board = ""

        for row in range(self.rows):
           line = ""
           for col in range(self.cols):
               address = start_address + row * cols + col
               cell_value = env.ram[address]
               line += "0" if int(cell_value) == 0xEF else "1"
               board += line
           #print(f"| {line} |")

        return actual , next , board

    def step_(self):

        # check start game cicle
        if ( self.firt_input or (self.end_cicle and self.ram[0x0048] == 0x01 ) ):
            actual , next , board = self.get_tetris_state()
            #getSecuence()
            self.secuence = ['A','NOOP','A','NOOP']
            self.i = 0
            self.firt_input = False

        #check end game cicle
        self.end_cicle = False
        if(self.ram[0x0048] == 0x08):
            self.end_cicle = True

        # aplay action secuence
        env.step(self.button_map[self.secuence[self.i]])

        # The last action is repeated until the end of the game cycle
        if self.i == len(self.secuence)-1 :
           self.i = len(self.secuence)-1
        else :
           self.i+=1

env = TetrisEnv(rom_path)
state = env.reset()
env._did_reset()

while not env._get_done():
    env.render()
    env.step_()
env.close()
