# 键盘监听
from pynput.keyboard import Key,Listener
from time import time

timeList = []
l = None

def on_press(key):
    global l
    if key != Key.esc:
        timeList.append(time()*1000)
    else:
        f = open(r'C:\\Users\Sieroy\Desktop\666.txt', 'w')
        f.write(','.join([str(int(i-timeList[0])) for i in timeList]))
        f.close()
        l.stop()
        
#监听键盘按键
with Listener(on_press=on_press) as listener:
    l = listener
    listener.join()

