from PIL import Image

def rgb_to_hex(r, g, b, a):
    return '#{:02x}{:02x}{:02x}{:02x}'.format(r, g, b, a)

_width = 0.66
_height = 1.117

#optimization alignment
_WIDTH_SIZE10 = 0.0287
_HEIGHT_SIZE10 = -0.4525

_width4x = _WIDTH_SIZE10 * 3
_height4x = _HEIGHT_SIZE10 * 3

im = Image.open(input(),'r')
im_width = im.width
im_height = im.height

image_data = list(im.getdata())

last_height = -1
last_color = 'none'
file_count = 0
tmp_char_limit = 4096
chars_per_pixel = 2
force_voffset = False
force_mark = False
chars_parsed = 0

size = 10
xoffset = 0
yoffset = 0
character = '_'


header = '<align=center><line-height=0%><width=0><size=' + str(size) + '><color=#000>'

optimized_pixels = [(-1,-1)]
last_was_optimized = False

optimized2x_once = False
optimized2x_once_offset = 0

sorted_pixels = [[-1],[-1],[-1],[-1],[-1],[-1],[-1],[-1],[-1],[-1],[-1],[-1],[-1],[-1],[-1],[-1],[-1],[-1],[-1],[-1],[-1]]
optimized_pixels = []

done = False
while done == False:
    new_pixel = False
    for index, pixel in enumerate(image_data):
        if ((pixel[3]) < 128):
            continue
    
        pixel_is_sorted_or_optimized = False
        for sorted_pixel_sizes in sorted_pixels:
            for sorted_pixel in sorted_pixel_sizes:
                if sorted_pixel == index:
                    pixel_is_sorted_or_optimized = True
                    break
            if pixel_is_sorted_or_optimized == True:
                break
        if pixel_is_sorted_or_optimized == True:
            continue
        for optimized_pixel in optimized_pixels:
            if optimized_pixel == index:
                pixel_is_sorted_or_optimized = True
                break
        if pixel_is_sorted_or_optimized == True:
            continue
        
        new_pixel = True
        
        hexcol = rgb_to_hex(pixel[0], pixel[1], pixel[2], pixel[3])
        
        if index + im_width > im_width*im_height - 1:
            sorted_pixels[0].append(index)
        elif hexcol != rgb_to_hex(image_data[index+im_width][0], image_data[index+im_width][1], image_data[index+im_width][2], image_data[index+im_width][3]):
            sorted_pixels[0].append(index)
        else:
            sorted_size = 1
            while index + im_width * sorted_size + sorted_size-1 < im_width*im_height:
                wrong_color = False
                
                to_be_optimized = []
                #vertical check
                for i in range(sorted_size+1):
                    check_index = index + sorted_size-1 + i * im_width
                    if hexcol != rgb_to_hex(image_data[check_index][0],image_data[check_index][1],image_data[check_index][2],image_data[check_index][3]):
                        wrong_color = True
                        break
                    if check_index != index:
                        to_be_optimized.append(check_index)
                
                #horizontal check
                for i in range(sorted_size):
                    check_index = index + (sorted_size-1) * im_width + i
                    if hexcol != rgb_to_hex(image_data[check_index][0],image_data[check_index][1],image_data[check_index][2],image_data[check_index][3]):
                        wrong_color = True
                        break
                    if check_index != index:
                        to_be_optimized.append(check_index)
                
                if wrong_color:
                    break
                else:
                    for idx in to_be_optimized:
                        optimized_pixels.append(idx)

                sorted_size += 1
            sorted_pixels[sorted_size-1].append(index)

    if new_pixel == False:
        done = True


file = open("tmp_out_part1.txt", "w")
for sorted_size, pixel_index_array in enumerate(sorted_pixels):
    if sorted_size == 0:
        out += '<size=' + str(size) + '>'
    else:
        out += '<size=' + str(size * (sorted_size+1)) + '>'

################
    
    for index in pixel_index_array:

        out = ''
        
        if ((chars_parsed * chars_per_pixel) % (tmp_char_limit - 2) == 0):
            file.close()
            file_count += 1
            file = open("tmp_out_part" + str(file_count) + '.txt', "w")
            print("tmp_out_part" + str(file_count) + '.txt')
            out = header
            force_voffset = True
            force_mark = True
            
        hexcol = rgb_to_hex(index[0], index[1], index[2], index[3])
        
        
        if sorted_size == 0:
            pass
        else:
            pass
        

            
        
        if (optimize2x == True):
            out += '<size=' + str(size*2) + '>'
        
        if (last_color != hexcol):
            out += '<mark=' + hexcol + '>'
            last_color = hexcol
        
        if (int( index / im_width ) != last_height or force_voffset == True or optimize2x == True):
            force_voffset = False
            if optimize2x == True:
                force_voffset = True
                
            if (optimize2x == False):
                out += '<voffset=' + str(round((int( index / im_width ) * _height )*-1 + yoffset + optimized2x_once_offset, 4)) + 'em>'
            if (optimize2x == True):
                out += '<voffset=' + str(round((int( index / im_width ) * _height )/2*-1 + yoffset + optimized_height_offset, 4)) + 'em>'
            
            last_height = int( index / im_width )
        
        if (optimize2x == False):
            out += '<pos=' + str(round((( index % im_width ) * 2 + 0 ) * _width + xoffset*2 + optimized_width_offset, 4)) + 'em>' + character
            out += '<pos=' + str(round((( index % im_width ) * 2 + 1 ) * _width + xoffset*2, 4)) + 'em>' + character
        
        if (optimize2x == True):
            out += '<pos=' + str(round((( index % im_width )) * _width + xoffset + optimized_width_offset, 4)) + 'em>' + character
            last_was_optimized = True

            
        file.write(out)
        chars_parsed += 1
    
file.close()
