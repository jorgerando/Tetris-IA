import gym
import random , time , os
import subprocess
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

        self.types_no_orientations = [
        'T','T','T','T',
        'J','J','J','J',
        'Z','Z',
        'O',
        'S','S',
        'L','L','L','L',
        'I',
        'I',
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

        #frame
        self.frame = None

        #Nivel
        self.level = 0

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
        actual = self.types_no_orientations[self.ram[0x0042]]
        next = self.types_no_orientations[self.ram[0x00BF]]
        #print( "Actual :" + str(actual) + " Next :" + str(next) )

        # board state
        start_address = 0x0400
        board = ""

        for row in range(self.rows):
           line = ""
           for col in range(self.cols):
               address = start_address + row * self.cols + col
               cell_value = env.ram[address]
               line += "0" if int(cell_value) == 0xEF else "1"
           board += line
           #print(f"| {line} |")

        return actual , next , board

    def generate_sequence(self,input_string):

      print(input_string)
      input_string = input_string.strip("()")
      rotation, move = map(int, input_string.split(","))

      rotation_sequence = []

      if rotation == 1 :
         rotation_sequence = ['A']
      elif rotation == 2 :
         rotation_sequence = ['A','NOOP','A']
      elif rotation == -1 :
         rotation_sequence = ['B']
      elif rotation == -2 :
         rotation_sequence = ['B','NOOP','B']

      move_sequence = []
      if move >= 0:
        move_sequence = ['right','NOOP'] * (move)
      else:
        move_sequence = ['left','NOOP'] * abs(move)

      secuence = rotation_sequence + move_sequence

      return secuence

    def getSecuence(self,actual,next,board):

        r, w = os.pipe()
        pid = os.fork()

        if pid == 0 :
            os.close(r)
            os.dup2(w, 1)
            os.close(w)
            os.execlp('./bestMove', 'cal', actual , next , board )
        else :
         os.close(w)
         with os.fdopen(r) as pipe:
            salida = pipe.read()
            print(f"Padre recibió: {salida.strip()}")
         os.wait()
         secuence = self.generate_sequence(salida)

         if (self.ram[0x0044] > 25) :
             secuence +=  ['NOOP']
         else :
             secuence += ['NOOP','down']

         print(secuence)
         return secuence

    def step_(self):

        # check start game cicle
        if ( self.firt_input or (self.end_cicle and self.ram[0x0048] == 0x01 ) ):
            actual , next , board = self.get_tetris_state()
            self.secuence = self.getSecuence(actual,next,board)

            self.level = self.ram[0x0044]
            print("Nivel : "+str(self.level ))

            self.i = 0
            self.firt_input = False

        #check end game cicle
        self.end_cicle = False
        if(self.ram[0x0048] == 0x08):
            self.end_cicle = True

        # aplay action secuence

        self.frame , _ , _ , _ = env.step(self.button_map[self.secuence[self.i]])

        if (self.level >= 45) :
            time.sleep(0.15)
            #print(self.secuence[self.i])

        # The last action is repeated until the end of the game cycle
        if self.i == len(self.secuence)-1 :
           self.i = len(self.secuence)-1
        else :
           self.i+=1


env = TetrisEnv(rom_path)
state = env.reset()
env._did_reset()
env.render()

while not env._get_done():
    env.render()
    env.step_()

state = env.reset()
env.render()

print("Nivel alcanzado : ", env.level )
