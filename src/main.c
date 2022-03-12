#include <stdio.h>
#include <math.h>
#include <assert.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define RGB_MAX_DISTANCE 510	// Diagonal from #00000000 to #ffffffff
#define RGB_CHANNELS (sizeof(Rgb) / sizeof(Channel))

typedef unsigned char Channel;
typedef struct {
	Channel r, g, b, a;
} Rgb;

typedef struct {
	unsigned char *data;
	int width, height;
} Image;

// ELI5:
// The best way to find the distance between two points in any dimensions
// (using the pythagoras theorem) is to look at how it works for 2D and 3D
// with a drawing[1].
//
// For 1D, the distance would simply be the line `a`.
// For 2D, we'd use `a` as the new base and `b` as the height, which gives
// us `d`.
// For 3D, the line `d` (which is the 2D distance) becomes the new base,
// and `c` the new height.
// So, for 4D, the 3D distance is the base and the height is the new axis.
//
// You can see how every time we go up one dimension the new axis is the
// new height, and the distance of the previous dimension is the new base.
//
// Note to self: Fuck me, this looks super clean. Geez, this is probably
// my best function so far. Ouch, that's sad :<
//
// [1]: https://i.imgur.com/cMvpNM8.png
double distance(double *first, double *second, int dimensions) {
	if (dimensions < 1) {
		return 0;
	}

	double base = distance(first, second, dimensions-1);
	double height = fabs(first[dimensions-1] - second[dimensions-1]);

	return sqrt(pow(base, 2) + pow(height, 2));
}

double *rgb_copy_double(double *dest, Rgb src) {
	Channel *channels = (Channel *) &src;
	for (size_t i = 0; i < RGB_CHANNELS; i++) {
		dest[i] = (double) channels[i];
	}
	return dest;
}

double rgb_difference(Rgb first, Rgb second) {

	// Ensure the other channels are the same when alpha is zero
	if (first.a == 0 || second.a == 0) {
		memcpy(&first, &second, sizeof(Rgb) - sizeof(Channel));
	}

	double first_point[RGB_CHANNELS], second_point[RGB_CHANNELS];
	return distance(
		rgb_copy_double(first_point, first),
		rgb_copy_double(second_point, second),
		RGB_CHANNELS
	);
}

Image image_load(char *filename) {
	Image image;
	image.data = stbi_load(
		filename,
		&image.width, &image.height,
		NULL, RGB_CHANNELS
	);
	return image;
}

size_t image_area(Image image) {
	return (size_t) image.width * image.height;
}

void image_free(Image image) {
	stbi_image_free(image.data);
}

double image_difference(Image first, Image second) {
	assert(first.width == second.width && first.height == second.height);

	double total = 0;
	for (size_t i = 0; i < image_area(first) * RGB_CHANNELS; i += RGB_CHANNELS) {
		Rgb first_pixel, second_pixel;
		memcpy(&first_pixel, first.data + i, RGB_CHANNELS * sizeof(*first.data));
		memcpy(&second_pixel, second.data + i, RGB_CHANNELS * sizeof(*second.data));
		total += rgb_difference(first_pixel, second_pixel);
	}

	return total;
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf(
			"Usage: %s IMAGE1 IMAGE2\n"
			"Compute the similarity between two images as a percentage.\n",
			argv[0]
		);
		return 0;
	}

	Image first = image_load(argv[1]), second = image_load(argv[2]);

	double difference = image_difference(first, second) / (RGB_MAX_DISTANCE * image_area(first));
	printf("%lf%%\n", (1 - difference) * 100);

	image_free(first); image_free(second);
}
