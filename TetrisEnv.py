import gym
from nes_py import NESEnv

rom_path = "./Tetris.nes"

class TetrisEnv(NESEnv):

    def __init__(self, rom_file):

        super().__init__(rom_path)

    def _did_reset(self):
        
         # Skip the legal notice, the home screen and the level selection
         for _ in range(150):
          self.ram[0x00C3] = 0x00
          self.ram[0x00C0] = 0x04
          self._frame_advance(4)

         return None

env = TetrisEnv(rom_path)
state = env.reset()
env._did_reset()

while True :
    env.render()
    action = env.action_space.sample()
    state, reward, done, info = env.step(4)

    env.ram[0x00C0] = 0x04

env.close()
