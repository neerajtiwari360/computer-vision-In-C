
#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// 3D image data: [height][width][3]
typedef struct {
    int width;
    int height;
    unsigned char *data;
} Image;

// OpenCV-like imread() — loads image as BGR
Image imread(const char *filename)
{
    Image img = {0};
    int channels;

    img.data = stbi_load(
        filename, &img.width, &img.height, &channels, 3
    );

    if (img.data == NULL)
        return img;

    // Convert RGB to BGR
    for (int i = 0; i < img.width * img.height; i++) {
        unsigned char temp = img.data[i * 3];
        img.data[i * 3] = img.data[i * 3 + 2];
        img.data[i * 3 + 2] = temp;
    }

    return img;
}

// OpenCV-like imwrite() — saves BGR image as PNG
int imwrite(const char *filename, const Image *img)
{
    if (!img || !img->data)
        return 0;

    // Convert BGR to RGB in a temporary buffer
    size_t size = (size_t)img->width * img->height * 3;
    unsigned char *rgb = malloc(size);

    if (!rgb)
        return 0;

    for (size_t i = 0; i < size; i += 3) {
        rgb[i]     = img->data[i + 2];
        rgb[i + 1] = img->data[i + 1];
        rgb[i + 2] = img->data[i];
    }

    int success = stbi_write_png(
        filename, img->width, img->height,
        3, rgb, img->width * 3
    );

    free(rgb);
    return success;
}

// Release image memory
void imfree(Image *img)
{
    if (img && img->data) {
        stbi_image_free(img->data);
        img->data = NULL;
    }
}


int main(void)
{
    Image img = imread("../../../assets/lenna.png");

    if (img.data == NULL) {
        printf("Failed to load image\n");
        return 1;
    }

    int width = img.width;
    int height = img.height;

    // View image as [height][width][3] in BGR order
    unsigned char (*pixels)[width][3] =
        (unsigned char (*)[width][3])img.data;

    // Convert BGR to grayscale
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {

            unsigned char B = pixels[y][x][0];
            unsigned char G = pixels[y][x][1];
            unsigned char R = pixels[y][x][2];

            unsigned char gray = (unsigned char)(
                0.114f * B +
                0.587f * G +
                0.299f * R + 0.5f
            );

            // Store grayscale in all 3 channels
            pixels[y][x][0] = gray;
            pixels[y][x][1] = gray;
            pixels[y][x][2] = gray;
        }
    }

    // Save grayscale-looking RGB PNG
    int success = imwrite(
        "../../../assets/lenna_gray.png", &img
    );

    if (success)
        printf("Grayscale image saved!\n");
    else
        printf("Failed to save image\n");

    imfree(&img);
    return success ? 0 : 1;
}

