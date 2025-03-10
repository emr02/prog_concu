# -*- coding: utf-8 -*-
import multiprocessing as mp
import os
import math
import array

size = 1000

def calcul(x,y,image,pixel_index):
        p_x = x/size * 2.0 - 1.0
        p_y = y/size * 2.0 - 1.0
        tz = 0.7
        zoo = pow(0.5, 13.0 * tz)
        cc_x = -0.05 + p_x * zoo
        cc_y = 0.6805 + p_y * zoo
        z_x = 0.0
        z_y = 0.0
        m2 = 0.0
        co = 0.0
        dz_x = 0.0
        dz_y = 0.0
        for i in range(2560):
            old_dz_x = 0.0
            old_z_x = 0.0
            if m2 > 1024.0:
                break
            old_dz_x = dz_x
            dz_x = 2.0 * z_x * dz_x - z_y * dz_y + 1.0
            dz_y = 2.0 * z_x * dz_y + z_y * old_dz_x

            old_z_x = z_x
            z_x = cc_x + z_x * z_x -z_y * z_y
            z_y = cc_y + 2.0 * old_z_x * z_y
            m2 = z_x * z_x + z_y * z_y
            co += 1.0
        d = 0.0
        if co < 2560.0:
            dot_z = z_x * z_x + z_y * z_y
            d = math.sqrt(dot_z/(dz_x*dz_x+dz_y*dz_y)) * math.log(dot_z)
            image[pixel_index + 0] = int(co % 256)
            d=4.0 * d /zoo
            if d < 0.0: d = 0.0
            if d > 1.0: d = 1.0
            image[pixel_index + 1] = int((1.0-d) * 76500 % 255.0) #int(math.fmod((1.0-d) * 255.0 * 300.0, 255.0))
            d = pow(d, 12.5)  #math.pow(d,50.0*0.25)

            image[pixel_index + 2] = int(d * 255.0)
        else:
            image[pixel_index + 0] = image[pixel_index + 1] = image[pixel_index + 2] = 0


#end calcul

image = bytearray(3*size*size)
for y in range(size):
    for x in range(size):
        calcul(x,y,image,3*(y * size + x))

fd = os.open("image1.ppm",os.O_CREAT|os.O_WRONLY,0o644)
os.write(fd,"P6\n{} {}\n255\n".format(size,size).encode())
os.write(fd,image)
os.close(fd)

