# 通过32位色图来生成地图信息文件

from PIL import Image
from sys import argv
from random import choice
import json

MU_GRID_OBJTYPE_NONE = (0, 0, 0, 255)  # 黑色标定空地区
MU_GRID_OBJTYPE_DARK = (200, 0, 255, 255)  # 紫色标定地图界线
MU_GRID_OBJTYPE_EMPTY = (255, 255, 255, 255)  # 白色标定空地块
MU_GRID_OBJTYPE_SNAKEHEAD = (0, 255, 0, 255)  # 绿色标定蛇头
MU_GRID_OBJTYPE_SNAKEBODY = (255, 255, 0, 255)  # 黄色标定蛇身
MU_GRID_OBJTYPE_BLOCK = (0, 0, 255, 255)  # 蓝色标定障碍物
MU_GRID_OBJTYPE_FOOD = (255, 200, 0, 255)  # 橙色标定食物

MU_SNAKE_DIRECTION_UP = 0
MU_SNAKE_DIRECTION_RIGHT = 1
MU_SNAKE_DIRECTION_DOWN = 2
MU_SNAKE_DIRECTION_LEFT = 3

def sum38(iterseq, start=0):
    s = start
    for i in iterseq:
        s += i
    return s

def select(objtype):
    return sum38([[(i[0], j[0]) for j in enumerate(i[1]) if j[1] == objtype]
                for i in enumerate(pixels)], start=[])

def sortSnake(snake, direct):
    so = (snake[0], snake[1]-1)
    if direct != MU_SNAKE_DIRECTION_UP and so in snakes:
        return [so] + sortSnake(so, MU_SNAKE_DIRECTION_DOWN)
    so = (snake[0]+1, snake[1])
    if direct != MU_SNAKE_DIRECTION_RIGHT and so in snakes:
        return [so] + sortSnake(so, MU_SNAKE_DIRECTION_LEFT)
    so = (snake[0], snake[1]+1)
    if direct != MU_SNAKE_DIRECTION_DOWN and so in snakes:
        return [so] + sortSnake(so, MU_SNAKE_DIRECTION_UP)
    so = (snake[0]-1, snake[1])
    if direct != MU_SNAKE_DIRECTION_LEFT and so in snakes:
        return [so] + sortSnake(so, MU_SNAKE_DIRECTION_RIGHT)
    return []

if __name__ == '__main__':
    global pixels, snakes
    
    img = Image.open(argv[1])
    encoder = json.JSONEncoder()
    mapmu = {}
    pixels = [[img.getpixel((i, j)) for j in range(64)] for i in range(64)]
    snakes = select(MU_GRID_OBJTYPE_SNAKEBODY)
    path = '\\'.join(argv[0].split('\\')[:-1])
    ofo = open(path+'\\map.mu', 'w')

    mapmu['none'] = [{'x':i[0], 'y':i[1]} for i in select(MU_GRID_OBJTYPE_NONE)]
    mapmu['darkPos'] = [{'x':i[0], 'y':i[1]} for i in select(MU_GRID_OBJTYPE_DARK)]
    mapmu['food'] = [{'x':i[0], 'y':i[1]} for i in select(MU_GRID_OBJTYPE_FOOD)]
    mapmu['blockPos'] = [{'x':i[0], 'y':i[1]} for i in select(MU_GRID_OBJTYPE_BLOCK)]
    mapmu['snakePos'] = [{'x':i[0], 'y':i[1]} for i in sortSnake(choice(select(MU_GRID_OBJTYPE_SNAKEHEAD)), -1)]
    
    ofo.write(encoder.encode(mapmu))
    ofo.close()
