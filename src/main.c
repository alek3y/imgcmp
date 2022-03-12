#include <stdio.h>
#include <math.h>
#include <assert.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

typedef unsigned char Channel;
typedef struct {
	Channel r, g, b, a;
} Rgb;

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
	for (size_t i = 0; i < sizeof(src)/sizeof(*channels); i++) {
		dest[i] = (double) channels[i];
	}
	return dest;
}

double rgb_difference(Rgb first, Rgb second) {

	// Ensure the other channels are the same when alpha is zero
	if (first.a == 0 || second.a == 0) {
		memcpy(&first, &second, sizeof(first) - sizeof(first.a));
	}

	size_t rgb_size = sizeof(first)/sizeof(first.r);
	double first_point[rgb_size], second_point[rgb_size];

	return distance(
		rgb_copy_double(first_point, first),
		rgb_copy_double(second_point, second),
		rgb_size
	);
}

int main(void) {
	Rgb color1 = {184, 64, 64, 255};
	Rgb color2 = {179, 55, 55, 200};
	printf("%lf\n", rgb_difference(color1, color2));
}

/*int main(void) {
	int width, height;
	unsigned char *pixels = stbi_load(
		"image.png",
		&width, &height,
		NULL, 4 //3
	);
	printf("%dx%d\n", width, height);
	printf(
		"%u, %u, %u, %u\n",
		pixels[0], pixels[1], pixels[2], pixels[3]
	);
}*/
