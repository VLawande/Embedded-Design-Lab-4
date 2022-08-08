The purpose of this experiment was to use memory mapping to interface the DE1SoC board to a Monitor using the VGA display port

The Basic VGA Interfacing script is based on a section of code provided to us in the DE1SoC data sheet, and displays my class section and my name surrounded in a green box with a blue outline.

The code consists 7 functions: Initialize(), Finalize(), video_text(), video_box(), resample_rgb(), get_data_bits(), and video_circle().

Initialize():
    The Initialize() function takes in the pointer fd, and assigns it to point to the computer's memory address. After confirming that the memory address could be found and opened, the mmap command is used to create a virtual base for the other functions to use, which is returned to the user.

Finalize():
    The Finalize() function takes in the virtual base and the pointer fd, and unmaps the virtual base from the pointer.

video_text():
    The video_text() function takes in 2 integers x and y, a character pointer text_ptr and a virtual base pBase. The function first creates a character buffer pointer and assigns it to the memory space of the top left pixel that we will be writing to. It then creates an offset variable which which is used to determine which pixel we are currently writing to. Then a while loop is used to write a value to each pixel, and the loop keeps iterating until the text is written

video_box():
    The video_box() function takes in 4 integers (x1, x2, y1, y2), a short value (pixel_color) and a virtual base (pBase). Integers x1 and y1 represent the top left corner pixel of the box, while integers x2 and y2 represent the bottom right corner of the box. The function works by assigning a using 2 for loops to iterate through all of the pixels within the box, and a series of pointers to change the color values of each pixel so that they match the specified pixel_color.

resample_rgb():
    The resample_rgb() function is used resample a 24-bit color to either a 16-bit color of an 8-bit color. 

get_data_bits():
    The get_data_bits() function gets the number of data bits from the taken in mode.
________________________________________________________________________________________________________________________________________________________________________________________________________________________

The VGA Interfacing Bonus script is an unfinished script that when fully completed would display (in addition to everything on the Basic VGA Interfacing script) an orange hexagon and an orange circle on the screen. These shapes would then travel from the top of the screen to the bottom and back until switch 0 on the De1SoC was flipped. Upon the switch being flipped the shapes would stop in place and the Finalize() script would be run. The direction that the shapes should be travelling in was determined by a conditionial which would determine if the top pixel of either shape had collided with the top/bottom of the screen.

New Functions:
video_circle():
    The video_circle() function is identical to the video_box() function except it takes in a radius of the circle instead of left and right bounds. The function works by iterating through each pixel on a square that is the same width and height as the circles diameter using 2 for loops. A conditional is then used to determine if the current pixel needs to be a certain color or if it should stay empty.

video_hexagon():
    The video_hexagon() function is similar to the video_box() function. It first establishes all the same parameters as the video_box() function, and then calls the video box function to make a box that has the same width as the height of the hexagon, and a that is half of the total height of the hexagon. 2 for loops are then used to add more boxes of height 2 that get less wide as the loop iterates, creating a hexagon.