import gym
from nes_py import NESEnv

rom_path = "./Tetris.nes"

class TetrisEnv(NESEnv):
    def __init__(self, rom_file):
        super().__init__(rom_path)



env = TetrisEnv(rom_path)
state = env.reset()

done = False
while True :
    env.render()
    action = env.action_space.sample()
    state, reward, done, info = env.step(4)

env.close()
