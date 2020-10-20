from sys import argv
from pynput.keyboard import Key,Listener
from time import time

path = '\\'.join(argv[0].split('\\')[:-1])
timeList = []
l = None

def on_press(key):
    global l
    if key != Key.esc:
        timeList.append(time()*1000)
    else:
        f = open(path+'\\notes.txt', 'w')
        f.write('['+','.join([str(int(i-timeList[0])) for i in timeList])+']')
        f.close()
        l.stop()
        
#监听键盘按键
with Listener(on_press=on_press) as listener:
    l = listener
    listener.join()

