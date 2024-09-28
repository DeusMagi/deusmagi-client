/**
 * @file
 * Color picker API header file.
 *
 * @author Alex Tokar
 */

#ifndef COLOR_PICKER_H
#define COLOR_PICKER_H

enum {
    /**
     * The color chooser.
     */
    COLOR_PICKER_ELEM_COLOR,
    /**
     * Hue chooser.
     */
    COLOR_PICKER_ELEM_HUE,

    /**
     * Number of elements.
     */
    COLOR_PICKER_ELEM_NUM
};

/**
 * One color picker element.
 */
typedef struct color_picker_element_struct {
    /**
     * Dimensions.
     */
    SDL_Rect coords;

    /**
     * If 1, this element is being dragged.
     */
    uint8_t dragging;
} color_picker_element_struct;

/**
 * Color picker structure.
 */
typedef struct color_picker_struct {
    /**
     * X position of the color picker.
     */
    int x;

    /**
     * Y position of the color picker.
     */
    int y;

    /**
     * X position of color picker's parent.
     */
    int px;

    /**
     * Y position of color picker's parent.
     */
    int py;

    /**
     * The elements.
     */
    color_picker_element_struct elements[COLOR_PICKER_ELEM_NUM];

    /**
     * Thickness of the border; 0 for none.
     */
    uint8_t border_thickness;

    /**
     * Which color is currently selected, in HSV (hue,saturation,value)
     * colorspace.
     */
    double hsv[3];

    void (*callback_func)(struct color_picker_struct *color_picker);
} color_picker_struct;

#endif
